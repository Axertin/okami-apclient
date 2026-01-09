#include <catch2/catch_test_macros.hpp>

#include "checks/check_types.hpp"
#include "checks/containers.hpp"
#include "mock_archipelagosocket.h"
#include "mock_spawntable.h"
#include "wolf_framework.hpp"

// ============================================================================
// Container ID formula tests
// ============================================================================

TEST_CASE("Container check ID calculation", "[containers][check_types]")
{
    // Container check = kContainerBase + (levelId << 8) + spawnIdx
    int64_t checkId = checks::getContainerCheckId(0x1234, 5);
    REQUIRE(checkId == (checks::kContainerBase + (0x1234 << 8) + 5));
}

TEST_CASE("Container check ID components can be extracted", "[containers][check_types]")
{
    int64_t checkId = checks::getContainerCheckId(0xABCD, 42);

    // Verify we can decode the level ID and spawn index
    uint16_t levelId = static_cast<uint16_t>((checkId - checks::kContainerBase) >> 8);
    int spawnIdx = static_cast<int>((checkId - checks::kContainerBase) & 0xFF);

    REQUIRE(levelId == 0xABCD);
    REQUIRE(spawnIdx == 42);
}

TEST_CASE("Container check ID ranges don't overlap", "[containers][check_types]")
{
    // Level N's max check ID should be less than level N+1's min check ID
    // Max spawn index is 127 (128 entries, 0-indexed)
    int64_t level5Max = checks::getContainerCheckId(5, 127);
    int64_t level6Min = checks::getContainerCheckId(6, 0);
    REQUIRE(level5Max < level6Min);

    // Same for larger level IDs
    int64_t levelFFMax = checks::getContainerCheckId(0xFF, 127);
    int64_t level100Min = checks::getContainerCheckId(0x100, 0);
    REQUIRE(levelFFMax < level100Min);
}

// ============================================================================
// Spawn table hook tests
// ============================================================================

class ContainerManFixture
{
  protected:
    mock::MockArchipelagoSocket socket_;
    std::vector<int64_t> receivedCheckIds_;
    std::unique_ptr<checks::ContainerMan> containerMan_;
    mock::SpawnTableBuilder tableBuilder_;

    void SetUp()
    {
        wolf::mock::reset();
        receivedCheckIds_.clear();

        // Reserve enough mock memory for spawn table and map ID
        // SPAWN_TABLE_OFFSET is 0xB66800, we need that plus sizeof(SpawnTable)
        wolf::mock::reserveMemory(checks::SPAWN_TABLE_OFFSET + sizeof(okami::SpawnTable) + checks::CURRENT_MAP_ID_OFFSET + sizeof(uint16_t));

        containerMan_ = std::make_unique<checks::ContainerMan>(socket_, [this](int64_t checkId) { receivedCheckIds_.push_back(checkId); });
    }

    void TearDown()
    {
        containerMan_.reset();
        wolf::mock::reset();
    }

    void setCurrentMapId(uint16_t mapId)
    {
        // Set map ID in mock memory at the expected offset
        auto *mapIdPtr = reinterpret_cast<uint16_t *>(&wolf::mock::mockMemory[checks::CURRENT_MAP_ID_OFFSET]);
        *mapIdPtr = mapId;
    }

    void triggerSpawnTableHook(okami::SpawnTable *table)
    {
        // Trigger the registered hook with the spawn table
        using SpawnTablePopulatorFn = void (*)(void *);
        wolf::mock::triggerHook<SpawnTablePopulatorFn>(checks::SPAWN_TABLE_POPULATOR_OFFSET, table);
    }

    void copyTableToMockMemory(okami::SpawnTable &table)
    {
        // Copy the spawn table to mock memory at SPAWN_TABLE_OFFSET for poll() to read
        auto *destTable = reinterpret_cast<okami::SpawnTable *>(&wolf::mock::mockMemory[checks::SPAWN_TABLE_OFFSET]);
        std::memcpy(destTable, &table, sizeof(okami::SpawnTable));
    }

