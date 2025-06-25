#include "archipelagosocket.h"

#include "gamehooks.h"
#include "logger.h"
#include "loginwindow.h"
#include "receive.h"

#pragma warning(push, 0)
#include <apuuid.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4996)
const std::string uuid_file = std::string(std::getenv("APPDATA")).append("\\uuid");
#pragma warning(pop)

const std::string CertStore = "mods/apclient/cacert.pem";
const std::string GameName = "Okami";
const int ItemHandlingVector = 0b111;
const long long PollingIntervalMs = 1000;

std::string ArchipelagoSocket::uuid = "";
bool ArchipelagoSocket::Connected = false;
bool ArchipelagoSocket::APSyncQueued = false;

static std::chrono::steady_clock::time_point lastPollTime;
APClient *ArchipelagoSocket::Client = nullptr;

ArchipelagoSocket &ArchipelagoSocket::instance()
{
    static ArchipelagoSocket socket;
    return socket;
}

void ArchipelagoSocket::clientConnect(LoginWindow *LoginData)
{
    std::string serverInput = LoginData->Server;
    logInfo("[apsocket] Connecting to %s as %s", serverInput.c_str(), LoginData->Slot);

    if (serverInput.empty())
    {
        logError("[apsocket] Server field is empty");
        if (LoginData)
        {
            LoginData->setMessage("Please enter a server address");
        }
        return;
    }

    // Build proper URI
    std::string uri;
    std::string uuidBase;

    if (serverInput.starts_with("ws://") || serverInput.starts_with("wss://"))
    {
        uri = serverInput;
        uuidBase = serverInput.substr(serverInput.find("://") + 3);
        logDebug("[apsocket] Using provided WebSocket URI: %s", uri.c_str());
    }
    else
    {
        // Treat as host:port
        std::string hostPort = serverInput;
        if (hostPort.find(':') == std::string::npos)
        {
            logError("[apsocket] No port specified in server address");
            if (LoginData)
            {
                LoginData->setMessage("Server must include port (e.g., localhost:38281)");
            }
            return;
        }

        // Use appropriate protocol based on host
        if (hostPort.starts_with("localhost") || hostPort.starts_with("127.0.0.1"))
        {
            uri = "ws://" + hostPort;
        }
        else
        {
            uri = "wss://" + hostPort;
        }
        uuidBase = hostPort;
        logDebug("[apsocket] Built URI: %s", uri.c_str());
    }

    // Validate certificate for secure connections
    if (uri.starts_with("wss://") && !std::filesystem::exists(CertStore))
    {
        logError("[apsocket] SSL certificate file missing: %s", CertStore.c_str());
        if (LoginData)
        {
            LoginData->setMessage("SSL certificate file missing");
        }
        return;
    }

    // Generate UUID
    try
    {
        uuid = ap_get_uuid(uuid_file, uuidBase);
        logDebug("[apsocket] Generated UUID: %s", uuid.c_str());
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Failed to generate UUID: %s", e.what());
        if (LoginData)
        {
            LoginData->setMessage("UUID generation failed");
        }
        return;
    }

    // Clean up existing client
    if (Client != nullptr)
    {
        logDebug("[apsocket] Cleaning up previous connection");
        delete Client;
        Client = nullptr;
    }

    // Create APClient
    try
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Client = new APClient(uuid, GameName, uri, CertStore);

        APSyncQueued = false;
        Connected = false;
        lastPollTime = std::chrono::steady_clock::now();

        if (LoginData)
        {
            LoginData->setMessage("Connecting...");
        }
        logDebug("[apsocket] APClient created successfully");
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Failed to create client: %s", e.what());
        if (LoginData)
        {
            LoginData->setMessage("Connection failed: " + std::string(e.what()));
        }
        return;
    }

    // Set up connection handlers
    Client->set_socket_connected_handler([]() { logDebug("[apsocket] Socket connected to server"); });

    Client->set_socket_disconnected_handler(
        []()
        {
            logDebug("[apsocket] Socket disconnected from server");
            Connected = false;
        });

    Client->set_room_info_handler(
        [LoginData]()
        {
            logDebug("[apsocket] Room info received, connecting to slot");
            std::list<std::string> Tags;
            Client->ConnectSlot(LoginData->Slot, LoginData->Password, ItemHandlingVector, Tags);
        });

    Client->set_slot_connected_handler(
        [LoginData](const nlohmann::json &Data)
        {
            logInfo("[apsocket] Connected successfully!");
            Connected = true;

            std::list<std::string> Tags = {};
            Client->ConnectUpdate(false, ItemHandlingVector, true, Tags);
            Client->StatusUpdate(APClient::ClientStatus::PLAYING);

            if (LoginData)
            {
                LoginData->setMessage("Connected successfully!");
            }
        });

    Client->set_slot_disconnected_handler(
        [LoginData]()
        {
            logWarning("[apsocket] Disconnected from slot");
            Connected = false;
            if (LoginData)
            {
                LoginData->setMessage("Disconnected from server");
            }
        });

    Client->set_slot_refused_handler(
        [LoginData](const std::list<std::string> &Errors)
        {
            std::string errorMsg = "Connection refused: ";
            for (const auto &error : Errors)
            {
                errorMsg += error + " ";
                logError("[apsocket] Slot refused: %s", error.c_str());
            }
            Connected = false;
            if (LoginData)
            {
                LoginData->setMessage(errorMsg);
            }
        });

    Client->set_items_received_handler(
        [](const std::list<APClient::NetworkItem> &Items)
        {
            logInfo("[apsocket] Received %d items", Items.size());
            for (const auto &Item : Items)
            {
                logDebug("[apsocket] Item ID: 0x%X, index: %d", Item.item, Item.index);
                if (Item.index > 0)
                {
                    receiveAPItem(Item.item);
                }
            }
        });

    Client->set_location_info_handler([](const std::list<APClient::NetworkItem> &Items)
                                      { logDebug("[apsocket] Received location info for %d items", Items.size()); });

    logDebug("[apsocket] Connection handlers configured");
}

