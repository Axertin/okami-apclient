#include "hana_valley.hpp"

#include <okami/brushes.hpp>
#include <wolf_framework.hpp>

#include "common.hpp"

namespace eventfix::hana_valley
{

namespace
{

// FUN_1804d35d0: this is the handler that prevents you from leaving the area.
// Just remove it so you can leave.

void __fastcall stubIssunBypass()
{
    return;
}

// FUN_1804D60E0: function shceduled after using sunrise with the Crystal ball in place
// Hit a deadend with this one. see docs

/*
void __fastcall stubBloomTutorial()
{
    wolf::logDebug("[Eventfix] Bypass bloom tutorial- Bloom Granted");
    eventfix::clearCutsceneModeBits();
    eventfix::setStateBit(0x4001F);
    eventfix::setStateBit(0x40024);
    eventfix::grantBrush(okami::BrushOverlay::bloom);
    return;
}*/

constexpr EventBypass kBypasses[] = {
    {"Hana Valley Guarding Sapling Exit", 0x4D35D0, stubIssunBypass},
    //{"Hana Valley Bloom Tutorial", 0x04D60E0, stubBloomTutorial}
};

} // namespace

std::span<const EventBypass> getBypasses()
{
    return kBypasses;
}

} // namespace eventfix::hana_valley
