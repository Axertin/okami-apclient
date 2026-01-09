#pragma once

#include <cstdint>
#include <string>

/**
 * @brief Archipelago reward system types and utilities
 *
 * Defines categories and range-based classification for AP rewards.
 */
namespace rewards
{

// Range constants for AP item ID classification
inline constexpr int64_t kGameItemMax = 0xFF;
inline constexpr int64_t kBrushBase = 0x100;
inline constexpr int64_t kBrushEnd = 0x115;
inline constexpr int64_t kProgressiveWeaponBase = 0x300;
inline constexpr int64_t kProgressiveWeaponEnd = 0x302;
inline constexpr int64_t kEventFlagBase = 0x303;
inline constexpr int64_t kEventFlagEnd = 0x308;

/**
 * @brief Categories of rewards based on AP item ID ranges
 */
enum class RewardCategory
{
    GameItem,  // 0x00-0xFF: direct game items, 0x300-0x302: progressive weapons
    Brush,     // 0x100-0x115: brush techniques (some progressive)
    EventFlag, // 0x303-0x308: event flags to set
    Unknown    // Unrecognized AP item ID
};

/**
 * @brief Determine the category for an AP item ID
 *
 * @param apItemId The Archipelago item ID
 * @return The category this item belongs to
 */
[[nodiscard]] constexpr RewardCategory getCategory(int64_t apItemId)
{
    if (apItemId >= 0 && apItemId <= kGameItemMax)
    {
        return RewardCategory::GameItem;
    }
    if (apItemId >= kBrushBase && apItemId <= kBrushEnd)
    {
        return RewardCategory::Brush;
    }
    if (apItemId >= kProgressiveWeaponBase && apItemId <= kProgressiveWeaponEnd)
    {
        return RewardCategory::GameItem; // Progressive weapons handled by game_items
    }
    if (apItemId >= kEventFlagBase && apItemId <= kEventFlagEnd)
    {
        return RewardCategory::EventFlag;
    }
    return RewardCategory::Unknown;
}

/**
 * @brief Check if an AP item ID is in a known range
 *
 * @param apItemId The Archipelago item ID
 * @return true if the item is recognized
 */
[[nodiscard]] constexpr bool isKnownItem(int64_t apItemId)
{
    return getCategory(apItemId) != RewardCategory::Unknown;
}

/**
 * @brief Error type for reward granting failures
 */
struct RewardError
{
    std::string message;
};

} // namespace rewards
