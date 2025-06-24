#pragma once
#include <cstdint>

namespace okami
{
/**
 * @brief Enum representing first-time acquisition flags by bit index across 32
 * bytes.
 *
 * Bit values represent contiguous bits starting from bit 0 (lowest bit of byte
 * 0), up through bit 255 (highest bit of byte 31).
 */
enum class AcquisitionOverlay : uint8_t
{
    // Byte 0
    demon_fang = 0,
    thunder_edge = 1,
    eighth_wonder = 2,
    blade_of_kusanagi = 3,
    seven_strike = 4,
    tsumugari = 5,
    tundra_beads = 6,
    resurrection_beads = 7,

    // Byte 1
    exorcism_beads = 8,
    life_beads = 9,
    devout_beads = 10,
    solar_flare = 11,
    trinity_mirror = 12,
    infinity_judge = 13,
    snarling_beast = 14,
    divine_retribution = 15,

    // Byte 2
    golden_peach = 16,
    mermaid_coin = 17,
    inkfinity_stone = 18,
    vengeance_slip = 19,
    peace_bell = 20,
    exorcism_slip_s = 21,
    exorcism_slip_m = 22,
    exorcism_slip_l = 23,

    // Byte 3
    ink_bottle = 24,
    astral_pouch = 25,
    sun_fragment = 26,
    holy_bone_l = 27,
    spirit_globe_l = 28,
    spirit_globe_m = 29,
    spirit_globe_s = 30,

    // Byte 4
    kamiki_orange = 32,
    millet_dumplings = 33,
    rice_stew = 34,
    sasa_cake = 35,
    sasa_hotchpotch = 36,
    sasa_dumplings = 37,
    sasa_fruit = 38,
    sasa_crab = 39,

    // Byte 5
    sasa_meat = 40,
    sasa_egg_rolls = 41,
    sasa_fish = 42,
    sasa_rice = 43,
    gourd = 44,
    grapes = 45,
    dumplings = 46,
    rice_balls = 47,

    // Byte 6
    roasted_fish = 48,
    bamboo_shoot = 49,
    roasted_meat = 50,
    bale_of_rice = 51,
    watermelon = 52,
    potato = 53,
    cherry_cake = 54,
    chinese_cabbage = 55,

    // Byte 7
    cabbage = 56,
    ginseng = 57,
    turnip = 58,
    radish = 59,
    baked_oranges = 60,
    orange = 61,
    apple = 62,
    peach = 63,

    // Byte 8
    golden_mushroom = 64,
    mother_tree = 65,
    legend_of_orochi = 66,
    vista_of_the_gods = 67,
    karmic_transformer_1 = 68,

    // Byte 9
    duty_orb = 79,

    // Byte 10
    justice_orb = 80,
    loyalty_orb = 81,
    black_demon_horn = 82,
    eyeball_of_fire = 83,
    lips_of_ice = 84,
    ogre_liver = 85,
    mask = 86,
    shell_amulet = 87,

    // Byte 11
    thunder_brew = 88,
    fox_rods = 89,
    dragon_orb = 90,
    border_key = 91,
    lucky_mallet = 92,
    canine_tracker = 93,
    oddly_shaped_turnip = 94,
    ruins_key = 95,

    // Byte 12
    galestorm_tech = 96,
    waterspout_tech_2 = 97,
    inferno_tech = 98,
    karmic_transformer_9 = 99,
    karmic_transformer_7 = 100,
    karmic_transformer_8 = 101,
    karmic_transformer_3 = 102,

    // Byte 13
    marlin_rod = 104,
    pinwheel = 105,
    herbal_medicine = 106,
    treasure_box = 108,
    blinding_snow = 109,
    charcoal = 110,
    travelers_charm = 111,

    // Byte 14
    holy_artifacts = 112,
    digging_tips = 113,
    battle_tips = 114,
    travel_tips = 115,
    brush_tips = 116,
    another_civilization = 117,
    land_of_the_gods = 118,
    cherry_bomb_3 = 119,

