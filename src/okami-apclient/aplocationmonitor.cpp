#include "aplocationmonitor.h"

#include <cinttypes>

#include "okami/maps.hpp"
#include "okami/savefile.hpp"

APLocationMonitor &APLocationMonitor::instance()
{
    static APLocationMonitor monitor;
    return monitor;
}

APLocationMonitor::~APLocationMonitor()
{
    cleanupMonitors();
}

void APLocationMonitor::setSendingEnable(bool status)
{
    sendingEnabled_ = status;
}

void APLocationMonitor::cleanupMonitors()
{
    // Destroy all bitfield monitors using wolf framework
    for (auto monitor : worldStateBitsMonitors_)
    {
        if (monitor)
        {
            wolf::destroyBitfieldMonitor(monitor);
        }
    }
    worldStateBitsMonitors_.clear();

    for (auto monitor : collectedObjectsMonitors_)
    {
        if (monitor)
        {
            wolf::destroyBitfieldMonitor(monitor);
        }
    }
    collectedObjectsMonitors_.clear();

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

    memoryMonitoringInitialized_ = false;
}

void APLocationMonitor::initialize()
{
    wolf::logInfo("[aplocation] Initializing AP Location Monitor");
    // only send locations when in gameplay, not main menu
    wolf::onPlayStart([this]() { setSendingEnable(true); });
    wolf::onReturnToMenu([this]() { setSendingEnable(false); });

    setupMemoryMonitoring();
}

void APLocationMonitor::setupMemoryMonitoring()
{
    // Use the fixed number of map types from the enum
    const size_t numMapTypes = static_cast<size_t>(okami::MapTypes::NUM_MAP_TYPES);

    // Resize vectors to hold monitors for all map types
    worldStateBitsMonitors_.resize(numMapTypes, nullptr);
    collectedObjectsMonitors_.resize(numMapTypes, nullptr);
    areasRestoredMonitors_.resize(numMapTypes, nullptr);
    globalFlagsMonitor_ = nullptr;

    // WorldStateBits monitors
    // AmmyCollections is at 0xB205D0
    for (size_t mapId = 0; mapId < numMapTypes; mapId++)
    {
        uintptr_t collectionsAddr = 0xB205D0 + offsetof(okami::WorldStateData, mapStateBits) + (mapId * sizeof(okami::BitField<256>));
        uintptr_t worldStateBitsAddr = collectionsAddr + offsetof(okami::CollectionData, world);

        worldStateBitsMonitors_[mapId] = wolf::createBitfieldMonitor(
            "main.dll", worldStateBitsAddr, (256 + 7) / 8, [this, mapId](unsigned int bitIndex, bool oldValue, bool newValue)
            { onWorldStateBitChange(static_cast<int>(mapId), bitIndex, oldValue, newValue); }, "WorldStateData monitor");
    }

    // CollectedObjects monitors - BitField<96> from MapState
    // MapState array is at 0xB322B0
    for (size_t mapId = 0; mapId < numMapTypes; mapId++)
    {
        uintptr_t mapStateAddr = 0xB322B0 + (mapId * sizeof(okami::MapState));
        uintptr_t collectedObjectsAddr = mapStateAddr + offsetof(okami::MapState, collectedObjects);

        collectedObjectsMonitors_[mapId] = wolf::createBitfieldMonitor(
            "main.dll", collectedObjectsAddr,
            12, // 96 bits = 12 bytes
            [this, mapId](unsigned int bitIndex, bool oldValue, bool newValue)
            { onCollectedObjectChange(static_cast<int>(mapId), bitIndex, oldValue, newValue); }, "CollectedObjects monitor");
    }

    // AreasRestored monitors - BitField<96> from MapState
    // MapState array is at 0xB322B0
    for (size_t mapId = 0; mapId < numMapTypes; mapId++)
    {
        uintptr_t mapStateAddr = 0xB322B0 + (mapId * sizeof(okami::MapState));
        uintptr_t areasRestoredAddr = mapStateAddr + offsetof(okami::MapState, areasRestored);

        areasRestoredMonitors_[mapId] = wolf::createBitfieldMonitor(
            "main.dll", areasRestoredAddr,
            12, // 96 bits = 12 bytes
            [this, mapId](unsigned int bitIndex, bool oldValue, bool newValue)
            { onAreasRestoredChange(static_cast<int>(mapId), bitIndex, oldValue, newValue); }, "AreasRestored monitor");
    }

    // Global flags monitor using the direct memory offset
    // Global game state flags are at 0xB6B2AC
    globalFlagsMonitor_ = wolf::createBitfieldMonitor(
        "main.dll", 0xB6B2AC, // globalGameStateFlags address
        11,                   // 86 bits = 11 bytes (rounded up)
        [this](unsigned int bitIndex, bool oldValue, bool newValue) { onGlobalFlagChange(bitIndex, oldValue, newValue); }, "GlobalFlags monitor");

    // Tracker GlobalGameState monitors -
    trackerGameProgressionMonitor_ = wolf::createBitfieldMonitor(
        "main.dll", 0xB21780 + offsetof(okami::TrackerData, gameProgressionBits), (96 + 7) / 8,
        [this](unsigned int bitIndex, bool oldValue, bool newValue) { onGameProgressBitChange(bitIndex, oldValue, newValue); }, "GameProgressionBits monitor");

    memoryMonitoringInitialized_ = true;
    wolf::logDebug("[aplocation] Memory monitoring initialized using wolf framework - tracking collectedObjects, areasRestored, and globalFlags for %zu maps",
                   numMapTypes);
}