    okami::SpawnTable *getTableInMockMemory()
    {
        return reinterpret_cast<okami::SpawnTable *>(&wolf::mock::mockMemory[checks::SPAWN_TABLE_OFFSET]);
    }
};

TEST_CASE_METHOD(ContainerManFixture, "Hook replaces container items with dummy", "[containers][hooks]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(0, 0x42); // Item at index 0
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);

    // Verify item was replaced with dummy
    REQUIRE(table.entries[0].spawn_data->item_id == checks::DUMMY_ITEM_ID);

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "Hook only modifies containers (spawn_type_1 == 1)", "[containers][hooks]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(0, 0x42); // Container at index 0
    okami::SpawnTable &table = tableBuilder_.build();

    // Change entry 1 to non-container type
    table.entries[1].flags = 1;
    table.entries[1].spawn_type_1 = 2; // Not a container
    okami::ContainerData nonContainerData{};
    nonContainerData.item_id = 0x99;
    table.entries[1].spawn_data = &nonContainerData;

    containerMan_->initialize();
    triggerSpawnTableHook(&table);

    // Container should be replaced
    REQUIRE(table.entries[0].spawn_data->item_id == checks::DUMMY_ITEM_ID);
    // Non-container should be untouched
    REQUIRE(table.entries[1].spawn_data->item_id == 0x99);

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "Hook processes multiple containers", "[containers][hooks]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(0, 0x10).addContainer(5, 0x20).addContainer(12, 0x30);
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);

    // All containers should be replaced
    REQUIRE(table.entries[0].spawn_data->item_id == checks::DUMMY_ITEM_ID);
    REQUIRE(table.entries[5].spawn_data->item_id == checks::DUMMY_ITEM_ID);
    REQUIRE(table.entries[12].spawn_data->item_id == checks::DUMMY_ITEM_ID);

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "Hook skips containers when socket disconnected", "[containers][hooks]")
{
    SetUp();
    socket_.setConnected(false); // Disconnected
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(0, 0x42);
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);

    // Item should NOT be replaced (isContainerInRando returns false when disconnected)
    REQUIRE(table.entries[0].spawn_data->item_id == 0x42);

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "Hook skips disabled entries (flags & 1 == 0)", "[containers][hooks]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(0, 0x42, 0); // flags = 0 (disabled)
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);

    // Disabled entry should NOT be replaced
    REQUIRE(table.entries[0].spawn_data->item_id == 0x42);

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "Hook skips entries with null spawn_data", "[containers][hooks]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    okami::SpawnTable table{};
    table.entries[0].flags = 1;
    table.entries[0].spawn_type_1 = 1;
    table.entries[0].spawn_data = nullptr; // No spawn data

    containerMan_->initialize();
    triggerSpawnTableHook(&table);

    // Should not crash, entry should be skipped
    REQUIRE(true);

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "Hook clears tracking on level change", "[containers][hooks]")
{
    SetUp();
    socket_.setConnected(true);

    // First level
    setCurrentMapId(0x0006);
    tableBuilder_.addContainer(0, 0x10);
    okami::SpawnTable &table1 = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table1);
    REQUIRE(table1.entries[0].spawn_data->item_id == checks::DUMMY_ITEM_ID);

    // Second level - new spawn table
    tableBuilder_.reset();
    setCurrentMapId(0x000A);
    tableBuilder_.addContainer(3, 0x20);
    okami::SpawnTable &table2 = tableBuilder_.build();

    triggerSpawnTableHook(&table2);
    REQUIRE(table2.entries[3].spawn_data->item_id == checks::DUMMY_ITEM_ID);

    TearDown();
}

// ============================================================================
// Pickup detection tests (poll)
// ============================================================================

