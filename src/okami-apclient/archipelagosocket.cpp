#include "archipelagosocket.h"

#include <wolf_framework.hpp>

#include "checkman.h"
#include "loginwindow.h"
#include "notificationwindow.h"
#include "rewardman.h"

#pragma warning(push, 0)
#include <apuuid.hpp>
#pragma warning(pop)

#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include <version.h>

#include "version_utils.h"

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
static const std::string SAVE_DIR = std::string(std::getenv("APPDATA")) + "\\okami-apsaves";
#pragma warning(pop)

namespace
{

// Check version compatibility and log appropriate warnings
void checkVersionCompatibility(const std::string &supportedVersion)
{
    if (supportedVersion.empty())
    {
        wolf::logWarning("[Socket] No supported_client_version in slot_data, skipping version check");
        return;
    }

    auto server = version_utils::parseVersion(supportedVersion);
    if (!server)
    {
        wolf::logWarning("[Socket] Could not parse supported_client_version '%s'", supportedVersion.c_str());
        return;
    }

    version_utils::Version client{version::major(), version::minor(), version::patch()};

    auto compat = version_utils::checkCompatibility(client, *server);

    switch (compat)
    {
    case version_utils::Compatibility::Compatible:
        wolf::logInfo("[Socket] Client version %d.%d.%d is compatible with APWorld (%s)", client.major, client.minor, client.patch, supportedVersion.c_str());
        break;
    case version_utils::Compatibility::ClientTooOld:
        wolf::logWarning("[Socket] Client version %d.%d.%d is missing features APWorld expects (%s). "
                         "Consider updating the client.",
                         client.major, client.minor, client.patch, supportedVersion.c_str());
        break;
    case version_utils::Compatibility::MajorMismatch:
        wolf::logWarning("[Socket] Client version %d.%d.%d is incompatible with APWorld (%s). "
                         "Major version mismatch.",
                         client.major, client.minor, client.patch, supportedVersion.c_str());
        break;
    }
}

} // namespace

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
            wolf::logError("[Socket] Main thread task failed: %s", e.what());
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
    wolf::logInfo("[Socket] %s", status.c_str());
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

std::string ArchipelagoSocket::getSaveFilePath(const std::string &saveKey) const
{
    return SAVE_DIR + "\\" + saveKey + ".save";
}

void ArchipelagoSocket::saveLastItemIndex(const std::string &saveKey, int lastIndex)
{
    try
    {
        std::filesystem::create_directories(SAVE_DIR);

        std::string filepath = getSaveFilePath(saveKey);
        std::ofstream file(filepath);
        if (file.is_open())
        {
            file << lastIndex;
            file.close();
            wolf::logDebug("[Socket] Saved last item index %d to %s", lastIndex, saveKey.c_str());
        }
        else
        {
            wolf::logWarning("[Socket] Failed to open save file for writing: %s", filepath.c_str());
        }
    }
    catch (const std::exception &e)
    {
        wolf::logError("[Socket] Failed to save last item index: %s", e.what());
    }
}

int ArchipelagoSocket::loadLastItemIndex(const std::string &saveKey)
{
    try
    {
        std::string filepath = getSaveFilePath(saveKey);
        if (!std::filesystem::exists(filepath))
        {
            wolf::logDebug("[Socket] No save file found for %s, starting fresh", saveKey.c_str());
            return -1;
        }

        std::ifstream file(filepath);
        if (file.is_open())
        {
            int lastIndex;
            file >> lastIndex;
            file.close();
            wolf::logInfo("[Socket] Loaded last item index %d from %s", lastIndex, saveKey.c_str());
            return lastIndex;
        }
        else
        {
            wolf::logWarning("[Socket] Failed to open save file for reading: %s", filepath.c_str());
            return -1;
        }
    }
    catch (const std::exception &e)
    {
        wolf::logError("[Socket] Failed to load last item index: %s", e.what());
        return -1;
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

    wolf::logInfo("[Socket] Connecting to %s as %s", server.c_str(), slot.c_str());

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
        hasAttemptedConnection_.store(true);
        lastPollTime_ = std::chrono::steady_clock::now();
        connectionStartTime_ = std::chrono::steady_clock::now();
        setStatus("Connecting...");
    }
    catch (const std::exception &e)
    {
        wolf::logError("[Socket] Connection setup failed: %s", e.what());
        setStatus("Connection failed: " + std::string(e.what()));
    }
}

