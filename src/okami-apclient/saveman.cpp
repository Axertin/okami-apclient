#include "saveman.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>


#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <wolf_framework.hpp>

#include "isocket.h"
#include "ui/notificationwindow.h"
#include <okami/maps.hpp>
#include <okami/offsets.hpp>

static std::string initSaveDir()
{
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
    const char *appdata = std::getenv("APPDATA");
#pragma warning(pop)
    if (!appdata)
    {
        wolf::logError("[SaveMan] APPDATA is unset; something's wrong with Windows");
        std::exit(1);
    }
    return std::string(appdata) + "\\okami-apsaves";
#else
    const char *home = std::getenv("HOME");
    if (!home)
    {
        wolf::logError("[SaveMan] HOME is unset; are you in a non-login non-user shell?");
        std::exit(1);
    }
    return std::string(home) + "/.local/share/okami-apsaves";
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

constexpr uint64_t kChecksumSeed = 0x9be6fa3b72afda1d;
constexpr uint32_t kHeaderMagic = 0x40400000;

// Hook offsets (relative to main.dll base)
constexpr uintptr_t kMcSaveCtorOffset = 0x1c3de0;     // FUN_1801c3de0: cMcSave constructor
constexpr uintptr_t kSaveGateOffset = 0x1c37d0;       // FUN_1801c37d0: cMcSave gate (vtable[1])
                                                        //   returns 1 -> show save UI, 0 -> skip to cleanup
constexpr uintptr_t kOkamiWriteKickoffOffset = 0x14e100; // FUN_18014e100: m2::SteamSaveWrite kickoff
                                                          //   calls ISteamRemoteStorage::FileWriteAsync
                                                          //   directly.
constexpr uintptr_t kOkamiPureReadOffset = 0x14f580;     // FUN_18014f580: Called by
                                                          //   m2::SaveDataManager with (dir, filename,
                                                          //   offset, userBuffer, &size)
} // namespace

static_assert(sizeof(okami::SaveSlot) == 0x172A0,
              "SaveSlot size mismatch — expected 0x172A0 bytes");
static_assert(sizeof(okami::SaveFile) == 30 * sizeof(okami::SaveSlot),
              "SaveFile size mismatch — expected 30x SaveSlot");

// =============================================================================
// Hook infrastructure
// =============================================================================

// Raw pointer for hook callbacks — set by installHooks(), cleared by destructor.
static SaveMan *g_saveMan = nullptr;

// Function signatures (x64 __fastcall, from Ghidra decompilation)
using CtorFn = void(__fastcall *)(void *pContext);
using GateFn = unsigned char(__fastcall *)(void *pMcSave);
// FUN_18014e100: the 5th param is a pointer to a std::_Func_impl-like holder whose
// [7]th qword (offset 0x38) points to the lambda object to be moved/destroyed.
using OkamiWriteKickoffFn = void *(__fastcall *)(void *pTracking, const char *pchFile,
                                                 const void *pvData, uint32_t cubData,
                                                 uintptr_t *pLambdaHolder);
// FUN_18014f580: pure-read wrapper. Returns status code, *pActualSize set on success.
// pDir and pFilename are hx::String* (C-string ptr at +0x08 of the struct).
using OkamiPureReadFn = uint32_t(__fastcall *)(const void *pDir, const void *pFilename,
                                               uint32_t offset, void *pUserBuffer,
                                               uint32_t *pActualSize);
// Original function pointers (populated by hookFunction)
static CtorFn s_origMcSaveCtor = nullptr;
static GateFn s_origSaveGate = nullptr;
static OkamiWriteKickoffFn s_origOkamiWriteKickoff = nullptr;
static OkamiPureReadFn s_origOkamiPureRead = nullptr;

// ISteamRemoteStorage function typedefs (x64 __fastcall)
// Vtable indices from Steamworks SDK ISteamRemoteStorage
using SteamFileExistsFn = bool(__fastcall *)(void *, const char *);                                // [13]
using SteamGetFileSizeFn = int32_t(__fastcall *)(void *, const char *);                            // [15]

