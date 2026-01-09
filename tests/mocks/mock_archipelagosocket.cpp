#include "mock_archipelagosocket.h"

#include <algorithm>
#include <sstream>

namespace mock
{

// === ISocket Interface Implementation ===

void MockArchipelagoSocket::connect(const std::string &server, const std::string &slot, const std::string &password)
{
    server_ = server;
    slot_ = slot;
    password_ = password;
    state_ = ConnectionState::Connecting;
    pollCount_ = 0;
    pollCountAtRoomInfo_ = 0;
    currentStatus_ = "Connecting to " + server + "...";
}

void MockArchipelagoSocket::disconnect()
{
    state_ = ConnectionState::Disconnected;
    currentStatus_ = "Disconnected";
}

bool MockArchipelagoSocket::isConnected() const
{
    return state_ == ConnectionState::Connected;
}

void MockArchipelagoSocket::sendLocation(int64_t locationID)
{
    if (state_ == ConnectionState::Connected)
    {
        sentLocations_.push_back(locationID);
    }
}

void MockArchipelagoSocket::sendLocations(const std::vector<int64_t> &locationIDs)
{
    if (state_ == ConnectionState::Connected)
    {
        for (int64_t loc : locationIDs)
        {
            sentLocations_.push_back(loc);
        }
    }
}

void MockArchipelagoSocket::gameFinished()
{
    if (state_ == ConnectionState::Connected)
    {
        gameFinishedCalled_ = true;
        statusUpdates_.push_back(30); // GOAL status
    }
}

void MockArchipelagoSocket::poll()
{
    pollCount_++;

    // Check for scheduled disconnect
    if (disconnectAfterPolls_ > 0 && pollCount_ >= disconnectAfterPolls_)
    {
        state_ = ConnectionState::Disconnected;
        currentStatus_ = "Connection lost";
        disconnectAfterPolls_ = -1;
        return;
    }

    // Advance handshake if still connecting
    if (state_ == ConnectionState::Connecting || state_ == ConnectionState::WaitingForSlot)
    {
        advanceHandshake();
        return;
    }

    // Only process items/scouts when connected
    if (state_ != ConnectionState::Connected)
    {
        return;
    }

    // Deliver pending items to main thread queue
    deliverPendingItems();
}

void MockArchipelagoSocket::processMainThreadTasks()
{
    while (!mainThreadTasks_.empty())
    {
        auto task = std::move(mainThreadTasks_.front());
        mainThreadTasks_.pop();
        task();
    }
}

std::string MockArchipelagoSocket::getItemName(int64_t id, int player) const
{
    auto key = std::make_pair(id, player);
    auto it = itemNames_.find(key);
    if (it != itemNames_.end())
    {
        return it->second;
    }
    // Fallback: check with player 0 (generic)
    key = std::make_pair(id, 0);
    it = itemNames_.find(key);
    if (it != itemNames_.end())
    {
        return it->second;
    }
    return "Unknown Item";
}

std::string MockArchipelagoSocket::getItemDesc(int player) const
{
    auto it = itemDescs_.find(player);
    if (it != itemDescs_.end())
    {
        return it->second;
    }
    return "";
}

std::string MockArchipelagoSocket::getConnectionInfo() const
{
    if (state_ == ConnectionState::Connected)
    {
        return server_ + " (slot: " + slot_ + ")";
    }
    return "";
}

std::string MockArchipelagoSocket::getUUID() const
{
    return uuid_;
}

std::string MockArchipelagoSocket::getStatus() const
{
    return currentStatus_;
}

bool MockArchipelagoSocket::scoutLocations(const std::list<int64_t> &locations, int createAsHint)
{
    if (state_ != ConnectionState::Connected)
    {
        return false;
    }

    scoutRequests_.push_back({locations, createAsHint});
    return true;
}

std::vector<ScoutedItem> MockArchipelagoSocket::scoutLocationsSync(const std::list<int64_t> &locations, int createAsHint, std::chrono::milliseconds timeout)
{
    (void)timeout; // Mock doesn't need real timeout

    if (state_ != ConnectionState::Connected)
    {
        return {};
    }

    scoutRequests_.push_back({locations, createAsHint});

    // Check for configured timeout
    if (scoutTimeouts_.count(locations) > 0)
    {
        return {};
    }

    // Check for specific response
    auto it = scoutResponses_.find(locations);
    if (it != scoutResponses_.end())
    {
        return it->second;
    }

    // Return default response
    return defaultScoutResponse_;
}

int MockArchipelagoSocket::getPlayerSlot() const
{
    if (state_ == ConnectionState::Connected)
    {
        return playerSlot_;
    }
    return -1;
}

// === Connection Configuration ===

void MockArchipelagoSocket::setHandshakeConfig(const HandshakeConfig &config)
{
    handshakeConfig_ = config;
}

void MockArchipelagoSocket::setRefuseConnection(bool refuse, const std::vector<std::string> &reasons)
{
    handshakeConfig_.shouldRefuseConnection = refuse;
    handshakeConfig_.refusalReasons = reasons;
}

// === State Inspection ===

ConnectionState MockArchipelagoSocket::getConnectionState() const
{
    return state_;
}

const std::string &MockArchipelagoSocket::getConnectedServer() const
{
    return server_;
}

const std::string &MockArchipelagoSocket::getConnectedSlot() const
{
    return slot_;
}

int MockArchipelagoSocket::getPollCount() const
{
    return pollCount_;
}

// === Message Tracking ===

bool MockArchipelagoSocket::wasLocationSent(int64_t locationId) const
{
    return std::find(sentLocations_.begin(), sentLocations_.end(), locationId) != sentLocations_.end();
}

size_t MockArchipelagoSocket::getSentLocationCount() const
{
    return sentLocations_.size();
}

std::vector<int64_t> MockArchipelagoSocket::getSentLocationsInOrder() const
{
    return sentLocations_;
}

bool MockArchipelagoSocket::hasStatusUpdate(int status) const
{
    return std::find(statusUpdates_.begin(), statusUpdates_.end(), status) != statusUpdates_.end();
}

int MockArchipelagoSocket::getLastStatusUpdate() const
{
    if (statusUpdates_.empty())
    {
        return -1;
    }
    return statusUpdates_.back();
}

const std::vector<int> &MockArchipelagoSocket::getStatusUpdates() const
{
    return statusUpdates_;
}

bool MockArchipelagoSocket::wasGameFinishedCalled() const
{
    return gameFinishedCalled_;
}

size_t MockArchipelagoSocket::getScoutRequestCount() const
{
    return scoutRequests_.size();
}

const SentScoutRequest &MockArchipelagoSocket::getScoutRequest(size_t index) const
{
    return scoutRequests_.at(index);
}

// === Item Delivery Simulation ===

void MockArchipelagoSocket::queueItemToReceive(int64_t itemId, int player, unsigned flags)
{
    QueuedItem item;
    item.itemId = itemId;
    item.playerSlot = player;
    item.flags = flags;
    item.index = nextItemIndex_++;
    pendingItems_.push(item);
}

void MockArchipelagoSocket::queueItemsToReceive(const std::vector<QueuedItem> &items)
{
    for (auto item : items)
    {
        item.index = nextItemIndex_++;
        pendingItems_.push(item);
    }
}

void MockArchipelagoSocket::setItemReceivedCallback(ItemReceivedCallback callback)
{
    itemReceivedCallback_ = std::move(callback);
}

// === Scout Response Simulation ===

void MockArchipelagoSocket::setScoutResponse(const std::list<int64_t> &forLocations, const std::vector<ScoutedItem> &response)
{
    scoutResponses_[forLocations] = response;
}

void MockArchipelagoSocket::setDefaultScoutResponse(const std::vector<ScoutedItem> &response)
{
    defaultScoutResponse_ = response;
}

void MockArchipelagoSocket::setScoutTimeout(const std::list<int64_t> &forLocations)
{
    scoutTimeouts_.insert(forLocations);
}

// === Error Simulation ===

void MockArchipelagoSocket::scheduleDisconnect(int afterPolls)
{
    disconnectAfterPolls_ = pollCount_ + afterPolls;
}

// === Player/Session Configuration ===

void MockArchipelagoSocket::setPlayerSlot(int slot)
{
    playerSlot_ = slot;
}

void MockArchipelagoSocket::setUUID(const std::string &uuid)
{
    uuid_ = uuid;
}

void MockArchipelagoSocket::setItemName(int64_t id, int player, const std::string &name)
{
    itemNames_[std::make_pair(id, player)] = name;
}

void MockArchipelagoSocket::setItemDesc(int player, const std::string &desc)
{
    itemDescs_[player] = desc;
}

// === Compatibility ===

void MockArchipelagoSocket::setConnected(bool connected)
{
    state_ = connected ? ConnectionState::Connected : ConnectionState::Disconnected;
    if (connected && !hasStatusUpdate(10))
    {
        statusUpdates_.push_back(10); // PLAYING
    }
}

void MockArchipelagoSocket::clearSentLocations()
{
    sentLocations_.clear();
}

// === Reset ===

void MockArchipelagoSocket::reset()
{
    state_ = ConnectionState::Disconnected;
    pollCount_ = 0;
    pollCountAtRoomInfo_ = 0;
    handshakeConfig_ = HandshakeConfig{};

    server_.clear();
    slot_.clear();
    password_.clear();
    uuid_ = "test-uuid-12345";
    currentStatus_ = "Not connected";

    playerSlot_ = 1;
    itemNames_.clear();
    itemDescs_.clear();

    sentLocations_.clear();
    statusUpdates_.clear();
    scoutRequests_.clear();
    gameFinishedCalled_ = false;

    while (!pendingItems_.empty())
    {
        pendingItems_.pop();
    }
    while (!mainThreadTasks_.empty())
    {
        mainThreadTasks_.pop();
    }
    itemReceivedCallback_ = nullptr;
    nextItemIndex_ = 0;

    scoutResponses_.clear();
    defaultScoutResponse_.clear();
    scoutTimeouts_.clear();

    disconnectAfterPolls_ = -1;
}

// === Private Helpers ===

void MockArchipelagoSocket::advanceHandshake()
{
    // Process Connecting -> WaitingForSlot
    if (state_ == ConnectionState::Connecting)
    {
        if (pollCount_ >= handshakeConfig_.pollsUntilRoomInfo)
        {
            // RoomInfo received, moving to WaitingForSlot
            state_ = ConnectionState::WaitingForSlot;
            pollCountAtRoomInfo_ = pollCount_;
            currentStatus_ = "Received room info, connecting to slot...";
            // Fall through to check WaitingForSlot (allows instant connection)
        }
        else
        {
            return;
        }
    }

    // Process WaitingForSlot -> Connected/Refused
    if (state_ == ConnectionState::WaitingForSlot)
    {
        int pollsSinceRoomInfo = pollCount_ - pollCountAtRoomInfo_;
        if (pollsSinceRoomInfo >= handshakeConfig_.pollsUntilSlotResponse)
        {
            if (handshakeConfig_.shouldRefuseConnection)
            {
                state_ = ConnectionState::Refused;
                if (!handshakeConfig_.refusalReasons.empty())
                {
                    std::ostringstream ss;
                    ss << "Connection refused: ";
                    for (size_t i = 0; i < handshakeConfig_.refusalReasons.size(); ++i)
                    {
                        if (i > 0)
                            ss << ", ";
                        ss << handshakeConfig_.refusalReasons[i];
                    }
                    currentStatus_ = ss.str();
                }
                else
                {
                    currentStatus_ = "Connection refused";
                }
            }
            else
            {
                state_ = ConnectionState::Connected;
                currentStatus_ = "Connected to " + server_;
                // Simulate StatusUpdate(PLAYING) being sent automatically
                statusUpdates_.push_back(10); // PLAYING
            }
        }
    }
}

void MockArchipelagoSocket::deliverPendingItems()
{
    while (!pendingItems_.empty())
    {
        QueuedItem item = pendingItems_.front();
        pendingItems_.pop();

        if (itemReceivedCallback_)
        {
            // Capture item by value for the lambda
            mainThreadTasks_.push([this, item]() { itemReceivedCallback_(item); });
        }
    }
}

} // namespace mock
