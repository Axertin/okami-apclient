#include <catch2/catch_test_macros.hpp>

#include "containermanager.h"
#include "mocks/mock_archipelagosocket.h"

// Pure function tests for the new index-based location ID system

TEST_CASE("container_manager::getContainerLocationId", "[containermanager]")
{
    SECTION("Calculates correct location ID with formula: BASE + (levelId << 8) + spawnIdx")
    {
        // Base case: level 0, index 0
        CHECK(container_manager::getContainerLocationId(0, 0) == 900000);

        // Level 6, index 0
        CHECK(container_manager::getContainerLocationId(6, 0) == 900000 + (6 << 8));

        // Level 6, index 5
        CHECK(container_manager::getContainerLocationId(6, 5) == 900000 + (6 << 8) + 5);

        // Max observed level (787), max index (127)
        CHECK(container_manager::getContainerLocationId(787, 127) == 900000 + (787 << 8) + 127);
    }

    SECTION("Different levels produce non-overlapping ID ranges")
    {
        // Level 6 max should be less than level 7 min
        int64_t level6Max = container_manager::getContainerLocationId(6, 127);
        int64_t level7Min = container_manager::getContainerLocationId(7, 0);
        CHECK(level6Max < level7Min);
    }

    SECTION("Is constexpr")
    {
        static_assert(container_manager::getContainerLocationId(6, 5) == 900000 + (6 << 8) + 5);
    }
}

TEST_CASE("container_manager::isContainerInRando", "[containermanager]")
{
    mock::MockArchipelagoSocket mockSocket;
    container_manager::setSocket(&mockSocket);

    SECTION("Returns true when connected")
    {
        mockSocket.setConnected(true);

        CHECK(container_manager::isContainerInRando(900000) == true);
        CHECK(container_manager::isContainerInRando(901536) == true);
        CHECK(container_manager::isContainerInRando(999999) == true);
    }

    SECTION("Returns false when disconnected")
    {
        mockSocket.setConnected(false);

        CHECK(container_manager::isContainerInRando(900000) == false);
        CHECK(container_manager::isContainerInRando(901536) == false);
        CHECK(container_manager::isContainerInRando(999999) == false);
    }

    container_manager::setSocket(nullptr);
}