// Original function pointers (populated by installSteamRedirect)
static SteamFileExistsFn s_origSteamFileExists = nullptr;
static SteamGetFileSizeFn s_origSteamGetFileSize = nullptr;

// Redirect state
static std::string g_redirectPath;
static std::mutex g_redirectMutex;

// Game passes filenames as "./Steam/OKAMI", "OKAMI", or "Steam/OKAMI" depending on method.
static bool isOkamiFile(const char *pchFile)
{
    if (!pchFile) return false;
    if (std::strcmp(pchFile, "OKAMI") == 0) return true;
    size_t len = std::strlen(pchFile);
    return len >= 6 && std::strcmp(pchFile + len - 6, "/OKAMI") == 0;
}

// Atomic write helper: returns true on success. Caller should already have validated
// that writing to `path` is the intended action (i.e. redirect active).
static bool writeOkamiPayloadAtomic(const std::string &path, const void *pvData, size_t cubData)
{
    const std::string tmpPath = path + ".tmp";
    {
        std::ofstream file(tmpPath, std::ios::binary);
        if (!file.is_open())
            return false;
        file.write(reinterpret_cast<const char *>(pvData), static_cast<std::streamsize>(cubData));
        file.close();
        if (!file.good())
        {
            std::error_code ec;
            std::filesystem::remove(tmpPath, ec);
            return false;
        }
    }
    std::error_code ec;
    std::filesystem::rename(tmpPath, path, ec);
    if (ec)
    {
        std::filesystem::remove(tmpPath, ec);
        return false;
    }
    return true;
}

static bool __fastcall hookSteamFileExists(void *pThis, const char *pchFile)
{
    {
        std::scoped_lock lock(g_redirectMutex);
        if (!g_redirectPath.empty() && isOkamiFile(pchFile))
        {
            bool exists = std::filesystem::exists(g_redirectPath);
            wolf::logDebug("[SaveMan] Steam redirect: FileExists(OKAMI) -> %s", exists ? "true" : "false");
            return exists;
        }
    }
    wolf::logDebug("[Steam] FileExists(\"%s\") -> passthrough", pchFile);
    return s_origSteamFileExists(pThis, pchFile);
}

static int32_t __fastcall hookSteamGetFileSize(void *pThis, const char *pchFile)
{
    {
        std::scoped_lock lock(g_redirectMutex);
        if (!g_redirectPath.empty() && isOkamiFile(pchFile))
        {
            if (std::filesystem::exists(g_redirectPath))
            {
                // Always report 30-slot size regardless of actual file format
                wolf::logDebug("[SaveMan] Steam redirect: GetFileSize(OKAMI) -> %d", static_cast<int32_t>(sizeof(okami::SaveFile)));
                return static_cast<int32_t>(sizeof(okami::SaveFile));
            }
            wolf::logDebug("[SaveMan] Steam redirect: GetFileSize(OKAMI) -> 0 (no file)");
            return 0;
        }
    }
    wolf::logDebug("[Steam] GetFileSize(\"%s\") -> passthrough", pchFile);
    return s_origSteamGetFileSize(pThis, pchFile);
}

