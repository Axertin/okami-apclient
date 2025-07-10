#include <catch2/catch_test_macros.hpp>

#include "archipelagosocket.h"

TEST_CASE("socket connection validation")
{
    ArchipelagoSocket &socket = ArchipelagoSocket::instance();

    // Test input validation - these don't hit the network
    socket.connect("", "player", "pass");
    REQUIRE_FALSE(socket.isConnected());
    REQUIRE(socket.getStatus() == "Server address cannot be empty");

    socket.connect("localhost:38281", "", "pass");
    REQUIRE_FALSE(socket.isConnected());
    REQUIRE(socket.getStatus() == "Slot name cannot be empty");

    // This will fail to connect, but should get past validation
    socket.connect("localhost:38281", "player", "pass");
    REQUIRE_FALSE(socket.isConnected());            // Will fail to connect, but that's expected
    REQUIRE(socket.getStatus() == "Connecting..."); // Should at least attempt
}
