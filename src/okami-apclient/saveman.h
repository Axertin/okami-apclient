#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>

#include <okami/savefile.hpp>

class ISocket;

/**
 * @brief AP save file manager
 *
 * Reads/writes game state directly from/to the 6 known memory regions,
 * completely bypassing the game's Steam Cloud save pipeline.
 * AP saves are stored as single-slot .oksav files in %APPDATA%/okami-apsaves/.
 */
class SaveMan
{
  public:
    explicit SaveMan(ISocket &socket);
    ~SaveMan();

    /// Initialize memory accessors. Must be called after main.dll is loaded.
    void initialize();

    // === Core save/load ===

    /// Snapshot all 6 memory regions, compute checksum, write to AP file
    [[nodiscard]] bool saveGameState();

    /// Read AP file, verify checksum, write all 6 memory regions back.
    /// Test-only: in production, OKAMI reads are served by hookOkamiPureRead
    /// writing directly into the game's read buffer.
    [[nodiscard]] bool loadGameState();

    /// Check if an AP save exists for the current connection
    [[nodiscard]] bool hasSaveFile() const;

    /// Check if ANY .oksav file exists (doesn't require socket connection)
    [[nodiscard]] static bool hasAnySaveFile();

    /// Delete the AP save for the current connection
    [[nodiscard]] bool deleteSave();

    // === AP mode management ===

    bool isApModeActive() const;
    void setApModeActive(bool active);
    bool isConnected() const;

    // === Auto-save support ===

    /// Queue an auto-save (called after check send)
    void queueAutoSave();

    /// Process pending auto-save with debounce (called from game tick)
    void processAutoSave();

    /// Returns true iff the current game state is safe to snapshot to disk:
    /// AP mode active, not on title screen, no save/load/area-load in flight.
    /// Used by processAutoSave to defer saves that would race with the engine.
    bool isSafeToSave() const;

    /// Get the full path to the current AP save file
    std::string getSavePath() const;

    // === Hooks ===

    /// Install hooks on the game's save/load functions.
    /// Must be called after initialize().
    void installHooks();

    /// Activate file redirect to the given path
    void activateRedirect(const std::string &path);

    /// Deactivate file redirect.
    void deactivateRedirect();

    /// Check if redirect is currently active.
    bool isRedirectActive() const;

    /// Compute XOR checksum (seed 0x9be6fa3b72afda1d) matching the game's algorithm.
    /// Covers slot[0] (header) and slot[0x10..end], skipping the checksum field at +0x08.
    static uint64_t computeChecksum(const okami::SaveSlot &slot);

    /// Emit a compact one-liner summarizing SaveMan state. Called by error paths
    /// so operators can diagnose what the mod believed at failure time.
    void logState(const char *context) const;

  private:
    ISocket &socket_;
    std::atomic<bool> apModeActive_{false};
    bool initialized_ = false;
    uintptr_t moduleBase_ = 0;

    /// Install pass-through hooks on ISteamRemoteStorage vtable methods.
    void installSteamRedirect();

    // === Memory snapshot helpers ===
    void snapshotToSlot(okami::SaveSlot &slot);

    /// Test-only. Production loads are serviced by hookOkamiPureRead.
    void restoreFromSlot(const okami::SaveSlot &slot);

    // === File I/O ===
    [[nodiscard]] bool writeSlotToFile(const okami::SaveSlot &slot);

    /// Read AP save from .oksav file into slot.
    /// Test-only. Production loads are serviced by hookOkamiPureRead.
    [[nodiscard]] bool readSlotFromFile(okami::SaveSlot &slot) const;

    // === Auto-save state ===
    std::atomic<bool> autoSavePending_{false};
    std::chrono::steady_clock::time_point lastAutoSaveRequest_;
    std::chrono::steady_clock::time_point autoSaveQueuedAt_;
    bool deferralWarned_ = false;
    std::mutex autoSaveMutex_;
    static constexpr auto AUTO_SAVE_DEBOUNCE = std::chrono::milliseconds(500);
    static constexpr auto AUTO_SAVE_DEFER_WARN = std::chrono::seconds(30);

    // === Memory addresses (resolved at init) ===
    uintptr_t characterStatsAddr_ = 0;
    uintptr_t trackerDataAddr_ = 0;
    uintptr_t collectionDataAddr_ = 0;
    uintptr_t mapDataAddr_ = 0;
    uintptr_t dialogBitsAddr_ = 0;
    uintptr_t customTexturesAddr_ = 0;
};
