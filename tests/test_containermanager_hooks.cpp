#include <catch2/catch_test_macros.hpp>

#include <filesystem>

#include "containermanager.h"
#include "isocket.h"
#include "mocks/mock_archipelagosocket.h"
#include "mocks/mock_spawntable.h"
#include "mocks/wolf_framework.hpp"

// Type alias for hook signature (matches containermanager.cpp)
using SpawnTablePopulatorFn = void (*)(void *);

// Test containers from tests/data/test_containers.json:
// Container 0: level 6, pos (100, 50, -30), contents 51
// Container 1: level 6, pos (200, 60, -40), contents 42
// Container 2: level 10, pos (300, 70, -50), contents 33

namespace
{

// Helper to set current map ID in mock memory
void setCurrentMapId(uint16_t mapId)
{
    // Ensure mock memory is large enough
    if (wolf::mock::mockMemory.size() < container_manager::CURRENT_MAP_ID_OFFSET + sizeof(uint16_t))
    {
        wolf::mock::mockMemory.resize(container_manager::CURRENT_MAP_ID_OFFSET + sizeof(uint16_t));
    }
    auto *ptr = reinterpret_cast<uint16_t *>(&wolf::mock::mockMemory[container_manager::CURRENT_MAP_ID_OFFSET]);
    *ptr = mapId;
}

// Helper to create a scouted item response
ScoutedItem makeScoutedItem(int64_t itemId, int64_t locationId, int player, unsigned flags = 0)
{
    return ScoutedItem{.item = itemId, .location = locationId, .player = player, .flags = flags};
}

} // namespace

TEST_CASE("SpawnTablePopulator hook replaces local items with game item IDs", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    // Setup mock socket
    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    mockSocket.setPlayerSlot(1);

    // Level 6 has containers 0 and 1, so hook scouts both
    // Scout response: local item (player 1) with AP item ID that maps to a game item
    int64_t loc0 = container_manager::getLocationId(0);
    int64_t loc1 = container_manager::getLocationId(1);
    mockSocket.setScoutResponse({loc0, loc1}, {makeScoutedItem(1001, loc0, 1), makeScoutedItem(1002, loc1, 1)});

    container_manager::setSocket(&mockSocket);

    // Load test containers and set up map ID
    REQUIRE(container_manager::loadContainersFromPath(
        std::filesystem::path(__FILE__).parent_path() / "data" / "test_containers.json"));
    setCurrentMapId(6);

    // Install hooks (without JSON loading)
    REQUIRE(container_manager::installHooks());

    // Build mock spawn table with container at the test position
    mock::SpawnTableBuilder builder;
    builder.addEntry(0, 100, 50, -30, 0x33); // Original item 0x33
    auto &table = builder.build();

    // Trigger the hook
    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Verify item was replaced with the mapped game item
    // AP item 1001 should map to a valid game item ID
    // (The exact ID depends on apitem.cpp mapping, but it should NOT be the dummy)
    REQUIRE(table.entries[0].spawn_data->item_id != 0x33); // Changed from original
}

TEST_CASE("SpawnTablePopulator hook uses dummy for external items", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    // Setup mock socket - we are player 1
    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    mockSocket.setPlayerSlot(1);

    // Level 6 has containers 0 and 1
    // Scout response: container 0 is EXTERNAL (player 2, not us)
    int64_t loc0 = container_manager::getLocationId(0);
    int64_t loc1 = container_manager::getLocationId(1);
    mockSocket.setScoutResponse({loc0, loc1},
                                {makeScoutedItem(1001, loc0, 2),    // External - player 2
                                 makeScoutedItem(1002, loc1, 1)});  // Local - player 1

    container_manager::setSocket(&mockSocket);

    REQUIRE(container_manager::loadContainersFromPath(
        std::filesystem::path(__FILE__).parent_path() / "data" / "test_containers.json"));
    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addEntry(0, 100, 50, -30, 0x33);
    auto &table = builder.build();

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // External items should use the dummy item (Chestnut)
    REQUIRE(table.entries[0].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
}

TEST_CASE("SpawnTablePopulator hook skips untracked containers", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    mockSocket.setPlayerSlot(1);
    container_manager::setSocket(&mockSocket);

    REQUIRE(container_manager::loadContainersFromPath(
        std::filesystem::path(__FILE__).parent_path() / "data" / "test_containers.json"));
    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    // Build spawn table with container at UNTRACKED position
    mock::SpawnTableBuilder builder;
    builder.addEntry(0, 999, 999, 999, 0x33); // Position not in containers.json
    auto &table = builder.build();

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Should remain unchanged
    REQUIRE(table.entries[0].spawn_data->item_id == 0x33);
}

TEST_CASE("SpawnTablePopulator hook handles no socket gracefully", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    // No socket set
    container_manager::setSocket(nullptr);

    REQUIRE(container_manager::loadContainersFromPath(
        std::filesystem::path(__FILE__).parent_path() / "data" / "test_containers.json"));
    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addEntry(0, 100, 50, -30, 0x33);
    auto &table = builder.build();

    // Should not crash, just skip randomization
    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Item unchanged when no socket
    REQUIRE(table.entries[0].spawn_data->item_id == 0x33);
}

TEST_CASE("SpawnTablePopulator hook handles disconnected socket gracefully", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(false); // Disconnected
    container_manager::setSocket(&mockSocket);

    REQUIRE(container_manager::loadContainersFromPath(
        std::filesystem::path(__FILE__).parent_path() / "data" / "test_containers.json"));
    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addEntry(0, 100, 50, -30, 0x33);
    auto &table = builder.build();

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Item unchanged when disconnected
    REQUIRE(table.entries[0].spawn_data->item_id == 0x33);
}

TEST_CASE("SpawnTablePopulator hook only affects containers for current level", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    mockSocket.setPlayerSlot(1);

    // Level 10 only has container 2, so hook will only scout that one
    int64_t loc2 = container_manager::getLocationId(2);
    mockSocket.setScoutResponse({loc2}, {makeScoutedItem(1003, loc2, 1)});

    container_manager::setSocket(&mockSocket);

    REQUIRE(container_manager::loadContainersFromPath(
        std::filesystem::path(__FILE__).parent_path() / "data" / "test_containers.json"));

    // Set current map to level 10 (only container 2 is on this level)
    setCurrentMapId(10);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    // Add container from level 6 (should be ignored since we're on level 10)
    builder.addEntry(0, 100, 50, -30, 0x33);
    // Add container from level 10 (should be processed)
    builder.addEntry(1, 300, 70, -50, 0x44);
    auto &table = builder.build();

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Level 6 container should be unchanged (not tracked for this level)
    REQUIRE(table.entries[0].spawn_data->item_id == 0x33);
    // Level 10 container should be modified
    REQUIRE(table.entries[1].spawn_data->item_id != 0x44);
}
