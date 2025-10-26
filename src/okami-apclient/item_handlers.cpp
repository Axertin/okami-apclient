#include "item_handlers.hpp"

#include <vector>

#include <okami/items.hpp>
#include <wolf_framework.hpp>

#include "aplocationmonitor.h"
#include "gamestate_accessors.hpp"

namespace item_handlers
{

static bool safeToReceive = false;
static std::vector<int> queuedItems;

void initialize()
{
    wolf::onPlayStart([]() { safeToReceive = true; });
    wolf::onReturnToMenu([]() { safeToReceive = false; });

    // block item pickups that are in AP
    // TODO: lookup settings from server to see what items are actually in rando and which are vanilla
    wolf::onItemPickupBlocking([](int itemID, int) -> bool { return apitems::findItem(itemID) != nullptr ? true : false; });
}

void reset()
{
    safeToReceive = false;
    queuedItems.clear();
}

bool receiveAPItem(int ItemID)
{
    queuedItems.push_back(ItemID);
    return true;
}

bool handleAPItems()
{
    if (queuedItems.empty() || !safeToReceive)
    {
        return false;
    }

    bool allSucceeded = true;

    for (int itemID : queuedItems)
    {
        bool success = item_handlers::handleItem(itemID);
        if (!success)
        {
            allSucceeded = false;
        }
    }

    queuedItems.clear();

    return allSucceeded;
}

bool handle_brush(const apitems::APItemDef *item)
{
    wolf::logDebug("[ItemHandler] Granting brush: %s (index %d)", item->name, item->brush.brush_index);

    // Set both usable and obtained flags for the brush
    apgame::usableBrushTechniques->Set(item->brush.brush_index);
    apgame::obtainedBrushTechniques->Set(item->brush.brush_index);

    return true;
}

bool handle_progressive_brush(const apitems::APItemDef *item)
{
    int brushIndex = item->progressive_brush.brush_index;

    // Check if base brush is already obtained
    bool hasBaseBrush = apgame::obtainedBrushTechniques->IsSet(brushIndex);

    if (!hasBaseBrush)
    {
        // First time receiving this brush - grant base version
        wolf::logDebug("[ItemHandler] Granting base progressive brush: %s (index %d)", item->name, brushIndex);
        apgame::usableBrushTechniques->Set(brushIndex);
        apgame::obtainedBrushTechniques->Set(brushIndex);
        return true;
    }

    // Already have base brush, check upgrades
    const unsigned int *upgradeBits = item->progressive_brush.upgrade_bits;
    size_t upgradeCount = item->progressive_brush.upgrade_count;

    // Find the next upgrade to give
    for (size_t i = 0; i < upgradeCount; i++)
    {
        if (!apgame::brushUpgrades->IsSet(upgradeBits[i]))
        {
            // Found the next upgrade to grant
            wolf::logDebug("[ItemHandler] Granting brush upgrade for %s: level %zu (bit %u)", item->name, i + 2, upgradeBits[i]);
            apgame::brushUpgrades->Set(upgradeBits[i]);
            return true;
        }
    }

    // All upgrades already obtained
    wolf::logDebug("[ItemHandler] All upgrades for %s already obtained", item->name);
    return true;
}

bool handle_inventory(const apitems::APItemDef *item)
{
    wolf::logDebug("[ItemHandler] Giving inventory item: %s (ID 0x%X)", item->name, item->inventory.item_id);

    // Use the wolf framework to give the item
    wolf::giveItem(item->inventory.item_id, 1);

    return true;
}

bool handle_event_flags(const apitems::APItemDef *item)
{
    wolf::logDebug("[ItemHandler] Setting event flags for: %s", item->name);

    const apitems::FlagInfo *flags = item->event.flags;
    size_t flagCount = item->event.flag_count;

    for (size_t i = 0; i < flagCount; i++)
    {
        const auto &flag = flags[i];
        wolf::logDebug("[ItemHandler]   Setting flag: %s bit %u", flag.accessor_name, flag.bit);

        // Map accessor name to actual accessor
        // TODO: This is a bit hacky, could use a registration system instead
        if (strcmp(flag.accessor_name, "keyItemsAcquired") == 0)
        {
            apgame::keyItemsAcquired->Set(flag.bit);
        }
        else if (strcmp(flag.accessor_name, "goldDustsAcquired") == 0)
        {
            apgame::goldDustsAcquired->Set(flag.bit);
        }
        else
        {
            wolf::logWarning("[ItemHandler] Unknown accessor: %s", flag.accessor_name);
            return false;
        }
    }

    return true;
}

bool handle_progressive(const apitems::APItemDef *item)
{
    wolf::logInfo("[ItemHandler] Handling progressive item: %s", item->name);

    const int *stages = item->progressive.stages;
    size_t stageCount = item->progressive.stage_count;

    // Check player's inventory to determine current stage
    auto inventory = apgame::collectionData->inventory;

    int currentStage = -1;
    for (size_t i = 0; i < stageCount; i++)
    {
        int weaponId = stages[i];
        if (inventory[weaponId] > 0)
        {
            currentStage = static_cast<int>(i);
        }
    }

    // Give next stage
    int nextStage = currentStage + 1;
    if (nextStage >= static_cast<int>(stageCount))
    {
        wolf::logDebug("[ItemHandler] Progressive %s already at max stage", item->name);
        return true;
    }

    int itemToGive = stages[nextStage];
    wolf::logDebug("[ItemHandler] Granting progressive %s stage %d (item 0x%X)", item->name, nextStage + 1, itemToGive);

    wolf::giveItem(itemToGive, 1);

    return true;
}

bool handleItem(int itemID)
{
    // Look up item definition
    const apitems::APItemDef *item = apitems::findItem(itemID);
    if (!item)
    {
        wolf::logWarning("[ItemHandler] Unknown item ID: 0x%X", itemID);
        return false;
    }

    wolf::logDebug("[ItemHandler] Processing item: %s (0x%X)", item->name, itemID);

    // Temporarily disable location monitor to prevent feedback loops
    APLocationMonitor::instance().enableSending(false);

    bool success = false;

    // Dispatch to appropriate handler
    switch (item->type)
    {
    case apitems::ItemHandlerType::Brush:
        success = handle_brush(item);
        break;

    case apitems::ItemHandlerType::ProgressiveBrush:
        success = handle_progressive_brush(item);
        break;

    case apitems::ItemHandlerType::Inventory:
        success = handle_inventory(item);
        break;

    case apitems::ItemHandlerType::EventFlags:
        success = handle_event_flags(item);
        break;

    case apitems::ItemHandlerType::Progressive:
        success = handle_progressive(item);
        break;

    default:
        wolf::logWarning("[ItemHandler] Unhandled item type for %s", item->name);
        success = false;
        break;
    }

    // Re-enable location monitor
    APLocationMonitor::instance().enableSending(true);

    if (success)
    {
        wolf::logInfo("[ItemHandler] Successfully handled item: %s", item->name);
    }
    else
    {
        wolf::logWarning("[ItemHandler] Failed to handle item: %s", item->name);
    }

    return success;
}

} // namespace item_handlers
