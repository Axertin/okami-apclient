#include "checks.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <algorithm>
#include "mock_socket.hpp"
#include "okami/okami.hpp"

TEST_CASE("checkBrushes detects and sends newly set brush flag", "[check]")
{
    MockSocket mock;
    ISocket &MockSock = mock;

    uint8_t MockMemory[4] = {0};
    uint8_t MockUsable[4] = {0};

    okami::AmmyObtainedBrushTechniques.bind<4>(reinterpret_cast<uintptr_t>(MockMemory));
    okami::AmmyUsableBrushTechniques.bind<4>(reinterpret_cast<uintptr_t>(MockUsable));

    checkBrushes(MockSock);

    REQUIRE(mock.sentLocations.empty());

    MockMemory[1] = 0x10; // turn on power_slash flag
    MockUsable[1] = 0x10; // turn on power_slash flag

    checkBrushes(MockSock);

    REQUIRE(mock.sentLocations.size() == 1);
    REQUIRE(mock.sentLocations[0] == (0x100 + static_cast<int>(okami::BrushOverlay::power_slash)));
    REQUIRE(MockUsable[1] == 0);
}
