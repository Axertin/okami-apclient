#include "containermanager.h"

#include <filesystem>
#include <fstream>
#include <list>

#include <nlohmann/json.hpp>
#include <wolf_framework.hpp>

#include "apitem.h"
#include "isocket.h"

// __fastcall is Windows-specific calling convention
#ifndef _WIN32
#define __fastcall
#endif

namespace container_manager
{

namespace
{

// Socket for AP communication (injected via setSocket)
ISocket *g_socket = nullptr;

// Containers indexed by level
std::unordered_map<int16_t, std::vector<ContainerDef>> g_containersByLevel;

// Original function pointer for SpawnTablePopulator hook
using SpawnTablePopulatorFn = void(__fastcall *)(void *spawn_table);
SpawnTablePopulatorFn g_originalSpawnTablePopulator = nullptr;

// Original function pointer for container interaction hook (TBD)
using ContainerInteractionFn = void(__fastcall *)(void *container_data);
ContainerInteractionFn g_originalContainerInteraction = nullptr;

// Hook state
bool g_initialized = false;

/**
 * @brief Load containers from JSON file
 *
 * Expected JSON format:
 * {
 *   "version": 1,
 *   "container_count": 1001,
 *   "containers": [
 *     {"id": 0, "level_id": 6, "contents_id": 51, "x": 72, "y": 9, "z": -60, "container_type": 0},
 *     ...
 *   ]
 * }
 */
bool loadContainersJson(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path))
    {
        wolf::logError("[ContainerManager] containers.json not found at: %s", path.string().c_str());
        return false;
    }

    try
    {
        std::ifstream file(path);
        nlohmann::json json = nlohmann::json::parse(file);

        const auto &containers = json["containers"];
        wolf::logInfo("[ContainerManager] Loading %zu containers from JSON", containers.size());

        g_containersByLevel.clear();

        for (const auto &c : containers)
        {
            ContainerDef def{.id = c["id"].get<int32_t>(),
                             .level_id = static_cast<int16_t>(c["level_id"].get<int>()),
                             .x = static_cast<int16_t>(c["x"].get<int>()),
                             .y = static_cast<int16_t>(c["y"].get<int>()),
                             .z = static_cast<int16_t>(c["z"].get<int>()),
                             .container_type = static_cast<uint8_t>(c["container_type"].get<int>()),
                             .original_contents_id = static_cast<uint8_t>(c["contents_id"].get<int>())};

            g_containersByLevel[def.level_id].push_back(def);
        }

        wolf::logInfo("[ContainerManager] Loaded %zu containers across %zu levels", containers.size(), g_containersByLevel.size());
        return true;
    }
    catch (const nlohmann::json::exception &e)
    {
        wolf::logError("[ContainerManager] Failed to parse containers.json: %s", e.what());
        return false;
    }
    catch (const std::exception &e)
    {
        wolf::logError("[ContainerManager] Failed to load containers.json: %s", e.what());
        return false;
    }
}

/**
 * @brief SpawnTablePopulator hook function
 *
 * Called after ITS data is parsed from the .bin level file.
 * We call the original first to populate the table, then modify entries.
 */
void __fastcall hkSpawnTablePopulator(void *spawn_table)
{
    // Call original to populate the table
    if (g_originalSpawnTablePopulator)
    {
        g_originalSpawnTablePopulator(spawn_table);
    }

    // Process the populated table
    detail::onSpawnTablePopulate(static_cast<okami::SpawnTable *>(spawn_table));
}

/**
 * @brief Container interaction hook function (when offset is found)
 */
void __fastcall hkContainerInteraction(void *container_data)
{
    // Process the interaction first
    detail::onContainerInteraction(container_data);

    // Call original
    if (g_originalContainerInteraction)
    {
        g_originalContainerInteraction(container_data);
    }
}

} // anonymous namespace

