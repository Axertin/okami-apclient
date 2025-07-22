#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

#include "data/maptype.hpp"

namespace okami
{

/**
 * @brief Structure holding map-specific flag descriptions.
 *
 * Each map can define descriptions for various flag categories.
 * Not all categories need to be present in every map's YAML file.
 */
struct MapStateConfig
{
    /// Main progression and quest flags, as well as state tracking
    std::unordered_map<unsigned, std::string> worldStateBits;

    /// User-defined indices (purpose varies by map)
    std::unordered_map<unsigned, std::string> userIndices;

    /// Treasure chests and collectible items
    std::unordered_map<unsigned, std::string> collectedObjects;

    /// Environmental restoration flags
    std::unordered_map<unsigned, std::string> areasRestored;

    /// Normal trees bloomed with Bloom technique
    std::unordered_map<unsigned, std::string> treesBloomed;

    /// Cursed trees bloomed
    std::unordered_map<unsigned, std::string> cursedTreesBloomed;

    /// Combat encounters cleared
    std::unordered_map<unsigned, std::string> fightsCleared;

    /// NPC interaction states
    std::unordered_map<unsigned, std::string> npcs;

    /// Map exploration progress
    std::unordered_map<unsigned, std::string> mapsExplored;

    /// Unknown fields (for reverse engineering)
    std::unordered_map<unsigned, std::string> field_DC;
    std::unordered_map<unsigned, std::string> field_E0;
};

/**
 * @brief Structure holding global flag descriptions.
 *
 * These flags apply across the entire game rather than specific maps.
 */
struct GlobalConfig
{
    /// Celestial Brush technique upgrades
    std::unordered_map<unsigned, std::string> brushUpgrades;

    /// Map-independent area restoration
    std::unordered_map<unsigned, std::string> areasRestored;

    /// Common states shared across maps
    std::unordered_map<unsigned, std::string> commonStates;

    /// Main story progression flags
    std::unordered_map<unsigned, std::string> gameProgress;

    /// Key items obtained
    std::unordered_map<unsigned, std::string> keyItemsFound;

    /// Gold Dust upgrades purchased
    std::unordered_map<unsigned, std::string> goldDustsFound;

    /// Animal companions found
    std::unordered_map<unsigned, std::string> animalsFound;

    /// Animals fed for the first time
    std::unordered_map<unsigned, std::string> animalsFedFirstTime;

    /// Global game state flags (menu states, etc.)
    std::unordered_map<unsigned, std::string> globalGameState;
};

/**
 * @brief Interface for accessing game state flag definitions and descriptions.
 *
 * This registry is the authoratative source for the meanings behind Okami's many state flags.
 */
class IGameStateRegistry
{
  public:
    virtual ~IGameStateRegistry() = default;

    /**
     * @brief Get description for a specific map state flag.
     * @param map The map to query
     * @param category The flag category name
     * @param bit_index The bit index within the category
     * @return Description or empty string if not found
     */
    virtual std::string_view getMapDescription(MapTypes::Enum map, std::string_view category, unsigned bit_index) const = 0;

    /**
     * @brief Get full configuration for a map.
     * @param map The map to query
     * @return Map configuration (may be empty)
     *
     * The returned reference must remain valid for the lifetime of the registry.
     */
    virtual const MapStateConfig &getMapConfig(MapTypes::Enum map) const = 0;

    /**
     * @brief Check if configuration exists for a map.
     * @param map The map to check
     * @return true if configuration is available
     */
    virtual bool hasMapConfig(MapTypes::Enum map) const = 0;

    /**
     * @brief Get description for a global state flag.
     * @param category The flag category name
     * @param bit_index The bit index within the category
     * @return Description or empty string if not found
     */
    virtual std::string_view getGlobalDescription(std::string_view category, unsigned bit_index) const = 0;

    /**
     * @brief Get full global configuration.
     * @return Global configuration
     *
     * The returned reference must remain valid for the lifetime of the registry.
     */
    virtual const GlobalConfig &getGlobalConfig() const = 0;

    /**
     * @brief Force reload of all configurations.
     *
     * Optional - implementations may choose to no-op if they don't
     * support reloading (e.g., compiled-in data).
     */
    virtual void reload() = 0;
};

} // namespace okami
