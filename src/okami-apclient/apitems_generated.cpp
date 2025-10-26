#include "apitems_generated.hpp"

namespace apitems
{

static const FlagInfo flags_Save_Rei[] = {
    {"keyItemsAcquired", 0},
};

static const FlagInfo flags_Save_Shin[] = {
    {"keyItemsAcquired", 1},
};

static const FlagInfo flags_Save_Chi[] = {
    {"keyItemsAcquired", 2},
};

static const FlagInfo flags_Save_Ko[] = {
    {"keyItemsAcquired", 3},
};

static const FlagInfo flags_Save_Tei[] = {
    {"keyItemsAcquired", 4},
};

static const FlagInfo flags_Serpent_Crystal[] = {
    {"keyItemsAcquired", 5},
};

static const unsigned int upgrades_Power_Slash[] = {0, 10};
static const unsigned int upgrades_Cherry_Bomb[] = {6, 11};

static const int stages_Progressive_Mirror[] = {0x13, 0x14};
static const int stages_Progressive_Rosary[] = {0x15, 0x16, 0x17, 0x18, 0x19};
static const int stages_Progressive_Sword[] = {0x1A, 0x1B, 0x1C, 0x1D, 0x1E};

// Item definitions
static const APItemDef item_defs[] = {
    // Sunrise
    {
        .id = 0x100,
        .name = "Sunrise",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 0},
    },
    // Rejuvenation
    {
        .id = 0x101,
        .name = "Rejuvenation",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 1},
    },
    // Power Slash
    {
        .id = 0x102,
        .name = "Power Slash",
        .type = ItemHandlerType::ProgressiveBrush,
        .progressive_brush = {.brush_index = 2, .upgrade_bits = upgrades_Power_Slash, .upgrade_count = 2},
    },
    // Cherry Bomb
    {
        .id = 0x103,
        .name = "Cherry Bomb",
        .type = ItemHandlerType::ProgressiveBrush,
        .progressive_brush = {.brush_index = 3, .upgrade_bits = upgrades_Cherry_Bomb, .upgrade_count = 2},
    },
    // Greensprout (Bloom)
    {
        .id = 0x104,
        .name = "Greensprout (Bloom)",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 4},
    },
    // Greensprout (Waterlily)
    {
        .id = 0x105,
        .name = "Greensprout (Waterlily)",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 5},
    },
    // Greensprout (Vine)
    {
        .id = 0x106,
        .name = "Greensprout (Vine)",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 6},
    },
    // Watersprout
    {
        .id = 0x107,
        .name = "Watersprout",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 7},
    },
    // Crescent
    {
        .id = 0x108,
        .name = "Crescent",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 8},
    },
    // Galestorm
    {
        .id = 0x109,
        .name = "Galestorm",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 9},
    },
    // Inferno
    {
        .id = 0x10A,
        .name = "Inferno",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 10},
    },
    // Veil of Mist
    {
        .id = 0x10B,
        .name = "Veil of Mist",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 11},
    },
    // Catwalk
    {
        .id = 0x10C,
        .name = "Catwalk",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 12},
    },
    // Thunderstorm
    {
        .id = 0x10D,
        .name = "Thunderstorm",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 13},
    },
    // Blizzard
    {
        .id = 0x10E,
        .name = "Blizzard",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 14},
    },
    // Mist Warp
    {
        .id = 0x10F,
        .name = "Mist Warp",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 15},
    },
    // Fireburst
    {
        .id = 0x110,
        .name = "Fireburst",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 16},
    },
    // Whirlwind
    {
        .id = 0x111,
        .name = "Whirlwind",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 17},
    },
    // Deluge
    {
        .id = 0x112,
        .name = "Deluge",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 18},
    },
    // Fountain
    {
        .id = 0x113,
        .name = "Fountain",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 19},
    },
    // Thunderbolt
    {
        .id = 0x114,
        .name = "Thunderbolt",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 20},
    },
    // Icestorm
    {
        .id = 0x115,
        .name = "Icestorm",
        .type = ItemHandlerType::Brush,
        .brush = {.brush_index = 21},
    },
    // Peace Bell
    {
        .id = 0xB,
        .name = "Peace Bell",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB},
    },
    // Golden Lucky Cat
    {
        .id = 0x95,
        .name = "Golden Lucky Cat",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x95},
    },
    // Thief's Glove
    {
        .id = 0x96,
        .name = "Thief's Glove",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x96},
    },
    // Wood Mat
    {
        .id = 0x97,
        .name = "Wood Mat",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x97},
    },
    // Golden Ink Pot
    {
        .id = 0x98,
        .name = "Golden Ink Pot",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x98},
    },
    // Fire Tablet
    {
        .id = 0x9D,
        .name = "Fire Tablet",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x9D},
    },
    // Canine Tracker
    {
        .id = 0x42,
        .name = "Canine Tracker",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x42},
    },
    // Lucky Mallet
    {
        .id = 0x43,
        .name = "Lucky Mallet",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x43},
    },
    // Border Key
    {
        .id = 0x44,
        .name = "Border Key",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x44},
    },
    // Dragon Orb
    {
        .id = 0x45,
        .name = "Dragon Orb",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x45},
    },
    // Fox Rods
    {
        .id = 0x46,
        .name = "Fox Rods",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x46},
    },
    // Thunder Brew
    {
        .id = 0x47,
        .name = "Thunder Brew",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x47},
    },
    // Shell Amulet
    {
        .id = 0x48,
        .name = "Shell Amulet",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x48},
    },
    // Mask
    {
        .id = 0x49,
        .name = "Mask",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x49},
    },
    // Ogre Liver
    {
        .id = 0x4A,
        .name = "Ogre Liver",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x4A},
    },
    // Lips of Ice
    {
        .id = 0x4B,
        .name = "Lips of Ice",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x4B},
    },
    // Eyeball of Fire
    {
        .id = 0x4C,
        .name = "Eyeball of Fire",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x4C},
    },
    // Black Demon Horn
    {
        .id = 0x4D,
        .name = "Black Demon Horn",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x4D},
    },
    // Golden Mushroom
    {
        .id = 0x5F,
        .name = "Golden Mushroom",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x5F},
    },
    // Gimmick Gear
    {
        .id = 0x60,
        .name = "Gimmick Gear",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x60},
    },
    // 8 Purification Sake
    {
        .id = 0x62,
        .name = "8 Purification Sake",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x62},
    },
    // Sewaprolo
    {
        .id = 0x63,
        .name = "Sewaprolo",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x63},
    },
    // Charcoal
    {
        .id = 0x71,
        .name = "Charcoal",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x71},
    },
    // Blinding Snow
    {
        .id = 0x72,
        .name = "Blinding Snow",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x72},
    },
    // Treasure Box
    {
        .id = 0x73,
        .name = "Treasure Box",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x73},
    },
    // Herbal Medicine
    {
        .id = 0x75,
        .name = "Herbal Medicine",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x75},
    },
    // Pinwheel
    {
        .id = 0x76,
        .name = "Pinwheel",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x76},
    },
    // Marlin Rod
    {
        .id = 0x77,
        .name = "Marlin Rod",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x77},
    },
    // Vista of the Gods
    {
        .id = 0x5C,
        .name = "Vista of the Gods",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x5C},
    },
    // Tsuta Ruins Key
    {
        .id = 0x40,
        .name = "Tsuta Ruins Key",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x40},
    },
    // Sun Fragment
    {
        .id = 0x5,
        .name = "Sun Fragment",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x5},
    },
    // Astral Pouch
    {
        .id = 0x6,
        .name = "Astral Pouch",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x6},
    },
    // Stray Bead
    {
        .id = 0xCC,
        .name = "Stray Bead",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xCC},
    },
    // Demon Fang
    {
        .id = 0x1F,
        .name = "Demon Fang",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x1F},
    },
    // Mermaid Coin
    {
        .id = 0xE,
        .name = "Mermaid Coin",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xE},
    },
    // Golden Peach
    {
        .id = 0xF,
        .name = "Golden Peach",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xF},
    },
    // Gold Dust
    {
        .id = 0x9E,
        .name = "Gold Dust",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x9E},
    },
    // Holy Bone L
    {
        .id = 0x4,
        .name = "Holy Bone L",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x4},
    },
    // Exorcism Slip L
    {
        .id = 0x8,
        .name = "Exorcism Slip L",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x8},
    },
    // Exorcism Slip M
    {
        .id = 0x9,
        .name = "Exorcism Slip M",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x9},
    },
    // Exorcism Slip S
    {
        .id = 0xA,
        .name = "Exorcism Slip S",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA},
    },
    // Vengeance Slip
    {
        .id = 0xC,
        .name = "Vengeance Slip",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC},
    },
    // Inkfinity Stone
    {
        .id = 0xD,
        .name = "Inkfinity Stone",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xD},
    },
    // Traveler's Charm
    {
        .id = 0x70,
        .name = "Traveler's Charm",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x70},
    },
    // Holy Bone M
    {
        .id = 0x8E,
        .name = "Holy Bone M",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x8E},
    },
    // Holy Bone S
    {
        .id = 0x8F,
        .name = "Holy Bone S",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x8F},
    },
    // Feedbag (Meat)
    {
        .id = 0x90,
        .name = "Feedbag (Meat)",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x90},
    },
    // Feedbag (Herbs)
    {
        .id = 0x91,
        .name = "Feedbag (Herbs)",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x91},
    },
    // Feedbag (Seeds)
    {
        .id = 0x92,
        .name = "Feedbag (Seeds)",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x92},
    },
    // Feedbag (Fish)
    {
        .id = 0x93,
        .name = "Feedbag (Fish)",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x93},
    },
    // Steel Fist Sake
    {
        .id = 0x99,
        .name = "Steel Fist Sake",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x99},
    },
    // Steel Soul Sake
    {
        .id = 0x9A,
        .name = "Steel Soul Sake",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x9A},
    },
    // Godly Charm
    {
        .id = 0x9B,
        .name = "Godly Charm",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x9B},
    },
    // White porcelain pot
    {
        .id = 0xA0,
        .name = "White porcelain pot",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA0},
    },
    // Kutani Pottery
    {
        .id = 0xA1,
        .name = "Kutani Pottery",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA1},
    },
    // Incense Burner
    {
        .id = 0xA3,
        .name = "Incense Burner",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA3},
    },
    // Vase
    {
        .id = 0xA4,
        .name = "Vase",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA4},
    },
    // Silver Pocket Watch
    {
        .id = 0xA5,
        .name = "Silver Pocket Watch",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA5},
    },
    // Rat Statue
    {
        .id = 0xA6,
        .name = "Rat Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA6},
    },
    // Bull Horn
    {
        .id = 0xA7,
        .name = "Bull Horn",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA7},
    },
    // Etched Glass
    {
        .id = 0xA9,
        .name = "Etched Glass",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xA9},
    },
    // Lacquerware Set
    {
        .id = 0xAA,
        .name = "Lacquerware Set",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xAA},
    },
    // Wooden Bear
    {
        .id = 0xAB,
        .name = "Wooden Bear",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xAB},
    },
    // Glass Beads
    {
        .id = 0xAD,
        .name = "Glass Beads",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xAD},
    },
    // Dragonfly Bead
    {
        .id = 0xAE,
        .name = "Dragonfly Bead",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xAE},
    },
    // Coral Fragment
    {
        .id = 0xB0,
        .name = "Coral Fragment",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB0},
    },
    // Crystal
    {
        .id = 0xB1,
        .name = "Crystal",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB1},
    },
    // Pearl
    {
        .id = 0xB2,
        .name = "Pearl",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB2},
    },
    // Ruby Tassels
    {
        .id = 0xB3,
        .name = "Ruby Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB3},
    },
    // Bull Statue
    {
        .id = 0xB4,
        .name = "Bull Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB4},
    },
    // Tiger Statue
    {
        .id = 0xB5,
        .name = "Tiger Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB5},
    },
    // Rabbit Statue
    {
        .id = 0xB6,
        .name = "Rabbit Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB6},
    },
    // Dragon Statue
    {
        .id = 0xB7,
        .name = "Dragon Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB7},
    },
    // Snake Statue
    {
        .id = 0xB8,
        .name = "Snake Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB8},
    },
    // Horse Statue
    {
        .id = 0xB9,
        .name = "Horse Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xB9},
    },
    // Sheep Statue
    {
        .id = 0xBA,
        .name = "Sheep Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xBA},
    },
    // Monkey Statue
    {
        .id = 0xBB,
        .name = "Monkey Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xBB},
    },
    // Rooster Statue
    {
        .id = 0xBC,
        .name = "Rooster Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xBC},
    },
    // Dog Statue
    {
        .id = 0xBD,
        .name = "Dog Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xBD},
    },
    // Boar Statue
    {
        .id = 0xBE,
        .name = "Boar Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xBE},
    },
    // Cat Statue
    {
        .id = 0xBF,
        .name = "Cat Statue",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xBF},
    },
    // Sapphire Tassels
    {
        .id = 0xC0,
        .name = "Sapphire Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC0},
    },
    // Emerald Tassels
    {
        .id = 0xC1,
        .name = "Emerald Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC1},
    },
    // Turquoise Tassels
    {
        .id = 0xC2,
        .name = "Turquoise Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC2},
    },
    // Agate Tassels
    {
        .id = 0xC3,
        .name = "Agate Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC3},
    },
    // Amber Tassels
    {
        .id = 0xC4,
        .name = "Amber Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC4},
    },
    // Cat's Eye Tassels
    {
        .id = 0xC5,
        .name = "Cat's Eye Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC5},
    },
    // Amethyst Tassels
    {
        .id = 0xC6,
        .name = "Amethyst Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC6},
    },
    // Jade Tassels
    {
        .id = 0xC7,
        .name = "Jade Tassels",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC7},
    },
    // Save Rei
    {
        .id = 0x303,
        .name = "Save Rei",
        .type = ItemHandlerType::EventFlags,
        .event = {.flags = flags_Save_Rei, .flag_count = 1},
    },
    // Save Shin
    {
        .id = 0x304,
        .name = "Save Shin",
        .type = ItemHandlerType::EventFlags,
        .event = {.flags = flags_Save_Shin, .flag_count = 1},
    },
    // Save Chi
    {
        .id = 0x305,
        .name = "Save Chi",
        .type = ItemHandlerType::EventFlags,
        .event = {.flags = flags_Save_Chi, .flag_count = 1},
    },
    // Save Ko
    {
        .id = 0x306,
        .name = "Save Ko",
        .type = ItemHandlerType::EventFlags,
        .event = {.flags = flags_Save_Ko, .flag_count = 1},
    },
    // Save Tei
    {
        .id = 0x307,
        .name = "Save Tei",
        .type = ItemHandlerType::EventFlags,
        .event = {.flags = flags_Save_Tei, .flag_count = 1},
    },
    // Loyalty Orb
    {
        .id = 0x4E,
        .name = "Loyalty Orb",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x4E},
    },
    // Justice Orb
    {
        .id = 0x4F,
        .name = "Justice Orb",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x4F},
    },
    // Duty Orb
    {
        .id = 0x50,
        .name = "Duty Orb",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x50},
    },
    // Serpent Crystal
    {
        .id = 0x308,
        .name = "Serpent Crystal",
        .type = ItemHandlerType::EventFlags,
        .event = {.flags = flags_Serpent_Crystal, .flag_count = 1},
    },
    // Divine Retribution
    {
        .id = 0x10,
        .name = "Divine Retribution",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x10},
    },
    // Snarling Beast
    {
        .id = 0x11,
        .name = "Snarling Beast",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x11},
    },
    // Infinity Judge
    {
        .id = 0x12,
        .name = "Infinity Judge",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x12},
    },
    // Trinity Mirror
    {
        .id = 0x13,
        .name = "Trinity Mirror",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x13},
    },
    // Solar Flare
    {
        .id = 0x14,
        .name = "Solar Flare",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x14},
    },
    // Devout Beads
    {
        .id = 0x15,
        .name = "Devout Beads",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x15},
    },
    // Life Beads
    {
        .id = 0x16,
        .name = "Life Beads",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x16},
    },
    // Exorcism Beads
    {
        .id = 0x17,
        .name = "Exorcism Beads",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x17},
    },
    // Resurrection Beads
    {
        .id = 0x18,
        .name = "Resurrection Beads",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x18},
    },
    // Tundra Beads
    {
        .id = 0x19,
        .name = "Tundra Beads",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x19},
    },
    // Tsumugari
    {
        .id = 0x1A,
        .name = "Tsumugari",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x1A},
    },
    // Seven Strike
    {
        .id = 0x1B,
        .name = "Seven Strike",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x1B},
    },
    // Blade of Kusanagi
    {
        .id = 0x1C,
        .name = "Blade of Kusanagi",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x1C},
    },
    // Eight Wonder
    {
        .id = 0x1D,
        .name = "Eight Wonder",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x1D},
    },
    // Thunder Edge
    {
        .id = 0x1E,
        .name = "Thunder Edge",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x1E},
    },
    // Progressive Mirror
    {
        .id = 0x300,
        .name = "Progressive Mirror",
        .type = ItemHandlerType::Progressive,
        .progressive = {.stages = stages_Progressive_Mirror, .stage_count = 2},
    },
    // Progressive Rosary
    {
        .id = 0x301,
        .name = "Progressive Rosary",
        .type = ItemHandlerType::Progressive,
        .progressive = {.stages = stages_Progressive_Rosary, .stage_count = 5},
    },
    // Progressive Sword
    {
        .id = 0x302,
        .name = "Progressive Sword",
        .type = ItemHandlerType::Progressive,
        .progressive = {.stages = stages_Progressive_Sword, .stage_count = 5},
    },
    // Karmic Returner
    {
        .id = 0xC8,
        .name = "Karmic Returner",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC8},
    },
    // Karmic Transformer 1
    {
        .id = 0x5B,
        .name = "Karmic Transformer 1",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x5B},
    },
    // Karmic Transformer 2
    {
        .id = 0xC9,
        .name = "Karmic Transformer 2",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xC9},
    },
    // Karmic Transformer 3
    {
        .id = 0x79,
        .name = "Karmic Transformer 3",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x79},
    },
    // Karmic Transformer 4
    {
        .id = 0xCF,
        .name = "Karmic Transformer 4",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xCF},
    },
    // Karmic Transformer 5
    {
        .id = 0xCB,
        .name = "Karmic Transformer 5",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xCB},
    },
    // Karmic Transformer 6
    {
        .id = 0xCA,
        .name = "Karmic Transformer 6",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0xCA},
    },
    // Karmic Transformer 7
    {
        .id = 0x7B,
        .name = "Karmic Transformer 7",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x7B},
    },
    // Karmic Transformer 8
    {
        .id = 0x7A,
        .name = "Karmic Transformer 8",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x7A},
    },
    // Karmic Transformer 9
    {
        .id = 0x7C,
        .name = "Karmic Transformer 9",
        .type = ItemHandlerType::Inventory,
        .inventory = {.item_id = 0x7C},
    },
};

