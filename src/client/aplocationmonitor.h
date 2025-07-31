#pragma once
#include <cstdint>
#include <memory>
#include <unordered_set>

#include "isocket.h"
#include "okami/bitfieldmonitor.hpp"
#include "okami/memorymap.hpp"

/**
 * @brief Monitors game state changes and automatically sends Archipelago location checks
 *
 * The APLocationMonitor class is responsible for detecting when the player achieves
 * various objectives in Okami HD and converting those achievements into Archipelago
 * location checks that are sent to the server.
 *
 * This system monitors several types of game state:
 * - Memory-based locations: BitField changes in map state, world state, and global flags
 * - Event-based locations: Item pickups, brush acquisitions, and shop purchases
 *
 * The monitor uses a combination of real-time memory monitoring (via BitFieldChangeDetector)
 * and game hook callbacks to detect when locations should be checked.
 *
 */
class APLocationMonitor
{
  public:
    /**
     * @brief Get the singleton instance of the location monitor
     * @return Reference to the global APLocationMonitor instance
     */
    static APLocationMonitor &instance();

    /**
     * @brief Initialize the location monitor and set up memory monitoring
     *
     * This should be called once during mod initialization. Sets up the memory
     * monitoring system for automatic location detection.
     */
    void initialize();

    /**
     * @brief Update the location monitor (call every frame)
     *
     * Polls all active memory monitors for changes and processes any detected
     * location achievements. Should be called from the main game loop.
     */
    void update();

    /**
     * @brief Reset the location monitor state
     *
     * Clears the sent locations cache and resets all memory monitors.
     * Used when disconnecting/reconnecting.
     */
    void reset();

    /**
     * @brief Handle item pickup events from game hooks
     * @param itemId The ID of the item that was picked up
     * @param quantity The quantity of the item (typically 1)
     *
     * Called by game hooks when the player picks up an item. Converts the
     * item pickup into a location check and sends it to the server.
     */
    void onItemPickup(int itemId, int quantity);

    /**
     * @brief Handle brush acquisition events from game hooks
     * @param brushIndex The index of the celestial brush technique acquired
     *
     * Called by game hooks when the player learns a new brush technique.
     * Converts the brush acquisition into a location check.
     */
    void onBrushAcquired(int brushIndex);

    /**
     * @brief Handle shop purchase events from game hooks
     * @param shopId The ID of the shop where the purchase occurred
     * @param itemSlot The slot number of the item purchased
     * @param itemId The ID of the item that was purchased
     *
     * Called by game hooks when the player purchases an item from a shop.
     * Converts the shop purchase into a location check based on shop and slot.
     */
    void onShopPurchase(int shopId, int itemSlot, int itemId);

    /**
     * @brief Set the socket for sending location checks
     * @param socket Pointer to the ISocket implementation for communication
     *
     * The monitor needs a socket reference to send location checks to the
     * Archipelago server. Should be set when the connection is established.
     */
    void setSocket(ISocket *socket)
    {
        socket_ = socket;
    }

    /**
     * @brief Enable or disable location sending
     * @param enabled Whether location checks should be sent to the server
     *
     * When disabled, the monitor will continue tracking state changes but
     * won't send any location checks. Useful for offline testing or when
     * not connected to an Archipelago session.
     */
    void enableSending(bool enabled)
    {
        sendingEnabled_ = enabled;
    }

    /**
     * @brief Check if location sending is currently enabled
     * @return True if location checks are being sent to the server
     */
    bool isSendingEnabled() const
    {
        return sendingEnabled_;
    }

    /**
     * @brief Calculate location ID for item pickup
     * @param itemId The game's internal item ID
     * @return Archipelago location ID (100000 + itemId)
     */
    static int64_t getItemLocationId(int itemId)
    {
        return 100000 + itemId;
    }

    /**
     * @brief Calculate location ID for brush acquisition
     * @param brushIndex The game's internal brush index
     * @return Archipelago location ID (200000 + brushIndex)
     */
    static int64_t getBrushLocationId(int brushIndex)
    {
        return 200000 + brushIndex;
    }

    /**
     * @brief Calculate location ID for shop purchase
     * @param shopId The game's internal shop ID
     * @param itemSlot The slot number within the shop
     * @return Archipelago location ID (300000 + shopId*1000 + itemSlot)
     */
    static int64_t getShopLocationId(int shopId, int itemSlot)
    {
        return 300000 + (shopId * 1000) + itemSlot;
    }

    /**
     * @brief Calculate location ID for world state bit change
     * @param mapId The map where the bit change occurred
     * @param bitIndex The specific bit that changed
     * @return Archipelago location ID (400000 + mapId*10000 + bitIndex)
     *
     * World state bits track various progression elements and quest states
     * that are specific to each map area.
     */
    static int64_t getWorldStateBitLocationId(int mapId, int bitIndex)
    {
        return 400000 + (mapId * 10000) + bitIndex;
    }

