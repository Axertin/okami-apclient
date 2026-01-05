#include "checkman.h"

#include <cinttypes>

#include "checks/containers.hpp"
#include "checks/gamestate_monitors.hpp"
#include "isocket.h"

CheckMan::CheckMan(ISocket &socket)
    : socket_(socket)
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

    // Set up bitfield monitors via factory functions
    gameProgressMonitor_ = checks::createGameProgressMonitor(callback);
    globalFlagsMonitor_ = checks::createGlobalFlagsMonitor(callback);
    worldStateMonitors_ = checks::createWorldStateMonitors(callback);
    collectedObjectMonitors_ = checks::createCollectedObjectMonitors(callback);
    areasRestoredMonitors_ = checks::createAreaRestoredMonitors(callback);

    // Set up container handler
    containerHandler_ = std::make_unique<checks::ContainerMan>(socket_, callback);
    containerHandler_->initialize();

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

    wolf::logInfo("[CheckMan] Check manager reset - will reinitialize monitors on next call to initialize()");
}

void CheckMan::shutdown()
{
    if (!initialized_)
    {
        return;
    }

    destroyMonitors();
    containerHandler_.reset();
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
    if (containerHandler_)
    {
        containerHandler_->poll();
    }
}

// ========================================
// Event-based check handlers
// ========================================

void CheckMan::onItemPickup(int itemId, int quantity)
{
    (void)quantity; // Currently unused

    if (!sendingEnabled_)
    {
        wolf::logWarning("[CheckMan] Sending disabled, ignoring item pickup");
        return;
    }

    int64_t checkId = checks::getItemCheckId(itemId);
    sendCheck(checkId);

    wolf::logDebug("[CheckMan] Item pickup: item=0x%X, check=%" PRId64, itemId, checkId);
}

void CheckMan::onBrushAcquired(int brushIndex)
{
    if (!sendingEnabled_)
    {
        wolf::logWarning("[CheckMan] Sending disabled, ignoring brush acquisition");
        return;
    }

    int64_t checkId = checks::getBrushCheckId(brushIndex);
    sendCheck(checkId);

    wolf::logDebug("[CheckMan] Brush acquired: brush=%d, check=%" PRId64, brushIndex, checkId);
}

void CheckMan::onShopPurchase(int shopId, int itemSlot, int itemId)
{
    (void)itemId; // Currently unused

    if (!sendingEnabled_)
    {
        wolf::logWarning("[CheckMan] Sending disabled, ignoring shop purchase");
        return;
    }

    int64_t checkId = checks::getShopCheckId(shopId, itemSlot);
    sendCheck(checkId);

    wolf::logDebug("[CheckMan] Shop purchase: shop=%d, slot=%d, check=%" PRId64, shopId, itemSlot, checkId);
}

// ========================================
// Server synchronization
// ========================================

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
    if (!sendingEnabled_)
    {
        wolf::logWarning("[CheckMan] Sending disabled, aborting send");
        return;
    }

    if (!socket_.isConnected())
    {
        wolf::logWarning("[CheckMan] Socket not connected, aborting send");
        return;
    }

    if (hasCheckBeenSent(checkId))
    {
        wolf::logWarning("[CheckMan] Check %" PRId64 " already sent, skipping", checkId);
        return;
    }

    socket_.sendLocation(checkId);
    markCheckSent(checkId);

    wolf::logInfo("[CheckMan] Sent check: %" PRId64 " (total sent: %zu)", checkId, sentChecks_.size());
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

bool CheckMan::isContainerInRando(int64_t locationId) const
{
    if (containerHandler_)
    {
        return containerHandler_->isContainerInRando(locationId);
    }
    return false;
}