// Item lookup registry
const std::unordered_map<int, const APItemDef *> item_registry = {
    {0x100, &item_defs[0]},   {0x101, &item_defs[1]},   {0x102, &item_defs[2]},  {0x103, &item_defs[3]},   {0x104, &item_defs[4]},   {0x105, &item_defs[5]},
    {0x106, &item_defs[6]},   {0x107, &item_defs[7]},   {0x108, &item_defs[8]},  {0x109, &item_defs[9]},   {0x10A, &item_defs[10]},  {0x10B, &item_defs[11]},
    {0x10C, &item_defs[12]},  {0x10D, &item_defs[13]},  {0x10E, &item_defs[14]}, {0x10F, &item_defs[15]},  {0x110, &item_defs[16]},  {0x111, &item_defs[17]},
    {0x112, &item_defs[18]},  {0x113, &item_defs[19]},  {0x114, &item_defs[20]}, {0x115, &item_defs[21]},  {0xB, &item_defs[22]},    {0x95, &item_defs[23]},
    {0x96, &item_defs[24]},   {0x97, &item_defs[25]},   {0x98, &item_defs[26]},  {0x9D, &item_defs[27]},   {0x42, &item_defs[28]},   {0x43, &item_defs[29]},
    {0x44, &item_defs[30]},   {0x45, &item_defs[31]},   {0x46, &item_defs[32]},  {0x47, &item_defs[33]},   {0x48, &item_defs[34]},   {0x49, &item_defs[35]},
    {0x4A, &item_defs[36]},   {0x4B, &item_defs[37]},   {0x4C, &item_defs[38]},  {0x4D, &item_defs[39]},   {0x5F, &item_defs[40]},   {0x60, &item_defs[41]},
    {0x62, &item_defs[42]},   {0x63, &item_defs[43]},   {0x71, &item_defs[44]},  {0x72, &item_defs[45]},   {0x73, &item_defs[46]},   {0x75, &item_defs[47]},
    {0x76, &item_defs[48]},   {0x77, &item_defs[49]},   {0x5C, &item_defs[50]},  {0x40, &item_defs[51]},   {0x5, &item_defs[52]},    {0x6, &item_defs[53]},
    {0xCC, &item_defs[54]},   {0x1F, &item_defs[55]},   {0xE, &item_defs[56]},   {0xF, &item_defs[57]},    {0x9E, &item_defs[58]},   {0x4, &item_defs[59]},
    {0x8, &item_defs[60]},    {0x9, &item_defs[61]},    {0xA, &item_defs[62]},   {0xC, &item_defs[63]},    {0xD, &item_defs[64]},    {0x70, &item_defs[65]},
    {0x8E, &item_defs[66]},   {0x8F, &item_defs[67]},   {0x90, &item_defs[68]},  {0x91, &item_defs[69]},   {0x92, &item_defs[70]},   {0x93, &item_defs[71]},
    {0x99, &item_defs[72]},   {0x9A, &item_defs[73]},   {0x9B, &item_defs[74]},  {0xA0, &item_defs[75]},   {0xA1, &item_defs[76]},   {0xA3, &item_defs[77]},
    {0xA4, &item_defs[78]},   {0xA5, &item_defs[79]},   {0xA6, &item_defs[80]},  {0xA7, &item_defs[81]},   {0xA9, &item_defs[82]},   {0xAA, &item_defs[83]},
    {0xAB, &item_defs[84]},   {0xAD, &item_defs[85]},   {0xAE, &item_defs[86]},  {0xB0, &item_defs[87]},   {0xB1, &item_defs[88]},   {0xB2, &item_defs[89]},
    {0xB3, &item_defs[90]},   {0xB4, &item_defs[91]},   {0xB5, &item_defs[92]},  {0xB6, &item_defs[93]},   {0xB7, &item_defs[94]},   {0xB8, &item_defs[95]},
    {0xB9, &item_defs[96]},   {0xBA, &item_defs[97]},   {0xBB, &item_defs[98]},  {0xBC, &item_defs[99]},   {0xBD, &item_defs[100]},  {0xBE, &item_defs[101]},
    {0xBF, &item_defs[102]},  {0xC0, &item_defs[103]},  {0xC1, &item_defs[104]}, {0xC2, &item_defs[105]},  {0xC3, &item_defs[106]},  {0xC4, &item_defs[107]},
    {0xC5, &item_defs[108]},  {0xC6, &item_defs[109]},  {0xC7, &item_defs[110]}, {0x303, &item_defs[111]}, {0x304, &item_defs[112]}, {0x305, &item_defs[113]},
    {0x306, &item_defs[114]}, {0x307, &item_defs[115]}, {0x4E, &item_defs[116]}, {0x4F, &item_defs[117]},  {0x50, &item_defs[118]},  {0x308, &item_defs[119]},
    {0x10, &item_defs[120]},  {0x11, &item_defs[121]},  {0x12, &item_defs[122]}, {0x13, &item_defs[123]},  {0x14, &item_defs[124]},  {0x15, &item_defs[125]},
    {0x16, &item_defs[126]},  {0x17, &item_defs[127]},  {0x18, &item_defs[128]}, {0x19, &item_defs[129]},  {0x1A, &item_defs[130]},  {0x1B, &item_defs[131]},
    {0x1C, &item_defs[132]},  {0x1D, &item_defs[133]},  {0x1E, &item_defs[134]}, {0x300, &item_defs[135]}, {0x301, &item_defs[136]}, {0x302, &item_defs[137]},
    {0xC8, &item_defs[138]},  {0x5B, &item_defs[139]},  {0xC9, &item_defs[140]}, {0x79, &item_defs[141]},  {0xCF, &item_defs[142]},  {0xCB, &item_defs[143]},
    {0xCA, &item_defs[144]},  {0x7B, &item_defs[145]},  {0x7A, &item_defs[146]}, {0x7C, &item_defs[147]},
};

const APItemDef *findItem(int id)
{
    auto it = item_registry.find(id);
    if (it != item_registry.end())
    {
        return it->second;
    }
    return nullptr;
}

} // namespace apitems
