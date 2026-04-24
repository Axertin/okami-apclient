#include "checkman.h"

#include <cinttypes>

#include "checks/brushes.hpp"
#include "checks/containers.hpp"
#include "checks/gamestate_monitors.hpp"
#include "checks/shops.hpp"
#include "isocket.h"

CheckMan::CheckMan(ISocket &socket) : socket_(socket)
{
}

CheckMan::~CheckMan()
{
    shutdown();
}

void CheckMan::initialize()
{
    if (initialized_)
    {
        wolf::logWarning("[CheckMan] Already initialized");
        return;
    }

    wolf::logInfo("[CheckMan] Initializing check manager");

    // Register gameplay state callbacks
    wolf::onPlayStart([this]() { enableSending(true); });
    wolf::onReturnToMenu([this]() { enableSending(false); });

    // Create callback for all monitors
    auto callback = [this](int64_t checkId) { sendCheck(checkId); };

    // Gamestate bitfield monitors (gameProgress, globalFlags, worldState,
    // collectedObjects, areasRestored) are disabled for now — they fire
    // frequently and aren't consumed by the APWorld yet.

    // Set up brush handler only when RandomizeBrushes is enabled.
    if (socket_.isSlotConfigReady() && socket_.getSlotConfig().randomizeBrushes)
    {
        brushHandler_ = std::make_unique<checks::BrushMan>(callback);
        brushHandler_->initialize();
    }

    // Set up container handler
    containerHandler_ = std::make_unique<checks::ContainerMan>(socket_, callback);
    containerHandler_->initialize();

    // Block the game from granting items when the player picks up randomized container items
    wolf::onItemPickupBlocking(
        [this](int itemId, int count) -> bool
        {
            (void)count;
            if (containerHandler_)
                return containerHandler_->shouldBlockItemPickup(itemId);
            return false;
        });

    // Set up shop handler
    shopHandler_ = std::make_unique<checks::ShopMan>(socket_, callback);
    shopHandler_->initialize();

    initialized_ = true;
    wolf::logInfo("[CheckMan] Check manager initialized");
}

void CheckMan::reset()
{
    sentChecks_.clear();
    destroyMonitors();
    if (containerHandler_)
    {
        containerHandler_->reset();
    }
    if (brushHandler_)
    {
        brushHandler_->reset();
    }
    if (shopHandler_)
    {
        shopHandler_->reset();
    }

    wolf::logInfo("[CheckMan] Check manager reset - will reinitialize monitors on next call to initialize()");
}

void CheckMan::shutdown()
{
    if (!initialized_)
    {
        return;
    }

    destroyMonitors();
    brushHandler_.reset();
    containerHandler_.reset();
    shopHandler_.reset();
    initialized_ = false;
}

void CheckMan::enableSending(bool enabled)
{
    sendingEnabled_ = enabled;
}

bool CheckMan::isSendingEnabled() const
{
    return sendingEnabled_;
}

void CheckMan::poll()
{
    // Lazily create brush handler once slot config arrives (connection happens after init)
    if (!brushHandler_ && socket_.isSlotConfigReady() && socket_.getSlotConfig().randomizeBrushes)
    {
        auto callback = [this](int64_t checkId) { sendCheck(checkId); };
        brushHandler_ = std::make_unique<checks::BrushMan>(callback);
        brushHandler_->initialize();
    }

    if (containerHandler_)
    {
        containerHandler_->poll();
    }
}

// ========================================
// Event-based check handlers
// ========================================

void CheckMan::onShopPurchase(int shopId, int itemSlot, int itemId)
{
    (void)itemId; // Currently unused

    if (!sendingEnabled_)
    {
        return;
    }

    int64_t checkId = checks::getShopCheckId(shopId, itemSlot);
    sendCheck(checkId);
}

// ========================================
// Server synchronization
// ========================================

void CheckMan::clearSentChecks()
{
    const size_t prior = sentChecks_.size();
    sentChecks_.clear();
    if (prior > 0)
        wolf::logInfo("[CheckMan] Cleared %zu tracked sent check(s) on disconnect", prior);
}

