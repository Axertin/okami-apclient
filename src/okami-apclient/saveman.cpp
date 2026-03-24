#include "saveman.h"

#include <cstring>
#include <filesystem>
#include <fstream>

#include <wolf_framework.hpp>

#include "isocket.h"
#include "ui/notificationwindow.h"
#include <okami/offsets.hpp>

static std::string initSaveDir()
{
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
    const char *appdata = std::getenv("APPDATA");
#pragma warning(pop)
    if (appdata)
        return std::string(appdata) + "\\okami-apsaves";
    return "okami-apsaves"; // fallback
#else
    // Non-Windows (test builds): use $HOME or /tmp
    const char *home = std::getenv("HOME");
    if (home)
        return std::string(home) + "/.okami-apsaves";
    return "/tmp/okami-apsaves";
#endif
}
static const std::string SAVE_DIR = initSaveDir();

namespace
{
// Memory region addresses (offsets relative to main.dll)
constexpr uintptr_t kCharacterStats = okami::main::characterStats;   // 0xB4DF90
constexpr uintptr_t kTrackerData = okami::main::trackerData;         // 0xB21780
constexpr uintptr_t kCollectionData = okami::main::collectionData;   // 0xB205D0
constexpr uintptr_t kMapData = okami::main::mapData;                 // 0xB322B0
constexpr uintptr_t kDialogBits = okami::main::dialogBits;           // 0xB36CF0
constexpr uintptr_t kCustomTextures = okami::main::customTextures;   // 0xB21820
constexpr uintptr_t kAreaNameStrId = okami::main::areaNameStrId;     // 0x79BEB4

constexpr uint64_t kChecksumSeed = 0x9be6fa3b72afda1d;
constexpr uint32_t kHeaderMagic = 0x40400000;

// Hook offsets (relative to main.dll base)
constexpr uintptr_t kMcLoadCtorOffset = 0x1c1db0;     // FUN_1801c1db0: cMcLoad constructor
constexpr uintptr_t kMcSaveCtorOffset = 0x1c3de0;     // FUN_1801c3de0: cMcSave constructor
constexpr uintptr_t kSaveGateOffset = 0x1c37d0;       // FUN_1801c37d0: cMcSave gate (vtable[1])
                                                        //   returns 1 → show save UI, 0 → skip to cleanup
constexpr uintptr_t kCurrentSlotIndex = 0x79BEB0;     // Active save slot index (uint32_t)
} // namespace

static_assert(sizeof(okami::SaveSlot) == 0x172A0,
              "SaveSlot size mismatch — expected 0x172A0 bytes");

// =============================================================================
// Hook infrastructure
// =============================================================================

// Raw pointer for hook callbacks — set by installHooks(), cleared by destructor.
static SaveMan *g_saveMan = nullptr;

// Function signatures (x64 __fastcall, from Ghidra decompilation)
using CtorFn = void(__fastcall *)(void *pContext);
using GateFn = unsigned char(__fastcall *)(void *pMcSave);

// Original function pointers (populated by hookFunction)
static CtorFn s_origMcLoadCtor = nullptr;
static CtorFn s_origMcSaveCtor = nullptr;
static GateFn s_origSaveGate = nullptr;

/// Hook: cMcSave gate function (FUN_1801c37d0, vtable[1]).
/// Called by the state machine (FUN_1801bfcb0) to decide whether to show the save UI.
/// Returns 1 → allocate buffers and open save slot picker (state 0).
/// Returns 0 → skip directly to cleanup (state 2), no UI ever shown.
/// When AP is connected, we return 0 so the vanilla save menu never opens.
static unsigned char __fastcall hookSaveGate(void *pMcSave)
{
    if (g_saveMan && g_saveMan->isConnected())
    {
        // Let the original run (allocates save buffers that cleanup expects to exist),
        // but override the return value so the state machine skips to state 2 (cleanup)
        // instead of state 0 (show save UI).
        s_origSaveGate(pMcSave);
        wolf::logInfo("[SaveMan] Save gate: blocking vanilla save UI (AP connected)");
        return 0;
    }
    return s_origSaveGate(pMcSave);
}

