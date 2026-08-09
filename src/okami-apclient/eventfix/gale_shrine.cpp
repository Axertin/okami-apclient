#include "gale_shrine.hpp"

#include <okami/brushes.hpp>
#include <wolf_framework.hpp>

#include "common.hpp"

namespace eventfix::gale_shrine
{

namespace
{

void __fastcall stubGalestormTutoBypass()
{
    eventfix::clearCutsceneModeBits();
    eventfix::grantBrush(okami::BrushOverlay::galestorm);
    // Set this flag to load the "healed" state on next reload
    eventfix::setStateBit(0x80023);
    // Set these to mark tutorial as completed
    eventfix::setStateBit(0x80026);
    eventfix::setStateBit(0x80033);
    wolf::logInfo("[eventfix] Galestorm tutorial removed; Granted bursh power");
}

constexpr EventBypass kBypasses[] = {
    {"Galestrom tutorail lock", 0x4EC0C0, stubGalestormTutoBypass},
};

} // namespace

std::span<const EventBypass> getBypasses()
{
    return kBypasses;
}

} // namespace eventfix::gale_shrine
