#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <thread>

#include "mock_archipelagosocket.h"
#include "saveman.h"
#include "wolf_framework.hpp"

#include <okami/offsets.hpp>
#include <okami/savefile.hpp>

// Highest memory offset used by SaveMan: CharacterStats at 0xB4DF90.
// Reserve enough mock memory to cover all game state regions.
static constexpr size_t kMockMemorySize = 0xB60000;

// =============================================================================
// Helpers
// =============================================================================

/// Set up a connected mock socket and initialized SaveMan.
/// Returns the SaveMan. Caller must keep socket alive.
static std::unique_ptr<SaveMan> makeSaveMan(mock::MockArchipelagoSocket &socket)
{
    socket.connect("localhost", "TestSlot", "");
    socket.setConnected(true);

    auto sm = std::make_unique<SaveMan>(socket);
    sm->initialize();
    return sm;
}

/// Remove the test save file if it exists.
static void cleanupSaveFile(SaveMan &sm)
{
    std::string path = sm.getSavePath();
    if (!path.empty())
    {
        std::filesystem::remove(path);
        std::filesystem::remove(path + ".tmp");
    }
}

// =============================================================================
// Checksum tests
// =============================================================================

TEST_CASE("Checksum: zero-filled slot yields seed", "[saveman][checksum]")
{
    okami::SaveSlot slot;
    std::memset(&slot, 0, sizeof(slot));

    uint64_t checksum = SaveMan::computeChecksum(slot);
    CHECK(checksum == 0x9be6fa3b72afda1d);
}

TEST_CASE("Checksum: header magic changes result", "[saveman][checksum]")
{
    okami::SaveSlot slot;
    std::memset(&slot, 0, sizeof(slot));

    uint64_t zeroChecksum = SaveMan::computeChecksum(slot);

    slot.header = 0x40400000;
    uint64_t headerChecksum = SaveMan::computeChecksum(slot);

    CHECK(headerChecksum != zeroChecksum);
    // XOR property: result = seed ^ data[0], so the difference is exactly the header value
    CHECK((zeroChecksum ^ headerChecksum) == 0x0000000040400000ULL);
}

TEST_CASE("Checksum: modifying checksum field does not change result", "[saveman][checksum]")
{
    okami::SaveSlot slot;
    std::memset(&slot, 0, sizeof(slot));
    slot.header = 0x40400000;

    uint64_t cs1 = SaveMan::computeChecksum(slot);

    slot.checksum = 0xDEADBEEFCAFEBABEULL;
    uint64_t cs2 = SaveMan::computeChecksum(slot);

    CHECK(cs1 == cs2);
}

TEST_CASE("Checksum: deterministic — same data always yields same result", "[saveman][checksum]")
{
    okami::SaveSlot slot;
    std::memset(&slot, 0, sizeof(slot));
    slot.header = 0x40400000;
    slot.areaNameStrId = 42;
    slot.timeRTC = 12345678ULL;

    uint64_t cs1 = SaveMan::computeChecksum(slot);
    uint64_t cs2 = SaveMan::computeChecksum(slot);
    CHECK(cs1 == cs2);
}

TEST_CASE("Checksum: modifying any data region changes result", "[saveman][checksum]")
{
    okami::SaveSlot slot;
    std::memset(&slot, 0, sizeof(slot));
    uint64_t baseline = SaveMan::computeChecksum(slot);

    SECTION("character data")
    {
        slot.character.currentHealth = 100;
        CHECK(SaveMan::computeChecksum(slot) != baseline);
    }

    SECTION("tracked data")
    {
        slot.tracked.timePlayed = 9999;
        CHECK(SaveMan::computeChecksum(slot) != baseline);
    }

    SECTION("collection data")
    {
        slot.collection.currentMoney = 50000;
        CHECK(SaveMan::computeChecksum(slot) != baseline);
    }

    SECTION("timeRTC")
    {
        slot.timeRTC = 1;
        CHECK(SaveMan::computeChecksum(slot) != baseline);
    }

    SECTION("areaNameStrId via header qword")
    {
        slot.areaNameStrId = 7;
        CHECK(SaveMan::computeChecksum(slot) != baseline);
    }
}

TEST_CASE("Checksum: XOR self-inverse property", "[saveman][checksum]")
{
    // XOR is self-inverse: applying the same change twice returns to original
    okami::SaveSlot slot;
    std::memset(&slot, 0, sizeof(slot));
    uint64_t original = SaveMan::computeChecksum(slot);

    slot.character.maxHealth = 999;
    uint64_t modified = SaveMan::computeChecksum(slot);
    CHECK(modified != original);

    slot.character.maxHealth = 0;
    uint64_t restored = SaveMan::computeChecksum(slot);
    CHECK(restored == original);
}

// =============================================================================
// File I/O roundtrip tests
// =============================================================================