void APLocationMonitor::onGameProgressBitChange(unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (things being achieved/unlocked)
    if (!oldValue && newValue)
    {
        int64_t locationId = getGameProgressLocationId(bitIndex);
        sendLocation(locationId);

        wolf::logDebug("[aplocation] GameProgressBit change: bit=%u, location=%" PRId64, bitIndex, locationId);
    }
}

void APLocationMonitor::onWorldStateBitChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (things being achieved/unlocked)
    if (!oldValue && newValue)
    {
        int64_t locationId = getWorldStateBitLocationId(mapId, bitIndex);
        sendLocation(locationId);

        wolf::logDebug("[aplocation] WorldStateBit change: %s, bit=%u, location=%" PRId64, okami::decodeMapName(mapId).c_str(), bitIndex, locationId);
    }
}

void APLocationMonitor::onCollectedObjectChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (items being collected)
    if (!oldValue && newValue)
    {
        int64_t locationId = getCollectedObjectLocationId(mapId, bitIndex);
        sendLocation(locationId);

        wolf::logDebug("[aplocation] CollectedObject change: %s, bit=%u, location=%" PRId64, okami::decodeMapName(mapId).c_str(), bitIndex, locationId);
    }
}

void APLocationMonitor::onAreasRestoredChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (areas being restored)
    if (!oldValue && newValue)
    {
        int64_t locationId = getAreasRestoredLocationId(mapId, bitIndex);
        sendLocation(locationId);

        wolf::logDebug("[aplocation] AreasRestored change: %s, bit=%u, location=%" PRId64, okami::decodeMapName(mapId).c_str(), bitIndex, locationId);
    }
}

void APLocationMonitor::onGlobalFlagChange(unsigned int bitIndex, bool oldValue, bool newValue)
{
    // Only send locations for 0->1 transitions (global progression)
    if (!oldValue && newValue)
    {
        int64_t locationId = getGlobalFlagLocationId(bitIndex);
        sendLocation(locationId);

        wolf::logDebug("[aplocation] GlobalFlag change: bit=%u, location=%" PRId64, bitIndex, locationId);
    }
}

void APLocationMonitor::onBrushAcquired(int brushIndex)
{
    if (!sendingEnabled_)
    {
        wolf::logWarning("[aplocation] Sending disabled, returning from onBrushAcquired early");
        return;
    }

    int64_t locationId = getBrushLocationId(brushIndex);
    sendLocation(locationId);

    wolf::logDebug("[aplocation] Brush acquired: brush=%d, location=%" PRId64, brushIndex, locationId);
}

void APLocationMonitor::onShopPurchase(int shopId, int itemSlot, int itemId)
{
    if (!sendingEnabled_)
    {
        wolf::logWarning("[aplocation] Sending disabled, returning from onShopPurchase early");
        return;
    }

    int64_t locationId = getShopLocationId(shopId, itemSlot);
    sendLocation(locationId);

    wolf::logDebug("[aplocation] Shop purchase: shop=%d, slot=%d, item=%d, location=%" PRId64, shopId, itemSlot, itemId, locationId);
}

void APLocationMonitor::sendLocation(int64_t locationId)
{
    if (!sendingEnabled_)
    {
        wolf::logWarning("[aplocation] Sending disabled (probably not in gameplay), aborting sending location");
        return;
    }

    if (!socket_ || !socket_->isConnected())
    {
        wolf::logWarning("[aplocation] Socket not connected, aborting sending location");
        return;
    }

    if (hasLocationBeenSent(locationId))
    {
        wolf::logWarning("[aplocation] Location %" PRId64 " already sent, aborting", locationId);
        return;
    }

    socket_->sendLocation(locationId);
    markLocationSent(locationId);

    wolf::logInfo("[aplocation] Sent location: %" PRId64 " (total sent: %zu)", locationId, sentLocations_.size());
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

    // Clean up and destroy existing monitors
    cleanupMonitors();

    wolf::logInfo("[aplocation] Location monitor reset using wolf framework - will reinitialize on next call to initialize()");
}
