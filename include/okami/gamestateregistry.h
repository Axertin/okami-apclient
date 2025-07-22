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
 * @brief YAML-based game state registry providing descriptions for game flags.
 *
 * The GameStateRegistry is a thread-safe singleton that loads and caches game state
 * flag definitions from YAML files. It provides human-readable descriptions for the
 * thousands of bit flags that Okami uses to track game progress, NPC interactions,
 * collected items, and other state.
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
    /**
     * @brief Constructs registry with specified config directory.
     * @param config_directory Path to directory containing global.yml and maps/ subdirectory
     */
    explicit GameStateRegistry(std::filesystem::path config_directory);
    ~GameStateRegistry() = default;

    // Non-copyable, non-movable singleton
    GameStateRegistry(const GameStateRegistry &) = delete;
    GameStateRegistry &operator=(const GameStateRegistry &) = delete;
    GameStateRegistry(GameStateRegistry &&) = delete;
    GameStateRegistry &operator=(GameStateRegistry &&) = delete;

    /**
     * @brief Gets the singleton instance.
     * @return Reference to the global GameStateRegistry instance
     *
     * On first call, initializes the registry with game-data directory located
     * relative to the module (DLL) location. Thread-safe initialization guaranteed.
     */
    static IGameStateRegistry &instance();

    /**
     * @brief Manually initialize with custom config directory.
     * @param config_dir Path to configuration directory
     *
     * Optional - if not called, instance() will auto-initialize with default location.
     * Must be called before first instance() access if custom path needed.
     */
    static void initialize(const std::filesystem::path &config_dir);

    /**
     * @brief Set a test instance for unit testing.
     * @param test_instance Mock implementation for testing
     *
     * When set, instance() returns this instead of the real singleton.
     * Use resetInstance() to clear.
     */
    static void setInstance(std::unique_ptr<IGameStateRegistry> test_instance);

    /**
     * @brief Clear test instance set by setInstance().
     */
    static void resetInstance();

    // ===== IGameStateRegistry Interface Implementation =====

    /**
     * @brief Get description for a specific map state flag.
     * @param map The map type to query
     * @param category Flag category (e.g. "worldStateBits", "collectedObjects")
     * @param bit_index The bit index within the category
     * @return Description string, or empty if not found/documented
     *
     * Thread-safe. Triggers lazy loading of map config on first access.
     */
    std::string_view getMapDescription(MapTypes::Enum map, std::string_view category, unsigned bit_index) const override;

    /**
     * @brief Get full configuration for a map.
     * @param map The map type to query
     * @return Reference to map configuration (empty if not found)
     *
     * Thread-safe. Triggers lazy loading of map config on first access.
     * The returned reference remains valid for the lifetime of the registry.
     *
     */
    const MapStateConfig &getMapConfig(MapTypes::Enum map) const override;

    /**
     * @brief Check if configuration exists for a map.
     * @param map The map type to check
     * @return true if YAML file was found and loaded for this map
     *
     * Thread-safe. Does not trigger loading.
     */
    bool hasMapConfig(MapTypes::Enum map) const override;

    /**
     * @brief Get description for a global state flag.
     * @param category Flag category (e.g. "globalGameState", "brushUpgrades")
     * @param bit_index The bit index within the category
     * @return Description string, or empty if not found/documented
     *
     * Thread-safe. Triggers lazy loading of global config on first access.
     */
    std::string_view getGlobalDescription(std::string_view category, unsigned bit_index) const override;

    /**
     * @brief Get full global configuration.
     * @return Reference to global configuration
     *
     * Thread-safe. Triggers lazy loading of global config on first access.
     * The returned reference remains valid for the lifetime of the registry.
     */
    const GlobalConfig &getGlobalConfig() const override;

    /**
     * @brief Force reload of all configurations.
     *
     * Clears all cached data. Next access will reload from YAML files.
     * Useful for development/debugging when YAML files change.
     *
     * Thread-safe but may cause brief performance hit as other threads
     * wait for reloading to complete.
     */
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
