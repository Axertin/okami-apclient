#include "aplocationmonitor.h"

#include <cinttypes>

#include "logger.h"

APLocationMonitor &APLocationMonitor::instance()
{
    static APLocationMonitor monitor;
    return monitor;
}

void APLocationMonitor::initialize()
{
    logInfo("[aplocation] Initializing AP Location Monitor");
    setupMemoryMonitoring();
}

void APLocationMonitor::update()
{
    if (!sendingEnabled_ || !socket_ || !socket_->isConnected())
    {
        return;
    }

    if (!memoryMonitoringInitialized_)
    {
        return;
    }

    // Update MapState monitors
    const auto &mapData = *okami::MapData.get_ptr();
    for (int mapId = 0; mapId < okami::MapTypes::NUM_MAP_TYPES; mapId++)
    {
        const auto &mapState = mapData[mapId];
        collectedObjectsMonitors_[mapId]->update(mapState.collectedObjects);
        areasRestoredMonitors_[mapId]->update(mapState.areasRestored);
    }

    // Update WorldStateData monitors
    // const auto &collectionData = *okami::AmmyCollections.get_ptr();
    // for (int mapId = 0; mapId < okami::MapTypes::NUM_MAP_TYPES; mapId++)
    // {
    //     worldStateBitsMonitors_[mapId]->update(collectionData.world.mapStateBits[mapId]);
    // }

    // Update global flags
    // globalFlagsMonitor_->update(*okami::GlobalGameStateFlags.get_ptr());
}

void APLocationMonitor::setupMemoryMonitoring()
{
    // WorldStateBits monitors - BitField<256> from CollectionData
    // for (int mapId = 0; mapId < okami::MapTypes::NUM_MAP_TYPES; mapId++)
    // {
    //     worldStateBitsMonitors_[mapId] = std::make_unique<okami::BitFieldChangeDetector<256>>([this, mapId](unsigned int bitIndex, bool oldValue, bool
    //     newValue)
    //                                                                                           { onWorldStateBitChange(mapId, bitIndex, oldValue, newValue);
    //                                                                                           });
    // }

    // CollectedObjects monitors - BitField<96> from MapState
    for (int mapId = 0; mapId < okami::MapTypes::NUM_MAP_TYPES; mapId++)
    {
        collectedObjectsMonitors_[mapId] = std::make_unique<okami::BitFieldChangeDetector<96>>(
            [this, mapId](unsigned int bitIndex, bool oldValue, bool newValue) { onCollectedObjectChange(mapId, bitIndex, oldValue, newValue); });
    }

    // AreasRestored monitors - BitField<96> from MapState
    for (int mapId = 0; mapId < okami::MapTypes::NUM_MAP_TYPES; mapId++)
    {
        areasRestoredMonitors_[mapId] = std::make_unique<okami::BitFieldChangeDetector<96>>([this, mapId](unsigned int bitIndex, bool oldValue, bool newValue)
                                                                                            { onAreasRestoredChange(mapId, bitIndex, oldValue, newValue); });
    }

    // Global flags monitor
    // globalFlagsMonitor_ = std::make_unique<okami::BitFieldChangeDetector<86>>([this](unsigned int bitIndex, bool oldValue, bool newValue)
    //                                                                           { onGlobalFlagChange(bitIndex, oldValue, newValue); });

    memoryMonitoringInitialized_ = true;
    logDebug("[aplocation] Memory monitoring initialized - tracking worldStateBits, collectedObjects, and areasRestored for %d maps",
             okami::MapTypes::NUM_MAP_TYPES);
}

void APLocationMonitor::onWorldStateBitChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (things being achieved/unlocked)
    if (!oldValue && newValue)
    {
        int64_t locationId = getWorldStateBitLocationId(mapId, bitIndex);
        sendLocation(locationId);

        logDebug("[aplocation] WorldStateBit change: %s, bit=%u, location=%" PRId64, okami::decodeMapName(mapId), bitIndex, locationId);
    }
}

