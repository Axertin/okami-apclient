#pragma once
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "okami/igamestateregistry.h"

namespace okami
{

/**
 * @brief YAML-based implementation of game state registry.
 *
 * Loads game state flag definitions from YAML files and provides thread-safe
 * access to flag descriptions and categories.
 */
class GameStateRegistry : public IGameStateRegistry
{
  private:
    std::unordered_map<MapTypes::Enum, MapStateConfig> map_configs_;
    GlobalConfig global_config_;
    std::filesystem::path config_dir_;
    bool global_loaded_ = false;

    // Thread safety
    mutable std::mutex mutex_;
    static inline GameStateRegistry *instance_;
    static inline std::once_flag init_flag_;

  public:
    explicit GameStateRegistry(std::filesystem::path config_directory);
    ~GameStateRegistry() = default;

    // Non-copyable, non-movable
    GameStateRegistry(const GameStateRegistry &) = delete;
    GameStateRegistry &operator=(const GameStateRegistry &) = delete;
    GameStateRegistry(GameStateRegistry &&) = delete;
    GameStateRegistry &operator=(GameStateRegistry &&) = delete;

    // Singleton access
    static IGameStateRegistry &instance();
    static void initialize(const std::filesystem::path &config_dir);

    // For testing - allows injection of mock implementations
    static void setInstance(std::unique_ptr<IGameStateRegistry> test_instance);
    static void resetInstance(); // For cleanup between tests

    // IGameStateRegistry implementation
    std::string_view getMapDescription(MapTypes::Enum map, std::string_view category, unsigned bit_index) const override;
    const MapStateConfig &getMapConfig(MapTypes::Enum map) const override;
    bool hasMapConfig(MapTypes::Enum map) const override;

    std::string_view getGlobalDescription(std::string_view category, unsigned bit_index) const override;
    const GlobalConfig &getGlobalConfig() const override;

    void reload() override;

  private:
    void loadMapConfig(MapTypes::Enum map);
    void loadGlobalConfig();
    MapStateConfig parseMapYamlFile(const std::filesystem::path &file_path) const;
    GlobalConfig parseGlobalYamlFile(const std::filesystem::path &file_path) const;
    static std::filesystem::path getModuleDirectory();

    // For testing
    static inline std::unique_ptr<IGameStateRegistry> test_instance_;
};

} // namespace okami