void CheckMan::syncWithServer(const std::list<int64_t> &serverCheckedLocations)
{
    // Add server-confirmed checks to local cache
    for (int64_t loc : serverCheckedLocations)
    {
        sentChecks_.insert(loc);
    }

    // Find checks we have locally but server doesn't know about
    std::vector<int64_t> toResend;
    for (int64_t loc : sentChecks_)
    {
        bool foundOnServer = false;
        for (int64_t serverLoc : serverCheckedLocations)
        {
            if (serverLoc == loc)
            {
                foundOnServer = true;
                break;
            }
        }
        if (!foundOnServer)
        {
            toResend.push_back(loc);
        }
    }

    // Resend missing checks
    if (!toResend.empty() && socket_.isConnected())
    {
        wolf::logInfo("[CheckMan] Resending %zu checks not confirmed by server", toResend.size());
        socket_.sendLocations(toResend);
    }

    wolf::logInfo("[CheckMan] Synced with server: %zu total checks tracked", sentChecks_.size());
}

void CheckMan::resendAllChecks()
{
    if (sentChecks_.empty())
    {
        return;
    }

    if (!socket_.isConnected())
    {
        wolf::logWarning("[CheckMan] Cannot resend checks: socket not connected");
        return;
    }

    std::vector<int64_t> allChecks(sentChecks_.begin(), sentChecks_.end());
    wolf::logInfo("[CheckMan] Resending all %zu tracked checks", allChecks.size());
    socket_.sendLocations(allChecks);
}

size_t CheckMan::getSentCount() const
{
    return sentChecks_.size();
}

// ========================================
// Check sending and deduplication
// ========================================

void CheckMan::sendCheck(int64_t checkId)
{
    if (!sendingEnabled_ || !socket_.isConnected())
    {
        wolf::logDebug("[CheckMan] Skipped check %" PRId64 " (sending=%d, connected=%d)",
                       checkId, sendingEnabled_ ? 1 : 0,
                       socket_.isConnected() ? 1 : 0);
        return;
    }

    if (hasCheckBeenSent(checkId))
    {
        wolf::logDebug("[CheckMan] Skipped check %" PRId64 " (already sent in this session "
                       "or synced from server)",
                       checkId);
        return;
    }

    socket_.sendLocation(checkId);
    markCheckSent(checkId);

    wolf::logInfo("[CheckMan] Sent check: %" PRId64 " (total: %zu)", checkId, sentChecks_.size());

    if (onCheckSentCallback_)
        onCheckSentCallback_();
}

bool CheckMan::hasCheckBeenSent(int64_t checkId) const
{
    return sentChecks_.count(checkId) > 0;
}

void CheckMan::markCheckSent(int64_t checkId)
{
    sentChecks_.insert(checkId);
}

// ========================================
// Monitor cleanup
// ========================================

void CheckMan::destroyMonitors()
{
    for (auto monitor : worldStateMonitors_)
    {
        if (monitor)
        {
            wolf::destroyBitfieldMonitor(monitor);
        }
    }
    worldStateMonitors_.clear();

    for (auto monitor : collectedObjectMonitors_)
    {
        if (monitor)
        {
            wolf::destroyBitfieldMonitor(monitor);
        }
    }
    collectedObjectMonitors_.clear();

    for (auto monitor : areasRestoredMonitors_)
    {
        if (monitor)
        {
            wolf::destroyBitfieldMonitor(monitor);
        }
    }
    areasRestoredMonitors_.clear();

    if (globalFlagsMonitor_)
    {
        wolf::destroyBitfieldMonitor(globalFlagsMonitor_);
        globalFlagsMonitor_ = nullptr;
    }

    if (gameProgressMonitor_)
    {
        wolf::destroyBitfieldMonitor(gameProgressMonitor_);
        gameProgressMonitor_ = nullptr;
    }
}

void CheckMan::setOnCheckSentCallback(std::function<void()> callback)
{
    onCheckSentCallback_ = std::move(callback);
}

bool CheckMan::isContainerInRando(int64_t locationId) const
{
    if (containerHandler_)
    {
        return containerHandler_->isContainerInRando(locationId);
    }
    return false;
}
