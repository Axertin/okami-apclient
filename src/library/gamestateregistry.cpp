#include "okami/gamestateregistry.h"

#include <format>
#include <fstream>
#include <iostream>
#include <mutex>

#include <yaml-cpp/yaml.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#endif

#define REGISTRY_LOG(msg) std::cout << "[GameStateRegistry] " << (msg) << std::endl
#define REGISTRY_ERR(msg) std::cerr << "[GameStateRegistry] ERROR: " << (msg) << std::endl

namespace okami
{

GameStateRegistry::GameStateRegistry(std::filesystem::path config_directory) : config_dir_(std::move(config_directory))
{
    if (!config_dir_.empty() && !std::filesystem::exists(config_dir_))
    {
        try
        {
            std::filesystem::create_directories(config_dir_);
        }
        catch (const std::exception &e)
        {
            REGISTRY_ERR(std::format("Exception creating yaml directories: {}", e.what()));
            // If we can't create directories, just continue with empty config
        }
    }
}

void GameStateRegistry::initialize(const std::filesystem::path &config_dir)
{
    std::call_once(init_flag_, [&config_dir]() { instance_ = new GameStateRegistry(config_dir); });
}

IGameStateRegistry &GameStateRegistry::instance()
{
    // Check for test instance first
    if (test_instance_)
    {
        return *test_instance_;
    }

    std::call_once(init_flag_,
                   []()
                   {
                       if (!instance_)
                       {
                           try
                           {
                               auto game_data_path = getModuleDirectory() / "game-data";
                               REGISTRY_LOG(std::format("Looking for game-data at: {}", game_data_path.string()));

                               if (!std::filesystem::exists(game_data_path))
                               {
                                   REGISTRY_ERR(std::format("game-data directory not found at: {}", game_data_path.string()));
                               }

                               instance_ = new GameStateRegistry(game_data_path);
                           }
                           catch (const std::exception &e)
                           {
                               REGISTRY_ERR(std::format("Exception during initialization: {}", e.what()));
                               // Fallback to empty config if anything fails
                               instance_ = new GameStateRegistry({});
                           }
                       }
                   });
    return *instance_;
}

void GameStateRegistry::setInstance(std::unique_ptr<IGameStateRegistry> test_instance)
{
    test_instance_ = std::move(test_instance);
}

void GameStateRegistry::resetInstance()
{
    test_instance_.reset();
    // Note: We intentionally don't reset instance_ as it's meant to live for program duration
}

std::string_view GameStateRegistry::getMapDescription(MapTypes::Enum map, std::string_view category, unsigned bit_index) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    // Load map config if not already loaded
    if (!hasMapConfig(map))
    {
        const_cast<GameStateRegistry *>(this)->loadMapConfig(map);
    }
    else
    {
    }

    static const MapStateConfig empty_config{};
    const auto &config = (map_configs_.find(map) != map_configs_.end()) ? map_configs_.at(map) : empty_config;

    static const std::unordered_map<std::string_view, const std::unordered_map<unsigned, std::string> MapStateConfig::*> category_map = {
        {"worldStateBits", &MapStateConfig::worldStateBits},
        {"userIndices", &MapStateConfig::userIndices},
        {"collectedObjects", &MapStateConfig::collectedObjects},
        {"areasRestored", &MapStateConfig::areasRestored},
        {"treesBloomed", &MapStateConfig::treesBloomed},
        {"cursedTreesBloomed", &MapStateConfig::cursedTreesBloomed},
        {"fightsCleared", &MapStateConfig::fightsCleared},
        {"npcs", &MapStateConfig::npcs},
        {"mapsExplored", &MapStateConfig::mapsExplored},
        {"field_DC", &MapStateConfig::field_DC},
        {"field_E0", &MapStateConfig::field_E0}};

    if (auto it = category_map.find(category); it != category_map.end())
    {
        const auto &target_map = config.*(it->second);
        if (auto desc_it = target_map.find(bit_index); desc_it != target_map.end())
        {
            return desc_it->second;
        }
    }

    return {};
}

