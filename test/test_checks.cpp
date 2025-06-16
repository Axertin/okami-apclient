#include "checks.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <algorithm>
#include "mock_socket.hpp"
#include "okami/okami.hpp"
#include "okami/items.hpp"

TEST_CASE("checkBrushes detects and sends newly set brush flag", "[check]")
{
    MockSocket mock;
    ISocket &MockSock = mock;

    uint8_t MockMemory[4] = {0};
    uint8_t MockUsable[4] = {0};

    okami::AmmyObtainedBrushTechniques.bind<4>(reinterpret_cast<uintptr_t>(MockMemory));
    okami::AmmyUsableBrushTechniques.bind<4>(reinterpret_cast<uintptr_t>(MockUsable));

    enableChecks();

    checkBrushes(MockSock);

    REQUIRE(mock.sentLocations.empty());

    MockMemory[1] = 0x10; // turn on power_slash flag
    MockUsable[1] = 0x10; // turn on power_slash flag

    checkBrushes(MockSock);

    REQUIRE(mock.sentLocations.size() == 1);
    REQUIRE(mock.sentLocations[0] == (0x100 + static_cast<int>(okami::BrushOverlay::power_slash)));
    REQUIRE(MockUsable[1] == 0);

    disableChecks();
}

TEST_CASE("checkItems detects and sends item checks", "[check]")
{
    MockSocket mock;
    ISocket &MockSock = mock;

    enableChecks();

    SECTION("Weapon (Infinity Judge)")
    {
        checkItems(0x12, MockSock);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 0x12);
        mock.sentLocations.clear();
    }

    SECTION("Artifact (Peace Bell)")
    {
        checkItems(0xB, MockSock);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 0xB);
        mock.sentLocations.clear();
    }

    SECTION("KeyItem (Astral Pouch)")
    {
        checkItems(0x6, MockSock);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 0x6);
        mock.sentLocations.clear();
    }

    SECTION("KT (Karmic Transformer 3)")
    {
        checkItems(0x79, MockSock);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 0x79);
        mock.sentLocations.clear();
    }

    SECTION("Treasure (Kutani Pottery)")
    {
        checkItems(0xA1, MockSock);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 0xA1);
        mock.sentLocations.clear();
    }

    SECTION("Map (Tsuta Ruins Map)")
    {
        checkItems(0xF8, MockSock);

        REQUIRE(mock.sentLocations.size() == 1);
        REQUIRE(mock.sentLocations[0] == 0xF8);
        mock.sentLocations.clear();
    }

    disableChecks();
}
