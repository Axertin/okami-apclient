#include "archipelagosocket.h"

#include "aplocationmonitor.h"
#include "gamehooks.h"
#include "logger.h"
#include "receive.h"

#pragma warning(push, 0)
#include <apuuid.hpp>
#pragma warning(pop)

#include <filesystem>
#include <string>

// File-local constants
static const std::string CERT_STORE = "mods/apclient/cacert.pem";
static const std::string GAME_NAME = "Okami HD";
static const int ITEM_HANDLING = 0b111;
static const auto POLL_INTERVAL_CONNECTED = std::chrono::milliseconds(1000);
static const auto POLL_INTERVAL_CONNECTING = std::chrono::milliseconds(200);
static const auto CONNECTION_TIMEOUT = std::chrono::seconds(10);

#pragma warning(push)
#pragma warning(disable : 4996)
static const std::string UUID_FILE = std::string(std::getenv("APPDATA")) + "\\uuid";
#pragma warning(pop)

ArchipelagoSocket &ArchipelagoSocket::instance()
{
    static ArchipelagoSocket socket;
    return socket;
}

bool ArchipelagoSocket::isConnected() const
{
    return connected_.load();
}

std::string ArchipelagoSocket::getStatus() const
{
    std::lock_guard<std::mutex> lock(statusMutex_);
    return currentStatus_;
}

std::string ArchipelagoSocket::getUUID() const
{
    std::lock_guard<std::mutex> lock(statusMutex_);
    return uuid_;
}

void ArchipelagoSocket::processMainThreadTasks()
{
    std::queue<std::function<void()>> tasksToProcess;

    {
        std::lock_guard<std::mutex> lock(taskMutex_);
        tasksToProcess.swap(mainThreadTasks_);
    }

    while (!tasksToProcess.empty())
    {
        auto task = std::move(tasksToProcess.front());
        tasksToProcess.pop();
        try
        {
            task();
        }
        catch (const std::exception &e)
        {
            logError("[apsocket] Main thread task failed: %s", e.what());
        }
    }
}

void ArchipelagoSocket::queueMainThreadTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(taskMutex_);
    mainThreadTasks_.push(std::move(task));
}

void ArchipelagoSocket::setStatus(const std::string &status)
{
    {
        std::lock_guard<std::mutex> lock(statusMutex_);
        currentStatus_ = status;
    }
    logInfo("[apsocket] %s", status.c_str());
}

std::string ArchipelagoSocket::buildURI(const std::string &server) const
{
    if (server.starts_with("ws://") || server.starts_with("wss://"))
    {
        return server;
    }

    if (server.find(':') == std::string::npos)
    {
        throw std::invalid_argument("Server must include port (e.g., localhost:38281)");
    }

    // Use secure WebSocket for non-local connections
    if (server.starts_with("localhost") || server.starts_with("127.0.0.1"))
    {
        return "ws://" + server;
    }
    else
    {
        return "wss://" + server;
    }
}

void ArchipelagoSocket::connect(const std::string &server, const std::string &slot, const std::string &password)
{
    if (server.empty())
    {
        setStatus("Server address cannot be empty");
        return;
    }

    if (slot.empty())
    {
        setStatus("Slot name cannot be empty");
        return;
    }

    logInfo("[apsocket] Connecting to %s as %s", server.c_str(), slot.c_str());

    // Disconnect any existing connection
    disconnect();

    try
    {
        // Build URI and generate UUID
        std::string uri = buildURI(server);
        std::string uuidBase = server;
        if (uri.starts_with("wss://") || uri.starts_with("ws://"))
        {
            uuidBase = uri.substr(uri.find("://") + 3);
        }

        // Validate SSL certificate if needed
        if (uri.starts_with("wss://") && !std::filesystem::exists(CERT_STORE))
        {
            setStatus("SSL certificate file missing: " + CERT_STORE);
            return;
        }

        // Generate UUID
        {
            std::lock_guard<std::mutex> lock(statusMutex_);
            uuid_ = ap_get_uuid(UUID_FILE, uuidBase);
        }

        // Create APClient
        {
            std::lock_guard<std::mutex> lock(clientMutex_);
            client_ = std::make_unique<APClient>(uuid_, GAME_NAME, uri, CERT_STORE);
            setupHandlers(slot, password);
        }

        connected_.store(false);
        hasAttemptedConnection_.store(true); // NEW: Mark that we've attempted connection
        lastPollTime_ = std::chrono::steady_clock::now();
        connectionStartTime_ = std::chrono::steady_clock::now();
        setStatus("Connecting...");
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Connection setup failed: %s", e.what());
        setStatus("Connection failed: " + std::string(e.what()));
    }
}