TEST_CASE("File I/O: save then load produces identical game state", "[saveman][fileio]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    // Write known values into mock game memory
    uintptr_t base = wolf::getModuleBase("main.dll");
    auto *charStats = reinterpret_cast<okami::CharacterStats *>(base + okami::main::characterStats);
    charStats->currentHealth = 200;
    charStats->maxHealth = 300;
    charStats->currentPraise = 1500;
    charStats->mainWeapon = 0x02;

    auto *collection = reinterpret_cast<okami::CollectionData *>(base + okami::main::collectionData);
    collection->currentMoney = 99999;
    collection->numSaves = 7;

    auto *tracker = reinterpret_cast<okami::TrackerData *>(base + okami::main::trackerData);
    tracker->timePlayed = 54321;
    tracker->gameOverCount = 3;

    // Save
    REQUIRE(sm->saveGameState());

    // Verify file exists
    std::string savePath = sm->getSavePath();
    REQUIRE(std::filesystem::exists(savePath));
    CHECK(std::filesystem::file_size(savePath) == sizeof(okami::SaveSlot));

    // Corrupt game memory
    charStats->currentHealth = 0;
    charStats->maxHealth = 0;
    collection->currentMoney = 0;
    tracker->timePlayed = 0;

    // Load
    REQUIRE(sm->loadGameState());

    // Verify restored
    CHECK(charStats->currentHealth == 200);
    CHECK(charStats->maxHealth == 300);
    CHECK(charStats->currentPraise == 1500);
    CHECK(charStats->mainWeapon == 0x02);
    CHECK(collection->currentMoney == 99999);
    CHECK(collection->numSaves == 7);
    CHECK(tracker->timePlayed == 54321);
    CHECK(tracker->gameOverCount == 3);

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

TEST_CASE("File I/O: hasSaveFile reflects file existence", "[saveman][fileio]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    cleanupSaveFile(*sm);
    CHECK_FALSE(sm->hasSaveFile());

    REQUIRE(sm->saveGameState());
    CHECK(sm->hasSaveFile());

    REQUIRE(sm->deleteSave());
    CHECK_FALSE(sm->hasSaveFile());

    wolf::mock::reset();
}

TEST_CASE("File I/O: load from non-existent file fails gracefully", "[saveman][fileio]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    cleanupSaveFile(*sm);
    CHECK_FALSE(sm->loadGameState());

    wolf::mock::reset();
}

TEST_CASE("File I/O: load from wrong-sized file fails", "[saveman][fileio]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    // Create a file with wrong size
    std::string path = sm->getSavePath();
    REQUIRE(!path.empty());
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    {
        std::ofstream f(path, std::ios::binary);
        char junk[64] = {};
        f.write(junk, sizeof(junk));
    }

    CHECK_FALSE(sm->loadGameState());

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

TEST_CASE("File I/O: saved file contains valid checksum", "[saveman][fileio]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    // Write some data
    uintptr_t base = wolf::getModuleBase("main.dll");
    auto *charStats = reinterpret_cast<okami::CharacterStats *>(base + okami::main::characterStats);
    charStats->currentHealth = 42;

    REQUIRE(sm->saveGameState());

    // Read the file directly and verify checksum
    okami::SaveSlot slot;
    std::string path = sm->getSavePath();
    std::ifstream f(path, std::ios::binary);
    REQUIRE(f.is_open());
    f.read(reinterpret_cast<char *>(&slot), sizeof(slot));
    f.close();

    uint64_t computed = SaveMan::computeChecksum(slot);
    CHECK(slot.checksum == computed);

    // Also verify the header magic was written
    CHECK(slot.header == 0x40400000);

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

// =============================================================================
// AP mode lifecycle tests
// =============================================================================

TEST_CASE("AP mode: starts inactive", "[saveman][lifecycle]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);

    CHECK_FALSE(sm->isApModeActive());
    wolf::mock::reset();
}

TEST_CASE("AP mode: setApModeActive toggles state", "[saveman][lifecycle]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);

    sm->setApModeActive(true);
    CHECK(sm->isApModeActive());

    sm->setApModeActive(false);
    CHECK_FALSE(sm->isApModeActive());

    wolf::mock::reset();
}

TEST_CASE("AP mode: getSavePath empty when disconnected", "[saveman][lifecycle]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    // Don't connect
    SaveMan sm(socket);
    sm.initialize();

    CHECK(sm.getSavePath().empty());

    wolf::mock::reset();
}

TEST_CASE("AP mode: save fails when not initialized", "[saveman][lifecycle]")
{
    mock::MockArchipelagoSocket socket;
    socket.connect("localhost", "TestSlot", "");
    socket.setConnected(true);

    SaveMan sm(socket);
    // Don't call initialize()

    CHECK_FALSE(sm.saveGameState());
    CHECK_FALSE(sm.loadGameState());
}

// =============================================================================
// Auto-save debounce tests
// =============================================================================

TEST_CASE("Auto-save: does not trigger when AP mode inactive", "[saveman][autosave]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    // AP mode is off by default

    cleanupSaveFile(*sm);

    sm->queueAutoSave();
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    sm->processAutoSave();

    // Should not have saved because AP mode is inactive
    CHECK_FALSE(sm->hasSaveFile());

    wolf::mock::reset();
}

TEST_CASE("Auto-save: triggers after debounce period", "[saveman][autosave]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    cleanupSaveFile(*sm);

    sm->queueAutoSave();

    // Immediately: should NOT fire (within debounce window)
    sm->processAutoSave();
    CHECK_FALSE(sm->hasSaveFile());

    // After debounce period: should fire
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    sm->processAutoSave();
    CHECK(sm->hasSaveFile());

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

TEST_CASE("Auto-save: rapid queues coalesce to single save", "[saveman][autosave]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    cleanupSaveFile(*sm);

    // Queue many times rapidly
    for (int i = 0; i < 10; i++)
    {
        sm->queueAutoSave();
        sm->processAutoSave(); // Each call within debounce window — no save
    }
    CHECK_FALSE(sm->hasSaveFile());

    // Wait for debounce then process
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    sm->processAutoSave();
    CHECK(sm->hasSaveFile());

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

// =============================================================================
// Snapshot/restore: verify all 6 memory regions survive roundtrip
// =============================================================================

TEST_CASE("Snapshot roundtrip: all 6 memory regions preserved", "[saveman][snapshot]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    uintptr_t base = wolf::getModuleBase("main.dll");

    // Set distinctive values in each region
    auto *charStats = reinterpret_cast<okami::CharacterStats *>(base + okami::main::characterStats);
    charStats->currentHealth = 0xBEEF;
    charStats->totalPraise = 0xCAFE;

    auto *tracker = reinterpret_cast<okami::TrackerData *>(base + okami::main::trackerData);
    tracker->timePlayed = 0xDEAD;

    auto *collection = reinterpret_cast<okami::CollectionData *>(base + okami::main::collectionData);
    collection->currentMoney = 0x1234;

    auto *mapData = reinterpret_cast<okami::MapState *>(base + okami::main::mapData);
    mapData[0].user[0] = 0xAAAA;
    mapData[5].user[3] = 0xBBBB;

    auto *dialogBits = reinterpret_cast<okami::BitField<512> *>(base + okami::main::dialogBits);
    // Set a specific bit in the first map's dialog bits
    auto *rawBits = reinterpret_cast<uint32_t *>(&dialogBits[0]);
    rawBits[0] = 0x12345678;

    auto *customTex = reinterpret_cast<okami::CustomTextures *>(base + okami::main::customTextures);
    auto *texBytes = reinterpret_cast<uint8_t *>(customTex);
    texBytes[0] = 0xAA;
    texBytes[1] = 0xBB;

    // Save
    REQUIRE(sm->saveGameState());

    // Zero all regions
    std::memset(charStats, 0, sizeof(okami::CharacterStats));
    std::memset(tracker, 0, sizeof(okami::TrackerData));
    std::memset(collection, 0, sizeof(okami::CollectionData));
    std::memset(mapData, 0, sizeof(okami::MapState) * okami::MapTypes::NUM_MAP_TYPES);
    std::memset(dialogBits, 0, sizeof(okami::BitField<512>) * okami::MapTypes::NUM_MAP_TYPES);
    std::memset(customTex, 0, sizeof(okami::CustomTextures));

    // Load
    REQUIRE(sm->loadGameState());

    // Verify all regions restored
    CHECK(charStats->currentHealth == 0xBEEF);
    CHECK(charStats->totalPraise == 0xCAFE);
    CHECK(tracker->timePlayed == 0xDEAD);
    CHECK(collection->currentMoney == 0x1234);
    CHECK(mapData[0].user[0] == 0xAAAA);
    CHECK(mapData[5].user[3] == 0xBBBB);
    CHECK(rawBits[0] == 0x12345678);
    CHECK(texBytes[0] == 0xAA);
    CHECK(texBytes[1] == 0xBB);

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

// =============================================================================
// Edge cases
// =============================================================================

TEST_CASE("Different connections produce different save files", "[saveman][edge]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket1;
    socket1.connect("server1", "Slot1", "");
    socket1.setConnected(true);

    mock::MockArchipelagoSocket socket2;
    socket2.connect("server2", "Slot2", "");
    socket2.setConnected(true);

    SaveMan sm1(socket1);
    sm1.initialize();
    SaveMan sm2(socket2);
    sm2.initialize();

    CHECK(sm1.getSavePath() != sm2.getSavePath());

    wolf::mock::reset();
}

TEST_CASE("deleteSave removes file", "[saveman][fileio]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    REQUIRE(sm->saveGameState());
    REQUIRE(sm->hasSaveFile());

    REQUIRE(sm->deleteSave());
    CHECK_FALSE(sm->hasSaveFile());

    wolf::mock::reset();
}
