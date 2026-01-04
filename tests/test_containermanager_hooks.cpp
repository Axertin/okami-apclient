#include <cstring>

#include <catch2/catch_test_macros.hpp>

#include "containermanager.h"
#include "isocket.h"
#include "mocks/mock_archipelagosocket.h"
#include "mocks/mock_spawntable.h"
#include "mocks/wolf_framework.hpp"

// Type alias for hook signature (matches containermanager.cpp)
using SpawnTablePopulatorFn = void (*)(void *);

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

} // namespace

TEST_CASE("SpawnTablePopulator hook replaces container item_id with dummy", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    // Setup mock socket
    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    mockSocket.setPlayerSlot(1);
    container_manager::setSocket(&mockSocket);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    // Build spawn table with container at index 0
    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F); // Container with Holy Bone S (0x8F)
    auto &table = builder.build();

    // Trigger the hook
    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Container's spawn_data->item_id should be replaced with dummy
    REQUIRE(table.entries[0].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
}

TEST_CASE("SpawnTablePopulator hook only modifies container entries (spawn_type_1 == 1)", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    container_manager::setSocket(&mockSocket);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F);
    auto &table = builder.build();

    // Manually add a non-container entry (spawn_type_1 != 1)
    okami::ContainerData nonContainerData{};
    nonContainerData.item_id = 0x5A;
    table.entries[1].spawn_data = &nonContainerData;
    table.entries[1].flags = 1;
    table.entries[1].spawn_type_1 = 0; // Not a container

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Container entry should be modified
    CHECK(table.entries[0].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
    // Non-container entry should NOT be modified
    CHECK(nonContainerData.item_id == 0x5A);
}

TEST_CASE("SpawnTablePopulator hook processes multiple containers", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    container_manager::setSocket(&mockSocket);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F);  // Holy Bone S
    builder.addContainer(5, 0x06);  // Some other item
    builder.addContainer(12, 0x22); // Another item
    auto &table = builder.build();

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // All container entries should be replaced with dummy
    CHECK(table.entries[0].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
    CHECK(table.entries[5].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
    CHECK(table.entries[12].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
}

TEST_CASE("SpawnTablePopulator hook handles no socket gracefully", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    // No socket set
    container_manager::setSocket(nullptr);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F);
    auto &table = builder.build();

    // Should not crash - items should still be replaced
    // (socket is only needed for sending locations on pickup)
    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    CHECK(table.entries[0].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
}

TEST_CASE("SpawnTablePopulator hook skips disabled entries", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    container_manager::setSocket(&mockSocket);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F, 0); // Disabled (flags = 0)
    auto &table = builder.build();

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Item should NOT be replaced since entry is disabled
    CHECK(table.entries[0].spawn_data->item_id == 0x8F);
}

TEST_CASE("SpawnTablePopulator hook clears tracking on level change", "[containermanager][hooks]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    container_manager::setSocket(&mockSocket);

    REQUIRE(container_manager::installHooks());

    // First level
    setCurrentMapId(6);
    mock::SpawnTableBuilder builder1;
    builder1.addContainer(0, 0x8F);
    auto &table1 = builder1.build();
    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table1);

    // Second level - should process fresh
    setCurrentMapId(10);
    mock::SpawnTableBuilder builder2;
    builder2.addContainer(0, 0x06);
    auto &table2 = builder2.build();
    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table2);

    // Both levels should have their items replaced
    CHECK(table1.entries[0].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
    CHECK(table2.entries[0].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
}

// ============================================================================
// Pickup Detection Tests
// ============================================================================

TEST_CASE("pollForPickups sends location when item is collected (spawn_type_1 == 0)", "[containermanager][hooks][pickup]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    container_manager::setSocket(&mockSocket);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    // Build spawn table with container (spawn_type_1 = 1)
    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F);
    auto &table = builder.build();

    // Trigger spawn hook to register the container
    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);
    CHECK(table.entries[0].spawn_data->item_id == container_manager::DUMMY_ITEM_ID);
    CHECK(table.entries[0].spawn_type_1 == 1); // Still closed

    // Verify no locations sent yet
    CHECK(mockSocket.getSentLocationCount() == 0);

    // Simulate item collected: spawn_type_1 goes from 1 -> 0
    table.entries[0].spawn_type_1 = 0;

    // Put the table in mock memory so pollForPickups can access it
    if (wolf::mock::mockMemory.size() < container_manager::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable))
    {
        wolf::mock::mockMemory.resize(container_manager::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable));
    }
    std::memcpy(&wolf::mock::mockMemory[container_manager::SPAWN_TABLE_OFFSET], &table, sizeof(okami::SpawnTable));

    // Poll for pickups
    container_manager::pollForPickups();

    // Verify location was sent
    auto sentLocations = mockSocket.getSentLocationsInOrder();
    REQUIRE(sentLocations.size() == 1);
    CHECK(sentLocations[0] == container_manager::getContainerLocationId(6, 0));
}

