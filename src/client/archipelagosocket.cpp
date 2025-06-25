
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

static std::string ConnectionStatus = "";
std::string ArchipelagoSocket::uuid = "";
static bool IsHTTPS = false;
static bool IsWSS = false;
static bool IsWS = false;
bool ArchipelagoSocket::Connected = false;
bool ArchipelagoSocket::APSyncQueued = false;

std::chrono::steady_clock::time_point lastPollTime;

APClient *ArchipelagoSocket::Client = nullptr;

ArchipelagoSocket &ArchipelagoSocket::instance()
{
    static ArchipelagoSocket socket;
    return socket;
}

void ArchipelagoSocket::clientConnect(LoginWindow *LoginData)
{
    std::string serverInput = LoginData->Server;
    logInfo("[apsocket] Starting connection attempt");
    logInfo("[apsocket] Raw server input: '%s'", serverInput.c_str());
    logInfo("[apsocket] Slot: '%s'", LoginData->Slot);
    logInfo("[apsocket] Password length: %d", strlen(LoginData->Password));

    // Clean and validate server input
    if (serverInput.empty())
    {
        logError("[apsocket] Server field is empty!");
        if (LoginData)
        {
            LoginData->setMessage("Please enter a server address");
        }
        return;
    }

    // Build proper URI with very explicit logging
    std::string uri;
    std::string uuidBase;

    if (serverInput.starts_with("ws://"))
    {
        uri = serverInput;
        uuidBase = serverInput.substr(5); // Remove ws://
        logDebug("[apsocket] Detected WebSocket URI, using as-is");
    }
    else if (serverInput.starts_with("wss://"))
    {
        uri = serverInput;
        uuidBase = serverInput.substr(6); // Remove wss://
        logDebug("[apsocket] Detected Secure WebSocket URI, using as-is");
    }
    else
    {
        // Treat as host:port
        std::string hostPort = serverInput;
        if (hostPort.find(':') == std::string::npos)
        {
            logError("[apsocket] No port specified", hostPort.c_str());
        }

        // For localhost, try regular WebSocket first
        if (hostPort.starts_with("localhost") || hostPort.starts_with("127.0.0.1"))
        {
            uri = "ws://" + hostPort;
            logDebug("[apsocket] Using non-secure WebSocket for localhost: %s", uri.c_str());
        }
        else
        {
            uri = "wss://" + hostPort;
            logDebug("[apsocket] Using secure WebSocket for remote server: %s", uri.c_str());
        }
        uuidBase = hostPort;
    }

    logDebug("[apsocket] Final URI: %s", uri.c_str());
    logDebug("[apsocket] UUID base: %s", uuidBase.c_str());

    // Check certificate file
    if (!std::filesystem::exists(CertStore))
    {
        logError("[apsocket] Certificate file not found: %s", CertStore.c_str());
        // For localhost with ws://, we might not need the cert file
        if (!uri.starts_with("ws://"))
        {
            if (LoginData)
            {
                LoginData->setMessage("SSL certificate file missing: " + CertStore);
            }
            return;
        }
        else
        {
            logWarning("[apsocket] Certificate missing but using non-secure connection");
        }
    }
    else
    {
        logInfo("[apsocket] Certificate file found, size: %d bytes", std::filesystem::file_size(CertStore));
    }

    // Generate UUID
    logDebug("[apsocket] Generating UUID...");
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
            LoginData->setMessage("UUID generation failed: " + std::string(e.what()));
        }
        return;
    }

    // Clean up existing client
    if (Client != nullptr)
    {
        logDebug("[apsocket] Cleaning up existing client");
        try
        {
            delete Client;
            Client = nullptr;
            logDebug("[apsocket] Previous client cleaned up successfully");
        }
        catch (const std::exception &e)
        {
            logError("[apsocket] Error cleaning up previous client: %s", e.what());
        }
    }

    // Create new client with detailed error handling
    logInfo("[apsocket] Creating APClient...");
    logInfo("[apsocket] - UUID: %s", uuid.c_str());
    logInfo("[apsocket] - Game: %s", GameName.c_str());
    logInfo("[apsocket] - URI: %s", uri.c_str());
    logInfo("[apsocket] - Cert: %s", CertStore.c_str());

    try
    {
        // Add a small delay to ensure clean state
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        Client = new APClient(uuid, GameName, uri, CertStore);
        logInfo("[apsocket] APClient created successfully");

        APSyncQueued = false;
        Connected = false;
        lastPollTime = std::chrono::steady_clock::now();

        if (LoginData)
        {
            LoginData->setMessage("APClient created, connecting to " + uri + "...");
        }
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Exception creating APClient: %s", e.what());
        if (LoginData)
        {
            LoginData->setMessage("Failed to create client: " + std::string(e.what()));
        }
        return;
    }
    catch (...)
    {
        logError("[apsocket] Unknown exception creating APClient");
        if (LoginData)
        {
            LoginData->setMessage("Unknown error creating client");
        }
        return;
    }

    // Set up all handlers with detailed logging
    logDebug("[apsocket] Setting up connection handlers...");

    Client->set_socket_connected_handler([]() { logInfo("[apsocket] *** HANDLER: Socket connected to server ***"); });

    Client->set_socket_disconnected_handler(
        []()
        {
            logInfo("[apsocket] *** HANDLER: Socket disconnected from server ***");
            Connected = false;
        });

    Client->set_room_info_handler(
        [LoginData]()
        {
            logInfo("[apsocket] *** HANDLER: Room info received ***");
            logInfo("[apsocket] Server is responding! Attempting slot connection...");
            logInfo("[apsocket] Slot name: '%s'", LoginData->Slot);

            std::list<std::string> Tags;
            try
            {
                Client->ConnectSlot(LoginData->Slot, LoginData->Password, ItemHandlingVector, Tags);
                logInfo("[apsocket] ConnectSlot called successfully");
            }
            catch (const std::exception &e)
            {
                logError("[apsocket] Exception in ConnectSlot: %s", e.what());
            }
        });

    Client->set_slot_connected_handler(
        [LoginData](const nlohmann::json &Data)
        {
            logInfo("[apsocket] *** HANDLER: Slot connected successfully ***");
            Connected = true;

            std::list<std::string> Tags = {};
            Client->ConnectUpdate(false, ItemHandlingVector, true, Tags);
            Client->StatusUpdate(APClient::ClientStatus::PLAYING);

            if (LoginData)
            {
                LoginData->setMessage("Connected successfully to slot!");
            }
        });

    Client->set_slot_disconnected_handler(
        [LoginData]()
        {
            logInfo("[apsocket] *** HANDLER: Slot disconnected ***");
            Connected = false;
            if (LoginData)
            {
                LoginData->setMessage("Slot disconnected");
            }
        });

    Client->set_slot_refused_handler(
        [LoginData](const std::list<std::string> &Errors)
        {
            logError("[apsocket] *** HANDLER: Slot connection refused ***");
            Connected = false;
            for (const auto &error : Errors)
            {
                logError("[apsocket] Refusal reason: %s", error.c_str());
            }
            if (LoginData)
            {
                std::string errorMsg = "Slot refused: ";
                for (const auto &error : Errors)
                {
                    errorMsg += error + " ";
                }
                LoginData->setMessage(errorMsg);
            }
        });

    // Set up item handlers
    Client->set_items_received_handler(
        [](const std::list<APClient::NetworkItem> &Items)
        {
            logInfo("[apsocket] *** HANDLER: Items received (count: %d) ***", Items.size());
            for (const auto &Item : Items)
            {
                logDebug("[apsocket] Received Item ID: 0x%X, index: %d", Item.item, Item.index);
            }
        });

    Client->set_location_info_handler(
        [](const std::list<APClient::NetworkItem> &Items)
        {
            for (const auto &Item : Items)
            {
                logDebug("[apsocket] Received location item, ID: 0x%X", Item.item);
            }
        });

    logInfo("[apsocket] All handlers configured");
    logInfo("[apsocket] Connection attempt initiated");
}

void ArchipelagoSocket::sendLocation(int64_t LocationID)
{
    logInfo("[apsocket] Sending Location 0x%X", LocationID);
    std::list<int64_t> Check;
    Check.push_back(LocationID);
    Client->LocationChecks(Check);
}

void ArchipelagoSocket::gameFinished()
{
    Client->StatusUpdate(APClient::ClientStatus::GOAL);
}

void ArchipelagoSocket::poll()
{
    if (Client)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPollTime).count();

        if (elapsedTimeMs >= (Connected ? PollingIntervalMs : 200))
        {
            try
            {
                Client->poll();
            }
            catch (const std::exception &e)
            {
                logError("[apsocket] Exception during poll: %s", e.what());
                Connected = false;
            }

            lastPollTime = now;
        }
    }
    else
    {
        static int noClientCount = 0;
        if (++noClientCount % 300 == 0)
        {
            logWarning("[apsocket] Poll called but no Client exists");
        }
    }
}

std::string ArchipelagoSocket::getItemName(int64_t ID, int Player)
{
    if (Client)
    {
        return Client->get_item_name(ID, Client->get_player_game(Player));
    }
    else
    {
        return "";
    }
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