// OKAMI async write kickoff hook (FUN_18014e100)
//
// The game's m2::SteamSaveWrite path calls FUN_18014e100 which in turn calls
// ISteamRemoteStorage::FileWriteAsync and registers a CCallResult. The caller
// (FUN_18014fa70) then busy-waits via OSYieldThread until the completion
// lambda flips a flag in the tracking struct's first byte.
//
// Intercepting here lets us:
//   1. Write AP data to the .oksav file directly.
//   2. Skip the Steam call entirely — no handle to manage, no Steam Cloud
//      touch (no OKAMI.apstub pollution).
//   3. Zero the tracking struct so the caller's cleanup guards
//      (`if (local_170 != 0)`, `if (local_188 != null)`) all skip.
//   4. Destroy the source lambda in-place exactly like the original tail did.
static void *__fastcall hookOkamiWriteKickoff(void *pTracking, const char *pchFile,
                                              const void *pvData, uint32_t cubData,
                                              uintptr_t *pLambdaHolder)
{
    if (isOkamiFile(pchFile))
    {
        std::string target;
        {
            std::scoped_lock lock(g_redirectMutex);
            target = g_redirectPath;
        }

        if (!target.empty())
        {
            bool isRealSave = false;
            if (cubData >= 8)
            {
                const auto *header = reinterpret_cast<const uint32_t *>(pvData);
                const uint32_t magic = header[0];
                const uint32_t areaNameStrId = header[1];
                isRealSave = (magic == kHeaderMagic && areaNameStrId < 0x36);
            }

            if (!isRealSave)
            {
                wolf::logInfo("[SaveMan] OKAMI write kickoff: empty-slot scaffolding, skipping .oksav "
                              "(%u bytes, Steam still bypassed)",
                              cubData);
            }
            else if (writeOkamiPayloadAtomic(target, pvData, static_cast<size_t>(cubData)))
            {
                wolf::logInfo("[SaveMan] OKAMI write kickoff -> %s (%u bytes), Steam bypassed",
                              target.c_str(), cubData);
            }
            else
            {
                wolf::logError("[SaveMan] OKAMI write kickoff: writeOkamiPayloadAtomic failed (%s, %u bytes)",
                               target.c_str(), cubData);
            }

            // Zero the ~0x70-byte tracking struct
            std::memset(pTracking, 0, 0x70);

            // Mirror the original's tail: destroy the source lambda if one was supplied.
            // `pLambdaHolder[7]` (offset 0x38) is a pointer to the lambda object.
            // For in-place (stack-embedded) lambdas the pointer equals pLambdaHolder itself;
            // the 2nd dtor arg is "was heap allocated" -> pass `slot != holder`.
            if (pLambdaHolder != nullptr)
            {
                uintptr_t slot = pLambdaHolder[7];
                if (slot != 0)
                {
                    auto *self = reinterpret_cast<uintptr_t *>(slot);
                    auto *vtable = reinterpret_cast<uintptr_t *>(*self);
                    using DtorFn = void(__fastcall *)(void *, bool);
                    auto dtor = reinterpret_cast<DtorFn>(vtable[4]); // vtable[0x20] / 8
                    dtor(self, reinterpret_cast<uintptr_t>(self) !=
                                   reinterpret_cast<uintptr_t>(pLambdaHolder));
                    pLambdaHolder[7] = 0;
                }
            }
            return pTracking;
        }

        // Redirect inactive. If AP mode is still active, something is wrong. log loudly
        // and let the write go through (the outer hookSaveGate should have prevented this).
        if (g_saveMan && g_saveMan->isApModeActive())
        {
            wolf::logError("[SaveMan] OKAMI write kickoff: redirect INACTIVE but AP mode ACTIVE: "
                           "forwarding to Steam (vanilla save at risk)");
        }
        else
        {
            wolf::logInfo("[SaveMan] OKAMI write kickoff -> passthrough (vanilla, %u bytes)", cubData);
        }
    }

    return s_origOkamiWriteKickoff(pTracking, pchFile, pvData, cubData, pLambdaHolder);
}

// The game's pure-read path passes (dir, filename, offset, userBuffer, &size)
// to FUN_18014f580, which does FileExists + GetFileSize + FileReadAsync and
// busy-waits on a completion lambda. Because the user buffer is an explicit
// parameter, we can memcpy .oksav directly into it and skip the Steam round
// trip entirely. Return value 0 = success; 0x8002b40b = file missing/empty
// (matches the original's error path).
// hx::String payload pointer lives at +0x08 of the struct; null -> empty string.
static const char *hxStringCStr(const void *hxStr)
{
    if (hxStr == nullptr)
        return "";
    const char *s = *reinterpret_cast<const char *const *>(
        static_cast<const uint8_t *>(hxStr) + 8);
    return s ? s : "";
}

