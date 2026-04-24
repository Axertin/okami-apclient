#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>

#include "mock_archipelagosocket.h"
#include "saveman.h"
#include "wolf_framework.hpp"

#include <okami/maps.hpp>
#include <okami/offsets.hpp>
#include <okami/savefile.hpp>

// Highest memory offset used by SaveMan: systemFlags at 0xB6B2B0 + sizeof(uint32_t).
// Reserve enough mock memory to cover all game state regions and flag globals.
static constexpr size_t kMockMemorySize = 0xB80000;

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

/// Access the flag globals read by SaveMan::isSafeToSave. Tests that want
/// auto-save to fire must call setSafe() after makeSaveMan.
struct GameFlagWriter
{
    uint32_t *saveFlags;
    uint32_t *sysFlags;
    uint32_t *areaFlags;
    uint16_t *mapId;

    explicit GameFlagWriter(uintptr_t base)
        : saveFlags(reinterpret_cast<uint32_t *>(base + okami::main::saveStateFlags)),
          sysFlags(reinterpret_cast<uint32_t *>(base + okami::main::systemFlags)),
          areaFlags(reinterpret_cast<uint32_t *>(base + okami::main::areaLoadFlags)),
          mapId(reinterpret_cast<uint16_t *>(base + okami::main::exteriorMapID))
    {
    }

    /// Gameplay-safe: no save/load op active, no area-load in flight, not title screen.
    /// saveStateFlags is zeroed — bits 6 (New Game marker) and 8 (area-load trigger)
    /// are intentionally NOT set here; isSafeToSave should not depend on them.
    void setSafe()
    {
        *saveFlags = 0;
        *sysFlags = 0;
        *areaFlags = 0;
        *mapId = 0x0122; // River of the Heavens (any non-title map)
    }
};

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
    CHECK(std::filesystem::file_size(savePath) == sizeof(okami::SaveFile));

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
    GameFlagWriter(wolf::getModuleBase("main.dll")).setSafe();

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
    GameFlagWriter(wolf::getModuleBase("main.dll")).setSafe();

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

