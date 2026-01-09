#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <unordered_set>
#include <vector>

#include <wolf_framework.hpp>

#include "checks/check_types.hpp"

// Forward declarations
class ISocket;

namespace checks
{
class ContainerMan;
class ShopMan;
} // namespace checks

/**
 * @brief Centralized manager for AP check (location) handling
 *
 * Responsibilities:
 * - Coordinate check category handlers
 * - Check deduplication
 * - Server communication
 * - Event forwarding to handlers
 * - Memory monitoring for bitfield-based checks
 * - Container randomization polling
 */
class CheckMan
{
  public:
    /**
     * @brief Construct a CheckMan
     * @param socket Reference to the socket for AP communication
     */
    explicit CheckMan(ISocket &socket);

    /**
     * @brief Destructor - cleans up monitors
     */
    ~CheckMan();

    // Non-copyable, non-movable (owns monitors)
    CheckMan(const CheckMan &) = delete;
    CheckMan &operator=(const CheckMan &) = delete;
    CheckMan(CheckMan &&) = delete;
    CheckMan &operator=(CheckMan &&) = delete;

    /**
     * @brief Initialize all check handlers and monitors
     *
     * Sets up memory monitoring, registers gameplay hooks.
     * Must be called after game is ready.
     */
    void initialize();

    /**
     * @brief Reset state (called on disconnect)
     *
     * Clears sent checks cache and resets monitors.
     */
    void reset();

    /**
     * @brief Shutdown and cleanup
     *
     * Destroys monitors and cleans up resources.
     */
    void shutdown();

    /**
     * @brief Enable or disable check sending
     * @param enabled Whether checks should be sent to server
     */
    void enableSending(bool enabled);

    /**
     * @brief Check if sending is currently enabled
     */
    [[nodiscard]] bool isSendingEnabled() const;

    /**
     * @brief Process checks that need polling (containers)
     *
     * Should be called every game tick.
     */
    void poll();

    /**
     * @brief Handle item pickup events
     * @param itemId The game's internal item ID
     * @param quantity The quantity picked up
     */
    void onItemPickup(int itemId, int quantity);

    /**
     * @brief Handle brush acquisition events
     * @param brushIndex The brush technique index
     */
    void onBrushAcquired(int brushIndex);

    /**
     * @brief Handle shop purchase events
     * @param shopId The shop ID
     * @param itemSlot The slot in the shop
     * @param itemId The item purchased
     */
    void onShopPurchase(int shopId, int itemSlot, int itemId);

    /**
     * @brief Sync local cache with server's confirmed checks
     * @param serverCheckedLocations List of check IDs the server has recorded
     */
    void syncWithServer(const std::list<int64_t> &serverCheckedLocations);

    /**
     * @brief Resend all tracked checks to the server
     *
     * Used during resync after desync detection.
     */
    void resendAllChecks();

    /**
     * @brief Get count of sent checks
     */
    [[nodiscard]] size_t getSentCount() const;

    /**
     * @brief Check if a container location is part of randomization
     * @param locationId The container location ID
     * @return true if randomized
     */
    [[nodiscard]] bool isContainerInRando(int64_t locationId) const;

  private:
    /**
     * @brief Send a check to the server
     * @param checkId The check ID to send
     */
    void sendCheck(int64_t checkId);

    /**
     * @brief Check if a check has already been sent
     */
    [[nodiscard]] bool hasCheckBeenSent(int64_t checkId) const;

    /**
     * @brief Mark a check as sent
     */
    void markCheckSent(int64_t checkId);

    /**
     * @brief Clean up memory monitors
     */
    void destroyMonitors();

    // Socket reference for sending checks
    ISocket &socket_;

    // Whether check sending is enabled
    bool sendingEnabled_ = false;

    // Cache of checks that have been sent
    std::unordered_set<int64_t> sentChecks_;

    // Memory monitors for bitfield-based checks
    std::vector<wolf::BitfieldMonitorHandle> worldStateMonitors_;
    std::vector<wolf::BitfieldMonitorHandle> collectedObjectMonitors_;
    std::vector<wolf::BitfieldMonitorHandle> areasRestoredMonitors_;
    wolf::BitfieldMonitorHandle globalFlagsMonitor_ = nullptr;
    wolf::BitfieldMonitorHandle gameProgressMonitor_ = nullptr;

    // Container handler (owns hook and tracking)
    std::unique_ptr<checks::ContainerMan> containerHandler_;

    // Shop handler (owns hooks and shop definitions)
    std::unique_ptr<checks::ShopMan> shopHandler_;

    // Initialization state
    bool initialized_ = false;
};
