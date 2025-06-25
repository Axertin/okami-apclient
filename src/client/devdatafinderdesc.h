
#include <unordered_map>

#include "okami/data/maptype.hpp"

const std::unordered_map<unsigned, const char *> worldStateMapBitsDesc[okami::MapTypes::NUM_MAP_TYPES + 1] = {
    // None/Global
    {
        {10, "Prevent use of celestial brush"},
        // 7 - battle paused, or tutorial about vulnerable enemies?
        // 8 - in battle? tutorial battle?
        // 9 - carrying susano
    },
    // KamikiVillageGameStart
    {
        {16, "Ran into the forcefield towards Kamiki (instant)"},
        // 15 and 17 get set when Issun starts conversation, 15 16 and 17 all get cleared after the convo
        {18, "Identified invisible forcefield towards Kamiki"},
        // 43, 44 - peach was cut down
    },
    // CaveOfNagi
    {
        {9, "Entered cave cutscene trigger"},
        // 10, 15 - both set when finishing cave intro cutscene
        {17, "Close enough to use rejuvenation on broken sword"},
        // 24 - can power slash the rock
        // 29, 53 - rock gets slashed
        // 36, 37 - triggered tutorial battle (only set during battle?)
        {38, "Constellation pulsing"},
        {43, "Set while in cave area"},
    },
    // KamikiVillage
    {
        {2, "Farm digging minigame is active"},
        {3, "Laundry pole restored"},
        {4, "Farm digging minigame was triggered at least once"},
        {5, "Completed digging minigame (Oddly Shaped Turnip spawns)"},
        {6, "Turned in Oddly Shaped Turnip"},
        {7, "Entered mill after repair"},
        {8, "Turned in Oddly Shaped Turnip"},
        {12, "Talked with 3 statues, platform trigger is enabled"},
        {14, "Battle with enemies attacking Mr Orange"},
        {15, "Finished battle with Mr Orange enemies"},
        {17, "Finished battle with Merchant enemies, entrance area restored"},
        {18, "Water wheel rejuvenation applied"},
        {19, "Water wheel repaired"},
        {20, "Trigger convo near Susano's house snoring"},
        {23, "Went to lookout while cursed"},
        {24, "Went to lookout after interacting with statues"},
        {25, "Expecting to draw the sun"},
        {29, "Interacted with Mr Orange at night"},
        {30, "Interacted with Mr Orange at night 2"},
        {31, "Interacted with Mr Orange at night 3"},
        // 13, 26, 164 - drew the sun, village to be uncursed
        {33, "Interacted with Mr Orange first time"},
        {34, "Interacted with Mr Orange second time"},
        {35, "Interacted with Mr Orange third time"},
        // 51, 52 - set after restoring laundry pole
        // 53, 54 - set after drying the laundry
        {57, "Interacted with Mrs Orange at night"},
        {58, "Interacted with Mrs Orange first time"},
        {59, "Interacted with Mrs Orange second time"},
        {60, "Interacted with Mrs Orange post-laundry pole 1"},
        {61, "Interacted with Mrs Orange at night post-quest"},
        {72, "Interacted with Susano sleeping first time"},
        {73, "Interacted with Susano sleeping boulder quest 1"},
        {74, "Interacted with Susano sleeping boulder quest 2"},
        // 75, 76, 77, 78 - Carrying Susano
        // 78 - only set while Susano is picked up
        // 79, 83 - set after bringing Susano to Merchant
        {80, "Susano runs off after bringing to Merchant"},
        {81, "Interacted with Susano after Merchant convo"},
        {82, "Susano given sake"},
        {84, "Interacted with Susano for boulder slash minigame"},
        {85, "Susano to slash big boulder"},
        {86, "Susano woken up"},
        {88, "Kushi triggered by water wheel repair"},
        {89, "Kushi moves inside mill"},
        {90, "Interacted with Kushi after repair"},
        {91, "Interacted with Kushi first time"},
        {92, "Interacted with Kushi second time"},
        {99, "Interacted with Mushi first time"},
        {101, "Interacted with Mushi at night"},
        {111, "Interacted with Mushi's Mama first time"},
        {112, "Interacted with Mushi's Mama second time"},
        {114, "Interacted with Mushi's Mama at night"},
        {120, "Merchant post-battle state"},
        {127, "Beat komuso enemies within 10s"},
        {134, "Interacted with Camille"},
        {135, "Interacted with Camille 2"},
        {153, "Iron training rock slashed"},
        {155, "Boulder removed"},
        {156, "Slash big boulder attempt made"},
        // 154 - slash minigame missed 1st dummy
        {165, "First seedback acquired"},
    },
    // HanaValley
    {},
    // TsutaRuins
    {},
    // CityCheckpoint
    {},
    // TsutaRuinsSpiderQueenArena
    {},
    // GaleShrine
    {},
    // KusaVillage
    {},
    // SasaSanctuary
    {},
    // AgataForestMadameFawnsHouse
    {},
    // DiggingMinigame
    {},
    // OnigiriDojoLessonRoom
    {},
    // GaleShrineCrimsonHelmArena
    {},
    // CalcifiedCavern
    {},
    // MoonCaveInterior
    {},
    // MoonCaveStaircaseandOrochiArena
    {},
    // RyoshimaCoastBanditSpiderArena
    {},
    // TakaPassStrayBead26Cave
    {},
    // KamuiCherryBomb3Cave
    {},
    // NRyoshimaCoastPowerSlash2Cave
    {},
    // NRyoshimaCoastCherryBomb2Cave
    {},
    // KamuiEzofujiPowerSlash3Cave
    {},
    // KamuiBlockheadGrandeCave
    {},
    // NRyoshimaCoastBanditSpiderArena
    {},
    // KamuiBanditSpiderArena
    {},
    // DragonPalaceStrayBead70Cave
    {},
    // NRyoshimaCoastStrayBead63Cave
    {},
    // NewGameIntroCutsceneMap
    {
        {2, "Transitioned from backstory dialogue to level"},
        {3, "Orochi finished speaking to Susano"},
    },
    // RiverOftheHeavens
    {
        {20, "Save point tutorial triggered"},
        {21, "Breaking pots tutorial triggered"},
        // 22 and 23 are set by breaking a pot
        {24, "Wall jump tutorial triggered"},
        {25, "Post-wall jump triggered"},
        {27, "Broken bridge cutscene triggered"},
        {28, "River of heavens area plaque scene triggered"},
        {29, "Stars on hill cutscene trigger"},
        // 30 - probably the hint on the constellation if you fail to draw it
        // 31, 32, 50, 55 - constellation completed
        {37, "Area introduction trigger when coming back down the hill from constellation"},
        // 39, 40 - set after using brush, cleared after issun convo
        {48, "Constellation pulsing (set to 1 then 0 every time it pulses)"},
        {49, "Broken bridge restored"},
        // 44, 51 - first used rejuvenation on the water (cutscene trigger)
        {52, "River can be rejuvinated (only set when close enough)"},
        {53, "Astral pouch cutscene triggered"},
    },
    // SeianCityAristocraticQtr
    {},
    // SeianCityCommonersQtr
    {},
    // SeianCityAristocraticQtrHimikosPalace
    {},
    // DragonPalace
    {},
    // InsidetheDragon
    {},
    // SunkenShip
    {},
    // ImperialPalaceAmmySize
    {},
    // ImperialPalaceIssunSize
    {},
    // OniIslandLowerInterior
    {},
    // OniIslandNinetailsArena
    {},
    // CatcallTower
    {},
    // ImperialPalaceBlightArena
    {},
    // SeianCityCommonersQtrKimonoShop
    {},
    // OniIslandExterior
    {},
    // OniIslandUpperInterior
    {},
    // OniIslandSidescroller
    {},
    // Wepkeer
    {},
    // KamikiVillage100yearsago
    {},
    // WawkuShrine
    {},
    // WawkuShrineNechkuArena
    {},
    // Ponctan
    {},
    // MoonCave100yearsago
    {},
    // ArkOfYamato
    {},
    // ArkOfYamatoSpiderQueenArena
    {},
    // ArkOfYamatoOrochiArena
    {},
    // ArkOfYamatoBlightArena
    {},
    // ArkOfYamatoNinetailsArena
    {},
    // ArkOfYamatoCrimsonHelmArena
    {},
    // PonctanMrsSealsHouse
    {},
    // Yoshpet
    {},
    // InnerYoshpet
    {},
    // ArkOfYamatoYamiArena
    {},
    // WepkeerSquare
    {},
    // WawkuShrineLechkuAndNechkuArena
    {},
    // FishingwithBenkeionSeianBridge
    {},
    // FishingwithBenkeinexttoHimikosPalace
    {},
    // FishingwithKokariinAgata
    {},
    // FishingwithKokariinKamui
    {},
    // Unknown70
    {},
    // FishingwithFishermaninNRyo
    {},
    // ShinshuField
    {
        {2, "Entered Shinshu field first time"},
    },
    // AgataForest
    {},
    // MoonCaveEntrance
    {},
    // TakaPass
    {},
    // RyoshimaCoast
    {},
    // Unknown76
    {},
    // NRyoshimaCoast
    {},
    // Kamui
    {},
    // KamuiEzofuji
    {},
    // ShinshuField100yearsago
    {},
    // MoonCaveEntrance100yearsago
    {},
    // Unk82
    {},
    // Unk83 (beyond last item normally)
    {},
};