/// Hook: cMcLoad constructor — when the user clicked "Continue AP Game" in the
/// login window, force slot 0 so the vanilla load completes quickly. The actual
/// AP restore happens in onPlayStart after the game finishes loading.
static void __fastcall hookMcLoadCtor(void *pContext)
{
    wolf::logDebug("[SaveMan] hookMcLoadCtor fired (pendingLoad=%d)",
                   g_saveMan ? (int)g_saveMan->isPendingLoad() : -1);
    if (g_saveMan && g_saveMan->isPendingLoad())
    {
        // Force slot 0 so the game has something to load (onPlayStart will overwrite)
        uintptr_t slotAddr = wolf::getModuleBase("main.dll") + kCurrentSlotIndex;
        uint32_t slot = 0;
        wolf::writeMemory(slotAddr, &slot, sizeof(slot));

        g_saveMan->setApModeActive(true);
        wolf::logInfo("[SaveMan] AP load: forcing slot 0, will restore on play start");
    }
    s_origMcLoadCtor(pContext);
}

/// Hook: cMcSave constructor — when AP connected, snapshot game state to .oksav.
/// The vanilla ctor still runs afterward, but hookSaveGate returns 0 so the state
/// machine skips directly to cleanup — no save slot picker, no Steam Cloud write.
static void __fastcall hookMcSaveCtor(void *pContext)
{
    wolf::logDebug("[SaveMan] hookMcSaveCtor fired (connected=%d)",
                   g_saveMan ? (int)g_saveMan->isConnected() : -1);
    if (g_saveMan && g_saveMan->isConnected())
    {
        g_saveMan->setApModeActive(true);
        g_saveMan->saveGameState();
        notificationwindow::queue("AP progress saved", 3.0f);
        wolf::logInfo("[SaveMan] AP save: game state saved to %s", g_saveMan->getSavePath().c_str());
    }
    s_origMcSaveCtor(pContext);
}

SaveMan::SaveMan(ISocket &socket) : socket_(socket)
{
}

SaveMan::~SaveMan()
{
    if (g_saveMan == this)
        g_saveMan = nullptr;
}

void SaveMan::initialize()
{
    uintptr_t base = wolf::getModuleBase("main.dll");
    if (base == 0)
    {
        wolf::logError("[SaveMan] Failed to get main.dll base address");
        return;
    }

    characterStatsAddr_ = base + kCharacterStats;
    trackerDataAddr_ = base + kTrackerData;
    collectionDataAddr_ = base + kCollectionData;
    mapDataAddr_ = base + kMapData;
    dialogBitsAddr_ = base + kDialogBits;
    customTexturesAddr_ = base + kCustomTextures;
    areaNameStrIdAddr_ = base + kAreaNameStrId;

    initialized_ = true;
    wolf::logInfo("[SaveMan] Initialized (base=0x%llX)", base);
}

// =============================================================================
// Core save/load
// =============================================================================

bool SaveMan::saveGameState()
{
    if (!initialized_)
    {
        wolf::logError("[SaveMan] Not initialized");
        return false;
    }

    okami::SaveSlot slot;
    std::memset(&slot, 0, sizeof(slot));

    snapshotToSlot(slot);
    slot.checksum = computeChecksum(slot);

    if (!writeSlotToFile(slot))
    {
        wolf::logError("[SaveMan] Failed to write save file");
        return false;
    }

    wolf::logInfo("[SaveMan] Game state saved to %s", getSavePath().c_str());
    return true;
}

bool SaveMan::loadGameState()
{
    if (!initialized_)
    {
        wolf::logError("[SaveMan] Not initialized");
        return false;
    }

    okami::SaveSlot slot;
    if (!readSlotFromFile(slot))
    {
        wolf::logError("[SaveMan] Failed to read save file");
        return false;
    }

    // Verify checksum
    uint64_t expected = computeChecksum(slot);
    if (slot.checksum != expected)
    {
        wolf::logWarning("[SaveMan] Checksum mismatch (file=0x%llX, computed=0x%llX) — loading anyway",
                         slot.checksum, expected);
    }

    restoreFromSlot(slot);
    wolf::logInfo("[SaveMan] Game state loaded from %s", getSavePath().c_str());
    return true;
}

bool SaveMan::hasSaveFile() const
{
    std::string path = getSavePath();
    if (path.empty())
        return false;
    return std::filesystem::exists(path);
}

bool SaveMan::deleteSave()
{
    std::string path = getSavePath();
    if (path.empty())
        return false;

    std::error_code ec;
    if (std::filesystem::remove(path, ec))
    {
        wolf::logInfo("[SaveMan] Deleted save file: %s", path.c_str());
        return true;
    }
    return false;
}