void ArchipelagoSocket::setupHandlers(const std::string &slot, const std::string &password)
{
    // Note: client_ is already locked when this is called

    client_->set_socket_connected_handler([]() { wolf::logDebug("[Socket] Socket connected"); });

    client_->set_socket_disconnected_handler(
        [this]()
        {
            wolf::logDebug("[Socket] Socket disconnected");
            connected_.store(false);

            // Clear any pending scout requests so scoutLocationsSync doesn't hang
            {
                std::lock_guard<std::mutex> lock(scoutMutex_);
                scoutPending_ = false;
            }
            scoutCondition_.notify_all();

            queueMainThreadTask([this]() { setStatus("Disconnected"); });
        });

    client_->set_room_info_handler(
        [this, slot, password]()
        {
            wolf::logDebug("[Socket] Room info received, connecting to slot");
            std::list<std::string> tags;
            client_->ConnectSlot(slot, password, ITEM_HANDLING, tags);
        });

    client_->set_slot_connected_handler(
        [this](const nlohmann::json &data)
        {
            wolf::logInfo("[Socket] Connected successfully!");
            connected_.store(true);

            if (checkMan_)
            {
                checkMan_->enableSending(true);
            }

            queueMainThreadTask([this]() { setStatus("Connected successfully!"); });

            // Load last processed item index for this session
            std::string saveKey = client_->get_slot() + "_" + client_->get_seed();
            lastProcessedItemIndex_ = loadLastItemIndex(saveKey);

            std::list<std::string> tags;
            client_->ConnectUpdate(false, ITEM_HANDLING, true, tags);
            client_->StatusUpdate(APClient::ClientStatus::PLAYING);

            // Parse slot_data configuration
            if (!data.is_null() && data.is_object())
            {
                auto result = SlotConfig::parse(data);
                if (result.has_value())
                {
                    slotConfig_ = result.value();
                }
                else
                {
                    wolf::logError("[Socket] Failed to parse slot_data: %s", result.error().c_str());
                    slotConfig_ = SlotConfig::defaults();
                }
            }
            else
            {
                wolf::logWarning("[Socket] No slot_data received, using defaults");
                slotConfig_ = SlotConfig::defaults();
            }
            slotConfigReady_.store(true, std::memory_order_release);

            // Check version compatibility
            checkVersionCompatibility(slotConfig_.supportedClientVersion);
        });

    client_->set_slot_disconnected_handler(
        [this]()
        {
            wolf::logWarning("[Socket] Slot disconnected");
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
                wolf::logError("[Socket] Slot refused: %s", error.c_str());
            }
            queueMainThreadTask([this, errorMsg]() { setStatus(errorMsg); });
        });

    client_->set_items_received_handler(
        [this](const std::list<APClient::NetworkItem> &items)
        {
            wolf::logInfo("[Socket] Received %zu items", items.size());

            // Check for full inventory reset (index 0 means accept as complete inventory)
            if (!items.empty() && items.front().index == 0)
            {
                wolf::logInfo("[Socket] Full inventory reset received (index 0)");
                // TODO: Clear player's current AP inventory before processing
                lastProcessedItemIndex_ = -1;
            }

            int highestIndex = lastProcessedItemIndex_;
            int expectedIndex = lastProcessedItemIndex_ + 1;
            int newItemCount = 0;

            for (const auto &item : items)
            {
                // Skip items we've already processed
                if (item.index <= lastProcessedItemIndex_)
                {
                    continue;
                }

                // Detect desync: gap in indices means we missed items
                if (item.index > expectedIndex && expectedIndex > 0)
                {
                    wolf::logWarning("[Socket] Desync detected: expected index %d, got %d. Requesting resync.", expectedIndex, item.index);
                    client_->Sync();
                    if (checkMan_)
                    {
                        checkMan_->resendAllChecks();
                    }
                    // Continue processing to avoid blocking gameplay during resync
                }

                if (item.index >= 0)
                {
                    queueMainThreadTask(
                        [this, itemId = item.item]()
                        {
                            if (rewardMan_)
                            {
                                rewardMan_->queueReward(itemId);
                            }
                            // Show notification banner for received item
                            notificationwindow::queue("Received: " + getLocalItemName(itemId));
                        });
                    newItemCount++;
                    highestIndex = std::max(highestIndex, item.index);
                    expectedIndex = item.index + 1;
                }
            }

            // Update and save the last processed index if we got new items
            if (highestIndex > lastProcessedItemIndex_)
            {
                lastProcessedItemIndex_ = highestIndex;
                std::string saveKey = client_->get_slot() + "_" + client_->get_seed();
                saveLastItemIndex(saveKey, lastProcessedItemIndex_);
                wolf::logInfo("[Socket] Processed %d new items (skipped %zu duplicates)", newItemCount, items.size() - newItemCount);
            }
        });

    client_->set_location_info_handler(
        [this](const std::list<APClient::NetworkItem> &items)
        {
            wolf::logDebug("[Socket] Received location info for %zu items", items.size());

            // Store scouted items and signal waiting threads
            {
                std::lock_guard<std::mutex> lock(scoutMutex_);
                scoutedItems_.clear();
                scoutedItems_.reserve(items.size());
                for (const auto &item : items)
                {
                    scoutedItems_.push_back(ScoutedItem{.item = item.item, .location = item.location, .player = item.player, .flags = item.flags});
                }
                scoutPending_ = false;
            }
            scoutCondition_.notify_all();
        });

    client_->set_location_checked_handler(
        [this](const std::list<int64_t> &locations)
        {
            wolf::logInfo("[Socket] Server reports %zu checked locations", locations.size());
            if (checkMan_)
            {
                checkMan_->syncWithServer(locations);
            }
        });
}

