#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

#include "data/maptype.hpp"

namespace okami
{

struct MapStateConfig
{
    std::unordered_map<unsigned, std::string> worldStateBits;
    std::unordered_map<unsigned, std::string> userIndices;
    std::unordered_map<unsigned, std::string> collectedObjects;
    std::unordered_map<unsigned, std::string> areasRestored;
    std::unordered_map<unsigned, std::string> treesBloomed;
    std::unordered_map<unsigned, std::string> cursedTreesBloomed;
    std::unordered_map<unsigned, std::string> fightsCleared;
    std::unordered_map<unsigned, std::string> npcs;
    std::unordered_map<unsigned, std::string> mapsExplored;
    std::unordered_map<unsigned, std::string> field_DC;
    std::unordered_map<unsigned, std::string> field_E0;
};

struct GlobalConfig
{
    std::unordered_map<unsigned, std::string> brushUpgrades;
    std::unordered_map<unsigned, std::string> areasRestored;
    std::unordered_map<unsigned, std::string> commonStates;
    std::unordered_map<unsigned, std::string> gameProgress;
    std::unordered_map<unsigned, std::string> keyItemsFound;
    std::unordered_map<unsigned, std::string> goldDustsFound;
    std::unordered_map<unsigned, std::string> animalsFound;
    std::unordered_map<unsigned, std::string> animalsFedFirstTime;
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

    // Map-specific methods
    virtual std::string_view getMapDescription(MapTypes::Enum map, std::string_view category, unsigned bit_index) const = 0;
    virtual const MapStateConfig &getMapConfig(MapTypes::Enum map) const = 0;
    virtual bool hasMapConfig(MapTypes::Enum map) const = 0;

    // Global descriptor methods
    virtual std::string_view getGlobalDescription(std::string_view category, unsigned bit_index) const = 0;
    virtual const GlobalConfig &getGlobalConfig() const = 0;

    // Reload all configs (for development)
    virtual void reload() = 0;
};

} // namespace okami
