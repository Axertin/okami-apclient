#pragma once

#include <cstdint>
#include <functional>

namespace checks
{

/**
 * @brief Watches the game's native brush-bit-edit function and turns SETs into
 * AP location checks.
 *
 * The implementation hooks `oEditBrushes` (main.dll +0x17C270) via wolf's
 * `onBrushEdit` callback. The hook receives `(bitIndex, operation)` directly
 * in the game's BrushOverlay convention (LSB-first within byte). Operation 0
 * (default) is a SET of both usable & obtained; ops 1/2/3 are clears we ignore.
 *
 * For SETs, BrushMan fires a check and blocks the original write so the brush
 * isn't unlocked from the dojo — the player receives whatever AP randomized to
 * that location instead.
 *
 * This replaces the prior memory-polling design, which had to fight a
 * BrushData->WorldStateData sync and required muting during reward grants.
 * Grants write memory directly without going through `oEditBrushes`, so no
 * coordination is needed.
 */
class BrushMan
{
  public:
    using CheckCallback = std::function<void(int64_t)>;

    explicit BrushMan(CheckCallback checkCallback);
    ~BrushMan();

    BrushMan(const BrushMan &) = delete;
    BrushMan &operator=(const BrushMan &) = delete;
    BrushMan(BrushMan &&) = delete;
    BrushMan &operator=(BrushMan &&) = delete;

    void initialize();
    void shutdown();
    void reset();

    // Per-tick housekeeping. Currently clears the "Celestial Brush Locked"
    // map-state bit so the in-game brush UI stays usable: the game sets it
    // during scene transitions (e.g. entering River of the Heavens for the
    // Sakuya cutscene), but in AP mode the player gets brushes via AP items,
    // so this gate is wrong.
    void tick();

  private:
    CheckCallback checkCallback_;
    bool initialized_ = false;
};

namespace detail
{

// Test-only: clear the cached "dispatcher lambda already registered with wolf"
// flag. Production registers exactly once per process; tests call this after
// wolf::mock::reset() (which wipes wolf's callback list) so the next
// BrushMan::initialize() re-registers a fresh dispatcher.
void resetBrushHookRegistrationForTests();

} // namespace detail

} // namespace checks
