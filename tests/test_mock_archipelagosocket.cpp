#include <catch2/catch_test_macros.hpp>

#include "mock_archipelagosocket.h"

using namespace mock;

TEST_CASE("MockArchipelagoSocket initial state", "[mock_socket]")
{
    MockArchipelagoSocket socket;

    CHECK(socket.getConnectionState() == ConnectionState::Disconnected);
    CHECK_FALSE(socket.isConnected());
    CHECK(socket.getPlayerSlot() == -1);
    CHECK(socket.getStatus() == "Not connected");
}

TEST_CASE("MockArchipelagoSocket connection handshake succeeds", "[mock_socket]")
{
    MockArchipelagoSocket socket;

    socket.connect("localhost:38281", "TestPlayer", "password123");

    CHECK(socket.getConnectionState() == ConnectionState::Connecting);
    CHECK_FALSE(socket.isConnected());
    CHECK(socket.getConnectedServer() == "localhost:38281");
    CHECK(socket.getConnectedSlot() == "TestPlayer");

    // First poll: RoomInfo arrives
    socket.poll();
    CHECK(socket.getConnectionState() == ConnectionState::WaitingForSlot);
    CHECK_FALSE(socket.isConnected());

    // Second poll: Connected
    socket.poll();
    CHECK(socket.getConnectionState() == ConnectionState::Connected);
    CHECK(socket.isConnected());
    CHECK(socket.getPlayerSlot() == 1);

    // Verify PLAYING status was auto-sent
    CHECK(socket.hasStatusUpdate(10));
}

TEST_CASE("MockArchipelagoSocket connection refused", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    socket.setRefuseConnection(true, {"InvalidPassword"});

    socket.connect("localhost:38281", "TestPlayer", "wrong_password");
    socket.poll();  // RoomInfo
    socket.poll();  // Refused

    CHECK(socket.getConnectionState() == ConnectionState::Refused);
    CHECK_FALSE(socket.isConnected());
    CHECK(socket.getStatus().find("InvalidPassword") != std::string::npos);
}

TEST_CASE("MockArchipelagoSocket configurable handshake timing", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config;
    config.pollsUntilRoomInfo = 3;
    config.pollsUntilSlotResponse = 2;
    socket.setHandshakeConfig(config);

    socket.connect("test", "slot", "");

    // Polls 1-2: Still connecting
    socket.poll();
    socket.poll();
    CHECK(socket.getConnectionState() == ConnectionState::Connecting);

    // Poll 3: RoomInfo arrives
    socket.poll();
    CHECK(socket.getConnectionState() == ConnectionState::WaitingForSlot);

    // Poll 4: Still waiting
    socket.poll();
    CHECK(socket.getConnectionState() == ConnectionState::WaitingForSlot);

    // Poll 5: Connected
    socket.poll();
    CHECK(socket.isConnected());
}

TEST_CASE("MockArchipelagoSocket instant connection", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config;
    config.pollsUntilRoomInfo = 0;
    config.pollsUntilSlotResponse = 0;
    socket.setHandshakeConfig(config);

    socket.connect("test", "slot", "");
    socket.poll();  // Should be connected immediately

    CHECK(socket.isConnected());
}

TEST_CASE("MockArchipelagoSocket tracks sent locations", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    CHECK(socket.getSentLocationCount() == 0);

    socket.sendLocation(100001);
    socket.sendLocation(100002);
    socket.sendLocation(100001);  // Duplicate (still tracked)

    CHECK(socket.getSentLocationCount() == 3);
    CHECK(socket.wasLocationSent(100001));
    CHECK(socket.wasLocationSent(100002));
    CHECK_FALSE(socket.wasLocationSent(100003));

    auto locations = socket.getSentLocationsInOrder();
    REQUIRE(locations.size() == 3);
    CHECK(locations[0] == 100001);
    CHECK(locations[1] == 100002);
    CHECK(locations[2] == 100001);
}

TEST_CASE("MockArchipelagoSocket ignores locations when not connected", "[mock_socket]")
{
    MockArchipelagoSocket socket;

    socket.sendLocation(100001);

    CHECK(socket.getSentLocationCount() == 0);
}

TEST_CASE("MockArchipelagoSocket gameFinished", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    CHECK_FALSE(socket.wasGameFinishedCalled());

    socket.gameFinished();

    CHECK(socket.wasGameFinishedCalled());
    CHECK(socket.hasStatusUpdate(30));  // GOAL status
}

TEST_CASE("MockArchipelagoSocket item delivery via task queue", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    std::vector<QueuedItem> receivedItems;
    socket.setItemReceivedCallback([&](const QueuedItem &item) { receivedItems.push_back(item); });

    socket.queueItemToReceive(0x100, 1, 0);
    socket.queueItemToReceive(0x101, 2, 1);

    // Items not yet processed
    CHECK(receivedItems.empty());

    // poll() moves items to main thread queue
    socket.poll();
    CHECK(receivedItems.empty());  // Still not processed

    // processMainThreadTasks() delivers items
    socket.processMainThreadTasks();
    REQUIRE(receivedItems.size() == 2);
    CHECK(receivedItems[0].itemId == 0x100);
    CHECK(receivedItems[0].playerSlot == 1);
    CHECK(receivedItems[1].itemId == 0x101);
    CHECK(receivedItems[1].playerSlot == 2);
    CHECK(receivedItems[1].flags == 1);
}

