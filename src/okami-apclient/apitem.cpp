#include "apitem.h"

#include <wolf_framework.hpp>

#include "aplocationmonitor.h"
#include "apitems_generated.hpp"
#include "gamestate_accessors.hpp"

namespace ap
{

namespace
{

// Handler implementations for each reward type

std::expected<void, RewardError> grantGameItem(const GameItemReward &reward)
{
    wolf::logDebug("[AP] Granting game item: 0x%02X", reward.item_id);
    wolf::giveItem(reward.item_id, 1);
    return {};
}

std::expected<void, RewardError> grantBrush(const BrushReward &reward)
{
    wolf::logDebug("[AP] Granting brush technique: index %d", reward.brush_index);
    apgame::usableBrushTechniques->Set(reward.brush_index);
    apgame::obtainedBrushTechniques->Set(reward.brush_index);
    return {};
}

std::expected<void, RewardError> grantProgressiveBrush(const ProgressiveBrushReward &reward)
{
    // Check if base brush is already obtained
    bool hasBase = apgame::obtainedBrushTechniques->IsSet(reward.brush_index);

    if (!hasBase)
    {
        wolf::logDebug("[AP] Granting base progressive brush: index %d", reward.brush_index);
        apgame::usableBrushTechniques->Set(reward.brush_index);
        apgame::obtainedBrushTechniques->Set(reward.brush_index);
        return {};
    }

    // Find next upgrade to give
    for (size_t i = 0; i < reward.upgrades.size(); i++)
    {
        uint32_t upgradeBit = reward.upgrades[i];
        if (!apgame::brushUpgrades->IsSet(upgradeBit))
        {
            wolf::logDebug("[AP] Granting brush upgrade: level %zu (bit %u)", i + 2, upgradeBit);
            apgame::brushUpgrades->Set(upgradeBit);
            return {};
        }
    }

    wolf::logDebug("[AP] Progressive brush already at max level");
    return {};
}

std::expected<void, RewardError> grantEventFlag(const EventFlagReward &reward)
{
    wolf::logDebug("[AP] Setting event flag: %.*s bit %u", static_cast<int>(reward.accessor.size()), reward.accessor.data(), reward.bit);

    // Map accessor name to actual accessor
    if (reward.accessor == "keyItemsAcquired")
    {
        apgame::keyItemsAcquired->Set(reward.bit);
    }
    else if (reward.accessor == "goldDustsAcquired")
    {
        apgame::goldDustsAcquired->Set(reward.bit);
    }
    else
    {
        return std::unexpected(RewardError{.message = "Unknown accessor: " + std::string(reward.accessor)});
    }

    return {};
}

std::expected<void, RewardError> grantMultiFlag(const MultiFlagReward &reward)
{
    for (const auto &flag : reward.flags)
    {
        wolf::logDebug("[AP] Setting flag: %.*s bit %u", static_cast<int>(flag.accessor.size()), flag.accessor.data(), flag.bit);

        if (flag.accessor == "keyItemsAcquired")
        {
            apgame::keyItemsAcquired->Set(flag.bit);
        }
        else if (flag.accessor == "goldDustsAcquired")
        {
            apgame::goldDustsAcquired->Set(flag.bit);
        }
        else
        {
            return std::unexpected(RewardError{.message = "Unknown accessor: " + std::string(flag.accessor)});
        }
    }

    return {};
}

std::expected<void, RewardError> grantProgressiveWeapon(const ProgressiveWeaponReward &reward)
{
    auto inventory = apgame::collectionData->inventory;

    // Find current stage
    int currentStage = -1;
    for (size_t i = 0; i < reward.stages.size(); i++)
    {
        if (inventory[reward.stages[i]] > 0)
        {
            currentStage = static_cast<int>(i);
        }
    }

    int nextStage = currentStage + 1;
    if (nextStage >= static_cast<int>(reward.stages.size()))
    {
        wolf::logDebug("[AP] Progressive weapon already at max stage");
        return {};
    }

    int itemToGive = reward.stages[nextStage];
    wolf::logDebug("[AP] Granting progressive weapon stage %d (item 0x%X)", nextStage + 1, itemToGive);
    wolf::giveItem(itemToGive, 1);

    return {};
}

std::expected<void, RewardError> grantFiller(const FillerReward &reward)
{
    wolf::logDebug("[AP] Filler item: %.*s", static_cast<int>(reward.message.size()), reward.message.data());
    // Nothing to grant - just log the message
    return {};
}

// Convert from old APItemDef to new Reward format
std::optional<Reward> convertFromLegacy(const apitems::APItemDef *item)
{
    if (!item)
    {
        return std::nullopt;
    }

    Reward reward{.ap_item_id = item->id, .data = FillerReward{.message = "Unknown item type"}};

    switch (item->type)
    {
    case apitems::ItemHandlerType::Brush:
        reward.data = BrushReward{.brush_index = item->brush.brush_index};
        break;

    case apitems::ItemHandlerType::ProgressiveBrush:
        reward.data = ProgressiveBrushReward{.brush_index = item->progressive_brush.brush_index,
                                             .upgrades = std::span<const uint32_t>(item->progressive_brush.upgrade_bits, item->progressive_brush.upgrade_count)};
        break;

    case apitems::ItemHandlerType::Inventory:
        reward.data = GameItemReward{.item_id = static_cast<uint8_t>(item->inventory.item_id)};
        break;

    case apitems::ItemHandlerType::EventFlags: {
        // Convert FlagInfo array to MultiFlagReward
        // Note: This is a bit awkward because the old format uses C arrays
        // For now, we'll just handle single flags - multi-flag items need a static array
        if (item->event.flag_count == 1)
        {
            reward.data = EventFlagReward{.accessor = item->event.flags[0].accessor_name, .bit = item->event.flags[0].bit};
        }
        else
        {
            // For multi-flag items, fall back to handling via legacy path
            // This is a limitation - we'd need static storage for the span
            wolf::logWarning("[AP] Multi-flag items not yet fully supported in new system");
            return std::nullopt;
        }
        break;
    }

    case apitems::ItemHandlerType::Progressive:
        reward.data = ProgressiveWeaponReward{.stages = std::span<const int>(item->progressive.stages, item->progressive.stage_count)};
        break;

    default:
        return std::nullopt;
    }

    return reward;
}

} // anonymous namespace

std::optional<Reward> getReward(int64_t ap_item_id)
{
    const apitems::APItemDef *item = apitems::findItem(static_cast<int>(ap_item_id));
    return convertFromLegacy(item);
}

std::expected<void, RewardError> grantReward(const Reward &reward)
{
    wolf::logInfo("[AP] Granting reward for AP item %lld", reward.ap_item_id);

    // Disable location monitor during granting
    APLocationMonitor::instance().enableSending(false);

    auto result = std::visit(
        [](auto &&data) -> std::expected<void, RewardError> {
            using T = std::decay_t<decltype(data)>;

            if constexpr (std::is_same_v<T, GameItemReward>)
            {
                return grantGameItem(data);
            }
            else if constexpr (std::is_same_v<T, BrushReward>)
            {
                return grantBrush(data);
            }
            else if constexpr (std::is_same_v<T, ProgressiveBrushReward>)
            {
                return grantProgressiveBrush(data);
            }
            else if constexpr (std::is_same_v<T, EventFlagReward>)
            {
                return grantEventFlag(data);
            }
            else if constexpr (std::is_same_v<T, MultiFlagReward>)
            {
                return grantMultiFlag(data);
            }
            else if constexpr (std::is_same_v<T, ProgressiveWeaponReward>)
            {
                return grantProgressiveWeapon(data);
            }
            else if constexpr (std::is_same_v<T, FillerReward>)
            {
                return grantFiller(data);
            }
            else
            {
                static_assert(false, "Unhandled reward type");
            }
        },
        reward.data);

    // Re-enable location monitor
    APLocationMonitor::instance().enableSending(true);

    if (result)
    {
        wolf::logInfo("[AP] Successfully granted reward");
    }
    else
    {
        wolf::logWarning("[AP] Failed to grant reward: %s", result.error().message.c_str());
    }

    return result;
}

bool isGameItem(int64_t ap_item_id)
{
    auto reward = getReward(ap_item_id);
    if (!reward)
    {
        return false;
    }
    return std::holds_alternative<GameItemReward>(reward->data);
}

std::optional<uint8_t> getGameItemId(int64_t ap_item_id)
{
    auto reward = getReward(ap_item_id);
    if (!reward)
    {
        return std::nullopt;
    }

    if (auto *gameItem = std::get_if<GameItemReward>(&reward->data))
    {
        return gameItem->item_id;
    }

    return std::nullopt;
}

} // namespace ap
