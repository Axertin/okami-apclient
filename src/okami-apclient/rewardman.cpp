#include "rewardman.h"

#include <cinttypes>

#include <wolf_framework.hpp>

#include "notificationwindow.h"
#include "rewards/brushes.hpp"
#include "rewards/event_flags.hpp"
#include "rewards/game_items.hpp"

RewardMan::RewardMan(CheckSendingCallback onCheckSendingChange) : onCheckSendingChange_(std::move(onCheckSendingChange))
{
    // Register gameplay state callbacks
    wolf::onPlayStart([this]() { setGrantingEnabled(true); });
    wolf::onReturnToMenu([this]() { setGrantingEnabled(false); });
}

void RewardMan::reset()
{
    std::lock_guard lock(queueMutex_);
    queuedRewards_.clear();
    grantingEnabled_ = false;
}

void RewardMan::queueReward(int64_t apItemId, const std::string &itemName)
{
    std::lock_guard lock(queueMutex_);
    queuedRewards_.push_back({apItemId, itemName});
}

bool RewardMan::processQueuedRewards()
{
    std::vector<QueuedReward> toProcess;

    {
        std::lock_guard lock(queueMutex_);
        if (queuedRewards_.empty() || !grantingEnabled_)
        {
            return true;
        }
        std::swap(toProcess, queuedRewards_);
    }

    bool allSucceeded = true;
    for (const auto &reward : toProcess)
    {
        auto result = grantReward(reward.apItemId);
        if (!result)
        {
            wolf::logWarning("[RewardMan] Failed to grant reward 0x%llX: %s", reward.apItemId, result.error().message.c_str());
            allSucceeded = false;
        }
        else
        {
            // Show notification for successfully granted reward
            notificationwindow::queue("Received: " + reward.itemName);
        }
    }

    return allSucceeded;
}

bool RewardMan::isGameItem(int64_t apItemId) const
{
    return rewards::game_items::isDirectGameItem(apItemId);
}

std::optional<uint8_t> RewardMan::getGameItemId(int64_t apItemId) const
{
    if (rewards::game_items::isDirectGameItem(apItemId))
    {
        return rewards::game_items::getItemId(apItemId);
    }
    return std::nullopt;
}

std::expected<void, rewards::RewardError> RewardMan::grantReward(int64_t apItemId)
{
    wolf::logInfo("[RewardMan] Granting reward for AP item 0x%llX", apItemId);

    // Disable check sending during granting to prevent feedback loops
    if (onCheckSendingChange_)
    {
        onCheckSendingChange_(false);
    }

    std::expected<void, rewards::RewardError> result;

    switch (rewards::getCategory(apItemId))
    {
    case rewards::RewardCategory::GameItem:
        result = rewards::game_items::grant(apItemId);
        break;
    case rewards::RewardCategory::Brush:
        result = rewards::brushes::grant(apItemId);
        break;
    case rewards::RewardCategory::EventFlag:
        result = rewards::event_flags::grant(apItemId);
        break;
    case rewards::RewardCategory::Unknown:
        wolf::logDebug("[RewardMan] Skipping unrecognized AP item 0x%" PRIX64, apItemId);
        result = {}; // Success - treat as no-op
        break;
    }

    // Re-enable check sending
    if (onCheckSendingChange_)
    {
        onCheckSendingChange_(true);
    }

    return result;
}

void RewardMan::setGrantingEnabled(bool enabled)
{
    grantingEnabled_ = enabled;
}

bool RewardMan::isGrantingEnabled() const
{
    return grantingEnabled_;
}

size_t RewardMan::getQueuedCount() const
{
    std::lock_guard lock(queueMutex_);
    return queuedRewards_.size();
}
