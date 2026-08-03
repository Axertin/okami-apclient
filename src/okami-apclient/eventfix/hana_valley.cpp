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
// Note 1: A bit too extreme, it gets triggered every tick in Hana Valley and tries to get you out of every cutscene in the area

void __fastcall stubIssunBypass()
{
    eventfix::clearCutsceneModeBits();
    wolf::logInfo("[eventfix] Let player leave Guardian sapling area");
}


constexpr EventBypass kBypasses[] = {
    {"Hana Valley Guarding Sapling Exit", 0x4D35D0, stubIssunBypass},
};

} // namespace

std::span<const EventBypass> getBypasses()
{
    return kBypasses;
}

} // namespace eventfix::hana_valley
