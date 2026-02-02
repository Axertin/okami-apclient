#pragma once

#include <cstdint>
#include <expected>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "rewards/reward_types.hpp"

/**
 * @brief Centralized manager for AP reward handling
 *
 * Responsibilities:
 * - Queue-based safe reward granting
 * - Category detection and dispatch to handlers
 * - Lifecycle management (enable/disable granting)
 */
class RewardMan
{
  public:
    /**
     * @brief Callback type for temporarily disabling check sending
     *
     * Called with false before granting rewards, true after.
     * This prevents feedback loops when receiving items triggers
     * location checks.
     */
    using CheckSendingCallback = std::function<void(bool enabled)>;

    /**
     * @brief Construct a RewardMan
     * @param onCheckSendingChange Optional callback to enable/disable check sending
     */
    explicit RewardMan(CheckSendingCallback onCheckSendingChange = nullptr);

    /**
     * @brief Reset state (called on disconnect)
     */
    void reset();

    /**
     * @brief Queue a reward for granting
     *
     * Thread-safe - can be called from network thread.
     *
     * @param apItemId The Archipelago item ID to queue
     * @param itemName Display name for the item (for notifications)
     */
    void queueReward(int64_t apItemId, const std::string &itemName);

    /**
     * @brief Process all queued rewards
     *
     * Must be called from main thread during game tick.
     * Only processes rewards when safe (in gameplay, not menu).
     *
     * @return true if all rewards were granted successfully
     */
    bool processQueuedRewards();

    /**
     * @brief Check if an AP item corresponds to a direct game inventory item
     *
     * Used to determine if an item can be directly placed in containers.
     * Returns false for progressive weapons, brushes, and event flags.
     *
     * @param apItemId The Archipelago item ID
     * @return true if this directly maps to a game item ID
     */
    [[nodiscard]] bool isGameItem(int64_t apItemId) const;

    /**
     * @brief Get the game item ID for container replacement
     *
     * Only valid for direct game items (0x00-0xFF).
     *
     * @param apItemId The Archipelago item ID
     * @return The game item ID if applicable, nullopt otherwise
     */
    [[nodiscard]] std::optional<uint8_t> getGameItemId(int64_t apItemId) const;

    /**
     * @brief Grant a reward immediately (bypasses queue)
     *
     * Only call from main thread when safe.
     *
     * @param apItemId The Archipelago item ID to grant
     * @return Success or error with message
     */
    [[nodiscard]] std::expected<void, rewards::RewardError> grantReward(int64_t apItemId);

    /**
     * @brief Enable or disable reward granting
     *
     * When disabled, processQueuedRewards() will not process items.
     * Used during menu screens or when not connected.
     *
     * @param enabled Whether granting should be enabled
     */
    void setGrantingEnabled(bool enabled);

    /**
     * @brief Check if reward granting is currently enabled
     */
    [[nodiscard]] bool isGrantingEnabled() const;

    /**
     * @brief Get the number of queued rewards
     */
    [[nodiscard]] size_t getQueuedCount() const;

  private:
    // Queued rewards waiting to be granted (item ID + display name)
    struct QueuedReward
    {
        int64_t apItemId;
        std::string itemName;
    };
    std::vector<QueuedReward> queuedRewards_;

    // Mutex protecting the queue (for thread-safe queueing)
    mutable std::mutex queueMutex_;

    // Whether granting is currently safe (in gameplay, not menu)
    bool grantingEnabled_ = false;

    // Callback to enable/disable check sending during granting
    CheckSendingCallback onCheckSendingChange_;
};