TEST_CASE("MockArchipelagoSocket item index tracking", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    std::vector<int> indices;
    socket.setItemReceivedCallback([&](const QueuedItem &item) { indices.push_back(item.index); });

    socket.queueItemToReceive(0x100);
    socket.queueItemToReceive(0x101);
    socket.queueItemToReceive(0x102);

    socket.poll();
    socket.processMainThreadTasks();

    REQUIRE(indices.size() == 3);
    CHECK(indices[0] == 0);
    CHECK(indices[1] == 1);
    CHECK(indices[2] == 2);
}

TEST_CASE("MockArchipelagoSocket scout responses", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    std::list<int64_t> locations = {300000, 300001};
    std::vector<ScoutedItem> response = {{.item = 0x100, .location = 300000, .player = 1, .flags = 0},
                                          {.item = 0x42, .location = 300001, .player = 2, .flags = 1}};
    socket.setScoutResponse(locations, response);

    auto result = socket.scoutLocationsSync(locations);

    REQUIRE(result.size() == 2);
    CHECK(result[0].item == 0x100);
    CHECK(result[0].location == 300000);
    CHECK(result[1].item == 0x42);
    CHECK(result[1].player == 2);

    CHECK(socket.getScoutRequestCount() == 1);
    CHECK(socket.getScoutRequest(0).locations == locations);
}

TEST_CASE("MockArchipelagoSocket default scout response", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    std::vector<ScoutedItem> defaultResponse = {{.item = 0xFFFF, .location = 0, .player = 1, .flags = 0}};
    socket.setDefaultScoutResponse(defaultResponse);

    // Request for locations that have no specific response configured
    auto result = socket.scoutLocationsSync({999999});

    REQUIRE(result.size() == 1);
    CHECK(result[0].item == 0xFFFF);
}

TEST_CASE("MockArchipelagoSocket scout timeout simulation", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    std::list<int64_t> locations = {400000};
    socket.setScoutTimeout(locations);

    auto result = socket.scoutLocationsSync(locations);

    CHECK(result.empty());
}

TEST_CASE("MockArchipelagoSocket async scoutLocations", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    std::list<int64_t> locations = {500000, 500001};

    bool result = socket.scoutLocations(locations, 1);

    CHECK(result);
    CHECK(socket.getScoutRequestCount() == 1);
    CHECK(socket.getScoutRequest(0).createAsHint == 1);
}

TEST_CASE("MockArchipelagoSocket scheduled disconnect", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    CHECK(socket.isConnected());

    socket.scheduleDisconnect(2);  // Disconnect after 2 more polls

    socket.poll();
    CHECK(socket.isConnected());

    socket.poll();
    CHECK_FALSE(socket.isConnected());
    CHECK(socket.getConnectionState() == ConnectionState::Disconnected);
}

TEST_CASE("MockArchipelagoSocket disconnect method", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    CHECK(socket.isConnected());

    socket.disconnect();

    CHECK_FALSE(socket.isConnected());
    CHECK(socket.getConnectionState() == ConnectionState::Disconnected);
}

TEST_CASE("MockArchipelagoSocket item name configuration", "[mock_socket]")
{
    MockArchipelagoSocket socket;

    socket.setItemName(0x100, 1, "Sunrise");
    socket.setItemName(0x100, 2, "Sunrise (Player 2)");
    socket.setItemName(0x101, 0, "Generic Brush");  // Player 0 = generic

    CHECK(socket.getItemName(0x100, 1) == "Sunrise");
    CHECK(socket.getItemName(0x100, 2) == "Sunrise (Player 2)");
    CHECK(socket.getItemName(0x101, 1) == "Generic Brush");  // Falls back to player 0
    CHECK(socket.getItemName(0x999, 1) == "Unknown Item");   // Not configured
}

TEST_CASE("MockArchipelagoSocket UUID and connection info", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);

    socket.setUUID("custom-uuid-abc");

    CHECK(socket.getUUID() == "custom-uuid-abc");

    // Connection info empty when not connected
    CHECK(socket.getConnectionInfo().empty());

    socket.connect("archipelago.gg:38281", "Ammy", "");
    socket.poll();

    CHECK(socket.getConnectionInfo().find("archipelago.gg:38281") != std::string::npos);
    CHECK(socket.getConnectionInfo().find("Ammy") != std::string::npos);
}

TEST_CASE("MockArchipelagoSocket reset clears all state", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);
    socket.connect("test", "slot", "");
    socket.poll();

    socket.sendLocation(100000);
    socket.queueItemToReceive(0x100);
    socket.gameFinished();

    socket.reset();

    CHECK(socket.getConnectionState() == ConnectionState::Disconnected);
    CHECK(socket.getSentLocationCount() == 0);
    CHECK_FALSE(socket.wasGameFinishedCalled());
    CHECK(socket.getStatusUpdates().empty());
    CHECK(socket.getPollCount() == 0);
}

TEST_CASE("MockArchipelagoSocket player slot configuration", "[mock_socket]")
{
    MockArchipelagoSocket socket;
    HandshakeConfig config{0, 0};
    socket.setHandshakeConfig(config);

    socket.setPlayerSlot(5);
    socket.connect("test", "slot", "");
    socket.poll();

    CHECK(socket.getPlayerSlot() == 5);
}

TEST_CASE("MockArchipelagoSocket poll count tracking", "[mock_socket]")
{
    MockArchipelagoSocket socket;

    CHECK(socket.getPollCount() == 0);

    socket.connect("test", "slot", "");
    socket.poll();
    CHECK(socket.getPollCount() == 1);

    socket.poll();
    CHECK(socket.getPollCount() == 2);

    socket.poll();
    CHECK(socket.getPollCount() == 3);
}