std::string_view GameStateRegistry::getGlobalDescription(std::string_view category, unsigned bit_index) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    // Load global config if not already loaded
    if (!global_loaded_)
    {
        const_cast<GameStateRegistry *>(this)->loadGlobalConfig();
    }

    const auto &config = getGlobalConfig();

    static const std::unordered_map<std::string_view, const std::unordered_map<unsigned, std::string> GlobalConfig::*> category_map = {
        {"brushUpgrades", &GlobalConfig::brushUpgrades},    {"areasRestored", &GlobalConfig::areasRestored},
        {"commonStates", &GlobalConfig::commonStates},      {"gameProgress", &GlobalConfig::gameProgress},
        {"keyItemsFound", &GlobalConfig::keyItemsFound},    {"goldDustsFound", &GlobalConfig::goldDustsFound},
        {"animalsFound", &GlobalConfig::animalsFound},      {"animalsFedFirstTime", &GlobalConfig::animalsFedFirstTime},
        {"globalGameState", &GlobalConfig::globalGameState}};

    if (auto it = category_map.find(category); it != category_map.end())
    {
        const auto &target_map = config.*(it->second);
        if (auto desc_it = target_map.find(bit_index); desc_it != target_map.end())
        {
            return desc_it->second;
        }
    }

    return {};
}

const MapStateConfig &GameStateRegistry::getMapConfig(MapTypes::Enum map) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!hasMapConfig(map))
    {
        const_cast<GameStateRegistry *>(this)->loadMapConfig(map);
    }

    static const MapStateConfig empty_config{};

    if (auto it = map_configs_.find(map); it != map_configs_.end())
    {
        return it->second;
    }

    return empty_config;
}

const GlobalConfig &GameStateRegistry::getGlobalConfig() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    // load global config if not already loaded
    if (!global_loaded_)
    {
        const_cast<GameStateRegistry *>(this)->loadGlobalConfig();
    }

    return global_config_;
}

bool GameStateRegistry::hasMapConfig(MapTypes::Enum map) const
{
    // Note: caller should already hold the lock
    bool exists = map_configs_.contains(map);

    return exists;
}

void GameStateRegistry::reload()
{
    REGISTRY_LOG("Reloading yaml registry...");
    std::lock_guard<std::mutex> lock(mutex_);
    map_configs_.clear();
    global_config_ = GlobalConfig{};
    global_loaded_ = false;
    // Configs will be lazy-loaded on next access
}

std::string normalizeMapName(const std::string &mapName)
{
    std::string result;
    for (char c : mapName)
    {
        if (std::isalnum(c))
        {
            result += c;
        }
        // Skip spaces, parentheses, etc.
    }
    return result;
}

void GameStateRegistry::loadMapConfig(MapTypes::Enum map)
{
    if (config_dir_.empty())
    {
        map_configs_[map] = MapStateConfig{};
        return;
    }

    // Normalize the map name to match generated filenames
    std::string mapName = normalizeMapName(MapTypes::GetName(map));
    auto file_path = config_dir_ / "maps" / std::format("{}.yml", mapName);

    if (!std::filesystem::exists(file_path))
    {
        map_configs_[map] = MapStateConfig{};
        return;
    }

    try
    {
        // REGISTRY_LOG(std::format("Loading map config: {}", file_path.filename().string()));
        map_configs_[map] = parseMapYamlFile(file_path);
    }
    catch (const std::exception &e)
    {
        REGISTRY_ERR(std::format("Failed to parse {}: {}", file_path.filename().string(), e.what()));
        map_configs_[map] = MapStateConfig{};
    }
}

void GameStateRegistry::loadGlobalConfig()
{
    // Note: caller should already hold the lock
    if (global_loaded_)
    {
        return; // Prevent double-loading
    }

    if (config_dir_.empty())
    {
        REGISTRY_LOG("Config directory is empty, using empty global config");
        global_config_ = GlobalConfig{};
        global_loaded_ = true;
        return;
    }

    auto file_path = config_dir_ / "global.yml";
    REGISTRY_LOG(std::format("Loading global.yml from: {}", file_path.string()));

    if (!std::filesystem::exists(file_path))
    {
        REGISTRY_ERR(std::format("global.yml not found at: {}", file_path.string()));
        global_config_ = GlobalConfig{};
        global_loaded_ = true;
        return;
    }

    try
    {
        global_config_ = parseGlobalYamlFile(file_path);
    }
    catch (const std::exception &e)
    {
        REGISTRY_ERR(std::format("Failed to parse global.yml: {}", e.what()));
        // Use empty config on parse failure
        global_config_ = GlobalConfig{};
    }

    global_loaded_ = true;
}

