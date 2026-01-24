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
    // === Session Info ===
    std::string seedNumber;            // Unique seed identifier
    std::string seedName;              // Human-readable seed name
    std::optional<int> totalLocations; // Total location count (if provided)

    std::string supportedClientVersion; // Minimum client version APWorld expects (semver, e.g. "1.0.0")

    // === Randomization Options ===
    bool randomizeContainers = false;
    bool randomizeShops = false;
    bool randomizeBrushes = false;

    // === General Options ===
    bool buriedChestsByNight = true;   // Buried chests logically require Crescent
    int karmicTransformers = 1;        // 0=excluded, 1=precollected, 2=in_item_pool
    bool openGameStart = true;         // Remove early events for open start
    bool progressiveWeapons = false;   // Progressive weapons vs individual
    bool removeBlockHead = true;       // Remove Blockhead encounters
    bool bloomGuardianSaplings = true; // Bloom guardian saplings at start

    // === Orochi Arc Options ===
    int requiredDoggorbs = 1; // Canine warriors needed for Gale Shrine (1-8)
    int canineRewards = 1;    // 0=vanilla, 1=randomized, 2=junk
    int moonCaveAccess = 0;   // 0=serpent_crystal, 1=crimson_helm, 2=open

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
