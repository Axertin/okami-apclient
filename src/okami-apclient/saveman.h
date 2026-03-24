#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>

#include <okami/savefile.hpp>

class ISocket;

/**
 * @brief AP save file manager (Strategy B: direct memory snapshot)
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
    bool saveGameState();

    /// Read AP file, verify checksum, write all 6 memory regions back
    bool loadGameState();

    /// Check if an AP save exists for the current connection
    bool hasSaveFile() const;

    /// Delete the AP save for the current connection
    bool deleteSave();

    // === AP mode management ===

    bool isApModeActive() const;
    void setApModeActive(bool active);
    bool isConnected() const;

    /// Flag a pending AP restore — set by McLoadCtor hook, consumed by onPlayStart.
    void setPendingLoad(bool pending);
    bool isPendingLoad() const;

    // === Auto-save support ===

    /// Queue an auto-save (called after check send)
    void queueAutoSave();

    /// Process pending auto-save with debounce (called from game tick)
    void processAutoSave();

    /// Get the full path to the current AP save file
    std::string getSavePath() const;

    // === Hooks ===

    /// Install hooks on the game's save/load functions.
    /// Must be called after initialize().
    void installHooks();

    /// Compute XOR checksum (seed 0x9be6fa3b72afda1d) matching the game's algorithm.
    /// Covers slot[0] (header) and slot[0x10..end], skipping the checksum field at +0x08.
    static uint64_t computeChecksum(const okami::SaveSlot &slot);

  private:
    ISocket &socket_;
    std::atomic<bool> apModeActive_{false};
    std::atomic<bool> pendingApLoad_{false};
    bool initialized_ = false;

    // === Memory snapshot helpers ===
    void snapshotToSlot(okami::SaveSlot &slot);
    void restoreFromSlot(const okami::SaveSlot &slot);

    // === File I/O ===
    bool writeSlotToFile(const okami::SaveSlot &slot);
    bool readSlotFromFile(okami::SaveSlot &slot) const;

    // === Auto-save state ===
    std::atomic<bool> autoSavePending_{false};
    std::chrono::steady_clock::time_point lastAutoSaveRequest_;
    std::mutex autoSaveMutex_;
    static constexpr auto AUTO_SAVE_DEBOUNCE = std::chrono::milliseconds(500);

    // === Memory addresses (resolved at init) ===
    uintptr_t characterStatsAddr_ = 0;
    uintptr_t trackerDataAddr_ = 0;
    uintptr_t collectionDataAddr_ = 0;
    uintptr_t mapDataAddr_ = 0;
    uintptr_t dialogBitsAddr_ = 0;
    uintptr_t customTexturesAddr_ = 0;
    uintptr_t areaNameStrIdAddr_ = 0;
};
