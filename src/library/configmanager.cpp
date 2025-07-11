#include "okami/configmanager.h"

#include <format>
#include <fstream>
#include <mutex>

#include <yaml-cpp/yaml.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#endif

namespace okami
{

ConfigManager::ConfigManager(std::filesystem::path config_directory) : config_dir_(std::move(config_directory))
{
    if (!std::filesystem::exists(config_dir_))
    {
        std::filesystem::create_directories(config_dir_);
    }
}

void ConfigManager::initialize(const std::filesystem::path &config_dir)
{
    instance_ = new ConfigManager(config_dir);
}

const ConfigManager &ConfigManager::instance()
{
    std::call_once(init_flag_,
                   []()
                   {
                       if (!instance_)
                       {
                           // Default initialization - try to find game-data relative to module
                           ConfigManager temp({});
                           auto game_data_path = temp.getModuleDirectory() / "game-data";
                           instance_ = new ConfigManager(game_data_path);
                       }
                   });
    return *instance_;
}

std::string_view ConfigManager::getMapDescription(MapTypes::Enum map, std::string_view category, unsigned bit_index) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!hasMapConfig(map))
    {
        const_cast<ConfigManager *>(this)->loadMapConfig(map);
    }

    const auto &config = getMapConfig(map);

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

std::string_view ConfigManager::getGlobalDescription(std::string_view category, unsigned bit_index) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!global_loaded_)
    {
        const_cast<ConfigManager *>(this)->loadGlobalConfig();
    }

    const auto &config = getGlobalConfig();

    // Use a static map for O(1) lookup and easy extensibility
    static const std::unordered_map<std::string_view, const std::unordered_map<unsigned, std::string> GlobalConfig::*> category_map = {
        {"brushUpgrades", &GlobalConfig::brushUpgrades}, {"areasRestored", &GlobalConfig::areasRestored},
        {"commonStates", &GlobalConfig::commonStates},   {"gameProgress", &GlobalConfig::gameProgress},
        {"keyItemsFound", &GlobalConfig::keyItemsFound}, {"goldDustsFound", &GlobalConfig::goldDustsFound},
        {"animalsFound", &GlobalConfig::animalsFound},   {"animalsFedFirstTime", &GlobalConfig::animalsFedFirstTime}};

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

const MapStateConfig &ConfigManager::getMapConfig(MapTypes::Enum map) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    static const MapStateConfig empty_config{};

    if (auto it = map_configs_.find(map); it != map_configs_.end())
    {
        return it->second;
    }

    return empty_config;
}

const GlobalConfig &ConfigManager::getGlobalConfig() const
{
    return global_config_;
}

bool ConfigManager::hasMapConfig(MapTypes::Enum map) const
{
    // Note: caller should already hold the lock
    return map_configs_.contains(map);
}

void ConfigManager::reload()
{
    std::lock_guard<std::mutex> lock(mutex_);
    map_configs_.clear();
    global_config_ = GlobalConfig{};
    global_loaded_ = false;
    // Configs will be lazy-loaded on next access
}

void ConfigManager::loadMapConfig(MapTypes::Enum map)
{
    auto file_path = config_dir_ / "maps" / std::format("{}.yml", MapTypes::GetName(map));

    if (!std::filesystem::exists(file_path))
    {
        map_configs_[map] = MapStateConfig{};
        return;
    }

    try
    {
        map_configs_[map] = parseMapYamlFile(file_path);
    }
    catch (const std::exception &)
    {
        // Log error and use empty config
        map_configs_[map] = MapStateConfig{};
    }
}

void ConfigManager::loadGlobalConfig()
{
    auto file_path = config_dir_ / "global.yml";

    if (!std::filesystem::exists(file_path))
    {
        global_config_ = GlobalConfig{};
        global_loaded_ = true;
        return;
    }

    try
    {
        global_config_ = parseGlobalYamlFile(file_path);
    }
    catch (const std::exception &)
    {
        // Log error and use empty config
        global_config_ = GlobalConfig{};
    }

    global_loaded_ = true;
}

MapStateConfig ConfigManager::parseMapYamlFile(const std::filesystem::path &file_path) const
{
    YAML::Node root = YAML::LoadFile(file_path.string());
    MapStateConfig config;

    auto parseCategory = [&](const std::string &key, auto &target_map)
    {
        if (auto node = root[key])
        {
            for (const auto &item : node)
            {
                unsigned index = item.first.as<unsigned>();
                std::string description = item.second.as<std::string>();
                target_map[index] = std::move(description);
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

GlobalConfig ConfigManager::parseGlobalYamlFile(const std::filesystem::path &file_path) const
{
    YAML::Node root = YAML::LoadFile(file_path.string());
    GlobalConfig config;

    auto parseCategory = [&](const std::string &key, auto &target_map)
    {
        if (auto node = root[key])
        {
            for (const auto &item : node)
            {
                unsigned index = item.first.as<unsigned>();
                std::string description = item.second.as<std::string>();
                target_map[index] = std::move(description);
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

    return config;
}

std::filesystem::path ConfigManager::getModuleDirectory() const
{
#ifdef _WIN32
    HMODULE hModule = nullptr;
    // Use a lambda converted to function pointer to get an address in this module
    auto dummy = +[]() {};
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCWSTR>(dummy), &hModule))
    {
        // Fallback to current directory if we can't get module path
        return std::filesystem::current_path();
    }

    wchar_t path[MAX_PATH];
    if (GetModuleFileNameW(hModule, path, MAX_PATH) == 0)
    {
        // Fallback to current directory if we can't get module filename
        return std::filesystem::current_path();
    }

    return std::filesystem::path(path).parent_path();
#else
    // For non-Windows platforms (including cross-compilation host)
    // This should never be called in practice since we're building a Windows DLL
    return std::filesystem::current_path();
#endif
}

} // namespace okami
