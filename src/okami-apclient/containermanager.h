#pragma once

#include <cstdint>

#include <okami/spawntable.h>

// Forward declaration
class ISocket;

/**
 * @brief Manages container-based item randomization
 *
 * The ContainerManager is responsible for:
 * - Hooking SpawnTablePopulator to replace container items with dummies at spawn time
 * - Tracking randomized container indices per level
 * - Detecting when containers are opened and sending locations to AP server
 *
 * Container entries (spawn_type_1 == 1) store their item_id directly in spawn_data->item_id.
 * We replace this with DUMMY_ITEM_ID and track the container index for pickup detection.
 *
 * Location IDs use the formula: CONTAINER_LOCATION_BASE + (levelId << 8) + spawnTableIdx
 * This produces unique IDs across all levels without collision.
 */
namespace container_manager
{

// Hook offsets
inline constexpr uintptr_t SPAWN_TABLE_POPULATOR_OFFSET = 0x49e570;

// Memory offsets
inline constexpr uintptr_t SPAWN_TABLE_OFFSET = 0xB66800;
inline constexpr uintptr_t CURRENT_MAP_ID_OFFSET = 0xB6B240;

// AP location ID constants
inline constexpr int64_t CONTAINER_LOCATION_BASE = 900000;

// Dummy item used for randomized containers
// TODO: Use different dummies to represent standard, progression, chaff, and troll items
inline constexpr uint8_t DUMMY_ITEM_ID = 0x83; // Chestnut

/**
 * @brief Initialize the container manager
 *
 * Installs the SpawnTablePopulator hook.
 * Should be called during mod initialization after WOLF is ready.
 *
 * @return true if initialization succeeded
 */
bool initialize();

/**
 * @brief Shutdown and cleanup
 *
 * Removes hooks and clears tracking data.
 */
void shutdown();

/**
 * @brief Calculate AP location ID from level and spawn table index
 *
 * Formula: CONTAINER_LOCATION_BASE + (levelId << 8) + spawnIdx
 * This produces unique IDs across all levels without collision.
 *
 * @param levelId Current level ID (0-65535)
 * @param spawnIdx Spawn table index (0-127)
 * @return The full AP location ID
 */
[[nodiscard]] constexpr int64_t getContainerLocationId(uint16_t levelId, int spawnIdx)
{
    return CONTAINER_LOCATION_BASE + (static_cast<int64_t>(levelId) << 8) + spawnIdx;
}

/**
 * @brief Check if a container location is part of randomization
 *
 * Stub implementation - always returns true if connected.
 * TODO: Will be replaced with server protocol check when available.
 *
 * @param locationId The AP location ID for this container
 * @return true if container is randomized, false for vanilla behavior
 */
[[nodiscard]] bool isContainerInRando(int64_t locationId);

/**
 * @brief Install hooks without full initialization (for testing)
 *
 * @return true if hook installation succeeded
 */
bool installHooks();

/**
 * @brief Set the socket for AP communication
 *
 * Must be called before container randomization will work.
 * Follows the same pattern as APLocationMonitor::setSocket().
 *
 * @param socket Pointer to the ISocket implementation
 */
void setSocket(ISocket *socket);

/**
 * @brief Poll for container openings
 *
 * Should be called every game tick. Checks if any tracked containers
 * have been opened and sends the corresponding location to the AP server.
 */
void pollForPickups();

// Internal - called by hooks
namespace detail
{
void onSpawnTablePopulate(okami::SpawnTable *table);
} // namespace detail

} // namespace container_manager
