#pragma once

#include <cstdint>

/**
 * @brief Archipelago check (location) system types and utilities
 *
 * Checks are game events that trigger sending a "location check" to the
 * AP server. Each check has a unique ID computed from its category and
 * game-specific identifiers.
 *
 * ID Range Scheme:
 * - 100000 + itemId:                    Item pickups
 * - 200000 + brushIndex:                Brush acquisitions
 * - 300000 + shopId*1000 + itemSlot:    Shop purchases
 * - 400000 + mapId*10000 + bitIndex:    World state changes
 * - 500000 + mapId*10000 + bitIndex:    Collected objects
 * - 600000 + mapId*10000 + bitIndex:    Area restorations
 * - 700000 + bitIndex:                  Global flags
 * - 800000 + bitIndex:                  Game progress flags
 * - 900000 + (levelId << 8) + spawnIdx: Container pickups
 */
namespace checks
{

// Check ID range bases
inline constexpr int64_t kItemPickupBase = 100000;
inline constexpr int64_t kBrushAcquisitionBase = 200000;
inline constexpr int64_t kShopPurchaseBase = 300000;
inline constexpr int64_t kWorldStateBase = 400000;
inline constexpr int64_t kCollectedObjectBase = 500000;
inline constexpr int64_t kAreaRestoredBase = 600000;
inline constexpr int64_t kGlobalFlagBase = 700000;
inline constexpr int64_t kGameProgressBase = 800000;
inline constexpr int64_t kContainerBase = 900000;

/**
 * @brief Calculate check ID for item pickup
 * @param itemId The game's internal item ID
 * @return Archipelago check ID (100000 + itemId)
 */
inline constexpr int64_t getItemCheckId(int itemId)
{
    return kItemPickupBase + itemId;
}

/**
 * @brief Calculate check ID for brush acquisition
 * @param brushIndex The game's internal brush index
 * @return Archipelago check ID (200000 + brushIndex)
 */
inline constexpr int64_t getBrushCheckId(int brushIndex)
{
    return kBrushAcquisitionBase + brushIndex;
}

/**
 * @brief Calculate check ID for shop purchase
 * @param shopId The game's internal shop ID
 * @param itemSlot The slot number within the shop
 * @return Archipelago check ID (300000 + shopId*1000 + itemSlot)
 */
inline constexpr int64_t getShopCheckId(int shopId, int itemSlot)
{
    return kShopPurchaseBase + (shopId * 1000) + itemSlot;
}

/**
 * @brief Calculate check ID for world state bit change
 * @param mapId The map where the bit change occurred
 * @param bitIndex The specific bit that changed
 * @return Archipelago check ID (400000 + mapId*10000 + bitIndex)
 */
inline constexpr int64_t getWorldStateCheckId(int mapId, int bitIndex)
{
    return kWorldStateBase + (mapId * 10000) + bitIndex;
}

/**
 * @brief Calculate check ID for collected object
 * @param mapId The map where the object was collected
 * @param bitIndex The specific object bit that was set
 * @return Archipelago check ID (500000 + mapId*10000 + bitIndex)
 */
inline constexpr int64_t getCollectedObjectCheckId(int mapId, int bitIndex)
{
    return kCollectedObjectBase + (mapId * 10000) + bitIndex;
}

/**
 * @brief Calculate check ID for area restoration
 * @param mapId The map where the area was restored
 * @param bitIndex The specific restoration bit that was set
 * @return Archipelago check ID (600000 + mapId*10000 + bitIndex)
 */
inline constexpr int64_t getAreaRestoredCheckId(int mapId, int bitIndex)
{
    return kAreaRestoredBase + (mapId * 10000) + bitIndex;
}

/**
 * @brief Calculate check ID for global flag change
 * @param bitIndex The specific global flag bit that changed
 * @return Archipelago check ID (700000 + bitIndex)
 */
inline constexpr int64_t getGlobalFlagCheckId(int bitIndex)
{
    return kGlobalFlagBase + bitIndex;
}

/**
 * @brief Calculate check ID for game progress flag change
 * @param bitIndex The specific game progress bit that changed
 * @return Archipelago check ID (800000 + bitIndex)
 */
inline constexpr int64_t getGameProgressCheckId(int bitIndex)
{
    return kGameProgressBase + bitIndex;
}

/**
 * @brief Calculate check ID for container pickup
 * @param levelId The level ID where the container exists
 * @param spawnIdx The spawn table index of the container
 * @return Archipelago check ID (900000 + (levelId << 8) + spawnIdx)
 */
inline constexpr int64_t getContainerCheckId(uint16_t levelId, int spawnIdx)
{
    return kContainerBase + (static_cast<int64_t>(levelId) << 8) + spawnIdx;
}

/**
 * @brief Determine which category a check ID belongs to
 */
enum class CheckCategory
{
    ItemPickup,
    BrushAcquisition,
    ShopPurchase,
    WorldState,
    CollectedObject,
    AreaRestored,
    GlobalFlag,
    GameProgress,
    Container,
    Unknown
};

/**
 * @brief Get the category for a check ID
 */
inline constexpr CheckCategory getCheckCategory(int64_t checkId)
{
    if (checkId >= kContainerBase)
        return CheckCategory::Container;
    if (checkId >= kGameProgressBase)
        return CheckCategory::GameProgress;
    if (checkId >= kGlobalFlagBase)
        return CheckCategory::GlobalFlag;
    if (checkId >= kAreaRestoredBase)
        return CheckCategory::AreaRestored;
    if (checkId >= kCollectedObjectBase)
        return CheckCategory::CollectedObject;
    if (checkId >= kWorldStateBase)
        return CheckCategory::WorldState;
    if (checkId >= kShopPurchaseBase)
        return CheckCategory::ShopPurchase;
    if (checkId >= kBrushAcquisitionBase)
        return CheckCategory::BrushAcquisition;
    if (checkId >= kItemPickupBase)
        return CheckCategory::ItemPickup;
    return CheckCategory::Unknown;
}

} // namespace checks