void APLocationMonitor::onCollectedObjectChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (items being collected)
    if (!oldValue && newValue)
    {
        int64_t locationId = getCollectedObjectLocationId(mapId, bitIndex);
        sendLocation(locationId);

        logDebug("[aplocation] CollectedObject change: %s, bit=%u, location=%" PRId64, okami::decodeMapName(mapId), bitIndex, locationId);
    }
}

void APLocationMonitor::onAreasRestoredChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (areas being restored)
    if (!oldValue && newValue)
    {
        int64_t locationId = getAreasRestoredLocationId(mapId, bitIndex);
        sendLocation(locationId);

        logDebug("[aplocation] AreasRestored change: %s, bit=%u, location=%" PRId64, okami::decodeMapName(mapId), bitIndex, locationId);
    }
}

void APLocationMonitor::onGlobalFlagChange(unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (global progression)
    if (!oldValue && newValue)
    {
        int64_t locationId = getGlobalFlagLocationId(bitIndex);
        sendLocation(locationId);

        logDebug("[aplocation] GlobalFlag change: bit=%u, location=%" PRId64, bitIndex, locationId);
    }
}

void APLocationMonitor::onItemPickup(int itemId, int quantity)
{
    if (!sendingEnabled_)
    {
        logWarning("[aplocation] Sending disabled, returning from onItemPickup early");
        return;
    }

    int64_t locationId = getItemLocationId(itemId);
    sendLocation(locationId);

    logDebug("[aplocation] Item pickup: item=%d, quantity=%d, location=%" PRId64, itemId, quantity, locationId);
}

void APLocationMonitor::onBrushAcquired(int brushIndex)
{
    if (!sendingEnabled_)
    {
        logWarning("[aplocation] Sending disabled, returning from onBrushAcquired early");
        return;
    }

    int64_t locationId = getBrushLocationId(brushIndex);
    sendLocation(locationId);

    logDebug("[aplocation] Brush acquired: brush=%d, location=%" PRId64, brushIndex, locationId);
}

void APLocationMonitor::onShopPurchase(int shopId, int itemSlot, int itemId)
{
    if (!sendingEnabled_)
    {
        logWarning("[aplocation] Sending disabled, returning from onShopPurchase early");
        return;
    }

    int64_t locationId = getShopLocationId(shopId, itemSlot);
    sendLocation(locationId);

    logDebug("[aplocation] Shop purchase: shop=%d, slot=%d, item=%d, location=%" PRId64, shopId, itemSlot, itemId, locationId);
}

void APLocationMonitor::sendLocation(int64_t locationId)
{
    if (!socket_ || !socket_->isConnected() || hasLocationBeenSent(locationId))
    {
        logWarning("[aplocation] Sending disabled, returning from sendLocation early");
        return;
    }

    socket_->sendLocation(locationId);
    markLocationSent(locationId);

    logInfo("[aplocation] Sent location: %" PRId64 " (total sent: %zu)", locationId, sentLocations_.size());
}

bool APLocationMonitor::hasLocationBeenSent(int64_t locationId) const
{
    return sentLocations_.count(locationId) > 0;
}

void APLocationMonitor::markLocationSent(int64_t locationId)
{
    sentLocations_.insert(locationId);
}

void APLocationMonitor::reset()
{
    sentLocations_.clear();

    if (memoryMonitoringInitialized_)
    {
        for (auto &monitor : worldStateBitsMonitors_)
        {
            monitor->reset();
        }
        for (auto &monitor : collectedObjectsMonitors_)
        {
            monitor->reset();
        }
        for (auto &monitor : areasRestoredMonitors_)
        {
            monitor->reset();
        }
        globalFlagsMonitor_->reset();
    }

    logInfo("[aplocation] Location monitor reset (will reinitialize on next update)");
}