TEST_CASE("pollForPickups ignores chest-opened state (spawn_type_1 == 3)", "[containermanager][hooks][pickup]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    container_manager::setSocket(&mockSocket);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F);
    auto &table = builder.build();

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Simulate chest opened but item floating (state 3)
    // Player might abandon and let it timeout -> reverts to state 1
    table.entries[0].spawn_type_1 = 3;

    if (wolf::mock::mockMemory.size() < container_manager::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable))
    {
        wolf::mock::mockMemory.resize(container_manager::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable));
    }
    std::memcpy(&wolf::mock::mockMemory[container_manager::SPAWN_TABLE_OFFSET], &table, sizeof(okami::SpawnTable));

    container_manager::pollForPickups();

    // No location should be sent - item not yet collected
    CHECK(mockSocket.getSentLocationCount() == 0);
}

TEST_CASE("pollForPickups does not send duplicate locations", "[containermanager][hooks][pickup]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    container_manager::setSocket(&mockSocket);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F);
    auto &table = builder.build();

    wolf::mock::triggerHook<SpawnTablePopulatorFn>(container_manager::SPAWN_TABLE_POPULATOR_OFFSET, &table);

    // Simulate item collected (spawn_type_1 = 0)
    table.entries[0].spawn_type_1 = 0;

    if (wolf::mock::mockMemory.size() < container_manager::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable))
    {
        wolf::mock::mockMemory.resize(container_manager::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable));
    }
    std::memcpy(&wolf::mock::mockMemory[container_manager::SPAWN_TABLE_OFFSET], &table, sizeof(okami::SpawnTable));

    // Poll multiple times
    container_manager::pollForPickups();
    container_manager::pollForPickups();
    container_manager::pollForPickups();

    // Should only have sent once (container removed from tracking after first detection)
    CHECK(mockSocket.getSentLocationCount() == 1);
}

TEST_CASE("pollForPickups skips untracked containers", "[containermanager][hooks][pickup]")
{
    wolf::mock::reset();
    container_manager::shutdown();

    mock::MockArchipelagoSocket mockSocket;
    mockSocket.setConnected(true);
    container_manager::setSocket(&mockSocket);

    setCurrentMapId(6);
    REQUIRE(container_manager::installHooks());

    // Build spawn table but DON'T trigger the spawn hook (so containers aren't tracked)
    mock::SpawnTableBuilder builder;
    builder.addContainer(0, 0x8F);
    auto &table = builder.build();
    table.entries[0].spawn_type_1 = 0; // Already "collected"

    if (wolf::mock::mockMemory.size() < container_manager::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable))
    {
        wolf::mock::mockMemory.resize(container_manager::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable));
    }
    std::memcpy(&wolf::mock::mockMemory[container_manager::SPAWN_TABLE_OFFSET], &table, sizeof(okami::SpawnTable));

    container_manager::pollForPickups();

    // No locations should be sent - container wasn't tracked
    CHECK(mockSocket.getSentLocationCount() == 0);
}
