#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>

namespace checks
{

/**
 * @brief Hook-based brush-acquisition detection.
 *
 * Hooks main.dll +0x17C270 via wolf::onBrushEdit. The hook runs inside wolf's
 * g_CallbackMutex, so we do as little as possible there: bounds-check,
 * transition-check, queue, return. checkCallback runs from tick() on the
 * game tick.
 *
 * Blocking is gated by setActive(true). Inactive = diagnostic no-op.
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

    // Enable/disable block-and-send. Safe from any thread.
    void setActive(bool active);

    // Drains queued checks; clears Celestial Brush Lock UI gate. Called from
    // CheckMan::poll().
    void tick();

  private:
    CheckCallback checkCallback_;
    bool initialized_ = false;

    std::mutex pendingMutex_;
    std::vector<int64_t> pendingChecks_;

    friend bool dispatchBrushEdit(int bitIndex, int operation, BrushMan &handler);
};

namespace detail
{

// Test-only: reset the static "dispatcher lambda registered" flag after
// wolf::mock::reset() so initialize() re-registers a fresh dispatcher.
void resetBrushHookRegistrationForTests();

} // namespace detail

} // namespace checks