bool initialize()
{
    if (g_initialized)
    {
        wolf::logWarning("[ContainerManager] Already initialized");
        return true;
    }

    wolf::logInfo("[ContainerManager] Initializing...");

    // Load containers from JSON
    // Path is relative to the DLL location: ./data/containers.json
    std::filesystem::path jsonPath = "data/containers.json";
    if (!loadContainersJson(jsonPath))
    {
        wolf::logError("[ContainerManager] Failed to load containers, aborting initialization");
        return false;
    }

    // Install SpawnTablePopulator hook
    wolf::logDebug("[ContainerManager] Installing SpawnTablePopulator hook at offset 0x%X", SPAWN_TABLE_POPULATOR_OFFSET);

    if (!wolf::hookFunction("main.dll", SPAWN_TABLE_POPULATOR_OFFSET, &hkSpawnTablePopulator, &g_originalSpawnTablePopulator))
    {
        wolf::logError("[ContainerManager] Failed to install SpawnTablePopulator hook");
        return false;
    }

    // Install container interaction hook (only if offset is known)
    if constexpr (CONTAINER_INTERACTION_OFFSET != 0)
    {
        wolf::logDebug("[ContainerManager] Installing ContainerInteraction hook at offset 0x%X", CONTAINER_INTERACTION_OFFSET);

        if (!wolf::hookFunction("main.dll", CONTAINER_INTERACTION_OFFSET, &hkContainerInteraction, &g_originalContainerInteraction))
        {
            wolf::logWarning("[ContainerManager] Failed to install ContainerInteraction hook");
            // Non-fatal - we can still do item replacement without location sends
        }
    }
    else
    {
        wolf::logInfo("[ContainerManager] ContainerInteraction offset not set, location sends disabled");
    }

    g_initialized = true;
    wolf::logInfo("[ContainerManager] Initialization complete");
    return true;
}

void shutdown()
{
    if (!g_initialized)
    {
        return;
    }

    wolf::logInfo("[ContainerManager] Shutting down...");

    // Note: WOLF framework handles hook cleanup automatically
    // Just clear our data
    g_containersByLevel.clear();
    g_originalSpawnTablePopulator = nullptr;
    g_originalContainerInteraction = nullptr;
    g_initialized = false;
}

bool installHooks()
{
    if (g_initialized)
    {
        wolf::logWarning("[ContainerManager] Already initialized");
        return true;
    }

    wolf::logDebug("[ContainerManager] Installing hooks only (test mode)");

    // Install SpawnTablePopulator hook
    if (!wolf::hookFunction("main.dll", SPAWN_TABLE_POPULATOR_OFFSET, &hkSpawnTablePopulator, &g_originalSpawnTablePopulator))
    {
        wolf::logError("[ContainerManager] Failed to install SpawnTablePopulator hook");
        return false;
    }

    // Install container interaction hook (only if offset is known)
    if constexpr (CONTAINER_INTERACTION_OFFSET != 0)
    {
        if (!wolf::hookFunction("main.dll", CONTAINER_INTERACTION_OFFSET, &hkContainerInteraction, &g_originalContainerInteraction))
        {
            wolf::logWarning("[ContainerManager] Failed to install ContainerInteraction hook");
        }
    }

    g_initialized = true;
    return true;
}

std::optional<ContainerDef> findContainer(int16_t level_id, int16_t x, int16_t y, int16_t z)
{
    auto it = g_containersByLevel.find(level_id);
    if (it == g_containersByLevel.end())
    {
        return std::nullopt;
    }

    for (const auto &c : it->second)
    {
        if (c.x == x && c.y == y && c.z == z)
        {
            return c;
        }
    }
    return std::nullopt;
}

std::vector<ContainerDef> getContainersForLevel(int16_t level_id)
{
    auto it = g_containersByLevel.find(level_id);
    if (it != g_containersByLevel.end())
    {
        return it->second;
    }
    return {};
}

std::size_t getContainerCount()
{
    std::size_t count = 0;
    for (const auto &[level_id, containers] : g_containersByLevel)
    {
        count += containers.size();
    }
    return count;
}

bool loadContainersFromPath(const std::filesystem::path &path)
{
    return loadContainersJson(path);
}

void clearContainers()
{
    g_containersByLevel.clear();
}

void setSocket(ISocket *socket)
{
    g_socket = socket;
}

