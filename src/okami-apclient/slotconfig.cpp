#include "slotconfig.h"

#include <wolf_framework.hpp>

namespace
{

// Helper to safely extract a bool from JSON with a default value
bool getBool(const nlohmann::json &obj, const std::string &key, bool defaultValue)
{
    if (!obj.contains(key))
    {
        return defaultValue;
    }

    const auto &value = obj[key];
    if (value.is_boolean())
    {
        return value.get<bool>();
    }

    // Handle integer-as-bool (0/1)
    if (value.is_number_integer())
    {
        return value.get<int>() != 0;
    }

    wolf::logWarning("[SlotConfig] Field '%s' has unexpected type, using default", key.c_str());
    return defaultValue;
}

// Helper to safely extract a string from JSON
std::string getString(const nlohmann::json &obj, const std::string &key, const std::string &defaultValue = "")
{
    if (!obj.contains(key))
    {
        return defaultValue;
    }

    const auto &value = obj[key];
    if (value.is_string())
    {
        return value.get<std::string>();
    }

    // Handle number-as-string (some APWorlds send numeric seeds as numbers)
    if (value.is_number())
    {
        return std::to_string(value.get<int64_t>());
    }

    wolf::logWarning("[SlotConfig] Field '%s' has unexpected type, using default", key.c_str());
    return defaultValue;
}

// Helper to safely extract an optional int from JSON
std::optional<int> getOptionalInt(const nlohmann::json &obj, const std::string &key)
{
    if (!obj.contains(key))
    {
        return std::nullopt;
    }

    const auto &value = obj[key];
    if (value.is_number_integer())
    {
        return value.get<int>();
    }

    wolf::logWarning("[SlotConfig] Field '%s' has unexpected type, ignoring", key.c_str());
    return std::nullopt;
}

// Helper to safely extract an int from JSON with a default value
int getInt(const nlohmann::json &obj, const std::string &key, int defaultValue)
{
    if (!obj.contains(key))
    {
        return defaultValue;
    }

    const auto &value = obj[key];
    if (value.is_number_integer())
    {
        return value.get<int>();
    }

    wolf::logWarning("[SlotConfig] Field '%s' has unexpected type, using default", key.c_str());
    return defaultValue;
}

} // namespace

std::expected<SlotConfig, std::string> SlotConfig::parse(const nlohmann::json &slotData)
{
    // Handle null or non-object gracefully
    if (slotData.is_null())
    {
        wolf::logDebug("[SlotConfig] slot_data is null, using defaults");
        return defaults();
    }

    if (!slotData.is_object())
    {
        wolf::logWarning("[SlotConfig] slot_data is not an object, using defaults");
        return defaults();
    }

    SlotConfig config{
        // Seed/session info (current APWorld fields)
        .seedNumber = getString(slotData, "SeedNumber"),
        .seedName = getString(slotData, "SeedName"),
        .totalLocations = getOptionalInt(slotData, "TotalLocations"),

        // Version compatibility
        .supportedClientVersion = getString(slotData, "supported_client_version"),

        // Randomization flags
        .randomizeContainers = getBool(slotData, "randomize_containers", false),
        .randomizeShops = getBool(slotData, "randomize_shops", false),
        .randomizeBrushes = getBool(slotData, "randomize_brushes", false),

        // Shop configuration
        .shopSlots = getInt(slotData, "shop_slots", 6),
    };

    wolf::logInfo("[SlotConfig] Parsed: seed=%s, name=%s, locations=%s, supported_version=%s",
                  config.seedNumber.empty() ? "(none)" : config.seedNumber.c_str(),
                  config.seedName.empty() ? "(none)" : config.seedName.c_str(),
                  config.totalLocations.has_value() ? std::to_string(*config.totalLocations).c_str() : "(none)",
                  config.supportedClientVersion.empty() ? "(none)" : config.supportedClientVersion.c_str());

    wolf::logDebug("[SlotConfig] Options: containers=%s, shops=%s, brushes=%s, shop_slots=%d",
                   config.randomizeContainers ? "true" : "false", config.randomizeShops ? "true" : "false",
                   config.randomizeBrushes ? "true" : "false", config.shopSlots);

    return config;
}

SlotConfig SlotConfig::defaults()
{
    return SlotConfig{
        .seedNumber = {},
        .seedName = {},
        .totalLocations = std::nullopt,
        .supportedClientVersion = {},
        .randomizeContainers = false,
        .randomizeShops = false,
        .randomizeBrushes = false,
        .shopSlots = 6,
    };
}