MapStateConfig GameStateRegistry::parseMapYamlFile(const std::filesystem::path &file_path) const
{
    YAML::Node root = YAML::LoadFile(file_path.string());
    MapStateConfig config;

    auto parseCategory = [&](const std::string &key, auto &target_map)
    {
        if (auto node = root[key])
        {
            for (const auto &item : node)
            {
                try
                {
                    unsigned index = item.first.as<unsigned>();
                    std::string description = item.second.as<std::string>();
                    target_map[index] = std::move(description);
                }
                catch (const std::exception &e)
                {
                    REGISTRY_ERR(std::format("Failed to parse map yaml entry: {}", e.what()));
                    // Skip malformed entries
                    continue;
                }
            }
        }
    };

    parseCategory("worldStateBits", config.worldStateBits);
    parseCategory("userIndices", config.userIndices);
    parseCategory("collectedObjects", config.collectedObjects);
    parseCategory("areasRestored", config.areasRestored);
    parseCategory("treesBloomed", config.treesBloomed);
    parseCategory("cursedTreesBloomed", config.cursedTreesBloomed);
    parseCategory("fightsCleared", config.fightsCleared);
    parseCategory("npcs", config.npcs);
    parseCategory("mapsExplored", config.mapsExplored);
    parseCategory("field_DC", config.field_DC);
    parseCategory("field_E0", config.field_E0);

    return config;
}

GlobalConfig GameStateRegistry::parseGlobalYamlFile(const std::filesystem::path &file_path) const
{
    YAML::Node root = YAML::LoadFile(file_path.string());
    GlobalConfig config;

    auto parseCategory = [&](const std::string &key, auto &target_map)
    {
        if (auto node = root[key])
        {
            for (const auto &item : node)
            {
                try
                {
                    unsigned index = item.first.as<unsigned>();
                    std::string description = item.second.as<std::string>();
                    target_map[index] = std::move(description);
                }
                catch (const std::exception &e)
                {
                    REGISTRY_ERR(std::format("Failed to parse global yaml entry: {}", e.what()));
                    // Skip malformed entries
                    continue;
                }
            }
        }
    };

    parseCategory("brushUpgrades", config.brushUpgrades);
    parseCategory("areasRestored", config.areasRestored);
    parseCategory("commonStates", config.commonStates);
    parseCategory("gameProgress", config.gameProgress);
    parseCategory("keyItemsFound", config.keyItemsFound);
    parseCategory("goldDustsFound", config.goldDustsFound);
    parseCategory("animalsFound", config.animalsFound);
    parseCategory("animalsFedFirstTime", config.animalsFedFirstTime);
    parseCategory("globalGameState", config.globalGameState);

    return config;
}

std::filesystem::path GameStateRegistry::getModuleDirectory()
{
#ifdef _WIN32
    HMODULE hModule = nullptr;
    // Use a lambda converted to function pointer to get an address in this module
    auto dummy = +[]() {};
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(dummy), &hModule))
    {
        REGISTRY_ERR("Failed to get current module path, falling back to current path");
        // Fallback to current directory if we can't get module path
        return std::filesystem::current_path();
    }

    wchar_t path[MAX_PATH];
    if (GetModuleFileNameW(hModule, path, MAX_PATH) == 0)
    {
        REGISTRY_ERR("Failed to get current module filename, falling back to current path");
        // Fallback to current directory if we can't get module filename
        return std::filesystem::current_path();
    }

    return std::filesystem::path(path).parent_path();
#else
    // For non-Windows platforms (including cross-compilation host)
    return std::filesystem::current_path();
#endif
}

} // namespace okami