    /**
     * @brief Calculate location ID for collected object
     * @param mapId The map where the object was collected
     * @param bitIndex The specific object bit that was set
     * @return Archipelago location ID (500000 + mapId*10000 + bitIndex)
     *
     * Collected objects include items, stray beads, and other collectibles
     * that are tracked per-map in the save data.
     */
    static int64_t getCollectedObjectLocationId(int mapId, int bitIndex)
    {
        return 500000 + (mapId * 10000) + bitIndex;
    }

    /**
     * @brief Calculate location ID for area restoration
     * @param mapId The map where the area was restored
     * @param bitIndex The specific restoration bit that was set
     * @return Archipelago location ID (600000 + mapId*10000 + bitIndex)
     *
     * Area restoration tracks when the player uses bloom techniques to
     * restore withered or cursed areas of the world.
     */
    static int64_t getAreasRestoredLocationId(int mapId, int bitIndex)
    {
        return 600000 + (mapId * 10000) + bitIndex;
    }

    /**
     * @brief Calculate location ID for global flag change
     * @param bitIndex The specific global flag bit that changed
     * @return Archipelago location ID (700000 + bitIndex)
     *
     * Global flags track game-wide progression that isn't tied to a
     * specific map, such as major story milestones.
     */
    static int64_t getGlobalFlagLocationId(int bitIndex)
    {
        return 700000 + bitIndex;
    }

    /**
     * @brief Get the number of locations sent to the server
     * @return Count of unique locations that have been checked
     */
    size_t getLocationsSentCount() const
    {
        return sentLocations_.size();
    }

  private:
    APLocationMonitor() = default;

    /**
     * @brief Send a location check to the Archipelago server
     * @param locationId The location ID to send
     *
     * Checks if the location has already been sent and if sending is enabled.
     * If valid, sends the location check and marks it as sent.
     */
    void sendLocation(int64_t locationId);

    /**
     * @brief Check if a location has already been sent
     * @param locationId The location ID to check
     * @return True if this location has been sent before
     */
    bool hasLocationBeenSent(int64_t locationId) const;

    /**
     * @brief Mark a location as having been sent
     * @param locationId The location ID to mark as sent
     *
     * Adds the location to the sent locations cache to prevent duplicate sends.
     */
    void markLocationSent(int64_t locationId);

    /**
     * @brief Set up automatic memory monitoring
     *
     * Creates BitFieldChangeDetector instances for all the memory regions
     * we want to monitor for location changes. Called during initialization.
     */
    void setupMemoryMonitoring();

    /**
     * @brief Handle world state bit changes
     * @param mapId The map where the change occurred
     * @param bitIndex The bit that changed
     * @param oldValue The previous value (should be false for locations)
     * @param newValue The new value (should be true for locations)
     */
    void onWorldStateBitChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue);

    /**
     * @brief Handle collected object bit changes
     * @param mapId The map where the collection occurred
     * @param bitIndex The object bit that changed
     * @param oldValue The previous value (should be false for locations)
     * @param newValue The new value (should be true for locations)
     */
    void onCollectedObjectChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue);

    /**
     * @brief Handle area restoration bit changes
     * @param mapId The map where the restoration occurred
     * @param bitIndex The restoration bit that changed
     * @param oldValue The previous value (should be false for locations)
     * @param newValue The new value (should be true for locations)
     */
    void onAreasRestoredChange(int mapId, unsigned int bitIndex, bool oldValue, bool newValue);

    /**
     * @brief Handle global flag bit changes
     * @param bitIndex The global flag bit that changed
     * @param oldValue The previous value (should be false for locations)
     * @param newValue The new value (should be true for locations)
     */
    void onGlobalFlagChange(unsigned int bitIndex, bool oldValue, bool newValue);

    // Socket for communicating with Archipelago server
    ISocket *socket_ = nullptr;
    // Whether location sending is currently enabled
    bool sendingEnabled_ = false;
    // Cache of location IDs that have already been sent to prevent duplicates
    std::unordered_set<int64_t> sentLocations_;

    // Memory monitors for automatic location detection
    std::array<std::unique_ptr<okami::BitFieldChangeDetector<256>>, okami::MapTypes::NUM_MAP_TYPES> worldStateBitsMonitors_;
    std::array<std::unique_ptr<okami::BitFieldChangeDetector<96>>, okami::MapTypes::NUM_MAP_TYPES> collectedObjectsMonitors_;
    std::array<std::unique_ptr<okami::BitFieldChangeDetector<96>>, okami::MapTypes::NUM_MAP_TYPES> areasRestoredMonitors_;
    std::unique_ptr<okami::BitFieldChangeDetector<86>> globalFlagsMonitor_;

    // Flag tracking whether memory monitoring has been properly initialized
    bool memoryMonitoringInitialized_ = false;
};
