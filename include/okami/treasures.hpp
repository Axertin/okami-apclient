#pragma once

namespace okami
{
namespace Treasures
{
/**
 * @brief Enum representing all treasures.
 */
enum Enum
{
    GlassBead,
    DragonflyBead,
    WoodenBear,
    CoralFragment,
    Vase,
    IncenseBurner,
    LacquerwareSet,
    BullHorn,
    RatStatue,
    BullStatue,
    TigerStatue,
    RabbitStatue,
    DragonStatue,
    SnakeStatue,
    HorseStatue,
    SheepStatue,
    MonkeyStatue,
    RoosterStatue,
    DogStatue,
    BoarStatue,
    CatStatue,
    Crystal,
    Pearl,
    AmethystTassels,
    AmberTassels,
    JadeTassels,
    CatsEyeTassels,
    AgateTassels,
    TurquoiseTassels,
    RubyTassels,
    SapphireTassels,
    EmeraldTassels,
    KutaniPottery,
    WhitePorcelainPot,
    EtchedGlass,
    SilverPocketWatch,
    NUM_TREASURES
};
const char *GetName(unsigned index);
} // namespace Treasures
} // namespace okami
