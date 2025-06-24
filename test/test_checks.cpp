#include "checks.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <algorithm>
#include "mock_socket.hpp"
#include "okami/memorymap.hpp"
#include "okami/items.hpp"

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

TEST_CASE("checkBrushes sends the correct location when given an index", "[check]")
{
    MockSocket mock;
    ISocket &MockSock = mock;

    enableChecks();

    checkBrushes(0x3, MockSock);

    REQUIRE(mock.sentLocations.size() == 1);
    REQUIRE(mock.sentLocations[0] == 0x103);
    mock.sentLocations.clear();

    disableChecks();
}
