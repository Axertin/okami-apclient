#include "brushes.hpp"

#include <atomic>

#include <wolf_framework.hpp>

#include "../gamestate_accessors.hpp"
#include "check_types.hpp"

namespace checks
{

namespace
{

// Operation codes for oEditBrushes (main.dll +0x17C270):
//   default (0)  -> SET both usable and obtained
//   1            -> clear usable
//   2            -> clear both
//   3            -> clear obtained
constexpr int kBrushOpClearUsable = 1;
constexpr int kBrushOpClearBoth = 2;
constexpr int kBrushOpClearObtained = 3;

bool isClearOp(int operation)
{
    return operation == kBrushOpClearUsable || operation == kBrushOpClearBoth || operation == kBrushOpClearObtained;
}

// wolf::onBrushEdit has no unregister API and captures into a runtime-owned
// callback list. We register exactly one dispatcher lambda per process that
// delegates to whichever BrushMan instance is currently active. The atomic
// pointer prevents a destroyed instance from dangling: shutdown() clears it
// and the dispatcher safely no-ops.
std::atomic<BrushMan *> g_activeHandler{nullptr};
bool g_dispatcherInstalled = false;

bool dispatchBrushEdit(int bitIndex, int operation, BrushMan::CheckCallback &checkCallback)
{
    if (isClearOp(operation))
        return false;

    checkCallback(getBrushCheckId(bitIndex));
    wolf::logDebug("[BrushMan] Brush set blocked, sent check %lld for brush idx %d", getBrushCheckId(bitIndex), bitIndex);
    // Block the original SET so the brush isn't unlocked from this event —
    // the AP item flow grants the actual reward.
    return true;
}

} // namespace

BrushMan::BrushMan(CheckCallback checkCallback) : checkCallback_(std::move(checkCallback))
{
}

BrushMan::~BrushMan()
{
    shutdown();
}

void BrushMan::initialize()
{
    if (initialized_)
        return;

    g_activeHandler.store(this);

    if (!g_dispatcherInstalled)
    {
        wolf::onBrushEdit(
            [](int bitIndex, int operation) -> bool
            {
                BrushMan *h = g_activeHandler.load();
                if (!h)
                    return false;
                return dispatchBrushEdit(bitIndex, operation, h->checkCallback_);
            });
        g_dispatcherInstalled = true;
    }

    initialized_ = true;
    wolf::logInfo("[BrushMan] Brush edit hook installed");
}

void BrushMan::shutdown()
{
    if (!initialized_)
        return;

    BrushMan *expected = this;
    g_activeHandler.compare_exchange_strong(expected, nullptr);
    initialized_ = false;
}

void BrushMan::reset()
{
    shutdown();
}

void BrushMan::tick()
{
    if (!initialized_)
        return;

    // The grant-time clear in rewards/brushes.cpp fires once when an item is
    // received — but on a fresh playthrough the game sets this bit during the
    // post-grant intro cutscene transition, leaving the UI permanently locked
    // until the bit is cleared again. Clearing it every tick is sub-microsecond
    // and reliably keeps the gate down.
    apgame::worldStateData->mapStateBits[0].Clear(10);
}

namespace detail
{

void resetBrushHookRegistrationForTests()
{
    g_activeHandler.store(nullptr);
    g_dispatcherInstalled = false;
}

} // namespace detail

} // namespace checks