static uint32_t __fastcall hookOkamiPureRead(const void *pDir, const void *pFilename,
                                             uint32_t offset, void *pUserBuffer,
                                             uint32_t *pActualSize)
{
    const char *filename = hxStringCStr(pFilename);

    if (isOkamiFile(filename))
    {
        std::string target;
        {
            std::scoped_lock lock(g_redirectMutex);
            target = g_redirectPath;
        }

        if (!target.empty())
        {
            std::error_code ec;
            if (!std::filesystem::exists(target, ec))
            {
                wolf::logWarning("[SaveMan] OKAMI pure-read: %s missing: returning error",
                                 target.c_str());
                if (pActualSize != nullptr)
                    *pActualSize = 0;
                return 0x8002b40b;
            }

            std::ifstream file(target, std::ios::binary);
            if (!file.is_open())
            {
                wolf::logError("[SaveMan] OKAMI pure-read: failed to open %s", target.c_str());
                if (pActualSize != nullptr)
                    *pActualSize = 0;
                return 0x8002b40b;
            }

            const uint32_t requested = (pActualSize != nullptr) ? *pActualSize : 0;
            if (requested == 0)
            {
                if (pActualSize != nullptr)
                    *pActualSize = 0;
                return 0x8002b40b;
            }

            file.seekg(offset);
            file.read(static_cast<char *>(pUserBuffer), requested);
            const auto bytesRead = static_cast<uint32_t>(file.gcount());
            if (pActualSize != nullptr)
                *pActualSize = bytesRead;

            wolf::logInfo("[SaveMan] OKAMI pure-read -> %s (offset=%u, %u/%u bytes, Steam bypassed)",
                          target.c_str(), offset, bytesRead, requested);
            return 0; // success
        }

        if (g_saveMan && g_saveMan->isApModeActive())
        {
            wolf::logError("[SaveMan] OKAMI pure-read: redirect INACTIVE but AP mode ACTIVE — "
                           "forwarding to Steam (will read vanilla save)");
        }
    }

    return s_origOkamiPureRead(pDir, pFilename, offset, pUserBuffer, pActualSize);
}

/// Hook: cMcSave gate function (FUN_1801c37d0, vtable[1]).
/// Called by the state machine (FUN_1801bfcb0) to decide whether to show the save UI.
/// Returns 1 -> allocate buffers and open save slot picker (state 0).
/// Returns 0 -> skip directly to cleanup (state 2), no UI ever shown.
/// When AP is connected, we return 0 so the vanilla save menu never opens —
/// this is the save-mirror / save-point interception point (Req 7).
static unsigned char __fastcall hookSaveGate(void *pMcSave)
{
    if (g_saveMan && g_saveMan->isConnected())
    {
        // Let the original run (allocates save buffers that cleanup expects to exist),
        // but override the return value so the state machine skips to state 2 (cleanup)
        // instead of state 0 (show save UI).
        s_origSaveGate(pMcSave);
        wolf::logInfo("[SaveMan] Save gate: skipping vanilla save-mirror UI (AP mode)");
        return 0;
    }
    return s_origSaveGate(pMcSave);
}

