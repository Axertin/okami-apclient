#include "game_items.hpp"

#include <array>
#include <cstdint>

#include <wolf_framework.hpp>

#include "../gamestate_accessors.hpp"

namespace rewards::game_items
{

namespace
{

// Progressive weapon stage definitions
// Each array lists the game item IDs in progression order
constexpr std::array<uint8_t, 2> kMirrorStages = {0x13, 0x14};                   // Trinity Mirror -> Solar Flare
constexpr std::array<uint8_t, 5> kRosaryStages = {0x15, 0x16, 0x17, 0x18, 0x19}; // Devout -> Tundra Beads
constexpr std::array<uint8_t, 5> kSwordStages = {0x1A, 0x1B, 0x1C, 0x1D, 0x1E};  // Tsumugari -> Thunder Edge

/**
 * @brief Get the stages array for a progressive weapon AP ID
 */
template <size_t N> constexpr const std::array<uint8_t, N> *getProgressiveStages(int64_t apItemId);

// Specializations
template <> constexpr const std::array<uint8_t, 2> *getProgressiveStages<2>(int64_t apItemId)
{
    if (apItemId == 0x300)
        return &kMirrorStages;
    return nullptr;
}

template <> constexpr const std::array<uint8_t, 5> *getProgressiveStages<5>(int64_t apItemId)
{
    if (apItemId == 0x301)
        return &kRosaryStages;
    if (apItemId == 0x302)
        return &kSwordStages;
    return nullptr;
}

/**
 * @brief Find the next stage item for a progressive weapon
 *
 * @param inventory Current inventory array (uint16_t counts per item ID)
 * @param apItemId The progressive weapon AP ID
 * @return The next item to grant, or nullopt if at max stage
 */
std::optional<uint8_t> findNextProgressiveStage(const uint16_t *inventory, int64_t apItemId)
{
    auto tryStages = [&]<size_t N>(const std::array<uint8_t, N> &stages) -> std::optional<uint8_t>
    {
        // Find current stage (highest owned item in the progression)
        int currentStage = -1;
        for (size_t i = 0; i < stages.size(); ++i)
        {
            if (inventory[stages[i]] > 0)
            {
                currentStage = static_cast<int>(i);
            }
        }

        // Return next stage if available
        size_t nextStage = static_cast<size_t>(currentStage + 1);
        if (nextStage < stages.size())
        {
            return stages[nextStage];
        }
        return std::nullopt;
    };

    switch (apItemId)
    {
    case 0x300:
        return tryStages(kMirrorStages);
    case 0x301:
        return tryStages(kRosaryStages);
    case 0x302:
        return tryStages(kSwordStages);
    default:
        return std::nullopt;
    }
}

} // anonymous namespace

std::expected<void, RewardError> grant(int64_t apItemId)
{
    uint8_t itemToGive;

    if (isProgressiveWeapon(apItemId))
    {
        auto inventory = apgame::collectionData->inventory;
        auto nextItem = findNextProgressiveStage(inventory, apItemId);

        if (!nextItem)
        {
            return {}; // Already at max stage
        }

        itemToGive = *nextItem;
    }
    else
    {
        itemToGive = getItemId(apItemId);
    }

    wolf::giveItem(itemToGive, 1);
    return {};
}

std::optional<uint8_t> getNextItemToGrant(int64_t apItemId)
{
    if (isProgressiveWeapon(apItemId))
    {
        auto inventory = apgame::collectionData->inventory;
        return findNextProgressiveStage(inventory, apItemId);
    }
    return getItemId(apItemId);
}

} // namespace rewards::game_items
