#include "checks.h"
#include "okami/okami.hpp"

void checkBrushes(ISocket &socket)
{
    if (!okami::AmmyObtainedBrushTechniques.isBound() || !okami::AmmyUsableBrushTechniques.isBound())
    {
        std::cout << "Check must wait until brushes are bound!" << std::endl;
        return;
    }

    static okami::BitfieldFlagsStorage<okami::BrushOverlay, 4> PrevBrushes;
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