// =============================================================================
// AP mode
// =============================================================================

bool SaveMan::isApModeActive() const
{
    return apModeActive_;
}

void SaveMan::setApModeActive(bool active)
{
    apModeActive_ = active;
    wolf::logInfo("[SaveMan] AP mode %s", active ? "activated" : "deactivated");
}

bool SaveMan::isConnected() const
{
    return socket_.isConnected();
}

void SaveMan::setPendingLoad(bool pending)
{
    pendingApLoad_ = pending;
}

bool SaveMan::isPendingLoad() const
{
    return pendingApLoad_;
}

// =============================================================================
// Auto-save
// =============================================================================

void SaveMan::queueAutoSave()
{
    std::lock_guard<std::mutex> lock(autoSaveMutex_);
    autoSavePending_ = true;
    lastAutoSaveRequest_ = std::chrono::steady_clock::now();
}

void SaveMan::processAutoSave()
{
    if (!autoSavePending_ || !apModeActive_)
        return;

    auto now = std::chrono::steady_clock::now();

    {
        std::lock_guard<std::mutex> lock(autoSaveMutex_);
        if (now - lastAutoSaveRequest_ < AUTO_SAVE_DEBOUNCE)
            return;
        autoSavePending_ = false;
    }

    saveGameState();
}

// =============================================================================
// Save path
// =============================================================================

std::string SaveMan::getSavePath() const
{
    if (!socket_.isConnected())
        return {};

    // Save key format: slot_seed (same as ArchipelagoSocket::getConnectionInfo)
    std::string saveKey = socket_.getConnectionInfo();
    if (saveKey.empty())
        return {};

    return (std::filesystem::path(SAVE_DIR) / (saveKey + ".oksav")).string();
}

// =============================================================================
// Memory snapshot
// =============================================================================

void SaveMan::snapshotToSlot(okami::SaveSlot &slot)
{
    // Header
    slot.header = kHeaderMagic;

    // Area name string ID
    slot.areaNameStrId = *reinterpret_cast<const uint32_t *>(areaNameStrIdAddr_);

    // RTC timestamp (Windows FILETIME format for compatibility)
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    // Convert to Windows FILETIME (100-ns intervals since 1601-01-01)
    // Unix epoch to FILETIME epoch offset: 116444736000000000
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    slot.timeRTC = static_cast<uint64_t>(us * 10) + 116444736000000000ULL;

    // Copy the 6 game state regions
    std::memcpy(&slot.character, reinterpret_cast<const void *>(characterStatsAddr_),
                sizeof(okami::CharacterStats));

    std::memcpy(&slot.tracked, reinterpret_cast<const void *>(trackerDataAddr_),
                sizeof(okami::TrackerData));

    std::memcpy(&slot.collection, reinterpret_cast<const void *>(collectionDataAddr_),
                sizeof(okami::CollectionData));

    std::memcpy(slot.MapData, reinterpret_cast<const void *>(mapDataAddr_),
                sizeof(slot.MapData));

    std::memcpy(slot.DialogBits, reinterpret_cast<const void *>(dialogBitsAddr_),
                sizeof(slot.DialogBits));

    std::memcpy(&slot.customTextures, reinterpret_cast<const void *>(customTexturesAddr_),
                sizeof(okami::CustomTextures));
}

void SaveMan::restoreFromSlot(const okami::SaveSlot &slot)
{
    // Write the 6 game state regions back to game memory
    std::memcpy(reinterpret_cast<void *>(characterStatsAddr_), &slot.character,
                sizeof(okami::CharacterStats));

    std::memcpy(reinterpret_cast<void *>(trackerDataAddr_), &slot.tracked,
                sizeof(okami::TrackerData));

    std::memcpy(reinterpret_cast<void *>(collectionDataAddr_), &slot.collection,
                sizeof(okami::CollectionData));

    std::memcpy(reinterpret_cast<void *>(mapDataAddr_), slot.MapData,
                sizeof(slot.MapData));

    std::memcpy(reinterpret_cast<void *>(dialogBitsAddr_), slot.DialogBits,
                sizeof(slot.DialogBits));

    std::memcpy(reinterpret_cast<void *>(customTexturesAddr_), &slot.customTextures,
                sizeof(okami::CustomTextures));

    // Write area name string ID back
    *reinterpret_cast<uint32_t *>(areaNameStrIdAddr_) = slot.areaNameStrId;
}

