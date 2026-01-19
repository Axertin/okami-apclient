#pragma once

#include <expected>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

/**
 * @brief Configuration and session data parsed from AP slot_data
 *
 * This struct holds randomizer settings and session info received from the server.
 * Tries to use safe defaults when fields are missing or malformed.
 */
struct SlotConfig
{
    std::string seedNumber;            // Unique seed identifier
    std::string seedName;              // Human-readable seed name
    std::optional<int> totalLocations; // Total location count (if provided)

    std::string supportedClientVersion; // Minimum client version APWorld expects (semver, e.g. "1.0.0")

    bool randomizeContainers = false;
    bool randomizeShops = false;
    bool randomizeBrushes = false;

    int shopSlots = 6;

    // bool skipCutscenes = false;

    /**
     * @brief Parse slot_data JSON into a SlotConfig
     *
     * Missing or malformed fields use safe defaults rather than failing.
     * Only returns an error for catastrophic parse failures.
     *
     * @param slotData The slot_data JSON object from Connected packet
     * @return SlotConfig on success, error message on failure
     */
    [[nodiscard]] static std::expected<SlotConfig, std::string> parse(const nlohmann::json &slotData);

    /**
     * @brief Create a default config (all randomization disabled)
     */
    [[nodiscard]] static SlotConfig defaults();
};