TEST_CASE("Snapshot captures live player position from Amaterasu object", "[saveman][snapshot]")
{
    // CharacterStats x/y/z (position) and u/v/w (rotation) are "set from elsewhere"
    // — populated by FUN_18043b040 just before vanilla save, reading from the
    // Amaterasu object at ammyModel. snapshotToSlot must inline that read so our
    // save carries the live position instead of the stale CharacterStats values,
    // otherwise reload spawns the player at the map's default origin.
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter(wolf::getModuleBase("main.dll")).setSafe();

    uintptr_t base = wolf::getModuleBase("main.dll");

    // Stage an Amaterasu object inside the mock region, and a separate position
    // vec3 it points to. Both addresses stay within kMockMemorySize (0xB80000).
    const uintptr_t ammyAddr = base + 0xB70000; // ammy struct — has rotation inline at +0xB0
    const uintptr_t posAddr  = base + 0xB78000; // vec3 that ammy+0xA8 points to

    // Live position (x,y,z) — what the player is actually standing at
    auto *pos = reinterpret_cast<float *>(posAddr);
    pos[0] = -51.0f;
    pos[1] = 10.0f;
    pos[2] = -402.0f;

    // ammy+0xA8 points at the position vec3
    *reinterpret_cast<const float **>(ammyAddr + 0xA8) = pos;

    // Rotation inline at ammy+0xB0..+0xB8
    auto *rot = reinterpret_cast<float *>(ammyAddr + 0xB0);
    rot[0] = 0.1f;
    rot[1] = 0.2f;
    rot[2] = 0.3f;

    // Global ammyModel pointer points at our staged object
    *reinterpret_cast<uintptr_t *>(base + okami::main::ammyModel) = ammyAddr;

    // Stale values in CharacterStats — what a pre-cMcSave snapshot would otherwise read
    auto *charStats = reinterpret_cast<okami::CharacterStats *>(base + okami::main::characterStats);
    charStats->x = 999.0f;
    charStats->y = 999.0f;
    charStats->z = 999.0f;
    charStats->u = 0.0f;
    charStats->v = 0.0f;
    charStats->w = 0.0f;

    cleanupSaveFile(*sm);
    REQUIRE(sm->saveGameState());

    okami::SaveSlot slot;
    std::ifstream f(sm->getSavePath(), std::ios::binary);
    REQUIRE(f.is_open());
    f.read(reinterpret_cast<char *>(&slot), sizeof(slot));

    CHECK(slot.character.x == Catch::Approx(-51.0f));
    CHECK(slot.character.y == Catch::Approx(10.0f));
    CHECK(slot.character.z == Catch::Approx(-402.0f));
    CHECK(slot.character.u == Catch::Approx(0.1f));
    CHECK(slot.character.v == Catch::Approx(0.2f));
    CHECK(slot.character.w == Catch::Approx(0.3f));

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

TEST_CASE("Snapshot overrides stale CollectionData map IDs with live values", "[saveman][snapshot]")
{
    // CollectionData.currentMapId and .lastMapId are documented (structs.hpp) as
    // "set from +0xB6B240 / +0xB6B248 on save" — their live values can lag behind
    // the actual map until the game's own pre-save step runs, which happens AFTER
    // hookMcSaveCtor. snapshotToSlot must patch them from exteriorMapID /
    // exteriorMapIDCopy so the save reloads into the room the player was actually in.
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    uintptr_t base = wolf::getModuleBase("main.dll");

    // Live map globals — where the player actually is.
    *reinterpret_cast<uint16_t *>(base + okami::main::exteriorMapID) = 0x0122;
    *reinterpret_cast<uint16_t *>(base + okami::main::exteriorMapIDCopy) = 0x0121;

    // Stale values in CollectionData — what a pre-cMcSave snapshot would otherwise read.
    auto *collection = reinterpret_cast<okami::CollectionData *>(base + okami::main::collectionData);
    collection->currentMapId = 0xDEAD;
    collection->lastMapId = 0xBEEF;

    cleanupSaveFile(*sm);
    REQUIRE(sm->saveGameState());

    okami::SaveSlot slot;
    std::ifstream f(sm->getSavePath(), std::ios::binary);
    REQUIRE(f.is_open());
    f.read(reinterpret_cast<char *>(&slot), sizeof(slot));

    CHECK(slot.collection.currentMapId == 0x0122); // from exteriorMapID
    CHECK(slot.collection.lastMapId == 0x0121);    // from exteriorMapIDCopy

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

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

// =============================================================================
// Golden save validation tests
// =============================================================================

/// Read the golden vanilla save fixture into a SaveFile.
static okami::SaveFile readFixtureSaveFile()
{
    okami::SaveFile file;
    std::string path = std::string(TEST_FIXTURES_DIR) + "/vanilla_save.bin";
    std::ifstream f(path, std::ios::binary);
    REQUIRE(f.is_open());
    f.read(reinterpret_cast<char *>(&file), sizeof(file));
    REQUIRE(f.good());
    return file;
}

TEST_CASE("Golden save: checksum matches game-computed value", "[saveman][golden]")
{
    auto file = readFixtureSaveFile();
    uint64_t computed = SaveMan::computeChecksum(file.slots[0]);
    CHECK(computed == file.slots[0].checksum);
}

TEST_CASE("Golden save: slot 0 header is valid", "[saveman][golden]")
{
    auto file = readFixtureSaveFile();
    CHECK(file.slots[0].header == 0x40400000);
    CHECK(file.slots[0].areaNameStrId == 0x0F);
    CHECK(file.slots[0].areaNameStrId < 0x36);
    CHECK(file.slots[0].checksum != 0);
    CHECK(file.slots[0].timeRTC != 0);
}

TEST_CASE("Golden save: slots 1-29 are empty", "[saveman][golden]")
{
    auto file = readFixtureSaveFile();
    for (int i = 1; i < 30; ++i)
    {
        INFO("slot " << i);
        CHECK(file.slots[i].header == 0);
        CHECK(file.slots[i].areaNameStrId == 0xFFFFFFFF);
    }
}

TEST_CASE("Golden save: struct fields match known values", "[saveman][golden]")
{
    auto file = readFixtureSaveFile();
    const auto &slot = file.slots[0];

    CHECK(slot.character.currentHealth == 900);
    CHECK(slot.character.maxHealth == 900);
    CHECK(slot.character.x == Catch::Approx(-51.0f));
    CHECK(slot.character.y == Catch::Approx(10.0f));
    CHECK(slot.character.z == Catch::Approx(-402.0f));
    CHECK(slot.tracked.timePlayed == 2448);
    CHECK(slot.collection.numSaves == 1);
    CHECK(slot.collection.currentMapId == 0x0122);
    CHECK(slot.collection.currentMoney == 0);
}

TEST_CASE("Golden save: areaNameID is consistent with currentMapId", "[saveman][golden]")
{
    auto file = readFixtureSaveFile();
    const auto &slot = file.slots[0];

    uint32_t lookupResult = okami::getAreaNameID(slot.collection.currentMapId);
    CHECK(lookupResult == slot.areaNameStrId);
}

TEST_CASE("MapToAreaNameID: all values are valid area name IDs", "[saveman][golden]")
{
    for (const auto &[mapId, areaNameId] : okami::MapToAreaNameID)
    {
        INFO("mapId 0x" << std::hex << mapId);
        CHECK(areaNameId < 0x36);
    }
}

TEST_CASE("getAreaNameID: unknown maps fall back to 0x35", "[saveman][golden]")
{
    CHECK(okami::getAreaNameID(0xFFFF) == 0x35);
    CHECK(okami::getAreaNameID(0x0000) == 0x35);
}

TEST_CASE("writeSlotToFile preserves slots 1-29", "[saveman][golden]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    // Copy fixture to the AP save path so writeSlotToFile has existing data
    std::string fixturePath = std::string(TEST_FIXTURES_DIR) + "/vanilla_save.bin";
    std::string savePath = sm->getSavePath();
    REQUIRE(!savePath.empty());
    std::filesystem::create_directories(std::filesystem::path(savePath).parent_path());
    std::filesystem::copy_file(fixturePath, savePath,
                               std::filesystem::copy_options::overwrite_existing);

    // Read original slots 1-29 for comparison
    okami::SaveFile originalFile;
    {
        std::ifstream f(fixturePath, std::ios::binary);
        REQUIRE(f.is_open());
        f.read(reinterpret_cast<char *>(&originalFile), sizeof(originalFile));
        REQUIRE(f.good());
    }

    // Write modified slot 0 via SaveMan (snapshots from mock memory)
    uintptr_t base = wolf::getModuleBase("main.dll");
    auto *charStats = reinterpret_cast<okami::CharacterStats *>(base + okami::main::characterStats);
    charStats->currentHealth = 999;
    REQUIRE(sm->saveGameState());

    // Read written file and verify slots 1-29 are unchanged
    okami::SaveFile writtenFile;
    {
        std::ifstream f(savePath, std::ios::binary);
        REQUIRE(f.is_open());
        f.read(reinterpret_cast<char *>(&writtenFile), sizeof(writtenFile));
        REQUIRE(f.good());
    }

    for (int i = 1; i < 30; ++i)
    {
        INFO("slot " << i);
        CHECK(std::memcmp(&writtenFile.slots[i], &originalFile.slots[i],
                           sizeof(okami::SaveSlot)) == 0);
    }

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

// =============================================================================
// isSafeToSave + deferred auto-save
// =============================================================================

TEST_CASE("isSafeToSave: false when AP mode inactive", "[saveman][safe]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    GameFlagWriter flags(wolf::getModuleBase("main.dll"));
    flags.setSafe();

    // apModeActive_ is false by default
    CHECK_FALSE(sm->isSafeToSave());

    wolf::mock::reset();
}

TEST_CASE("isSafeToSave: true when in gameplay with all flags clear", "[saveman][safe]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter flags(wolf::getModuleBase("main.dll"));
    flags.setSafe();

    CHECK(sm->isSafeToSave());

    wolf::mock::reset();
}

TEST_CASE("isSafeToSave: false on title screen", "[saveman][safe]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter flags(wolf::getModuleBase("main.dll"));
    flags.setSafe();
    *flags.mapId = static_cast<uint16_t>(MapID::TitleScreen);

    CHECK_FALSE(sm->isSafeToSave());

    wolf::mock::reset();
}

TEST_CASE("isSafeToSave: false while save op active (bit 22)", "[saveman][safe]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter flags(wolf::getModuleBase("main.dll"));
    flags.setSafe();
    *flags.sysFlags |= (1u << 22);

    CHECK_FALSE(sm->isSafeToSave());

    wolf::mock::reset();
}

TEST_CASE("isSafeToSave: true when bit 6 (New Game marker) is set", "[saveman][safe]")
{
    // Bit 6 of saveStateFlags is sticky after New Game entry (FUN_180438810 sets
    // 0x140 = bits 6+8 on the New Game gameplay path). It is NOT a busy flag —
    // it's a status marker that persists through gameplay. isSafeToSave must
    // ignore it, otherwise auto-save is permanently blocked after New Game.
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter flags(wolf::getModuleBase("main.dll"));
    flags.setSafe();
    *flags.saveFlags |= (1u << 6);

    CHECK(sm->isSafeToSave());

    wolf::mock::reset();
}

TEST_CASE("isSafeToSave: true when bit 8 is clear (transient area-load trigger)", "[saveman][safe]")
{
    // Bit 8 of saveStateFlags is a one-shot signal the boot thread raises to trigger
    // the initial area load; FUN_180489770 consumes and clears it. During normal
    // gameplay it is almost always 0, so isSafeToSave must not require it.
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter flags(wolf::getModuleBase("main.dll"));
    flags.setSafe();
    *flags.saveFlags &= ~(1u << 8); // bit 8 clear (normal gameplay)

    CHECK(sm->isSafeToSave());

    wolf::mock::reset();
}

TEST_CASE("isSafeToSave: false during area load (0x6001000 mask)", "[saveman][safe]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter flags(wolf::getModuleBase("main.dll"));
    flags.setSafe();
    *flags.areaFlags = 0x00001000; // one bit in the mask

    CHECK_FALSE(sm->isSafeToSave());

    wolf::mock::reset();
}

TEST_CASE("Auto-save: deferred while unsafe, fires once flags clear", "[saveman][autosave][safe]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter flags(wolf::getModuleBase("main.dll"));
    flags.setSafe();
    *flags.sysFlags |= (1u << 22); // save op active — unsafe

    cleanupSaveFile(*sm);

    sm->queueAutoSave();
    std::this_thread::sleep_for(std::chrono::milliseconds(600));

    // First tick after debounce: unsafe, save deferred, pending flag preserved.
    sm->processAutoSave();
    CHECK_FALSE(sm->hasSaveFile());

    // Clear the unsafe flag. Next tick should fire.
    *flags.sysFlags &= ~(1u << 22);
    sm->processAutoSave();
    CHECK(sm->hasSaveFile());

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}

TEST_CASE("Auto-save: direct saveGameState clears pending state", "[saveman][autosave]")
{
    // If the user hits a save mirror while an auto-save is pending, hookMcSaveCtor
    // calls saveGameState() directly. That must reset the pending state so the next
    // check-sent queue starts a fresh 30s deferral clock instead of inheriting the
    // old queued-at timestamp (which would trigger a spurious >30s warning).
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);
    GameFlagWriter(wolf::getModuleBase("main.dll")).setSafe();

    cleanupSaveFile(*sm);

    // Queue an auto-save (simulates a check-sent)
    sm->queueAutoSave();

    // User hits a save mirror -> direct save, not via processAutoSave.
    REQUIRE(sm->saveGameState());
    CHECK(sm->hasSaveFile());

    // Pending should have been cleared. A subsequent processAutoSave should be a no-op —
    // we verify by deleting the file and ticking: nothing should write it back.
    REQUIRE(sm->deleteSave());
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    sm->processAutoSave();
    CHECK_FALSE(sm->hasSaveFile());

    wolf::mock::reset();
}

TEST_CASE("Cold-start save produces game-compatible empty slots", "[saveman][golden]")
{
    wolf::mock::reset();
    wolf::mock::reserveMemory(kMockMemorySize);

    mock::MockArchipelagoSocket socket;
    auto sm = makeSaveMan(socket);
    sm->setApModeActive(true);

    // Ensure no pre-existing file
    cleanupSaveFile(*sm);
    REQUIRE_FALSE(sm->hasSaveFile());

    // Save with no existing file — cold-start path
    REQUIRE(sm->saveGameState());

    // Read the written file and verify slots 1-29 match game's empty-slot format
    okami::SaveFile writtenFile;
    {
        std::ifstream f(sm->getSavePath(), std::ios::binary);
        REQUIRE(f.is_open());
        f.read(reinterpret_cast<char *>(&writtenFile), sizeof(writtenFile));
        REQUIRE(f.good());
    }

    for (int i = 1; i < 30; ++i)
    {
        INFO("slot " << i);
        CHECK(writtenFile.slots[i].header == 0);
        CHECK(writtenFile.slots[i].areaNameStrId == 0xFFFFFFFF);
        CHECK(writtenFile.slots[i].checksum == 0);
        CHECK(writtenFile.slots[i].timeRTC == 0);
    }

    cleanupSaveFile(*sm);
    wolf::mock::reset();
}
