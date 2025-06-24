#include "receive.h"
#ifdef _WIN32
#include "gamehooks.h"
#endif
#include "logger.h"
#include "okami/brushes.hpp"
#include "okami/items.hpp"
#include "okami/memorymap.hpp"

bool receiveAPItem(int ItemID)
{
    if (ItemID > 0xFF)
    {
        if (ItemID < 0x200)
        {
            int index = ItemID - 0x100;
            logDebug("[receive] Receiving brush 0x%X", index);
            return giveBrush(index);
        }
    }
    else
    {
        if (okami::ItemTable.count(ItemID) != 0)
        {
            logDebug("[receive] Receiving %s (0x%X)",
                     okami::ItemTable.at(ItemID).Name, ItemID);
#ifdef _WIN32
            GameHooks::giveItem(ItemID, 1);
#endif
            return true;
        }
    }

    logWarning("[receive] Received unknown item 0x%X", ItemID);
    return false;
}

bool giveBrush(int BrushIndex)
{
    okami::AmmyCollections->world.usableBrushTechniques.Set(BrushIndex);
    okami::AmmyCollections->world.obtainedBrushTechniques.Set(BrushIndex);
    return true;
}