namespace detail
{

// Cached scouted data for current level
std::unordered_map<int64_t, ScoutedItem> g_scoutedContainers;

void onSpawnTablePopulate(okami::SpawnTable *table)
{
    // Get current level ID
    uintptr_t main_base = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
    auto *currentMapPtr = reinterpret_cast<uint16_t *>(main_base + CURRENT_MAP_ID_OFFSET);
    uint16_t currentLevel = *currentMapPtr;

    wolf::logDebug("[ContainerManager] SpawnTable populated for level 0x%04X", currentLevel);

    // Get containers for this level
    auto levelContainers = getContainersForLevel(static_cast<int16_t>(currentLevel));
    if (levelContainers.empty())
    {
        wolf::logDebug("[ContainerManager] No tracked containers for level 0x%04X", currentLevel);
        return;
    }

    wolf::logInfo("[ContainerManager] Level 0x%04X has %zu tracked containers", currentLevel, levelContainers.size());

    // Build list of location IDs to scout
    std::list<int64_t> locationIds;
    for (const auto &container : levelContainers)
    {
        locationIds.push_back(getLocationId(container.id));
    }

    // Scout containers from AP server (blocking)
    if (!g_socket)
    {
        wolf::logWarning("[ContainerManager] No socket configured, skipping container randomization");
        return;
    }

    if (!g_socket->isConnected())
    {
        wolf::logWarning("[ContainerManager] Not connected to AP, skipping container randomization");
        return;
    }

    wolf::logDebug("[ContainerManager] Scouting %zu locations...", locationIds.size());
    auto scoutedItems = g_socket->scoutLocationsSync(locationIds);

    if (scoutedItems.empty())
    {
        wolf::logWarning("[ContainerManager] Scout returned no items, leaving containers vanilla");
        return;
    }

    wolf::logInfo("[ContainerManager] Received %zu scouted items", scoutedItems.size());

    // Build lookup map from location_id -> scouted data
    g_scoutedContainers.clear();
    for (const auto &item : scoutedItems)
    {
        g_scoutedContainers[item.location] = item;
    }

    // Get our player slot to determine local vs external items
    int ourSlot = g_socket->getPlayerSlot();

    int matchedCount = 0;
    int replacedCount = 0;

    // Scan spawn table entries and apply replacements
    for (int i = 0; i < 128; i++)
    {
        okami::SpawnTableEntry *entry = &table->entries[i];

        // Check if entry is enabled and has spawn_data
        if (!(entry->flags & 1) || !entry->spawn_data)
        {
            continue;
        }

        okami::ContainerData *sd = entry->spawn_data;

        // Try to find this container in our database
        auto containerOpt = findContainer(static_cast<int16_t>(currentLevel), sd->position_x, sd->position_y, sd->position_z);

        if (!containerOpt)
        {
            continue;
        }

        const auto &container = *containerOpt;
        int64_t locationId = getLocationId(container.id);
        matchedCount++;

        // Check if we have scouted data for this container
        auto scoutIt = g_scoutedContainers.find(locationId);
        if (scoutIt == g_scoutedContainers.end())
        {
            wolf::logDebug("[ContainerManager] Container %d not in scout data, leaving vanilla", container.id);
            continue;
        }

        const auto &scouted = scoutIt->second;
        bool isLocal = (scouted.player == ourSlot);

        // Determine what item to place in the container
        uint8_t newItemId = DUMMY_ITEM_ID;

        if (isLocal)
        {
            // Local item - try to resolve to actual game item
            auto gameItemId = ap::getGameItemId(scouted.item);
            if (gameItemId)
            {
                newItemId = *gameItemId;
                wolf::logDebug("[ContainerManager] Container %d: local GameItem 0x%02X", container.id, newItemId);
            }
            else
            {
                // Non-GameItem reward (brush, event flag, etc.) - use dummy
                wolf::logDebug("[ContainerManager] Container %d: local non-GameItem, using dummy", container.id);
            }
        }
        else
        {
            // External item - always use dummy
            wolf::logDebug("[ContainerManager] Container %d: external item for player %d, using dummy", container.id, scouted.player);
        }

        wolf::logDebug("[ContainerManager] Container %d: replacing 0x%02X with 0x%02X (AP item %lld)", container.id, sd->item_id, newItemId, scouted.item);

        sd->item_id = newItemId;
        replacedCount++;
    }

    wolf::logInfo("[ContainerManager] Matched %d/%zu containers, replaced %d items", matchedCount, levelContainers.size(), replacedCount);
}

void onContainerInteraction(void *container_data)
{
    // TODO: Implement when we have the hook offset
    // This will:
    // 1. Extract position from container_data
    // 2. Look up container ID
    // 3. Send location to AP server
    // 4. If dummy item, grant the actual AP reward

    wolf::logDebug("[ContainerManager] Container interaction (handler not yet implemented)");
}

} // namespace detail

} // namespace container_manager
