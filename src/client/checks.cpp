#include "checks.h"
#include "logger.h"
#include "okami/memorymap.hpp"
#include "okami/items.hpp"
#include "okami/maps.hpp"

static bool checksEnabled = false;

void enableChecks()
{
    checksEnabled = true;
}
void disableChecks()
{
    checksEnabled = false;
}

bool isChecksEnabled()
{
    return checksEnabled;
}

void checkBrushes(ISocket &socket)
{
    static okami::BitfieldFlagsStorage<okami::BrushOverlay, 4> PrevBrushes;
    if (!checksEnabled)
    {
        PrevBrushes.copyFrom(okami::AmmyObtainedBrushTechniques.get());
    }
    else
    {
        if (!okami::AmmyObtainedBrushTechniques.isBound() || !okami::AmmyUsableBrushTechniques.isBound())
        {
            logWarning("[checks] Check must wait until brushes are bound!");
            return;
        }

        auto CurrentBrushes = okami::AmmyObtainedBrushTechniques.get();

        if (!PrevBrushes.isEqual(CurrentBrushes))
        {
            auto diff = PrevBrushes.diffBits(CurrentBrushes);
            for (auto flag : diff)
            {
                if (flag != okami::BrushOverlay::sunrise_default)
                {
                    logDebug("[checks] Sending Brush Location 0x%X", (0x100 + static_cast<int>(flag)));

                    if (socket.isConnected())
                        socket.sendLocation(0x100 + static_cast<int>(flag));

                    // Un-set the usable flag that the game has set
                    okami::AmmyUsableBrushTechniques.clear(flag);
                }
            }
        }

        PrevBrushes.copyFrom(CurrentBrushes);
    }
}

void checkBrushes(int index, ISocket &socket)
{
    if (checksEnabled)
    {
        int BrushLocation = 0x100 + index;
        logDebug("[checks] Sending Brush Location 0x%X", BrushLocation);

        if (socket.isConnected())
            socket.sendLocation(BrushLocation);
    }
}

bool checkItems(int ItemID, ISocket &socket)
{
    if (checksEnabled)
    {
        if (okami::ItemTable.count(ItemID) == 0)
        {
            return false;
        }

        auto Item = okami::ItemTable.at(ItemID);

        switch (Item.Category)
        {
        case okami::ItemCategory::KT:
        case okami::ItemCategory::Treasure:
        case okami::ItemCategory::KeyItem:
        case okami::ItemCategory::Weapon:
        case okami::ItemCategory::Artifact:
        case okami::ItemCategory::Map:
            logInfo("[checks] Sending Item Location 0x%X", ItemID);
            if (socket.isConnected())
            {
                socket.sendLocation(ItemID);
                return true;
            }
            break;

        default:
            break;
        }
    }

    return false;
}

void startChecks()
{

    for (;;)
    {
        if (okami::ExteriorMapID != 0xC00)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
        {
            logInfo("[checks] Waiting for title screen to be exited");
            while (okami::ExteriorMapID == 0xC00) // Title Menu
            {
                checksEnabled = false;
            }

            logInfo("[checks] Exiting main menu, initializing checks");
            std::this_thread::sleep_for(std::chrono::seconds(15)); // Wait for memory to initialize

            logInfo("[checks] Delay Finished!");
            checksEnabled = true;
        }
    }
}

void checkInit()
{
    checksEnabled = false;
    std::thread([]
                {                                 
                std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for game to initialize
                startChecks(); })
        .detach();
}