void ArchipelagoSocket::sendLocation(int64_t LocationID)
{
    if (!Client || !Connected)
    {
        logWarning("[apsocket] Cannot send location - not connected");
        return;
    }

    logInfo("[apsocket] Sending location check: 0x%X", LocationID);
    std::list<int64_t> Check;
    Check.push_back(LocationID);
    Client->LocationChecks(Check);
}

void ArchipelagoSocket::gameFinished()
{
    if (Client && Connected)
    {
        logInfo("[apsocket] Game completed!");
        Client->StatusUpdate(APClient::ClientStatus::GOAL);
    }
}

void ArchipelagoSocket::poll()
{
    if (Client)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPollTime).count();

        // Poll more frequently when not connected to establish connection faster
        auto interval = Connected ? PollingIntervalMs : 200LL;

        if (elapsedTimeMs >= interval)
        {
            try
            {
                Client->poll();
            }
            catch (const std::exception &e)
            {
                logError("[apsocket] Poll error: %s", e.what());
                Connected = false;
            }
            lastPollTime = now;
        }
    }
}

std::string ArchipelagoSocket::getItemName(int64_t ID, int Player)
{
    if (Client)
    {
        return Client->get_item_name(ID, Client->get_player_game(Player));
    }
    return "";
}

std::string ArchipelagoSocket::getItemDesc(int player)
{
    if (Client)
    {
        return "Item for " + Client->get_player_alias(player) + " playing " + Client->get_player_game(player);
    }
    return "";
}

std::string ArchipelagoSocket::getAddrInfo()
{
    if (Client)
    {
        return Client->get_slot() + "_" + Client->get_seed();
    }
    return "";
}

bool ArchipelagoSocket::scoutLocations(std::list<int64_t> Locations, int CreateAsHint)
{
    if (Client)
    {
        return Client->LocationScouts(Locations, CreateAsHint);
    }
    return false;
}

bool ArchipelagoSocket::isConnected() const
{
    return Connected;
}

std::string ArchipelagoSocket::getUUID() const
{
    return uuid;
}