/// Hook: cMcSave constructor — when AP connected, snapshot game state to .oksav.
/// The vanilla ctor still runs afterward, but hookSaveGate returns 0 so the state
/// machine skips directly to cleanup — no save slot picker, no Steam Cloud write.
static void __fastcall hookMcSaveCtor(void *pContext)
{
    if (!g_saveMan)
    {
        wolf::logError("[SaveMan] hookMcSaveCtor fired with g_saveMan=null — mod not fully initialized");
        s_origMcSaveCtor(pContext);
        return;
    }

    if (g_saveMan->isConnected())
    {
        g_saveMan->setApModeActive(true);
        if (!g_saveMan->saveGameState())
        {
            wolf::logError("[SaveMan] cMcSave ctor (save-mirror): saveGameState failed");
        }
        else
        {
            notificationwindow::queue("AP progress saved", 3.0f);
            wolf::logInfo("[SaveMan] AP save (save-mirror): game state saved to %s",
                          g_saveMan->getSavePath().c_str());
        }
    }
    else
    {
        wolf::logDebug("[SaveMan] cMcSave constructor (vanilla save path)");
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
    if (SAVE_DIR.empty())
    {
        wolf::logError("[SaveMan] Save directory unavailable; disabling AP save system");
        return;
    }

    moduleBase_ = wolf::getModuleBase("main.dll");
    if (moduleBase_ == 0)
    {
        wolf::logError("[SaveMan] Failed to get main.dll base address");
        return;
    }

    characterStatsAddr_ = moduleBase_ + kCharacterStats;
    trackerDataAddr_ = moduleBase_ + kTrackerData;
    collectionDataAddr_ = moduleBase_ + kCollectionData;
    mapDataAddr_ = moduleBase_ + kMapData;
    dialogBitsAddr_ = moduleBase_ + kDialogBits;
    customTexturesAddr_ = moduleBase_ + kCustomTextures;
    initialized_ = true;
    wolf::logInfo("[SaveMan] Initialized (base=0x%llX)", moduleBase_);
}

// =============================================================================
// Core save/load
// =============================================================================

bool SaveMan::saveGameState()
{
    if (!initialized_)
    {
        wolf::logError("[SaveMan] saveGameState: not initialized");
        logState("saveGameState");
        return false;
    }

    okami::SaveSlot slot;
    std::memset(&slot, 0, sizeof(slot));

    snapshotToSlot(slot);
    slot.checksum = computeChecksum(slot);

    wolf::logDebug("[SaveMan] Slot header: magic=0x%08X, +04=0x%08X, checksum=0x%llX, time=0x%llX",
                   slot.header, slot.areaNameStrId, slot.checksum, slot.timeRTC);

    if (!writeSlotToFile(slot))
    {
        wolf::logError("[SaveMan] saveGameState: failed to write save file");
        logState("saveGameState");
        return false;
    }

    // Any successful save — whether from processAutoSave, save-mirror, or manual —
    // satisfies the pending auto-save request. Clear the debounce state so the next
    // check-sent queue starts a fresh 30s deferral clock.
    {
        std::scoped_lock lock(autoSaveMutex_);
        autoSavePending_ = false;
        deferralWarned_ = false;
    }

    wolf::logInfo("[SaveMan] Game state saved to %s", getSavePath().c_str());
    return true;
}

bool SaveMan::loadGameState()
{
    if (!initialized_)
    {
        wolf::logError("[SaveMan] loadGameState: not initialized");
        logState("loadGameState");
        return false;
    }

    okami::SaveSlot slot;
    if (!readSlotFromFile(slot))
    {
        wolf::logError("[SaveMan] loadGameState: failed to read save file");
        logState("loadGameState");
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

bool SaveMan::hasAnySaveFile()
{
    try
    {
        if (!std::filesystem::exists(SAVE_DIR))
            return false;
        for (const auto &entry : std::filesystem::directory_iterator(SAVE_DIR))
        {
            if (entry.path().extension() == ".oksav")
                return true;
        }
    }
    catch (...)
    {
    }
    return false;
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

// =============================================================================
// Auto-save
// =============================================================================

void SaveMan::queueAutoSave()
{
    std::scoped_lock lock(autoSaveMutex_);
    auto now = std::chrono::steady_clock::now();
    if (!autoSavePending_)
        autoSaveQueuedAt_ = now;
    autoSavePending_ = true;
    lastAutoSaveRequest_ = now;
    deferralWarned_ = false;
}

void SaveMan::processAutoSave()
{
    if (!autoSavePending_ || !apModeActive_)
        return;

    auto now = std::chrono::steady_clock::now();

    {
        std::scoped_lock lock(autoSaveMutex_);
        if (now - lastAutoSaveRequest_ < AUTO_SAVE_DEBOUNCE)
            return;
    }

    if (!isSafeToSave())
    {
        // Keep pending; retry next tick. Log once if we've been deferring for a long time.
        std::scoped_lock lock(autoSaveMutex_);
        if (!deferralWarned_ && now - autoSaveQueuedAt_ >= AUTO_SAVE_DEFER_WARN)
        {
            wolf::logWarning("[SaveMan] Auto-save deferred >30s — game state not safe "
                             "(in load / save / title / map transition)");
            deferralWarned_ = true;
        }
        return;
    }

    {
        std::scoped_lock lock(autoSaveMutex_);
        autoSavePending_ = false;
        deferralWarned_ = false;
    }

    (void)saveGameState();
}

bool SaveMan::isSafeToSave() const
{
    if (!apModeActive_)
        return false;
    if (!initialized_)
        return false;

    // Title screen — no gameplay state to snapshot.
    uint16_t mapId = *reinterpret_cast<const uint16_t *>(moduleBase_ + okami::main::exteriorMapID);
    if (mapId == static_cast<uint16_t>(MapID::TitleScreen) ||
        mapId == static_cast<uint16_t>(MapID::TitleScreenDemoCutscene))
        return false;

    // Save operation already in flight — bit 22 of systemFlags is the cMcSys
    // blocking-loop "busy" bit, set only while cMcSave/cMcLoad/cMcBoot is running.
    // This is the only reliable busy signal in saveStateFlags / systemFlags. We
    // deliberately do NOT check saveStateFlags bits 6 or 8:
    //   - bit 6 is a sticky "New Game entry occurred" marker (set by FUN_180438810).
    //   - bit 8 is a transient one-shot signal set by the boot thread to trigger
    //     the initial area load, cleared by FUN_180489770 on consumption. During
    //     normal gameplay it is almost always 0.
    uint32_t sysFlags = *reinterpret_cast<const uint32_t *>(moduleBase_ + okami::main::systemFlags);
    if (sysFlags & (1u << 22))
        return false;

    // Area-load pipeline active — any bits in 0x6001000 mask set means map load in progress.
    uint32_t areaFlags = *reinterpret_cast<const uint32_t *>(moduleBase_ + okami::main::areaLoadFlags);
    if (areaFlags & 0x6001000u)
        return false;

    return true;
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

    // areaNameID at +0x04: title-screen location label (0x00–0x35 valid).
    // Look up from the live exterior map ID — 0x79BEB4 is only populated
    // inside the game's own save routine and reads 0xFFFFFFFF otherwise.
    uint16_t mapId = *reinterpret_cast<const uint16_t *>(
        moduleBase_ + okami::main::exteriorMapID);
    slot.areaNameStrId = okami::getAreaNameID(mapId);

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

    // CharacterStats.x/y/z (position) and u/v/w (rotation) are flagged "set from
    // elsewhere" in structs.hpp. The game populates them in FUN_18043b040 right
    // before vanilla save by reading the Amaterasu object at ammyModel (0xB6B2D0):
    //   position comes via pointer at ammy+0xA8 -> vec3 {x,y,z}
    //   rotation is inline at ammy+0xB0..+0xB8 as three floats
    // hookMcSaveCtor fires before that step, so the live values in CharacterStats
    // are stale — leaving the player at the map's default spawn on reload (often
    // origin, outside geometry). Patch them in from the live sources here.
    {
        uintptr_t ammy = *reinterpret_cast<const uintptr_t *>(
            moduleBase_ + okami::main::ammyModel);
        if (ammy != 0)
        {
            const auto *posPtr = *reinterpret_cast<const float *const *>(ammy + 0xA8);
            if (posPtr != nullptr)
            {
                slot.character.x = posPtr[0];
                slot.character.y = posPtr[1];
                slot.character.z = posPtr[2];
            }
            const auto *rot = reinterpret_cast<const float *>(ammy + 0xB0);
            slot.character.u = rot[0];
            slot.character.v = rot[1];
            slot.character.w = rot[2];
        }
    }

    std::memcpy(&slot.tracked, reinterpret_cast<const void *>(trackerDataAddr_),
                sizeof(okami::TrackerData));

    std::memcpy(&slot.collection, reinterpret_cast<const void *>(collectionDataAddr_),
                sizeof(okami::CollectionData));

    // CollectionData carries map-identity fields (currentMapId at +0x02,
    // lastMapId at +0x04) that the comment in structs.hpp flags as
    // "only loaded into this struct on save". The game's own pre-save
    // step (FUN_1801c2a40) populates these from the live exteriorMapID /
    // exteriorMapIDCopy globals. hookMcSaveCtor runs BEFORE that step,
    // so the in-memory values here are stale — typically the parent/"top"
    // map instead of the sub-map the player is actually in, which is why
    // reload spawns in the wrong room. Patch them from the live sources.
    {
        slot.collection.currentMapId = mapId; // already read from exteriorMapID
        slot.collection.lastMapId = *reinterpret_cast<const uint16_t *>(
            moduleBase_ + okami::main::exteriorMapIDCopy);
    }

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

        // Read-modify-write: preserve existing slots 1-29 (game-created format),
        // only replace slot 0 with our AP data.
        auto saveFile = std::make_unique<okami::SaveFile>();
        {
            std::ifstream existing(path, std::ios::binary);
            if (existing.is_open() && existing.read(reinterpret_cast<char *>(saveFile.get()), sizeof(okami::SaveFile)))
            {
                // Existing file read OK — slots 1-29 preserved
            }
            else
            {
                // No existing file or read failed — initialize to match game's empty-slot format:
                // all zeros except areaNameStrId = 0xFFFFFFFF per slot.
                std::memset(saveFile.get(), 0, sizeof(okami::SaveFile));
                for (int i = 0; i < 30; ++i)
                    saveFile->slots[i].areaNameStrId = 0xFFFFFFFF;
            }
        }
        std::memcpy(&saveFile->slots[0], &slot, sizeof(okami::SaveSlot));

        file.write(reinterpret_cast<const char *>(saveFile.get()), sizeof(okami::SaveFile));
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
        if (fileSize != sizeof(okami::SaveFile))
        {
            wolf::logError("[SaveMan] Invalid save file size: %zu (expected %zu)",
                           static_cast<size_t>(fileSize), sizeof(okami::SaveFile));
            return false;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            wolf::logError("[SaveMan] Failed to open %s for reading", path.c_str());
            return false;
        }

        // AP data is always in slot 0
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
    constexpr int kTotalHooks = 4;

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

    if (wolf::hookFunction("main.dll", kOkamiWriteKickoffOffset,
                           reinterpret_cast<void *>(&hookOkamiWriteKickoff),
                           reinterpret_cast<void **>(&s_origOkamiWriteKickoff)))
        ++installed;
    else
        wolf::logError("[SaveMan] FAILED: OKAMI write kickoff hook at 0x%zX",
                       kOkamiWriteKickoffOffset);

    if (wolf::hookFunction("main.dll", kOkamiPureReadOffset,
                           reinterpret_cast<void *>(&hookOkamiPureRead),
                           reinterpret_cast<void **>(&s_origOkamiPureRead)))
        ++installed;
    else
        wolf::logError("[SaveMan] FAILED: OKAMI pure-read hook at 0x%zX",
                       kOkamiPureReadOffset);

    wolf::logInfo("[SaveMan] Hooks installed: %d/%d", installed, kTotalHooks);

    installSteamRedirect();
}

// =============================================================================
// ISteamRemoteStorage redirect
// =============================================================================

void SaveMan::installSteamRedirect()
{
#ifdef _WIN32
    uintptr_t mainBase = wolf::getModuleBase("main.dll");
    if (mainBase == 0)
    {
        wolf::logError("[SaveMan] Steam redirect: main.dll base not found");
        return;
    }

    HMODULE steamApi = GetModuleHandleA("steam_api64.dll");
    if (!steamApi)
    {
        wolf::logError("[SaveMan] Steam redirect: steam_api64.dll not loaded");
        return;
    }

    using ContextInitFn = void *(*)(void *);
    auto contextInit = reinterpret_cast<ContextInitFn>(GetProcAddress(steamApi, "SteamInternal_ContextInit"));
    if (!contextInit)
    {
        wolf::logError("[SaveMan] Steam redirect: SteamInternal_ContextInit not found");
        return;
    }

    // Game stores context init data at mainBase + 0x792CB0 (confirmed by decompilation of FUN_180150560)
    void *ctx = contextInit(reinterpret_cast<void *>(mainBase + 0x792CB0));
    if (!ctx)
    {
        wolf::logError("[SaveMan] Steam redirect: SteamInternal_ContextInit returned null");
        return;
    }

    // Try offset 0x48 first (confirmed by decompilation), fall back to 0x40 (standard SDK)
    void *remoteStorage = *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(ctx) + 0x48);
    if (!remoteStorage)
    {
        wolf::logWarning("[SaveMan] Steam redirect: ctx+0x48 is null, trying ctx+0x40");
        remoteStorage = *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(ctx) + 0x40);
    }
    if (!remoteStorage)
    {
        wolf::logError("[SaveMan] Steam redirect: ISteamRemoteStorage is null");
        return;
    }

    void **vtable = *reinterpret_cast<void ***>(remoteStorage);
    if (!vtable)
    {
        wolf::logError("[SaveMan] Steam redirect: vtable is null");
        return;
    }

    wolf::logInfo("[SaveMan] ISteamRemoteStorage at %p, vtable at %p", remoteStorage, vtable);

    // Direct vtable patching instead of MinHook inline hooks.
    // steamclient64.dll functions are only ~0x40 bytes each — MinHook's 14-byte
    // x64 JMP trampoline corrupts adjacent function entries, causing stack overflow.
    // Vtable patching is safe: save original pointer, overwrite entry, done.
    DWORD oldProtect;
    int steamInstalled = 0;
    constexpr int kTotalSteamHooks = 2;

    auto patchVtable = [&](int idx, const char *name, void *hookFn, void **origPtr) {
        *origPtr = vtable[idx];
        if (VirtualProtect(&vtable[idx], sizeof(void *), PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            vtable[idx] = hookFn;
            VirtualProtect(&vtable[idx], sizeof(void *), oldProtect, &oldProtect);
            ++steamInstalled;
            wolf::logInfo("[SaveMan] Patched vtable[%d] %s: %p -> %p", idx, name, *origPtr, hookFn);
        }
        else
        {
            wolf::logError("[SaveMan] FAILED to patch vtable[%d] %s (VirtualProtect failed)", idx, name);
            *origPtr = nullptr;
        }
    };

    patchVtable(13, "FileExists",
                reinterpret_cast<void *>(static_cast<SteamFileExistsFn>(&hookSteamFileExists)),
                reinterpret_cast<void **>(&s_origSteamFileExists));
    patchVtable(15, "GetFileSize",
                reinterpret_cast<void *>(static_cast<SteamGetFileSizeFn>(&hookSteamGetFileSize)),
                reinterpret_cast<void **>(&s_origSteamGetFileSize));

    wolf::logInfo("[SaveMan] Steam vtable patches: %d/%d installed", steamInstalled, kTotalSteamHooks);
#endif
}

void SaveMan::activateRedirect(const std::string &path)
{
    std::scoped_lock lock(g_redirectMutex);
    g_redirectPath = path;
    wolf::logInfo("[SaveMan] Steam redirect activated: %s", path.c_str());
}

void SaveMan::deactivateRedirect()
{
    std::scoped_lock lock(g_redirectMutex);
    g_redirectPath.clear();
    wolf::logInfo("[SaveMan] Steam redirect deactivated");
}

bool SaveMan::isRedirectActive() const
{
    std::scoped_lock lock(g_redirectMutex);
    return !g_redirectPath.empty();
}

void SaveMan::logState(const char *context) const
{
    std::string path = getSavePath();
    bool fileExists = false;
    uintmax_t fileSize = 0;
    if (!path.empty())
    {
        std::error_code ec;
        fileExists = std::filesystem::exists(path, ec);
        if (fileExists)
            fileSize = std::filesystem::file_size(path, ec);
    }
    std::string redirectPath;
    {
        std::scoped_lock lock(g_redirectMutex);
        redirectPath = g_redirectPath;
    }
    wolf::logInfo("[SaveMan] state(%s): apMode=%d connected=%d initialized=%d "
                  "path='%s' exists=%d size=%llu redirect='%s'",
                  context ? context : "",
                  static_cast<int>(apModeActive_.load()),
                  static_cast<int>(isConnected()),
                  static_cast<int>(initialized_),
                  path.c_str(),
                  fileExists ? 1 : 0,
                  static_cast<unsigned long long>(fileSize),
                  redirectPath.c_str());
}
