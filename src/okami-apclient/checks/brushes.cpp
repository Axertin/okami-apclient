#include "brushes.hpp"

#include <atomic>
#include <cinttypes>

#include <wolf_framework.hpp>

#include "../gamestate_accessors.hpp"
#include "check_types.hpp"

namespace checks
{

namespace
{

// Operation codes for FUN_18017c270:
//   0 -> SET both usable & obtained (default)
//   1 -> clear usable
//   2 -> clear both
//   3 -> clear obtained
constexpr int kBrushOpSet = 0;
constexpr int kBrushOpClearUsable = 1;
constexpr int kBrushOpClearBoth = 2;
constexpr int kBrushOpClearObtained = 3;

constexpr int kBrushBitMin = 0;
constexpr int kBrushBitMax = 31; // BrushOverlay enum lives in [0..31]

bool isClearOp(int operation)
{
    return operation == kBrushOpClearUsable || operation == kBrushOpClearBoth || operation == kBrushOpClearObtained;
}

// One dispatcher per process. wolf has no unregister API, so we keep a static
// active-handler atomic and the dispatcher delegates to whichever BrushMan is
// currently active.
std::atomic<BrushMan *> g_activeHandler{nullptr};
std::atomic<bool> g_active{false};
bool g_dispatcherInstalled = false;

// Read obtained-bit from BrushData source. Returns false if accessor unbound
// (we treat unknown state as "not obtained" but the caller should also
// gate on g_active so this only matters in tests/edge cases).
bool isObtainedBitSet(int bitIndex)
{
    if (!apgame::obtainedBrushesSource.is_bound())
        return false;
    const auto *bytes = reinterpret_cast<const volatile uint8_t *>(apgame::obtainedBrushesSource.get_ptr());
    return (bytes[bitIndex / 8] & static_cast<uint8_t>(1u << (bitIndex % 8))) != 0;
}

} // namespace

bool dispatchBrushEdit(int bitIndex, int operation, BrushMan &handler)
{
    if (isClearOp(operation))
        return false;

    // Anything other than SET (0) is unexpected — log and pass through.
    if (operation != kBrushOpSet)
    {
        wolf::logWarning("[BrushMan] unknown op=%d bit=%d -> passthrough", operation, bitIndex);
        return false;
    }

    // The game function masks bit indices into 32 bits (param_2 & 0x8000001f),
    // so out-of-range values can occur. Not a brush event; never block.
    if (bitIndex < kBrushBitMin || bitIndex > kBrushBitMax)
    {
        wolf::logWarning("[BrushMan] out-of-range bit=%d -> passthrough", bitIndex);
        return false;
    }

    if (!g_active.load(std::memory_order_acquire))
        return false;

    // Transition gate: re-sets (save restore, cutscene bookkeeping) pass through.
    if (isObtainedBitSet(bitIndex))
        return false;

    const int64_t checkId = getBrushCheckId(bitIndex);
    {
        std::lock_guard<std::mutex> lock(handler.pendingMutex_);
        handler.pendingChecks_.push_back(checkId);
    }
    return true;
}

BrushMan::BrushMan(CheckCallback checkCallback) : checkCallback_(std::move(checkCallback))
{
    wolf::logDebug("[BrushMan] constructed");
}

BrushMan::~BrushMan()
{
    shutdown();
}

void BrushMan::initialize()
{
    if (initialized_)
    {
        wolf::logWarning("[BrushMan] initialize called twice");
        return;
    }

    g_activeHandler.store(this, std::memory_order_release);

    if (!g_dispatcherInstalled)
    {
        wolf::onBrushEdit(
            [](int bitIndex, int operation) -> bool
            {
                BrushMan *h = g_activeHandler.load(std::memory_order_acquire);
                if (!h)
                    return false;
                return dispatchBrushEdit(bitIndex, operation, *h);
            });
        g_dispatcherInstalled = true;
        wolf::logInfo("[BrushMan] dispatcher installed");
    }

    initialized_ = true;
}

void BrushMan::shutdown()
{
    if (!initialized_)
        return;

    g_active.store(false, std::memory_order_release);

    BrushMan *expected = this;
    g_activeHandler.compare_exchange_strong(expected, nullptr);

    {
        std::lock_guard<std::mutex> lock(pendingMutex_);
        if (!pendingChecks_.empty())
            wolf::logWarning("[BrushMan] shutdown discarding %zu queued checks", pendingChecks_.size());
        pendingChecks_.clear();
    }

    initialized_ = false;
    wolf::logDebug("[BrushMan] shutdown");
}

void BrushMan::reset()
{
    shutdown();
}

void BrushMan::setActive(bool active)
{
    const bool prev = g_active.exchange(active, std::memory_order_acq_rel);
    if (prev != active)
        wolf::logInfo("[BrushMan] active %d -> %d", prev ? 1 : 0, active ? 1 : 0);
}

void BrushMan::tick()
{
    if (!initialized_)
        return;

    // Drain queued checks. We swap-out under the lock so checkCallback runs
    // outside it (callback may take other mutexes / do I/O).
    std::vector<int64_t> drained;
    {
        std::lock_guard<std::mutex> lock(pendingMutex_);
        drained.swap(pendingChecks_);
    }
    for (int64_t id : drained)
    {
        wolf::logInfo("[BrushMan] check sent for bit=%d (id=%" PRId64 ")", static_cast<int>(id - checks::kBrushAcquisitionBase), id);
        if (checkCallback_)
            checkCallback_(id);
    }

    // Clear Celestial Brush Locked UI gate. The game sets this bit during
    // scene transitions (e.g. River of the Heavens entry); in AP mode we
    // grant brushes via AP items, so the gate must stay down.
    if (apgame::worldStateData.is_bound())
        apgame::worldStateData->mapStateBits[0].Clear(10);
}

namespace detail
{

void resetBrushHookRegistrationForTests()
{
    g_activeHandler.store(nullptr);
    g_active.store(false);
    g_dispatcherInstalled = false;
}

} // namespace detail

} // namespace checks