void ArchipelagoSocket::setupHandlers(const std::string &slot, const std::string &password)
{
    // Note: client_ is already locked when this is called

    client_->set_socket_connected_handler([]() { logDebug("[apsocket] Socket connected"); });

    client_->set_socket_disconnected_handler(
        [this]()
        {
            logDebug("[apsocket] Socket disconnected");
            connected_.store(false);
            queueMainThreadTask([this]() { setStatus("Disconnected"); });
        });

    client_->set_room_info_handler(
        [this, slot, password]()
        {
            logDebug("[apsocket] Room info received, connecting to slot");
            std::list<std::string> tags;
            client_->ConnectSlot(slot, password, ITEM_HANDLING, tags);
        });

    client_->set_slot_connected_handler(
        [this](const nlohmann::json &data)
        {
            logInfo("[apsocket] Connected successfully!");
            connected_.store(true);

            APLocationMonitor::instance().enableSending(true);

            queueMainThreadTask([this]() { setStatus("Connected successfully!"); });

            std::list<std::string> tags;
            client_->ConnectUpdate(false, ITEM_HANDLING, true, tags);
            client_->StatusUpdate(APClient::ClientStatus::PLAYING);
        });

    client_->set_slot_disconnected_handler(
        [this]()
        {
            logWarning("[apsocket] Slot disconnected");
            connected_.store(false);
            queueMainThreadTask([this]() { setStatus("Disconnected from slot"); });
        });

    client_->set_slot_refused_handler(
        [this](const std::list<std::string> &errors)
        {
            connected_.store(false);
            std::string errorMsg = "Connection refused: ";
            for (const auto &error : errors)
            {
                errorMsg += error + " ";
                logError("[apsocket] Slot refused: %s", error.c_str());
            }
            queueMainThreadTask([this, errorMsg]() { setStatus(errorMsg); });
        });

    client_->set_items_received_handler(
        [this](const std::list<APClient::NetworkItem> &items)
        {
            logInfo("[apsocket] Received %zu items", items.size());

            for (const auto &item : items)
            {
                if (item.index > 0)
                {
                    queueMainThreadTask([itemId = item.item]() { receiveAPItem(itemId); });
                }
            }
        });

    client_->set_location_info_handler([](const std::list<APClient::NetworkItem> &items)
                                       { logDebug("[apsocket] Received location info for %zu items", items.size()); });
}

void ArchipelagoSocket::disconnect()
{
    connected_.store(false);

    std::lock_guard<std::mutex> lock(clientMutex_);
    if (client_)
    {
        client_.reset();
        setStatus("Disconnected");
    }
    // Note: Don't reset hasAttemptedConnection_ here - user might reconnect
}

void ArchipelagoSocket::poll()
{
    // Don't poll if we've never attempted a connection
    if (!hasAttemptedConnection_.load())
    {
        return;
    }

    try
    {
        withClient(
            [this](APClient &client)
            {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = now - lastPollTime_;
                auto interval = connected_.load() ? POLL_INTERVAL_CONNECTED : POLL_INTERVAL_CONNECTING;

                // Check for connection timeout
                if (!connected_.load())
                {
                    auto connectionElapsed = now - connectionStartTime_;
                    if (connectionElapsed >= CONNECTION_TIMEOUT)
                    {
                        logError("[apsocket] Connection timed out");
                        connected_.store(false);
                        queueMainThreadTask([this]() { setStatus("Connection timed out"); });
                        // Clean up and return - don't throw
                        {
                            std::lock_guard<std::mutex> lock(clientMutex_);
                            client_.reset();
                        }
                        return;
                    }
                }

                if (elapsed >= interval)
                {
                    client.poll();
                    lastPollTime_ = now;
                }
            });
    }
    catch (const std::exception &e)
    {
        bool wasConnected = connected_.load();
        connected_.store(false);

        // Only log if we thought we were connected
        if (wasConnected)
        {
            logError("[apsocket] Poll failed while connected: %s", e.what());
            queueMainThreadTask([this, error = std::string(e.what())]() { setStatus("Connection lost: " + error); });
        }

        // Clean up failed connection
        {
            std::lock_guard<std::mutex> lock(clientMutex_);
            client_.reset();
        }
    }
}

void ArchipelagoSocket::sendLocation(int64_t locationID)
{
    try
    {
        withClient(
            [locationID](APClient &client)
            {
                logInfo("[apsocket] Sending location: 0x%llX", locationID);
                client.LocationChecks({locationID});
            });
    }
    catch (const std::exception &e)
    {
        logWarning("[apsocket] Failed to send location: %s", e.what());
    }
}

void ArchipelagoSocket::gameFinished()
{
    try
    {
        withClient(
            [](APClient &client)
            {
                logInfo("[apsocket] Game completed!");
                client.StatusUpdate(APClient::ClientStatus::GOAL);
            });
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Failed to report game completion: %s", e.what());
    }
}

std::string ArchipelagoSocket::getItemName(int64_t id, int player) const
{
    try
    {
        return withClient([id, player](APClient &client) { return client.get_item_name(id, client.get_player_game(player)); });
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Failed to get item name: %s", e.what());
        return "Unknown Item";
    }
}

std::string ArchipelagoSocket::getItemDesc(int player) const
{
    try
    {
        return withClient([player](APClient &client) { return "Item for " + client.get_player_alias(player) + " (" + client.get_player_game(player) + ")"; });
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Failed to get item description: %s", e.what());
        return "Unknown Player";
    }
}

std::string ArchipelagoSocket::getConnectionInfo() const
{
    try
    {
        return withClient([](APClient &client) { return client.get_slot() + "_" + client.get_seed(); });
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Failed to get connection info: %s", e.what());
        return "";
    }
}

bool ArchipelagoSocket::scoutLocations(const std::list<int64_t> &locations, int createAsHint)
{
    try
    {
        return withClient([&locations, createAsHint](APClient &client) { return client.LocationScouts(locations, createAsHint); });
    }
    catch (const std::exception &e)
    {
        logError("[apsocket] Failed to scout locations: %s", e.what());
        return false;
    }
}
