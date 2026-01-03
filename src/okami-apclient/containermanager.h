#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

#include <okami/spawntable.h>

// Forward declaration
class ISocket;

/**
 * @brief Manages container-based item randomization
 *
 * The ContainerManager is responsible for:
 * - Loading container definitions from JSON
 * - Hooking SpawnTablePopulator to modify container contents at spawn time
 * - Tracking container positions to AP location ID mappings
 * - Handling container interaction events (when hook offset is found)
 *
 * Container locations use the ID range starting at CONTAINER_LOCATION_BASE (900000).
 */
namespace container_manager
{

// Hook offsets
inline constexpr uintptr_t SPAWN_TABLE_POPULATOR_OFFSET = 0x49e570;
inline constexpr uintptr_t CONTAINER_INTERACTION_OFFSET = 0x000000; // TBD

// Memory offsets
inline constexpr uintptr_t SPAWN_TABLE_OFFSET = 0xB66800;
inline constexpr uintptr_t CURRENT_MAP_ID_OFFSET = 0xB6B240;

// AP location ID constants
inline constexpr int64_t CONTAINER_LOCATION_BASE = 900000;

// Dummy item used for external/non-GameItem rewards
inline constexpr uint8_t DUMMY_ITEM_ID = 0x83; // Chestnut

/**
 * @brief Container definition loaded from containers.json
 */
struct ContainerDef
{
    int32_t id;       // Unique container ID (becomes AP location offset)
    int16_t level_id; // Level where container exists
    int16_t x, y, z;  // Position coordinates
    uint8_t container_type;
    uint8_t original_contents_id; // Original item ID (for reference)
};

/**
 * @brief Initialize the container manager
 *
 * Loads containers.json and installs the SpawnTablePopulator hook.
 * Should be called during mod initialization after WOLF is ready.
 *
 * @return true if initialization succeeded
 */
bool initialize();

/**
 * @brief Shutdown and cleanup
 *
 * Removes hooks and clears loaded data.
 */
void shutdown();

/**
 * @brief Find a container by position
 *
 * @param level_id The level ID to search in
 * @param x X coordinate
 * @param y Y coordinate
 * @param z Z coordinate
 * @return The container definition if found
 */
[[nodiscard]] std::optional<ContainerDef> findContainer(int16_t level_id, int16_t x, int16_t y, int16_t z);

/**
 * @brief Get all containers for a specific level
 *
 * @param level_id The level to get containers for
 * @return Vector of container definitions for that level
 */
[[nodiscard]] std::vector<ContainerDef> getContainersForLevel(int16_t level_id);

/**
 * @brief Calculate AP location ID for a container
 *
 * @param container_id The container's unique ID
 * @return The full AP location ID (CONTAINER_LOCATION_BASE + container_id)
 */
[[nodiscard]] constexpr int64_t getLocationId(int32_t container_id)
{
    return CONTAINER_LOCATION_BASE + container_id;
}

/**
 * @brief Get the total number of loaded containers
 *
 * @return Number of containers loaded from JSON
 */
[[nodiscard]] std::size_t getContainerCount();

/**
 * @brief Load containers from a JSON file (for testing)
 *
 * @param path Path to the containers.json file
 * @return true if loading succeeded
 */
bool loadContainersFromPath(const std::filesystem::path &path);

/**
 * @brief Clear all loaded container data (for testing)
 */
void clearContainers();

/**
 * @brief Install hooks without loading JSON (for testing)
 *
 * This is useful when you want to load containers from a test fixture
 * but still need the hooks installed for hook-level testing.
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

// Internal - called by hooks
namespace detail
{
void onSpawnTablePopulate(okami::SpawnTable *table);
void onContainerInteraction(void *container_data);
} // namespace detail

} // namespace container_manager
