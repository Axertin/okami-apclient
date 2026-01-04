#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "isocket.h"

namespace mock
{

enum class ConnectionState
{
    Disconnected,   // Initial state, no connection attempted
    Connecting,     // Socket opened, waiting for RoomInfo
    WaitingForSlot, // RoomInfo received, waiting for slot response
    Connected,      // Fully connected
    Refused         // Connection refused by server
};

struct HandshakeConfig
{
    int pollsUntilRoomInfo = 1;     // Polls before RoomInfo arrives
    int pollsUntilSlotResponse = 1; // Polls after RoomInfo before Connected/Refused
    bool shouldRefuseConnection = false;
    std::vector<std::string> refusalReasons;
};

struct QueuedItem
{
    int64_t itemId;
    int64_t locationId = 0;
    int playerSlot = 1;
    unsigned flags = 0;
    int index = 0;
};

struct SentScoutRequest
{
    std::list<int64_t> locations;
    int createAsHint;
};

/**
 * Comprehensive mock implementation of ISocket for testing.
 *
 * Simulates the full Archipelago connection handshake, tracks all outbound
 * messages, and allows configuration of server responses for testing.
 */
class MockArchipelagoSocket : public ISocket
{
  public:
    MockArchipelagoSocket() = default;
    ~MockArchipelagoSocket() override = default;

    // === ISocket Interface Implementation ===

    void connect(const std::string &server, const std::string &slot, const std::string &password) override;
    void disconnect() override;
    bool isConnected() const override;

    void sendLocation(int64_t locationID) override;
    void gameFinished() override;
    void poll() override;
    void processMainThreadTasks() override;

    std::string getItemName(int64_t id, int player) const override;
    std::string getItemDesc(int player) const override;
    std::string getConnectionInfo() const override;
    std::string getUUID() const override;
    std::string getStatus() const override;

    bool scoutLocations(const std::list<int64_t> &locations, int createAsHint) override;
    std::vector<ScoutedItem> scoutLocationsSync(const std::list<int64_t> &locations, int createAsHint = 0,
                                                std::chrono::milliseconds timeout = std::chrono::seconds(5)) override;
    int getPlayerSlot() const override;

    // === Connection Configuration ===

    void setHandshakeConfig(const HandshakeConfig &config);
    void setRefuseConnection(bool refuse, const std::vector<std::string> &reasons = {});

    // === State Inspection ===

    ConnectionState getConnectionState() const;
    const std::string &getConnectedServer() const;
    const std::string &getConnectedSlot() const;
    int getPollCount() const;

    // === Message Tracking (Test Assertions) ===

    bool wasLocationSent(int64_t locationId) const;
    size_t getSentLocationCount() const;
    std::vector<int64_t> getSentLocationsInOrder() const;

    bool hasStatusUpdate(int status) const;
    int getLastStatusUpdate() const;
    const std::vector<int> &getStatusUpdates() const;

    bool wasGameFinishedCalled() const;

    size_t getScoutRequestCount() const;
    const SentScoutRequest &getScoutRequest(size_t index) const;

    // === Item Delivery Simulation ===

    using ItemReceivedCallback = std::function<void(const QueuedItem &)>;

    void queueItemToReceive(int64_t itemId, int player = 1, unsigned flags = 0);
    void queueItemsToReceive(const std::vector<QueuedItem> &items);
    void setItemReceivedCallback(ItemReceivedCallback callback);

    // === Scout Response Simulation ===

    void setScoutResponse(const std::list<int64_t> &forLocations, const std::vector<ScoutedItem> &response);
    void setDefaultScoutResponse(const std::vector<ScoutedItem> &response);
    void setScoutTimeout(const std::list<int64_t> &forLocations);

    // === Error Simulation ===

    void scheduleDisconnect(int afterPolls);

    // === Player/Session Configuration ===

    void setPlayerSlot(int slot);
    void setUUID(const std::string &uuid);
    void setItemName(int64_t id, int player, const std::string &name);
    void setItemDesc(int player, const std::string &desc);

    // === Compatibility (for tests migrating from MockISocket) ===

    void setConnected(bool connected);
    void clearSentLocations();

    // === Reset ===

    void reset();

  private:
    void advanceHandshake();
    void deliverPendingItems();

    // Connection state machine
    ConnectionState state_ = ConnectionState::Disconnected;
    int pollCount_ = 0;
    int pollCountAtRoomInfo_ = 0;
    HandshakeConfig handshakeConfig_;

    // Connection info
    std::string server_;
    std::string slot_;
    std::string password_;
    std::string uuid_ = "test-uuid-12345";
    std::string currentStatus_ = "Not connected";

    // Player info
    int playerSlot_ = 1;

    // Item names: (id, player) -> name
    std::map<std::pair<int64_t, int>, std::string> itemNames_;
    // Item descriptions: player -> desc
    std::map<int, std::string> itemDescs_;

    // Message tracking
    std::vector<int64_t> sentLocations_;
    std::vector<int> statusUpdates_;
    std::vector<SentScoutRequest> scoutRequests_;
    bool gameFinishedCalled_ = false;

    // Item delivery
    std::queue<QueuedItem> pendingItems_;
    std::queue<std::function<void()>> mainThreadTasks_;
    ItemReceivedCallback itemReceivedCallback_;
    int nextItemIndex_ = 0;

    // Scout responses: locations -> response
    std::map<std::list<int64_t>, std::vector<ScoutedItem>> scoutResponses_;
    std::vector<ScoutedItem> defaultScoutResponse_;
    std::set<std::list<int64_t>> scoutTimeouts_;

    // Error simulation
    int disconnectAfterPolls_ = -1;
};

} // namespace mock