    // Byte 15
    cherry_bomb_2 = 120,
    power_slash_3 = 121,
    tribe_of_the_moon = 122,
    celestial_envoy = 123,
    sewaprolo = 124,
    purification_sake_8 = 125,
    northern_land = 126,
    gimmick_gear = 127,

    // Byte 16
    fog_pot = 128,
    gold_dust = 129,
    fire_tablet = 130,
    water_tablet = 131,
    godly_charm = 132,
    steel_soul_sake = 133,
    steel_fist_sake = 134,
    golden_ink_pot = 135,

    // Byte 17
    wood_mat = 136,
    thieves_glove = 137,
    golden_lucky_cat = 138,
    sashimi = 139,
    feedbag_fish = 140,
    feedbag_seeds = 141,
    feedbag_herbs = 142,
    feedbag_meat = 143,

    // Byte 18
    holy_bone_s = 144,
    holy_bone_m = 145,
    waterspout_tech_1 = 146,
    veil_of_mist_tech = 147,
    mark_of_kabegami = 148,
    ink_bullet_tips = 149,
    greensprout_tips = 150,
    enhancing_weapons = 151,

    // Byte 19
    fleeing_battle = 152,
    godhood_tips = 153,
    feeding = 154,
    enhancing_divinity = 155,
    chestnut = 156,
    power_slash_2 = 158,
    thunderstorm_tech = 159,

    // Byte 20
    cat_statue = 160,
    boar_statue = 161,
    dog_statue = 162,
    rooster_statue = 163,
    monkey_statue = 164,
    sheep_statue = 165,
    horse_statue = 166,
    snake_statue = 167,

    // Byte 21
    dragon_statue = 168,
    rabbit_statue = 169,
    tiger_statue = 170,
    bull_statue = 171,
    ruby_tassels = 172,
    pearl = 173,
    crystal = 174,
    coral_fragment = 175,

    // Byte 22
    dragonfly_bead = 177,
    glass_beads = 178,
    wooden_bear = 180,
    lacquerware_set = 181,
    etched_glass = 182,

    // Byte 23
    bull_horn = 184,
    rat_statue = 185,
    silver_pocket_watch = 186,
    vase = 187,
    incense_burner = 188,
    kutani_pottery = 190,
    white_porcelain_pot = 191,

    // Byte 24
    killifish = 192,
    smelt = 193,
    trout = 194,
    sweetfish = 195,
    yellowtail = 196,
    bonito = 197,
    clownfish = 198,
    black_bass = 199,

    // Byte 25
    robalo = 200,
    huchen = 201,
    koi = 202,
    salmon = 203,
    striped_snapper = 204,
    red_snapper = 205,
    mountain_trout = 206,
    supreme_tuna = 207,

    // Byte 26
    karmic_transformer_4 = 208,
    giant_salmon = 209,
    string_of_beads = 210,
    stray_bead = 211,
    karmic_transformer_5 = 212,
    karmic_transformer_6 = 213,
    karmic_transformer_2 = 214,
    karmic_returner = 215,

    // Byte 27
    jade_tassels = 216,
    amethyst_tassels = 217,
    cats_eye_tassels = 218,
    amber_tassels = 219,
    agate_tassels = 220,
    turqoise_tassels = 221,
    emerald_tassels = 222,
    sapphire_tassels = 223,

    // Byte 28
    cutlass_fish = 224,
    whopper = 225,
    gale_shrine_map = 226,
    wawku_shrine_map = 227,
    oni_island_map = 228,
    imperial_palace_map = 229,
    moon_cave_map = 230,
    tsuta_ruins_map = 231,

    // Byte 29
    squid = 232,
    octopus = 233,
    sea_horse = 234,
    loggerhead_turtle = 235,
    marlin = 236,
    starfish = 237,
    river_crab = 238,
    blowfish = 239,

    // Byte 30
    manta = 240,
    nautilus = 241,
    scallop = 242,
    crawfish = 243,
    lobster = 244,
    goby = 245,
    giant_catfish = 246,
    catfish = 247,

    // Byte 31
    monkfish = 248,
    oarfish = 249,
    moray = 250,
    loach = 251,
    freshwater_eel = 252,
    sunfish = 253,
    sturgeon = 254,
    flying_fish = 255
};
} // namespace okami
