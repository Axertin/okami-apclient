#pragma once
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "okami/data/maptype.hpp"

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
};

class ConfigManager
{
  private:
    std::unordered_map<MapTypes::Enum, MapStateConfig> map_configs_;
    GlobalConfig global_config_;
    std::filesystem::path config_dir_;
    bool global_loaded_ = false;

    // Thread safety
    mutable std::mutex mutex_;
    static inline ConfigManager *instance_;
    static inline std::once_flag init_flag_;

  public:
    explicit ConfigManager(std::filesystem::path config_directory);
    ~ConfigManager() = default;

    // Non-copyable, non-movable
    ConfigManager(const ConfigManager &) = delete;
    ConfigManager &operator=(const ConfigManager &) = delete;
    ConfigManager(ConfigManager &&) = delete;
    ConfigManager &operator=(ConfigManager &&) = delete;

    // Singleton access
    static const ConfigManager &instance();
    static void initialize(const std::filesystem::path &config_dir);

    // Map-specific methods
    std::string_view getMapDescription(MapTypes::Enum map, std::string_view category, unsigned bit_index) const;
    const MapStateConfig &getMapConfig(MapTypes::Enum map) const;
    bool hasMapConfig(MapTypes::Enum map) const;

    // Global descriptor methods
    std::string_view getGlobalDescription(std::string_view category, unsigned bit_index) const;
    const GlobalConfig &getGlobalConfig() const;

    // Reload all configs (for development)
    void reload();

  private:
    void loadMapConfig(MapTypes::Enum map);
    void loadGlobalConfig();
    MapStateConfig parseMapYamlFile(const std::filesystem::path &file_path) const;
    GlobalConfig parseGlobalYamlFile(const std::filesystem::path &file_path) const;
    std::filesystem::path getModuleDirectory() const;
};

} // namespace okami
