#include "containers.hpp"

#include <cinttypes>

#include <okami/spawntable.h>

#include <wolf_framework.hpp>

#include "../isocket.h"
#include "check_types.hpp"

namespace checks
{

// Static member initialization
ContainerMan::SpawnTablePopulatorFn ContainerMan::originalSpawnTablePopulator_ = nullptr;
ContainerMan *ContainerMan::activeInstance_ = nullptr;

ContainerMan::ContainerMan(ISocket &socket, CheckCallback checkCallback) : socket_(socket), checkCallback_(std::move(checkCallback))
{
}

ContainerMan::~ContainerMan()
{
    shutdown();
}

void ContainerMan::initialize()
{
    if (initialized_)
    {
        wolf::logWarning("[ContainerMan] Already initialized");
        return;
    }

    activeInstance_ = this;

    if (!wolf::hookFunction("main.dll", SPAWN_TABLE_POPULATOR_OFFSET, reinterpret_cast<void *>(&hookSpawnTablePopulator),
                            reinterpret_cast<void **>(&originalSpawnTablePopulator_)))
    {
        wolf::logError("[ContainerMan] Failed to install SpawnTablePopulator hook");
        return;
    }

    initialized_ = true;
    wolf::logDebug("[ContainerMan] Container hook installed");
}

void ContainerMan::shutdown()
{
    if (!initialized_)
    {
        return;
    }

    trackedContainerIndices_.clear();
    activeInstance_ = nullptr;
    initialized_ = false;
}

void ContainerMan::reset()
{
    trackedContainerIndices_.clear();
}

void ContainerMan::hookSpawnTablePopulator(void *spawnTable)
{
    // Call original
    if (originalSpawnTablePopulator_)
    {
        originalSpawnTablePopulator_(spawnTable);
    }

    // Process via active instance
    if (activeInstance_)
    {
        activeInstance_->onSpawnTablePopulate(spawnTable);
    }
}

void ContainerMan::onSpawnTablePopulate(void *spawnTable)
{
    auto *table = static_cast<okami::SpawnTable *>(spawnTable);

    // Get current level ID
    uintptr_t mainBase = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
    auto *currentMapPtr = reinterpret_cast<uint16_t *>(mainBase + CURRENT_MAP_ID_OFFSET);
    currentLevelId_ = *currentMapPtr;

    // Clear tracking from previous level
    trackedContainerIndices_.clear();

    wolf::logDebug("[ContainerMan] SpawnTable populated for level 0x%04X", currentLevelId_);

    int replacedCount = 0;

    // Scan spawn table entries for containers (spawn_type_1 == 1)
    for (int i = 0; i < 128; i++)
    {
        okami::SpawnTableEntry *entry = &table->entries[i];

        // Check if entry is enabled and has spawn_data
        if (!(entry->flags & 1) || !entry->spawn_data)
        {
            continue;
        }

        // Only process container entries
        if (entry->spawn_type_1 != 1)
        {
            continue;
        }

        // Calculate check ID for this container
        int64_t checkId = getContainerCheckId(currentLevelId_, i);

        // Check if this container is part of randomization
        if (!isContainerInRando(checkId))
        {
            continue;
        }

        // Replace item with dummy
        okami::ContainerData *containerData = entry->spawn_data;
        wolf::logDebug("[ContainerMan] Idx %d: Replacing item 0x%02X with dummy 0x%02X (check %" PRId64 ")", i, containerData->item_id, DUMMY_ITEM_ID, checkId);

        containerData->item_id = DUMMY_ITEM_ID;
        trackedContainerIndices_.insert(i);
        replacedCount++;
    }

    wolf::logDebug("[ContainerMan] Randomized %d containers in level 0x%04X", replacedCount, currentLevelId_);
}

void ContainerMan::poll()
{
    if (trackedContainerIndices_.empty() || !socket_.isConnected())
    {
        return;
    }

    uintptr_t mainBase = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
    auto *table = reinterpret_cast<okami::SpawnTable *>(mainBase + SPAWN_TABLE_OFFSET);

    std::vector<int> pickedUp;
    for (int containerIdx : trackedContainerIndices_)
    {
        if (containerIdx < 0 || containerIdx >= 128)
        {
            continue;
        }

        okami::SpawnTableEntry *entry = &table->entries[containerIdx];

        // Container collected when spawn_type_1 becomes 0
        // State machine: 1 (closed) -> 3 (opened, item floating) -> 0 (collected)
        if (entry->spawn_type_1 == 0)
        {
            int64_t checkId = getContainerCheckId(currentLevelId_, containerIdx);

            wolf::logInfo("[ContainerMan] Container at idx %d collected, sending check %" PRId64, containerIdx, checkId);
            checkCallback_(checkId);

            pickedUp.push_back(containerIdx);
        }
    }

    for (int idx : pickedUp)
    {
        trackedContainerIndices_.erase(idx);
    }
}

bool ContainerMan::isContainerInRando(int64_t locationId) const
{
    (void)locationId; // Individual container filtering not yet implemented

    // Check if connected and config indicates containers are randomized
    if (!socket_.isConnected() || !socket_.isSlotConfigReady())
    {
        return false;
    }

    return socket_.getSlotConfig().randomizeContainers;
}

} // namespace checks