const std::unordered_map<unsigned, const char *> animalsFedDesc = {
    {3, "Kamiki village - Chicken"},      {4, "Kamiki village entrance - Hare"}, {5, "Kamiki village - Dog (Hayabusa)"},
    {6, "Kamiki village hill - Sparrow"}, {7, "Kamiki village pond - Sparrow"},  {8, "Kamiki village farm - Sparrow"},

};

// logbookAvailable
const std::unordered_map<unsigned, const char *> tracker1Desc = {
    // 2 - Battle started after talking with merchant
    // 3 - Yomigami gives rejuvenation
    // 4 - Tachigami gives power slash
    // 6 - After gaining sunrise and entering battle
    // 7 - After picking up susano first time
    // 9 - Guardian Sapling Trouble journal added
    // 36 - river of heavens stars twinkling sets this
    // 37 - boulder slash minigame failed
    // 41 - Mrs Orange gave Cherry Cake for quest
    // 42 - cave of nagi constellation sets this
    // 45 - boulder removed
};

// unknown
const std::unordered_map<unsigned, const char *> tracker2Desc = {
    // 16 - kamiki entrance hare fed
    // 19 - set when kamiki pond sparrow fed
    // 23 - when Hayabusa gets fed
};

const std::unordered_map<unsigned, const char *> mapBitsDesc[okami::MapTypes::NUM_MAP_TYPES] = {
    // None/Global
    {},
    // KamikiVillageGameStart
    {},
    // CaveOfNagi
    {{1120, "Chest on the other side of the river (Stray Bead)"}},
    // KamikiVillage
    {
        {1, "Entered Susano's Bunker first time"},
        {2, "Interacted with 3 statues"}, // Mrs Orange statue
        {3, "Quest marker to get to lookout"},
        {4, "Marchant spoken to first time"},
        {5, "Sake gets spawned in mill"},
        // 94 - Set post merchant battle
        // 95 - set after talking with 3 statues, cleared post merchant battle

        // 128 - 159 - int for NPC interacted with as the first statue
        {157, "First statue id"},
        {158, "First statue id"},
        {159, "First statue id"},
        // 160 - 191 - int for NPC interacted with as the second statue
        {189, "Second statue id"},
        {190, "Second statue id"},
        {191, "Second statue id"},
        // 192 - 223 - int for number of statues interacted with
        // 196-199 - int for number of turnips dug in digging minigame
        {222, "Num statues interacted with"},
        {223, "Num statues interacted with"},

        // 240 - 255 - int for digging minigame timer
        {240, "Digging minigame timer"},
        {241, "Digging minigame timer"},
        {242, "Digging minigame timer"},
        {243, "Digging minigame timer"},
        {244, "Digging minigame timer"},
        {245, "Digging minigame timer"},
        {246, "Digging minigame timer"},
        {247, "Digging minigame timer"},
        {248, "Digging minigame timer"},
        {249, "Digging minigame timer"},
        {250, "Digging minigame timer"},
        {251, "Digging minigame timer"},
        {252, "Digging minigame timer"},
        {253, "Digging minigame timer"},
        {254, "Digging minigame timer"},
        {255, "Digging minigame timer"},
        {1023, "Chest on the ledge opened"},
        {1024, "Chest on hill dug up"},
        {1030, "Chest on farm dug up"},
        {1032, "Chest in Mr Orange's house dug up"},
        {1061, "Buried Chest on island dug up"},
        {1120, "Chest on hill opened (Traveler's Charm)"},
        {1126, "Chest on farm opened (Dragonfly Bead)"},
        {1127, "Chest in river by bridge (further) opened (Rabbit Statue)"},
        {1128, "Chest in Mr Orange's house opened (Coral Fragment)"},
        {1129, "Mill Bale of Rice 1"},
        {1130, "Mill Bale of Rice 2"},
        {1131, "Mill Bale of Rice 3"},
        {1132, "Mill Rice Balls 1"},
        {1133, "Mill Rice Balls 2"},
        {1134, "Mill Rice Balls 3"},
        {1135, "Mr Orange's House Orange 1"},
        {1136, "Mr Orange's House Orange 2"},
        {1137, "Mr Orange's House Orange 3"},
        {1138, "Mr Orange's House Orange 4"},
        {1139, "Mr Orange's House Orange 5"},
        {1140, "Chest in water by waterfall opened (Vase)"},
        {1141, "Chest in river by bridge (closer) opened (Glass Beads)"},
        {1142, "Kushi's House Bale of Rice 1"},
        {1143, "Kushi's House Bale of Rice 1"},
        {1144, "Kushi's House Bale of Rice 1"},
        {1149, "Solar orb in the pond 1"},
        {1150, "Solar orb in the pond 2"},
        {1151, "Solar orb in the pond 3"},
        {1152, "Solar orb in the pond 4"},
        {1154, "Chest behind rock on island opened (Dragonfly Bead)"},
        {1157, "Buried chest on island opened (Wooden Bear)"},
        {1158, "Chest by pond opened (Feedbag (Seeds))"},
        {1200, "Vista of the Gods given to Susano"},
        {1296, "Pond by lookout restored"},
        {1256, "Time of day"},
        {1257, "Time of day"},
        {1258, "Time of day"},
        {1259, "Time of day"},
        {1260, "Time of day"},
        {1261, "Time of day"},
        {1262, "Time of day"},
        {1263, "Time of day"},
        {1264, "Time of day"},
        {1265, "Time of day"},
        {1266, "Time of day"},
        {1267, "Time of day"},
        {1268, "Time of day"},
        {1269, "Time of day"},
        {1270, "Time of day"},
        {1271, "Time of day"},
        {1272, "Time of day"},
        {1273, "Time of day"},
        {1274, "Time of day"},
        {1275, "Time of day"},
        {1276, "Time of day"},
        {1277, "Time of day"},
        {1278, "Time of day"},
        {1279, "Time of day"},
        {1297, "Merchant battle/Entrance area restored"},
        {1568, "Mr Orange has more to say"},
        {1569, "Mrs Orange has more to say"},
        {1570, "Susano has more to say"},
        {1571, "Kushi has more to say"},
        {1572, "Mushi has more to say"},
    },
    // HanaValley
    {},
    // TsutaRuins
    {},
    // CityCheckpoint
    {},
    // TsutaRuinsSpiderQueenArena
    {},
    // GaleShrine
    {},
    // KusaVillage
    {},
    // SasaSanctuary
    {},
    // AgataForestMadameFawnsHouse
    {},
    // DiggingMinigame
    {},
    // OnigiriDojoLessonRoom
    {},
    // GaleShrineCrimsonHelmArena
    {},
    // CalcifiedCavern
    {},
    // MoonCaveInterior
    {},
    // MoonCaveStaircaseandOrochiArena
    {},
    // RyoshimaCoastBanditSpiderArena
    {},
    // TakaPassStrayBead26Cave
    {},
    // KamuiCherryBomb3Cave
    {},
    // NRyoshimaCoastPowerSlash2Cave
    {},
    // NRyoshimaCoastCherryBomb2Cave
    {},
    // KamuiEzofujiPowerSlash3Cave
    {},
    // KamuiBlockheadGrandeCave
    {},
    // NRyoshimaCoastBanditSpiderArena
    {},
    // KamuiBanditSpiderArena
    {},
    // DragonPalaceStrayBead70Cave
    {},
    // NRyoshimaCoastStrayBead63Cave
    {},
    // NewGameIntroCutsceneMap
    {},
    // RiverOftheHeavens
    {{1120, "First chest (Holy Bone S)"}, {1121, "Second chest (Astral Pouch)"}},
    // SeianCityAristocraticQtr
    {},
    // SeianCityCommonersQtr
    {},
    // SeianCityAristocraticQtrHimikosPalace
    {},
    // DragonPalace
    {},
    // InsidetheDragon
    {},
    // SunkenShip
    {},
    // ImperialPalaceAmmySize
    {},
    // ImperialPalaceIssunSize
    {},
    // OniIslandLowerInterior
    {},
    // OniIslandNinetailsArena
    {},
    // CatcallTower
    {},
    // ImperialPalaceBlightArena
    {},
    // SeianCityCommonersQtrKimonoShop
    {},
    // OniIslandExterior
    {},
    // OniIslandUpperInterior
    {},
    // OniIslandSidescroller
    {},
    // Wepkeer
    {},
    // KamikiVillage100yearsago
    {},
    // WawkuShrine
    {},
    // WawkuShrineNechkuArena
    {},
    // Ponctan
    {},
    // MoonCave100yearsago
    {},
    // ArkOfYamato
    {},
    // ArkOfYamatoSpiderQueenArena
    {},
    // ArkOfYamatoOrochiArena
    {},
    // ArkOfYamatoBlightArena
    {},
    // ArkOfYamatoNinetailsArena
    {},
    // ArkOfYamatoCrimsonHelmArena
    {},
    // PonctanMrsSealsHouse
    {},
    // Yoshpet
    {},
    // InnerYoshpet
    {},
    // ArkOfYamatoYamiArena
    {},
    // WepkeerSquare
    {},
    // WawkuShrineLechkuAndNechkuArena
    {},
    // FishingwithBenkeionSeianBridge
    {},
    // FishingwithBenkeinexttoHimikosPalace
    {},
    // FishingwithKokariinAgata
    {},
    // FishingwithKokariinKamui
    {},
    // Unknown70
    {},
    // FishingwithFishermaninNRyo
    {},
    // ShinshuField
    {
        {1247, "Entered Shinshu field first time"},
        {1256, "Time of day"},
        {1257, "Time of day"},
        {1258, "Time of day"},
        {1259, "Time of day"},
        {1260, "Time of day"},
        {1261, "Time of day"},
        {1262, "Time of day"},
        {1263, "Time of day"},
        {1264, "Time of day"},
        {1265, "Time of day"},
        {1266, "Time of day"},
        {1267, "Time of day"},
        {1268, "Time of day"},
        {1269, "Time of day"},
        {1270, "Time of day"},
        {1271, "Time of day"},
        {1272, "Time of day"},
        {1273, "Time of day"},
        {1274, "Time of day"},
        {1275, "Time of day"},
        {1276, "Time of day"},
        {1277, "Time of day"},
        {1278, "Time of day"},
        {1279, "Time of day"},
    },
    // AgataForest
    {},
    // MoonCaveEntrance
    {},
    // TakaPass
    {},
    // RyoshimaCoast
    {},
    // Unknown76
    {},
    // NRyoshimaCoast
    {},
    // Kamui
    {},
    // KamuiEzofuji
    {},
    // ShinshuField100yearsago
    {},
    // MoonCaveEntrance100yearsago
    {},
    // Unk82
    {},
};

