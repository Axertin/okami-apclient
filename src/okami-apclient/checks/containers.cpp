#include "containers.hpp"

#include <cinttypes>
#include <list>

#include <okami/spawntable.h>

#include <okami/itemtype.hpp>
#include <wolf_framework.hpp>

#include "../itempatch.hpp"
#include "../rewards/game_items.hpp"
#include "../rewards/reward_types.hpp"
#include "check_types.hpp"

namespace checks
{

constexpr uint8_t DUMMY_ITEM_ID = 0x83; // Chestnut

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
    scoutedItems_.clear();
    pendingContainerItems_.clear();
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
    scoutedItems_.clear();
    pendingContainerItems_.clear();

    // First pass: scan spawn table entries for containers, track indices (don't replace yet)
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

        trackedContainerIndices_.insert(i);
    }

    // Scout all tracked container locations to get item classification data
    scoutContainerLocations();

    // Helper to select a native Okami dummy item based on AP classification flags
    auto nativeDummy = [](unsigned flags)
    {
        if (rewards::isTrap(flags))
            return okami::ItemTypes::OkamiTrapItem;
        if (rewards::isProgression(flags))
            return okami::ItemTypes::OkamiProgressionItem;
        return okami::ItemTypes::OkamiStandardItem;
    };

    // Second pass: replace items using scouted data
    int replacedCount = 0;
    for (int i : trackedContainerIndices_)
    {
        okami::SpawnTableEntry *entry = &table->entries[i];
        okami::ContainerData *containerData = entry->spawn_data;

        int64_t checkId = getContainerCheckId(currentLevelId_, i);
        auto it = scoutedItems_.find(checkId);

        if (it == scoutedItems_.end())
        {
            // No scouted data — fallback to chestnut
            containerData->item_id = DUMMY_ITEM_ID;
            pendingContainerItems_[containerData->item_id]++;
            wolf::logDebug("[ContainerMan] Container %d: no scout data, using fallback dummy 0x%02X", i, DUMMY_ITEM_ID);
        }
        else
        {
            const auto &scouted = it->second;
            okami::ItemTypes::Enum gameItem;

            const int mySlot = socket_.getPlayerSlot();
            const bool isNative = !rewards::isForeignItem(scouted.player, mySlot);

            if (isNative && rewards::game_items::isDirectGameItem(scouted.item))
            {
                // Native direct game item — use actual game item ID for vanilla 3D model
                gameItem = static_cast<okami::ItemTypes::Enum>(rewards::game_items::getItemId(scouted.item));
                wolf::logDebug("[ContainerMan] Container %d: native AP item %" PRId64 " -> game item %d", i, scouted.item, static_cast<int>(gameItem));
            }
            else if (isNative && rewards::game_items::isProgressiveWeapon(scouted.item))
            {
                // Progressive weapons need dummy
                gameItem = nativeDummy(scouted.flags);
                itempatch::registerScoutedItemName(checkId, socket_.getItemName(scouted.item, socket_.getPlayerSlot()));
                wolf::logDebug("[ContainerMan] Container %d: native AP item %" PRId64 " -> progressive weapon, using dummy %d", i, scouted.item,
                               static_cast<int>(gameItem));
            }
            else if (isNative)
            {
                // Native brushes, event flags, etc.
                gameItem = nativeDummy(scouted.flags);
                itempatch::registerScoutedItemName(checkId, socket_.getItemName(scouted.item, socket_.getPlayerSlot()));
                wolf::logDebug("[ContainerMan] Container %d: native AP item %" PRId64 " -> non-game item, using dummy %d (flags=0x%x)", i, scouted.item,
                               static_cast<int>(gameItem), scouted.flags);
            }
            else
            {
                // Foreign item — select AP dummy type based on classification flags
                if (rewards::isTrap(scouted.flags))
                    gameItem = okami::ItemTypes::ForeignTrapItem;
                else if (rewards::isProgression(scouted.flags))
                    gameItem = okami::ItemTypes::ForeignProgressionItem;
                else
                    gameItem = okami::ItemTypes::ForeignStandardItem;
                itempatch::registerScoutedItemName(checkId, socket_.getItemName(scouted.item, scouted.player));
                wolf::logDebug("[ContainerMan] Container %d: foreign AP item %" PRId64 " -> dummy type %d (flags=0x%x)", i, scouted.item,
                               static_cast<int>(gameItem), scouted.flags);
            }

            containerData->item_id = static_cast<uint8_t>(gameItem);
        }

        pendingContainerItems_[containerData->item_id]++;
        replacedCount++;
    }

    if (replacedCount > 0)
    {
        wolf::logInfo("[ContainerMan] Randomized %d containers in level 0x%04X", replacedCount, currentLevelId_);
    }
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
            itempatch::setContainerContext(checkId);
            checkCallback_(checkId);
            itempatch::clearContainerContext();
            pickedUp.push_back(containerIdx);
        }
    }

    for (int idx : pickedUp)
    {
        trackedContainerIndices_.erase(idx);
    }
}

void ContainerMan::scoutContainerLocations()
{
    std::list<int64_t> locationsToScout;

    for (int idx : trackedContainerIndices_)
    {
        locationsToScout.push_back(getContainerCheckId(currentLevelId_, idx));
    }

    if (locationsToScout.empty() || !socket_.isConnected())
    {
        return;
    }

    // Scout all locations synchronously
    auto scoutedItems = socket_.scoutLocationsSync(locationsToScout, 0);

    if (scoutedItems.empty())
    {
        return;
    }

    // Store results in cache
    for (const auto &item : scoutedItems)
    {
        scoutedItems_[item.location] = item;
    }
}

bool ContainerMan::isContainerInRando(int64_t locationId) const
{
    // Check if connected and config indicates containers are randomized
    if (!socket_.isConnected() || !socket_.isSlotConfigReady())
    {
        return false;
    }

    if (!socket_.getSlotConfig().randomizeContainers)
    {
        return false;
    }

    // Only include containers that exist in the APWorld
    return socket_.isValidLocation(locationId);
}

bool ContainerMan::shouldBlockItemPickup(int itemId)
{
    auto it = pendingContainerItems_.find(static_cast<uint8_t>(itemId));
    if (it != pendingContainerItems_.end() && it->second > 0)
    {
        it->second--;
        if (it->second <= 0)
            pendingContainerItems_.erase(it);
        wolf::logDebug("[ContainerMan] Blocked item pickup for item 0x%02X from randomized container", itemId);
        return true;
    }
    return false;
}

} // namespace checks