TEST_CASE_METHOD(ContainerManFixture, "poll sends check when item collected", "[containers][hooks][pickup]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(5, 0x42);
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);

    // Copy table to mock memory for poll() to read
    copyTableToMockMemory(table);

    // Simulate item collection: spawn_type_1 changes from 1 to 0
    okami::SpawnTable *mockTable = getTableInMockMemory();
    mockTable->entries[5].spawn_type_1 = 0;

    containerMan_->poll();

    REQUIRE(receivedCheckIds_.size() == 1);
    REQUIRE(receivedCheckIds_[0] == checks::getContainerCheckId(0x0006, 5));

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "poll ignores chest-opened state", "[containers][hooks][pickup]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(5, 0x42);
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);
    copyTableToMockMemory(table);

    // Chest opened but item not yet collected (state 3)
    okami::SpawnTable *mockTable = getTableInMockMemory();
    mockTable->entries[5].spawn_type_1 = 3;

    containerMan_->poll();

    // No callback yet - item still floating
    REQUIRE(receivedCheckIds_.empty());

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "poll does not send duplicate checks", "[containers][hooks][pickup]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(5, 0x42);
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);
    copyTableToMockMemory(table);

    // Collect the item
    okami::SpawnTable *mockTable = getTableInMockMemory();
    mockTable->entries[5].spawn_type_1 = 0;

    containerMan_->poll();
    containerMan_->poll();
    containerMan_->poll();

    // Callback should only be invoked once
    REQUIRE(receivedCheckIds_.size() == 1);

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "poll skips untracked containers", "[containers][hooks][pickup]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    // Don't trigger the spawn hook - container won't be tracked
    okami::SpawnTable table{};
    table.entries[5].spawn_type_1 = 0; // Already collected
    copyTableToMockMemory(table);

    containerMan_->initialize();
    containerMan_->poll();

    // No callback - container wasn't tracked
    REQUIRE(receivedCheckIds_.empty());

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "poll skips when socket disconnected", "[containers][hooks][pickup]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(5, 0x42);
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);
    copyTableToMockMemory(table);

    // Disconnect after tracking
    socket_.setConnected(false);

    okami::SpawnTable *mockTable = getTableInMockMemory();
    mockTable->entries[5].spawn_type_1 = 0;

    containerMan_->poll();

    // No callback - socket disconnected
    REQUIRE(receivedCheckIds_.empty());

    TearDown();
}

TEST_CASE_METHOD(ContainerManFixture, "poll handles multiple container pickups", "[containers][hooks][pickup]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(0, 0x10).addContainer(5, 0x20).addContainer(12, 0x30);
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);
    copyTableToMockMemory(table);

    okami::SpawnTable *mockTable = getTableInMockMemory();

    // Collect first container
    mockTable->entries[0].spawn_type_1 = 0;
    containerMan_->poll();
    REQUIRE(receivedCheckIds_.size() == 1);
    REQUIRE(receivedCheckIds_[0] == checks::getContainerCheckId(0x0006, 0));

    // Collect second container
    mockTable->entries[5].spawn_type_1 = 0;
    containerMan_->poll();
    REQUIRE(receivedCheckIds_.size() == 2);
    REQUIRE(receivedCheckIds_[1] == checks::getContainerCheckId(0x0006, 5));

    // Collect third container
    mockTable->entries[12].spawn_type_1 = 0;
    containerMan_->poll();
    REQUIRE(receivedCheckIds_.size() == 3);
    REQUIRE(receivedCheckIds_[2] == checks::getContainerCheckId(0x0006, 12));

    TearDown();
}

// ============================================================================
// Reset tests
// ============================================================================

TEST_CASE_METHOD(ContainerManFixture, "reset clears tracked containers", "[containers]")
{
    SetUp();
    socket_.setConnected(true);
    setCurrentMapId(0x0006);

    tableBuilder_.addContainer(5, 0x42);
    okami::SpawnTable &table = tableBuilder_.build();

    containerMan_->initialize();
    triggerSpawnTableHook(&table);
    copyTableToMockMemory(table);

    // Reset clears tracking
    containerMan_->reset();

    // Container collection should not trigger callback
    okami::SpawnTable *mockTable = getTableInMockMemory();
    mockTable->entries[5].spawn_type_1 = 0;
    containerMan_->poll();

    REQUIRE(receivedCheckIds_.empty());

    TearDown();
}
