// Item sell prices pulled directly from game data

#include "okami/shopdata.h"

#include <array>

namespace okami
{

static_assert(sizeof(ISLHeader) == 16);
static_assert(sizeof(ItemShopStock) == 12);

SellValueArray DefaultItemSellPrices = {
    -1,    -1, -1, -1,
    1250, // HolyBoneL
    -1,    -1, -1,
    5000, // ExorcismSlipL
    3500, // ExorcismSlipM
    2000, // ExorcismSlipS
    -1,
    5000, // VengeanceSlip
    2500, // InkfinityStone
    500,  // MermaidCoin
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    750, // HolyBoneM
    250, // HolyBoneS
    250, // FeedbagMeat
    250, // FeedbagHerbs
    250, // FeedbagSeeds
    250, // FeedbagFish
    -1,    -1, -1, -1, -1,
    1000, // SteelFistSake
    500,  // SteelSoulSake
    -1,    -1, -1, -1, -1,
    15000, // WhitePorcelainPot
    12000, // KutaniPottery
    -1,
    2000,  // IncenseBurner
    1900,  // Vase
    30000, // SilverPocketWatch
    3300,  // RatStatue
    3000,  // BullHorn
    -1,
    20000, // EtchedGlass
    2500,  // LacquerwareSet
    1500,  // WoodenBear
    -1,
    1000, // GlassBeads
    1100, // DragonflyBead
    -1,
    1700,  // CoralFragment
    5000,  // Crystal
    5500,  // Pearl
    9000,  // RubyTassels
    3400,  // BullStatue
    3500,  // TigerStatue
    3600,  // RabbitStatue
    3700,  // DragonStatue
    3800,  // SnakeStatue
    3900,  // HorseStatue
    4000,  // SheepStatue
    4100,  // MonkeyStatue
    4200,  // RoosterStatue
    4300,  // DogStatue
    4400,  // BoarStatue
    4500,  // CatStatue
    9500,  // SapphireTassels
    10000, // EmeraldTassels
    8500,  // TurqoiseTassels
    8000,  // AgateTassels
    6500,  // AmberTassels
    7500,  // CatsEyeTassels
    6000,  // AmethystTassels
    7000,  // JadeTassels
    -1,    -1, -1, -1, -1, -1,
    4900, // GiantSalmon
    -1,
    14000, // SupremeTuna
    3000,  // MountainTrout
    1800,  // RedSnapper
    900,   // StripedSnapper
    1950,  // Salmon
    3600,  // Koi
    2900,  // Huchen
    3700,  // Robalo
    1600,  // BlackBass
    350,   // Clownfish
    4000,  // Bonito
    7500,  // Yellowtail
    950,   // Sweetfish
    1000,  // Trout
    300,   // Smelt
    200,   // Killifish
    1500,  // FlyingFish
    6700,  // Sturgeon
    6900,  // Sunfish
    1700,  // FreshwaterEel
    800,   // Loach
    3800,  // Moray
    15000, // Oarfish
    8000,  // Monkfish
    2000,  // Catfish
    6800,  // GiantCatfish
    500,   // Goby
    1900,  // Lobster
    450,   // Crawfish
    550,   // Scallop
    2100,  // Nautilus
    7000,  // Manta
    1100,  // Blowfish
    100,   // RiverCrab
    400,   // Starfish
    12000, // Marlin
    3900,  // LoggerheadTurtle
    600,   // SeaHorse
    3000,  // Octopus
    2500,  // Squid
    -1,    -1, -1, -1, -1, -1,
    5000,  // Whopper
    10000, // CutlassFish
};

SellValueArray DefaultTakaPassItemSellPrices = {
    -1,    -1, -1, -1,
    1250, // HolyBoneL
    -1,    -1, -1,
    5000, // ExorcismSlipL
    3500, // ExorcismSlipM
    2000, // ExorcismSlipS
    -1,
    5000, // VengeanceSlip
    2500, // InkfinityStone
    500,  // MermaidCoin
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    750, // HolyBoneM
    250, // HolyBoneS
    250, // FeedbagMeat
    250, // FeedbagHerbs
    250, // FeedbagSeeds
    250, // FeedbagFish
    -1,    -1, -1, -1, -1,
    1000, // SteelFistSake
    500,  // SteelSoulSake
    -1,    -1, -1, -1, -1,
    15000, // WhitePorcelainPot
    12000, // KutaniPottery
    -1,
    2000,  // IncenseBurner
    1900,  // Vase
    30000, // SilverPocketWatch
    3300,  // RatStatue
    3000,  // BullHorn
    -1,
    20000, // EtchedGlass
    2500,  // LacquerwareSet
    1500,  // WoodenBear
    -1,
    1000, // GlassBeads
    1100, // DragonflyBead
    -1,
    1700,  // CoralFragment
    5000,  // Crystal
    5500,  // Pearl
    9000,  // RubyTassels
    3400,  // BullStatue
    3500,  // TigerStatue
    3600,  // RabbitStatue
    3700,  // DragonStatue
    3800,  // SnakeStatue
    3900,  // HorseStatue
    4000,  // SheepStatue
    4100,  // MonkeyStatue
    4200,  // RoosterStatue
    4300,  // DogStatue
    4400,  // BoarStatue
    4500,  // CatStatue
    9500,  // SapphireTassels
    10000, // EmeraldTassels
    8500,  // TurqoiseTassels
    8000,  // AgateTassels
    6500,  // AmberTassels
    7500,  // CatsEyeTassels
    6000,  // AmethystTassels
    7000,  // JadeTassels
    -1,    -1, -1, -1, -1, -1,
    7400, // GiantSalmon (changed)
    -1,
    14000, // SupremeTuna
    4500,  // MountainTrout (changed)
    1800,  // RedSnapper
    900,   // StripedSnapper
    2900,  // Salmon (changed)
    3600,  // Koi
    4400,  // Huchen (changed)
    3700,  // Robalo
    1600,  // BlackBass
    350,   // Clownfish
    4000,  // Bonito
    7500,  // Yellowtail
    1400,  // Sweetfish (changed)
    1500,  // Trout (changed)
    300,   // Smelt
    300,   // Killifish (changed)
    1500,  // FlyingFish
    6700,  // Sturgeon
    6900,  // Sunfish
    1700,  // FreshwaterEel
    800,   // Loach
    3800,  // Moray
    15000, // Oarfish
    8000,  // Monkfish
    3000,  // Catfish (changed)
    6800,  // GiantCatfish
    750,   // Goby (changed)
    1900,  // Lobster
    700,   // Crawfish (changed)
    550,   // Scallop
    2100,  // Nautilus
    7000,  // Manta
    1100,  // Blowfish
    150,   // RiverCrab (changed)
    400,   // Starfish
    12000, // Marlin
    3900,  // LoggerheadTurtle
    600,   // SeaHorse
    3000,  // Octopus
    2500,  // Squid
    -1,    -1, -1, -1, -1, -1,
    7500,  // Whopper (changed)
    10000, // CutlassFish
};

SellValueArray DefaultSeianFishShopItemSellPrices = {
    -1,    -1, -1, -1,
    1250, // HolyBoneL
    -1,    -1, -1,
    5000, // ExorcismSlipL
    3500, // ExorcismSlipM
    2000, // ExorcismSlipS
    -1,
    5000, // VengeanceSlip
    2500, // InkfinityStone
    500,  // MermaidCoin
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    750, // HolyBoneM
    250, // HolyBoneS
    250, // FeedbagMeat
    250, // FeedbagHerbs
    250, // FeedbagSeeds
    250, // FeedbagFish
    -1,    -1, -1, -1, -1,
    1000, // SteelFistSake
    500,  // SteelSoulSake
    -1,    -1, -1, -1, -1,
    15000, // WhitePorcelainPot
    12000, // KutaniPottery
    -1,
    2000,  // IncenseBurner
    1900,  // Vase
    30000, // SilverPocketWatch
    3300,  // RatStatue
    3000,  // BullHorn
    -1,
    20000, // EtchedGlass
    2500,  // LacquerwareSet
    1500,  // WoodenBear
    -1,
    1000, // GlassBeads
    1100, // DragonflyBead
    -1,
    1700,  // CoralFragment
    5000,  // Crystal
    5500,  // Pearl
    9000,  // RubyTassels
    3400,  // BullStatue
    3500,  // TigerStatue
    3600,  // RabbitStatue
    3700,  // DragonStatue
    3800,  // SnakeStatue
    3900,  // HorseStatue
    4000,  // SheepStatue
    4100,  // MonkeyStatue
    4200,  // RoosterStatue
    4300,  // DogStatue
    4400,  // BoarStatue
    4500,  // CatStatue
    9500,  // SapphireTassels
    10000, // EmeraldTassels
    8500,  // TurqoiseTassels
    8000,  // AgateTassels
    6500,  // AmberTassels
    7500,  // CatsEyeTassels
    6000,  // AmethystTassels
    7000,  // JadeTassels
    -1,    -1, -1, -1, -1, -1,
    7400, // GiantSalmon (changed)
    -1,
    14000, // SupremeTuna
    4500,  // MountainTrout (changed)
    2700,  // RedSnapper (changed)
    1400,  // StripedSnapper (changed)
    2900,  // Salmon (changed)
    5400,  // Koi (changed)
    4400,  // Huchen (changed)
    5600,  // Robalo (changed)
    2400,  // BlackBass (changed)
    550,   // Clownfish (changed)
    6000,  // Bonito (changed)
    7500,  // Yellowtail
    1400,  // Sweetfish (changed)
    1500,  // Trout (changed)
    450,   // Smelt (changed)
    300,   // Killifish (changed)
    1500,  // FlyingFish
    9700,  // Sturgeon (changed)
    9900,  // Sunfish (changed)
    2600,  // FreshwaterEel (changed)
    1200,  // Loach (changed)
    5700,  // Moray (changed)
    15000, // Oarfish
    8000,  // Monkfish
    3000,  // Catfish (changed)
    9800,  // GiantCatfish (changed)
    750,   // Goby (changed)
    2900,  // Lobster (changed)
    700,   // Crawfish (changed)
    550,   // Scallop
    3200,  // Nautilus (changed)
    10000, // Manta (changed)
    1700,  // Blowfish (changed)
    150,   // RiverCrab (changed)
    600,   // Starfish (changed)
    18000, // Marlin (changed)
    5900,  // LoggerheadTurtle (changed)
    600,   // SeaHorse
    3000,  // Octopus
    2500,  // Squid
    -1,    -1, -1, -1, -1, -1,
    7500,  // Whopper (changed)
    15000, // CutlassFish (changed)
};

} // namespace okami
