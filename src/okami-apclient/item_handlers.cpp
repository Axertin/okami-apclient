#include "item_handlers.hpp"

#include <vector>

#include <okami/items.hpp>
#include <wolf_framework.hpp>

#include "apitem.h"
#include "apitems_generated.hpp"

namespace item_handlers
{

static bool safeToReceive = false;
static std::vector<int> queuedItems;

void initialize()
{
    wolf::onPlayStart([]() { safeToReceive = true; });
    wolf::onReturnToMenu([]() { safeToReceive = false; });
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
        if (!handleItem(itemID))
        {
            allSucceeded = false;
        }
    }

    queuedItems.clear();

    return allSucceeded;
}

bool handleItem(int itemID)
{
    auto reward = ap::getReward(itemID);
    if (!reward)
    {
        wolf::logWarning("[ItemHandler] Unknown item ID: 0x%X", itemID);
        return false;
    }

    auto result = ap::grantReward(*reward);
    if (!result)
    {
        wolf::logWarning("[ItemHandler] Failed to grant item 0x%X: %s", itemID, result.error().message.c_str());
        return false;
    }

    return true;
}

} // namespace item_handlers
