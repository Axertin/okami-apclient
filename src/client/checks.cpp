#include "checks.h"
#include "okami/okami.hpp"
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
            std::cout << "Check must wait until brushes are bound!" << std::endl;
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
                    std::cout << "Sending Brush Location 0x" << std::hex << (0x100 + static_cast<int>(flag)) << std::dec << std::endl;

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
            std::cout << "Sending Item Location 0x" << std::hex << ItemID << std::dec << std::endl;
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

    std::cout << "[checks] Waiting for title screen to be exited" << std::endl;
    while (okami::ExeriorMapID == 0xC00) // Title Menu
    {
        checksEnabled = false;
    }

    std::cout << "[checks] Exiting main menu, initializing checks" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(15)); // Wait for memory to initialize

    std::cout << "[checks] Delay Finished!" << std::endl;
    checksEnabled = true;
}

void checkInit()
{
    std::thread([]
                {                                 
                std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for game to initialize
                startChecks(); })
        .detach();
}