const std::unordered_map<unsigned, const char *> dialogBitsDesc[okami::MapTypes::NUM_MAP_TYPES] = {
    // None/Global
    {},
    // KamikiVillageGameStart
    {
        {2, "Ammy awakened by Sakuya"},
        {7, "Informed about the impenetrable barrier"},
        {8, "Second dialogue after hitting barrier"},
        {11, "Examined the fruit"},
    },
    // CaveOfNagi
    {
        {1, "Failed to use celestial brush"},
        {3, "Introduction on cave statue"},
        {4, "Mentions the broken sword"},
        {5, "Second mention of broken sword"},
        {6, "Issun comments when you use celestial brush but do nothing"},
        {7, "Second comment when not completing the sword with the brush"},
        {8, "Comment about handling the celestial brush outside of Nagi statue range"},
        {9, "Second comment about celestial brush outside of Nagi statue range"},
        {11, "Third comment about celestial brush outside of Nagi statue range"},
        {12, "Third comment about failing to draw the sword"},
        {14, "Finished commenting on constellation"},
        {15, "Tachigami dialogue"},
        {16, "Issun talks about using power slash"},
        {17, "Comments if you don't use power slash after time limit"},
        {21, "Discussion after slicing the rock"},
        {22, "Second part of discussion after slicing the rock"},
        {28, "Introduced to tutorial battle"},
        {29, "Introduced to enemy vulnerable state"},
        {40, "Examined fountain of Nagi sign"},
        {43, "Comment on completed Nagi statue"},
        {44, "Interacting with the closed wooden blockade"},
        {45, "Interact with blockade after getting power slash"},
    },
    // KamikiVillage
    {
        // 1 - gets set when pond gets restored
        {4, "Mr Orange initial convo 1"},
        {5, "Mr Orange initial convo 2"},
        {6, "Mr Orange initial convo 3"},
        {7, "Bashed Mr Orange"},
        {11, "Mr Orange initial night convo 1"},
        {12, "Mr Orange initial night convo 2"},
        {13, "Mr Orange initial night convo 3"},
        {14, "Mr Orange initial night convo 4"},
        {16, "Mr Orange second night convo 1"},
        {17, "Mr Orange second night convo nothing left"},
        {38, "Mrs Orange initial night convo 1"},
        {39, "Mrs Orange initial night convo 2"},
        {40, "Bashed Mrs Orange"},
        {41, "Mrs Orange initial convo 1"},
        {43, "Mrs Orange initial convo 2"},
        {44, "Mrs Orange initial convo 3"},
        {45, "Mrs Orange response to laundry pole restored"},
        {47, "Mrs Orange post-laundry pole convo 1"},
        {48, "Mrs Orange post-laundry pole convo 2"},
        {49, "Mrs Orange response to dry laundry"},
        {50, "Mrs Orange post-dry laundry convo"},
        {55, "Mrs Orange reward night time convo part 1"},
        {56, "Mrs Orange reward night time convo part 2"},
        {59, "Mrs Orange post quest night convo"},
        {65, "Susano sleeping convo 1"},
        {66, "Susano sleeping convo 2"},
        {67, "Sleeping Susano bashed"},
        {68, "Susano snores too loud convo (triggered near house)"},
        {69, "Susano sleeping boulder quest convo 1"},
        {70, "Susano sleeping boulder quest convo 2"},
        {71, "Susano sleeping boulder quest bashed"},
        {91, "Susano convo after brought to Merchant"},
        {92, "Convo after Susano runs off from Merchant"},
        {93, "Susano post-merchant convo 1"},
        {94, "Susano post-merchant convo 1 part 2"},
        {95, "Susano post-merchant convo 1 part 3"},
        {98, "Susano wants sake convo"},
        {99, "Lazy Susano night time convo"},
        {100, "Bashed lazy Susano at night"},
        {101, "Bashed lazy Susano"},
        {102, "Gave sake to Susano convo"},
        {103, "Susano pre-boulder minigame convo"},
        {105, "Susano pre-boulder minigame convo 2 (after failed attempt)"},
        {106, "Bashed Susano post-sake"},
        {109, "Issun tells you NPCs can have more to say (Kushi first time)"},
        {110, "Talked with Kushi second time"},
        {111, "Kushi third time"},
        {112, "Bashed Kushi"},
        {114, "Kushi sleeping at night"},
        {115, "Bashed Kushi sleeping at night"},
        {118, "Kushi response to water wheel repair"},
        {120, "Kushi post-mill repair convo 1"},
        {123, "Kushi post-mill repair convo 2"},
        {132, "Merchant auto comment post-battle"},
        {137, "Bashed Merchant pushing rock"},
        {138, "Bashed Merchant"},
        {139, "Merchant initial convo"},
        {141, "Merchant repeat convo after bringing Susano"},
        {143, "Merchant shop convo 1"},
        {144, "Merchant shop convo 2"},
        {146, "Bashed Merchant shop"},
        {154, "Mushi initial convo 1"},
        {155, "Mushi initial convo 2"},
        {158, "Bashed Mushi"},
        {159, "Mushi acknowledges you completing digging minigame"},
        {160, "Mushi post-minigame convo"},
        {164, "Mushi initial night convo 1"},
        {165, "Mushi initial night convo 2"},
        {171, "Mushi's Mama initial convo 1"},
        {172, "Mushi's Mama initial convo 2"},
        {175, "Mushi's Mama bashed"},
        {180, "Mushi's Mama initial night convo 1"},
        {181, "Mushi's Mama initial night convo 2"},
        {187, "Interact with monument while cursed"},
        {188, "Interact with monument"},
        {189, "Interact with sign while cursed"},
        {190, "Sign in front of Mr Orange's house"},
        {191, "Sign in front of Mushi's house"},
        {192, "Sign in front of Kushi's house"},
        {193, "Sign in front of Susano's house"},
        {194, "Interact with objects while cursed"},
        {195, "Interact with Kushi's storefront (nobody home)"},
        {197, "Interact with Kushi's house (drinking sake)"},
        {200, "Interact with boulder while cursed"},
        {201, "Bash too many times dialogue"},
        {206, "Interact with iron training rock"},
        {207, "Nothing to eat here - object interaction"},
        {208, "Mr Orange house Interact with stew"},
        {209, "Smelling Kushi's bedroll"},
        {210, "Kushi's house interact with barrel of sake"},
        {211, "Interact with Susano's jug blocker"},
        {212, "Interact with Susano's Kushi portrait"},
        {213, "Interact with Susano's bed"},
        {215, "Interact with sacred tree while cursed"},
        {216, "Mushi's house pot of stew"},
        {217, "Interact with millstone when waterwheel is broken"},
        {218, "Interact with millstone when waterwheel is repaired"},
        {221, "Interacted with statue of nagi in tree"},
        {227, "Interact with training swords"},
        {228, "Spoke with Camille"},
        {229, "Spoke with Camille 2"},
        {230, "Spoke with Camille 3"},
        {231, "Bashed Camellia"},
        {232, "Bashed Camille"},
        {239, "Intro to Kamiki"},
        {247, "Komuso first talk"},
        {248, "Komuso talk after defeating enemies"},
        {249, "Komuso talk after defeating enemies in 10s"},
        {250, "Komuso talk after monsters defeated"},
        {251, "Bash komuso"},
        {258, "First statue dialogue"},
        {259, "Second statue dialogue"},
        {260, "Interact with same statue dialogue"},
        {261, "Interact with same statue dialogue 2"},
        {262, "Third statue dialogue"},
        {264, "Tells you to go to the sacred deck after talking with 3 statues"},
        {265, "Bashing into a statue"},
        {267, "Interaction at lookout while cursed"},
        {268, "Dialogue at lookout after talking to statues"},
        {270, "Issun telling you to draw a big circle in the sky"},
        {273, "Dialogue after drawing the sun"},
        {281, "Intro to using praise"},
        {283, "Used praise after intro"},
        {284, "Skipped using praise after intro"},
        {285, "Intro to seedbags"},
        {286, "Talking about the monsters attacking Mr orange"},
        {287, "Susano introduction convo"},
        {288, "Big convo after removing boulder"},
        {307, "Issun hints what to do for boulder slash minigame"},
        {308, "Boulder slash minigame convo after slashing the 2 dummies"},
        {309, "Issun hints boulder slash minigame iron rock"},
        {310, "Susano tried Exploding Implosion first time"},
        {311, "Susano surprised at exploding implosion convo"},
        {312, "Pre-slash big boulder convo"},
        {313, "Second attempt to slash big boulder"},
    },
    // HanaValley
    {},
    // TsutaRuins
    {},
    // CityCheckpoint
    {},
    // TsutaRuinsSpiderQueenArena
    {},
    // GaleShrine
    {},
    // KusaVillage
    {},
    // SasaSanctuary
    {},
    // AgataForestMadameFawnsHouse
    {},
    // DiggingMinigame
    {},
    // OnigiriDojoLessonRoom
    {},
    // GaleShrineCrimsonHelmArena
    {},
    // CalcifiedCavern
    {},
    // MoonCaveInterior
    {},
    // MoonCaveStaircaseandOrochiArena
    {},
    // RyoshimaCoastBanditSpiderArena
    {},
    // TakaPassStrayBead26Cave
    {},
    // KamuiCherryBomb3Cave
    {},
    // NRyoshimaCoastPowerSlash2Cave
    {},
    // NRyoshimaCoastCherryBomb2Cave
    {},
    // KamuiEzofujiPowerSlash3Cave
    {},
    // KamuiBlockheadGrandeCave
    {},
    // NRyoshimaCoastBanditSpiderArena
    {},
    // KamuiBanditSpiderArena
    {},
    // DragonPalaceStrayBead70Cave
    {},
    // NRyoshimaCoastStrayBead63Cave
    {},
    // NewGameIntroCutsceneMap
    {
        {2, "Backstory dialogue completed"},
    },
    // RiverOftheHeavens
    {
        {2, "Breaking pots introduced"},
        {3, "Extra tip for Breaking pots (same dialogue)"},
        {4, "Dialog after breaking your first pot"},
        {5, "Introduction to wall jump"},
        {6, "Extra tip for wall jump (same dialogue)"},
        {7, "Wall jump tutorial completed, congrats"},
        {8, "Wall jump tutorial skipped (different dialogue)"},
        {9, "Intro to astral pouch"},
        {10, "Comment if you try to ignore the astral pouch chest"},
        {11, "Finished intro to broken bridge convo"},
        {12, "Finished second part of broken bridge convo"},
        {13, "Finished entire broken bridge convo"},
        // 14 River of heavens introduction skipped?
        {15, "Issun comments about the celestial river after getting rejuvenation"},
        {16, "Comments on your first successful use of rejuvenation"},
        {17, "Issun comment when attempting to use celestial brush"},
        {18, "Finished intro about the stars twinkling"},
        {19, "Finished second part of constellation dialogue"},
        {20, "Issun failed at drawing constellation"},
        {21, "Waited too long to draw constellation, more button tutorial"},
        {22, "Complains that you didn't do anything for the constellation"},
        {23, "Issun's second attempt at finishing the constellation"},
        {28, "River of Heavens area introduced"},
        {29, "Post-constellation, comments about receiving rejuvenation"},
        {30, "Camera controls introduced"},
        {31, "Save points introduced"},
        {36, "River of Heavens area second conversation"},
    },
    // SeianCityAristocraticQtr
    {},
    // SeianCityCommonersQtr
    {},
    // SeianCityAristocraticQtrHimikosPalace
    {},
    // DragonPalace
    {},
    // InsidetheDragon
    {},
    // SunkenShip
    {},
    // ImperialPalaceAmmySize
    {},
    // ImperialPalaceIssunSize
    {},
    // OniIslandLowerInterior
    {},
    // OniIslandNinetailsArena
    {},
    // CatcallTower
    {},
    // ImperialPalaceBlightArena
    {},
    // SeianCityCommonersQtrKimonoShop
    {},
    // OniIslandExterior
    {},
    // OniIslandUpperInterior
    {},
    // OniIslandSidescroller
    {},
    // Wepkeer
    {},
    // KamikiVillage100yearsago
    {},
    // WawkuShrine
    {},
    // WawkuShrineNechkuArena
    {},
    // Ponctan
    {},
    // MoonCave100yearsago
    {},
    // ArkOfYamato
    {},
    // ArkOfYamatoSpiderQueenArena
    {},
    // ArkOfYamatoOrochiArena
    {},
    // ArkOfYamatoBlightArena
    {},
    // ArkOfYamatoNinetailsArena
    {},
    // ArkOfYamatoCrimsonHelmArena
    {},
    // PonctanMrsSealsHouse
    {},
    // Yoshpet
    {},
    // InnerYoshpet
    {},
    // ArkOfYamatoYamiArena
    {},
    // WepkeerSquare
    {},
    // WawkuShrineLechkuAndNechkuArena
    {},
    // FishingwithBenkeionSeianBridge
    {},
    // FishingwithBenkeinexttoHimikosPalace
    {},
    // FishingwithKokariinAgata
    {},
    // FishingwithKokariinKamui
    {},
    // Unknown70
    {},
    // FishingwithFishermaninNRyo
    {},
    // ShinshuField
    {
        {188, "First entrance conversation"},
    },
    // AgataForest
    {},
    // MoonCaveEntrance
    {},
    // TakaPass
    {},
    // RyoshimaCoast
    {},
    // Unknown76
    {},
    // NRyoshimaCoast
    {},
    // Kamui
    {},
    // KamuiEzofuji
    {},
    // ShinshuField100yearsago
    {},
    // MoonCaveEntrance100yearsago
    {},
    // Unk82
    {},
};
