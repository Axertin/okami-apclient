#include "containermanager.h"

#include <unordered_set>
#include <vector>

#include <wolf_framework.hpp>

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

// Track container indices that were randomized (cleared on level load)
std::unordered_set<int> g_randoContainerIndices;

// Current level ID (cached)
uint16_t g_currentLevelId = 0;

// Original function pointer for SpawnTablePopulator hook
using SpawnTablePopulatorFn = void(__fastcall *)(void *spawn_table);
SpawnTablePopulatorFn g_originalSpawnTablePopulator = nullptr;

// Hook state
bool g_initialized = false;

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

} // anonymous namespace

bool initialize()
{
    if (g_initialized)
    {
        wolf::logWarning("[ContainerManager] Already initialized");
        return true;
    }

    if (!wolf::hookFunction("main.dll", SPAWN_TABLE_POPULATOR_OFFSET, &hkSpawnTablePopulator, &g_originalSpawnTablePopulator))
    {
        wolf::logError("[ContainerManager] Failed to install SpawnTablePopulator hook");
        return false;
    }

    g_initialized = true;
    return true;
}

void shutdown()
{
    if (!g_initialized)
    {
        return;
    }

    g_randoContainerIndices.clear();
    g_originalSpawnTablePopulator = nullptr;
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

    g_initialized = true;
    return true;
}

void setSocket(ISocket *socket)
{
    g_socket = socket;
}

bool isContainerInRando(int64_t locationId)
{
    // Stub implementation - always returns true if connected to AP
    // TODO: Query AP server for slot data to determine which containers
    // are in the randomization pool vs vanilla
    (void)locationId; // Suppress unused warning
    if (!g_socket)
        return false;

    return g_socket->isConnected();
}

void pollForPickups()
{
    // Skip if no tracked containers or no socket
    if (g_randoContainerIndices.empty() || !g_socket || !g_socket->isConnected())
    {
        return;
    }

    // Get spawn table pointer
    uintptr_t main_base = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
    auto *table = reinterpret_cast<okami::SpawnTable *>(main_base + SPAWN_TABLE_OFFSET);

    // Check each tracked container to see if it's been opened/picked up
    std::vector<int> pickedUp;
    for (int containerIdx : g_randoContainerIndices)
    {
        if (containerIdx < 0 || containerIdx >= 128)
        {
            continue;
        }

        okami::SpawnTableEntry *entry = &table->entries[containerIdx];

        // Container item is collected when spawn_type_1 becomes 0
        // State machine: 1 (closed) -> 3 (opened, item floating) -> 0 (collected)
        // We only send on state 0, not state 3, because chest may timeout and revert
        if (entry->spawn_type_1 == 0)
        {
            int64_t locationId = getContainerLocationId(g_currentLevelId, containerIdx);

            wolf::logInfo("[ContainerManager] Container at idx %d collected, sending location %lld", containerIdx, locationId);
            g_socket->sendLocation(locationId);

            pickedUp.push_back(containerIdx);
        }
    }

    // Remove opened containers from tracking
    for (int idx : pickedUp)
    {
        g_randoContainerIndices.erase(idx);
    }
}

namespace detail
{

void onSpawnTablePopulate(okami::SpawnTable *table)
{
    // Get current level ID
    uintptr_t main_base = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
    auto *currentMapPtr = reinterpret_cast<uint16_t *>(main_base + CURRENT_MAP_ID_OFFSET);
    g_currentLevelId = *currentMapPtr;

    // Clear tracking from previous level
    g_randoContainerIndices.clear();

    wolf::logDebug("[ContainerManager] SpawnTable populated for level 0x%04X", g_currentLevelId);

    int replacedCount = 0;

    // Scan spawn table entries for containers (spawn_type_1 == 1)
    // Item ID is stored directly in the container's spawn_data->item_id
    for (int i = 0; i < 128; i++)
    {
        okami::SpawnTableEntry *entry = &table->entries[i];

        // Check if entry is enabled and has spawn_data
        if (!(entry->flags & 1) || !entry->spawn_data)
        {
            continue;
        }

        // Only process container entries (spawn_type_1 == 1 = chests)
        if (entry->spawn_type_1 != 1)
        {
            continue;
        }

        // Calculate location ID for this container
        int64_t locationId = getContainerLocationId(g_currentLevelId, i);

        // Check if this container is part of randomization
        if (!isContainerInRando(locationId))
        {
            continue;
        }

        // Replace item directly in the container's spawn_data
        okami::ContainerData *containerData = entry->spawn_data;
        wolf::logDebug("[ContainerManager] Idx %d: Replacing item 0x%02X with dummy 0x%02X (location %lld)", i, containerData->item_id, DUMMY_ITEM_ID,
                       locationId);

        containerData->item_id = DUMMY_ITEM_ID;
        g_randoContainerIndices.insert(i);
        replacedCount++;
    }

    wolf::logDebug("[ContainerManager] Randomized %d containers in level 0x%04X", replacedCount, g_currentLevelId);
}

} // namespace detail

} // namespace container_manager