// =============================================================================
// Checksum (ported from OriginEdit — XOR all qwords except the checksum field)
// =============================================================================

uint64_t SaveMan::computeChecksum(const okami::SaveSlot &slot)
{
    const auto *data = reinterpret_cast<const uint64_t *>(&slot);
    constexpr size_t totalQwords = sizeof(okami::SaveSlot) / sizeof(uint64_t);

    // Seed XOR'd with first qword (header + areaNameStrId)
    uint64_t checksum = kChecksumSeed ^ data[0];

    // Skip data[1] (the checksum field at offset +0x08)
    // XOR all qwords from offset +0x10 onward
    for (size_t i = 2; i < totalQwords; ++i)
    {
        checksum ^= data[i];
    }

    return checksum;
}

// =============================================================================
// File I/O
// =============================================================================

bool SaveMan::writeSlotToFile(const okami::SaveSlot &slot)
{
    std::string path = getSavePath();
    if (path.empty())
        return false;

    try
    {
        std::filesystem::create_directories(SAVE_DIR);

        // Atomic write: write to .tmp, then rename
        std::string tmpPath = path + ".tmp";

        std::ofstream file(tmpPath, std::ios::binary);
        if (!file.is_open())
        {
            wolf::logError("[SaveMan] Failed to open %s for writing", tmpPath.c_str());
            return false;
        }

        file.write(reinterpret_cast<const char *>(&slot), sizeof(okami::SaveSlot));
        file.close();

        if (!file.good())
        {
            wolf::logError("[SaveMan] Write error to %s", tmpPath.c_str());
            std::filesystem::remove(tmpPath);
            return false;
        }

        // Atomic rename
        std::error_code ec;
        std::filesystem::rename(tmpPath, path, ec);
        if (ec)
        {
            wolf::logError("[SaveMan] Failed to rename %s -> %s: %s",
                           tmpPath.c_str(), path.c_str(), ec.message().c_str());
            std::filesystem::remove(tmpPath);
            return false;
        }

        return true;
    }
    catch (const std::exception &e)
    {
        wolf::logError("[SaveMan] Exception writing save: %s", e.what());
        return false;
    }
}

bool SaveMan::readSlotFromFile(okami::SaveSlot &slot) const
{
    std::string path = getSavePath();
    if (path.empty())
        return false;

    try
    {
        auto fileSize = std::filesystem::file_size(path);
        if (fileSize != sizeof(okami::SaveSlot))
        {
            wolf::logError("[SaveMan] Invalid save file size: %zu (expected %zu)",
                           static_cast<size_t>(fileSize), sizeof(okami::SaveSlot));
            return false;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            wolf::logError("[SaveMan] Failed to open %s for reading", path.c_str());
            return false;
        }

        file.read(reinterpret_cast<char *>(&slot), sizeof(okami::SaveSlot));

        if (!file.good())
        {
            wolf::logError("[SaveMan] Read error from %s", path.c_str());
            return false;
        }

        return true;
    }
    catch (const std::exception &e)
    {
        wolf::logError("[SaveMan] Exception reading save: %s", e.what());
        return false;
    }
}

// =============================================================================
// Hook installation
// =============================================================================

void SaveMan::installHooks()
{
    g_saveMan = this;
    int installed = 0;

    if (wolf::hookFunction("main.dll", kMcLoadCtorOffset,
                           reinterpret_cast<void *>(&hookMcLoadCtor),
                           reinterpret_cast<void **>(&s_origMcLoadCtor)))
        ++installed;
    else
        wolf::logError("[SaveMan] FAILED: McLoad ctor hook at 0x%zX", kMcLoadCtorOffset);

    if (wolf::hookFunction("main.dll", kMcSaveCtorOffset,
                           reinterpret_cast<void *>(&hookMcSaveCtor),
                           reinterpret_cast<void **>(&s_origMcSaveCtor)))
        ++installed;
    else
        wolf::logError("[SaveMan] FAILED: McSave ctor hook at 0x%zX", kMcSaveCtorOffset);

    if (wolf::hookFunction("main.dll", kSaveGateOffset,
                           reinterpret_cast<void *>(&hookSaveGate),
                           reinterpret_cast<void **>(&s_origSaveGate)))
        ++installed;
    else
        wolf::logError("[SaveMan] FAILED: SaveGate hook at 0x%zX", kSaveGateOffset);

    wolf::logInfo("[SaveMan] Hooks installed: %d/3", installed);
}