void ArchipelagoSocket::disconnect()
{
    connected_.store(false);
    slotConfigReady_.store(false, std::memory_order_release);
    lastProcessedItemIndex_ = -1;

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
                        wolf::logError("[Socket] Connection timed out");
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
            wolf::logError("[Socket] Poll failed while connected: %s", e.what());
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
        withClient([locationID](APClient &client) { client.LocationChecks({locationID}); });
    }
    catch (const std::exception &e)
    {
        wolf::logWarning("[Socket] Failed to send location: %s", e.what());
    }
}

void ArchipelagoSocket::sendLocations(const std::vector<int64_t> &locationIDs)
{
    if (locationIDs.empty())
    {
        return;
    }

    try
    {
        withClient(
            [&locationIDs](APClient &client)
            {
                std::list<int64_t> locList(locationIDs.begin(), locationIDs.end());
                client.LocationChecks(locList);
            });
    }
    catch (const std::exception &e)
    {
        wolf::logWarning("[Socket] Failed to send locations: %s", e.what());
    }
}

void ArchipelagoSocket::gameFinished()
{
    try
    {
        withClient(
            [](APClient &client)
            {
                wolf::logInfo("[Socket] Game completed!");
                client.StatusUpdate(APClient::ClientStatus::GOAL);
            });
    }
    catch (const std::exception &e)
    {
        wolf::logError("[Socket] Failed to report game completion: %s", e.what());
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
        wolf::logError("[Socket] Failed to get item name: %s", e.what());
        return "Unknown Item";
    }
}

std::string ArchipelagoSocket::getLocalItemName(int64_t id) const
{
    try
    {
        return withClient([id](APClient &client) { return client.get_item_name(id, GAME_NAME); });
    }
    catch (const std::exception &e)
    {
        wolf::logError("[Socket] Failed to get local item name: %s", e.what());
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
        wolf::logError("[Socket] Failed to get item description: %s", e.what());
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
        wolf::logError("[Socket] Failed to get connection info: %s", e.what());
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
        wolf::logError("[Socket] Failed to scout locations: %s", e.what());
        return false;
    }
}

std::vector<ScoutedItem> ArchipelagoSocket::scoutLocationsSync(const std::list<int64_t> &locations, int createAsHint, std::chrono::milliseconds timeout)
{
    if (locations.empty())
    {
        return {};
    }

    if (!isConnected())
    {
        wolf::logWarning("[Socket] Cannot scout: not connected");
        return {};
    }

    wolf::logDebug("[Socket] Scouting %zu locations synchronously", locations.size());

    // Set up pending state
    {
        std::lock_guard<std::mutex> lock(scoutMutex_);
        scoutedItems_.clear();
        scoutPending_ = true;
    }

    // Send the scout request
    if (!scoutLocations(locations, createAsHint))
    {
        std::lock_guard<std::mutex> lock(scoutMutex_);
        scoutPending_ = false;
        return {};
    }

    // Wait for response with timeout
    // Note: We need to keep polling while waiting
    auto deadline = std::chrono::steady_clock::now() + timeout;

    while (true)
    {
        // Check if we got a response
        {
            std::unique_lock<std::mutex> lock(scoutMutex_);
            if (!scoutPending_)
            {
                wolf::logDebug("[Socket] Scout completed, received %zu items", scoutedItems_.size());
                return std::move(scoutedItems_);
            }
        }

        // Check timeout
        if (std::chrono::steady_clock::now() >= deadline)
        {
            wolf::logWarning("[Socket] Scout timed out after %lldms", timeout.count());
            std::lock_guard<std::mutex> lock(scoutMutex_);
            scoutPending_ = false;
            return {};
        }

        // Poll the client to process incoming messages
        poll();

        // Brief sleep to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int ArchipelagoSocket::getPlayerSlot() const
{
    try
    {
        return withClient([](APClient &client) { return client.get_player_number(); });
    }
    catch (const std::exception &)
    {
        return -1;
    }
}

void ArchipelagoSocket::setRewardMan(RewardMan *rewardMan)
{
    rewardMan_ = rewardMan;
}

void ArchipelagoSocket::setCheckMan(CheckMan *checkMan)
{
    checkMan_ = checkMan;
}

const SlotConfig &ArchipelagoSocket::getSlotConfig() const
{
    return slotConfig_;
}

bool ArchipelagoSocket::isSlotConfigReady() const
{
    return slotConfigReady_.load(std::memory_order_acquire);
}
