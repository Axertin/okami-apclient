
#include <unordered_map>

#include "okami/data/maptype.hpp"
#include "okami/data/structs.hpp"

namespace okami
{
const std::unordered_map<unsigned, const char *> animalsFedDesc = {
    {1, "Hana Valley campfire - Monkey"},
    {2, "Hana Valley mural - Boar Piglet"},
    {3, "Kamiki village - Chicken"},
    {4, "Kamiki village entrance - Hare"},
    {5, "Kamiki village - Dog (Hayabusa)"},
    {6, "Kamiki village hill - Sparrow"},
    {7, "Kamiki village pond - Sparrow"},
    {8, "Kamiki village farm - Sparrow"},
    {10, "Shinshu Field gate area by guardian tree - Hare"},
    {11, "Shinshu Field kiln - Hare"},
    {14, "Shinshu Field grass patch by entrance - Boar Piglet"},
    {37, "Shinshu Field Agata Forest Port - Boar"},
    {38, "Shinshu Field dojo - Pig"},
    {39, "Shinshu Field east path - Hare"},
    {41, "Shinshu Field near dojo - Horse"},
};

// logbookAvailable
const std::unordered_map<unsigned, const char *> tracker1Desc = {
    // 2 - Battle started after talking with merchant
    // 3 - Yomigami gives rejuvenation
    // 4 - Tachigami gives power slash
    // 6 - After gaining sunrise and entering battle
    // 7 - After picking up susano first time
    // 9 - Guardian Sapling Trouble journal added
    // 16 - spoke with susano after mural battle in Hana Valley
    // 17 - mural battle (new enemy) in Hana Valley
    // 18 - restored mural in Hana Valley
    // 19 - restored sapling and shown constellation Hana Valley
    // 20 - greensprout obtained
    // 24 - revived Shinshu sapling
    // 36 - river of heavens stars twinkling sets this
    // 37 - boulder slash minigame failed
    // 41 - Mrs Orange gave Cherry Cake for quest
    // 42 - cave of nagi constellation sets this
    // 45 - boulder removed
    // 53 - Hana Valley bear defeated
};

// unknown
const std::unordered_map<unsigned, const char *> tracker2Desc = {
    {16, "First hare fed"}, {19, "First sparrow fed"}, {20, "First monkey fed"}, {21, "First boar piglet fed"},
    {22, "First boar fed"}, {23, "First dog fed"},     {34, "First pig fed"},    {38, "First horse fed"},
};

struct MapDesc
{
    std::unordered_map<unsigned, const char *> worldStateBits;
    std::unordered_map<unsigned, const char *> userIndices;
    std::unordered_map<unsigned, const char *> buriedObjects;
    std::unordered_map<unsigned, const char *> collectedObjects;
    std::unordered_map<unsigned, const char *> field_98;
    std::unordered_map<unsigned, const char *> areasRestored;
    std::unordered_map<unsigned, const char *> treesBloomed;
    std::unordered_map<unsigned, const char *> field_B0;
    std::unordered_map<unsigned, const char *> hellGatesCleared;
    std::unordered_map<unsigned, const char *> npcHasMoreToSay;
    std::unordered_map<unsigned, const char *> npcUnknown;
    std::unordered_map<unsigned, const char *> field_D4;
    std::unordered_map<unsigned, const char *> field_DC;
    std::unordered_map<unsigned, const char *> field_E0;
    std::unordered_map<unsigned, const char *> dialogBits;
};

const std::unordered_map<unsigned, MapDesc>
    mapDataDesc =
        {
            {0,
             {
                 .worldStateBits =
                     {
                         // 3, 4 - set to 1 when doing a constellation
                         {10, "Prevent use of celestial brush"},
                         // 7 - battle paused, or tutorial about vulnerable enemies?
                         // 8 - in battle? tutorial battle?
                         // 9 - carrying susano
                         {41, "Shinshu Field restored"},
                         {84, "First dojo cutscene trigger"},
                     },
             }},
            {MapTypes::KamikiVillageGameStart,
             {
                 .worldStateBits =
                     {
                         {16, "Ran into the forcefield towards Kamiki (instant)"},
                         // 15 and 17 get set when Issun starts conversation, 15 16 and 17 all get cleared after the convo
                         {18, "Identified invisible forcefield towards Kamiki"},
                         // 43, 44 - peach was cut down
                     },
                 .dialogBits =
                     {
                         {2, "Ammy awakened by Sakuya"},
                         {7, "Informed about the impenetrable barrier"},
                         {8, "Second dialogue after hitting barrier"},
                         {11, "Examined the fruit"},
                     },
             }},
            {MapTypes::CaveOfNagi,
             {
                 .worldStateBits =
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
                 .collectedObjects =
                     {
                         {0, "Chest on the other side of the river (Stray Bead)"},
                     },
                 .dialogBits =
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
             }},
            {MapTypes::KamikiVillage,
             {
                 .worldStateBits =
                     {
                         {2, "Farm digging minigame is active"},
                         {3, "Laundry pole restored"},
                         {4, "Farm digging minigame was triggered at least once"},
                         {5, "Completed digging minigame (Oddly Shaped Turnip spawns)"},
                         {6, "Turned in Oddly Shaped Turnip"},
                         {7, "Entered mill after repair"},
                         {8, "Turned in Oddly Shaped Turnip"},
                         {11, "Returned to village after getting bloom convo"},
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
                 .userIndices =
                     {
                         // 0, 1, 2 - bit flags
                         /*
                 {1, "Entered Susano's Bunker first time"},
                 {2, "Interacted with 3 statues"},
                 {3, "Quest marker to get to lookout"},
                 {4, "Marchant spoken to first time"},
                 {5, "Sake gets spawned in mill"},*/
                         // 94 - Set post merchant battle
                         // 95 - set after talking with 3 statues, cleared post merchant battle
                         {4, "First cursed statue interacted with"},
                         {5, "Second cursed statue interacted with"},
                         {6, "Number of cursed statues interacted with"},
                         {7, "Digging minigame timer"},

                         // 31 - {1023, "Chest on the ledge opened"},
                     },
                 .buriedObjects =
                     {
                         {0, "Chest on hill dug up"},
                         {6, "Chest on farm dug up"},
                         {8, "Chest in Mr Orange's house dug up"},
                         {37, "Buried Chest on island dug up"},
                     },
                 .collectedObjects =
                     {
                         {0, "Chest on hill opened (Traveler's Charm)"},
                         {6, "Chest on farm opened (Dragonfly Bead)"},
                         {7, "Chest in river by bridge (further) opened (Rabbit Statue)"},
                         {8, "Chest in Mr Orange's house opened (Coral Fragment)"},
                         {9, "Mill Bale of Rice 1"},
                         {10, "Mill Bale of Rice 2"},
                         {11, "Mill Bale of Rice 3"},
                         {12, "Mill Rice Balls 1"},
                         {13, "Mill Rice Balls 2"},
                         {14, "Mill Rice Balls 3"},
                         {15, "Mr Orange's House Orange 1"},
                         {16, "Mr Orange's House Orange 2"},
                         {17, "Mr Orange's House Orange 3"},
                         {18, "Mr Orange's House Orange 4"},
                         {19, "Mr Orange's House Orange 5"},
                         {20, "Chest in water by waterfall opened (Vase)"},
                         {21, "Chest in river by bridge (closer) opened (Glass Beads)"},
                         {22, "Kushi's House Bale of Rice 1"},
                         {23, "Kushi's House Bale of Rice 1"},
                         {24, "Kushi's House Bale of Rice 1"},
                         {29, "Solar orb in the pond 1"},
                         {30, "Solar orb in the pond 2"},
                         {31, "Solar orb in the pond 3"},
                         {32, "Solar orb in the pond 4"},
                         {34, "Chest behind rock on island opened (Dragonfly Bead)"},
                         {37, "Buried chest on island opened (Wooden Bear)"},
                         {38, "Chest by pond opened (Feedbag (Seeds))"},
                         {80, "Vista of the Gods given to Susano"}, // does this belong here?
                     },
                 .areasRestored =
                     {
                         {16, "Pond by lookout restored"},
                         {17, "Merchant battle/Entrance area restored"},
                     },
                 .npcHasMoreToSay =
                     {
                         {0, "Mr Orange"},
                         {1, "Mrs Orange"},
                         {2, "Susano"},
                         {3, "Kushi"},
                         {4, "Mushi"},
                     },
                 .dialogBits =
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
             }},
            {MapTypes::HanaValley,
             {
                 .worldStateBits =
                     {
                         {1, "Hit campfire group trigger"},
                         {2, "Campfire group cleared for good"},
                         {3, "Campfire group dancing"},
                         {4, "In campfire group battle"},
                         {5, "Triggered cutscene for cursed tree"},
                         {6, "Triggered cutscene for stunned cursed tree"},
                         {7, "Triggered mural battle"},
                         {8, "Mural battle started"},
                         {9, "Mural battle cleared"},
                         {10, "Mural battle cleared"},
                         {11, "Mural battle cleared"},
                         {12, "Currently talking with Susano"},
                         {13, "Triggered panning to mural"},
                         {15, "Spawned sun on mural"},
                         {16, "Spawned sun on mural"},
                         {17, "Susano ran off"},
                         {18, "Triggered sapling room cutscene (first time)"},
                         {19, "Triggered Susano slash minigame cutscene"},
                         {21, "Freed crystal"},
                         {23, "Crystal placed into position"},
                         {25, "Crystal placed into position"},
                         {26, "Triggered convo after crystal placed"},
                         {27, "Getting mural explanation again"},
                         {30, "Sun spawned for sapling"},
                         {31, "Constellation completed"},
                         {35, "Sapling bloomed"},
                         {36, "Constellation completed"},
                         {37, "Sapling bloomed"},
                         {38, "Cursed grass trigger"},
                         {39, "Finished cursed grass intro"},
                         {40, "Restoring cursed grass"},
                         {42, "Restored campfire group area for good"},
                         {43, "Restored campfire group area (cursed)"},
                         {44, "Mural battle cleared for good"},
                         {45, "Restoring cursed grass"},
                         {47, "Post-campfire battle dialogue triggered"},
                         {48, "Triggering sun cutscene without crystal"},
                         {51, "Set during distrub bear's sleep convo"},
                         {53, "Issun constellation explanation if messing around"},
                         {55, "Explanation for bloom"},
                         {57, "Set while Issun explains cursed grass"},
                         {58, "Being given Greensprout Tips scroll"},
                         {60, "cursed tree related"},
                         {61, "Examined the mural"},
                         {62, "Examined sapling"},
                         {65, "Examined the mural"},
                         {66, "Spawned sun on mural"},
                         {68, "Examined sapling"},
                         {71, "Crystal is on fountain"},
                         {75, "Campfire lit/group active"},
                         {76, "Cursed grass restored"},
                         {77, "All trees restored (reward chest)"},
                         // 78 - set when cleaning mural, unset after susano dialog
                         {80, "Constellation pulse"},
                         {82, "Hana Valley restoration process"},
                         {83, "cursed tree related"},
                         {85, "Interacting with constellation"},
                     },
                 .buriedObjects =
                     {
                         {0, "Clover on cliff"},
                         {1, "Buried chest just after campfire"},
                         {3, "Clover by entrance"},
                         {4, "Island clover"},
                     },
                 .collectedObjects =
                     {
                         {0, "Clover on cliff bloomed"},
                         {1, "Buried chest just after campfire (Stray Bead)"},
                         {3, "Clover by entrance"},
                         {4, "Island clover"},
                         {5, "Chest near entrance (Traveler's Charm)"},
                         {6, "Island chest ([Digging Tips])"},
                         {80, "Bloom reward chest (Sun Fragment)"},
                     },
                 .areasRestored =
                     {
                         {16, "Campfire area"},
                         {18, "Mural room"},
                         {20, "Cursed grass"},
                         {28, "Sapling room"},
                     },
                 .treesBloomed =
                     {
                         {0, "First tree by entrance"},
                         {1, "Second tree by entrance"},
                         {2, "First tree from campfire"},
                         {3, "Second tree from campfire"},
                         {4, "Dead tree by bridge"},
                         {5, "Campfire right side 1"}, // facing from entrance
                         {6, "Island tree"},
                         {7, "Campfire right side 2"},
                         {8, "Campfire right side 3"},
                         {9, "Campfire left side 1"},
                         {10, "Campfire left side 2"},
                         {11, "Campfire left side 3"},
                     },
                 .field_B0 =
                     {
                         {0, "Cursed tree closest to mural"},
                         {1, "Cursed tree closest to campfire"},
                         {2, "Cursed tree by waterfall"},
                         {3, "Cursed tree by bridge"},
                     },
             }},
            {MapTypes::TsutaRuins, {}},
            {MapTypes::CityCheckpoint, {}},
            {MapTypes::TsutaRuinsSpiderQueenArena, {}},
            {MapTypes::GaleShrine, {}},
            {MapTypes::KusaVillage, {}},
            {MapTypes::SasaSanctuary, {}},
            {MapTypes::AgataForestMadameFawnsHouse, {}},
            {MapTypes::DiggingMinigame, {}},
            {MapTypes::OnigiriDojoLessonRoom, {}},
            {MapTypes::GaleShrineCrimsonHelmArena, {}},
            {MapTypes::CalcifiedCavern, {}},
            {MapTypes::MoonCaveInterior, {}},
            {MapTypes::MoonCaveStaircaseandOrochiArena, {}},
            {MapTypes::RyoshimaCoastBanditSpiderArena, {}},
            {MapTypes::TakaPassStrayBead26Cave, {}},
            {MapTypes::KamuiCherryBomb3Cave, {}},
            {MapTypes::NRyoshimaCoastPowerSlash2Cave, {}},
            {MapTypes::NRyoshimaCoastCherryBomb2Cave, {}},
            {MapTypes::KamuiEzofujiPowerSlash3Cave, {}},
            {MapTypes::KamuiBlockheadGrandeCave, {}},
            {MapTypes::NRyoshimaCoastBanditSpiderArena, {}},
            {MapTypes::KamuiBanditSpiderArena, {}},
            {MapTypes::DragonPalaceStrayBead70Cave, {}},
            {MapTypes::NRyoshimaCoastStrayBead63Cave, {}},
            {MapTypes::NewGameIntroCutsceneMap,
             {
                 .worldStateBits =
                     {
                         {2, "Transitioned from backstory dialogue to level"},
                         {3, "Orochi finished speaking to Susano"},
                     },
                 .dialogBits =
                     {
                         {2, "Backstory dialogue completed"},
                     },
             }},
            {MapTypes::RiverOftheHeavens,
             {
                 .worldStateBits =
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
                 .collectedObjects =
                     {
                         {0, "First chest (Holy Bone S)"},
                         {1, "Second chest (Astral Pouch)"},
                     },
                 .dialogBits =
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
             }},
            {MapTypes::SeianCityAristocraticQtr, {}},
            {MapTypes::SeianCityCommonersQtr, {}},
            {MapTypes::SeianCityAristocraticQtrHimikosPalace, {}},
            {MapTypes::DragonPalace, {}},
            {MapTypes::InsidetheDragon, {}},
            {MapTypes::SunkenShip, {}},
            {MapTypes::ImperialPalaceAmmySize, {}},
            {MapTypes::ImperialPalaceIssunSize, {}},
            {MapTypes::OniIslandLowerInterior, {}},
            {MapTypes::OniIslandNinetailsArena, {}},
            {MapTypes::CatcallTower, {}},
            {MapTypes::ImperialPalaceBlightArena, {}},
            {MapTypes::SeianCityCommonersQtrKimonoShop, {}},
            {MapTypes::OniIslandExterior, {}},
            {MapTypes::OniIslandUpperInterior, {}},
            {MapTypes::OniIslandSidescroller, {}},
            {MapTypes::Wepkeer, {}},
            {MapTypes::KamikiVillage100yearsago, {}},
            {MapTypes::WawkuShrine, {}},
            {MapTypes::WawkuShrineNechkuArena, {}},
            {MapTypes::Ponctan, {}},
            {MapTypes::MoonCave100yearsago, {}},
            {MapTypes::ArkOfYamato, {}},
            {MapTypes::ArkOfYamatoSpiderQueenArena, {}},
            {MapTypes::ArkOfYamatoOrochiArena, {}},
            {MapTypes::ArkOfYamatoBlightArena, {}},
            {MapTypes::ArkOfYamatoNinetailsArena, {}},
            {MapTypes::ArkOfYamatoCrimsonHelmArena, {}},
            {MapTypes::PonctanMrsSealsHouse, {}},
            {MapTypes::Yoshpet, {}},
            {MapTypes::InnerYoshpet, {}},
            {MapTypes::ArkOfYamatoYamiArena, {}},
            {MapTypes::WepkeerSquare, {}},
            {MapTypes::WawkuShrineLechkuAndNechkuArena, {}},
            {MapTypes::FishingwithBenkeionSeianBridge, {}},
            {MapTypes::FishingwithBenkeinexttoHimikosPalace, {}},
            {MapTypes::FishingwithKokariinAgata, {}},
            {MapTypes::FishingwithKokariinKamui, {}},
            {MapTypes::Unknown70, {}},
            {MapTypes::FishingwithFishermaninNRyo, {}},
            {MapTypes::ShinshuField,
             {
                 .worldStateBits =
                     {
                         {2, "Entered Shinshu field first time"},
                         {3, "Entered cursed zone for first time"},
                         {4, "Returned to Shinshu with Bloom"},
                         {5, "Sapling bloomed"},
                         {6, "Dialog trigger for distant guardian sapling"},
                         {8, "Lifted curse zone"},
                         {9, "Lifted curse zone"},
                         {13, "Cursed grass by Agata Port restored"},
                         {14, "Cursed grass by entrance restored"},
                         {15, "Nameless Man's Kiln restored"},
                         {21, "Issun stops you from going back to Kamiki"},
                         {22, "You choose to leave Shinshu"},
                         {25, "Inspected Agata Forest Port sign"},
                         {29, "Gate tutorial trigger"},
                         {30, "First gate cleared, finish tutorial trigger"},
                         {46, "Dojo gate cleared"},
                         {52, "Nameless Man's Kiln restored"},
                         {66, "Ida ran off"},
                         {93, "Spoke to Ida first time"},
                         {94, "Ida ran off"},
                         {97, "Spoke to Nameless Man intro (cursed) 1"},
                         {98, "Spoke to Nameless Man intro (cursed) 2"},
                         {99, "Spoke to Nameless Man post-bloom (cursed)"},
                         {101, "Spoke to Nameless Man at night 1"},
                         {102, "Spoke to Nameless Man at night 2"},
                         {103, "Nameless Man finished giving rice (resets next day)"},
                         {104, "Spoke to Nameless Man post-kiln 1"},
                         {138, "Spoke to Merchant"},
                     },
                 .userIndices =
                     {
                         {0, "Number of times Nameless Man gave you rice"},
                     },
                 .buriedObjects =
                     {
                         {0, "Buried chest between 3 bushes behind merchant"},
                         {7, "Chest buried between 3 bushes near sapling"},
                         {8, "Chest buried between bushes behind kiln"},
                         {11, "Clover behind merchant"},
                     },
                 .collectedObjects =
                     {
                         {0, "Buried chest between 3 bushes behind merchant (Wooden Bear)"},
                         {1, "Chest behind tree (Traveler's Charm)"},
                         {6, "Chest near entrance from cursed gate (Vengeance Slip)"},
                         {7, "Chest buried between 3 bushes near sapling (Stray Bead)"},
                         {8, "Chest buried between bushes behind kiln (Stray Bead)"},
                         {11, "Clover behind merchant"},
                         {19, "Tree chest ([Fleeing Battle])"},
                         {20, "Agata Forest port chest ([Feeding])"},
                     },
                 .areasRestored =
                     {
                         {16, "Gate area by tree"},
                         {17, "Nameless Man's Kiln"},
                         {18, "Cursed gate by entrance"},
                         {19, "Dojo"},
                         {22, "Cursed grass by entrance"},
                         {23, "Cursed grass by Agata Port"},
                         {31, "Main area restored"},
                     },
                 .treesBloomed =
                     {
                         {1, "Left side from entrance tree 1"},
                         {2, "Left side from entrance tree 2"},
                         {3, "Right side from entrance tree 1"},
                         {4, "Right side from entrance tree 2"},
                         {5, "Right side from entrance tree 3"},
                         {10, "Left side from entrance tree 3"},
                     },
                 .hellGatesCleared =
                     {
                         {0, "Gate by tree cleared"},
                         {1, "Gate by Nameless Man"},
                         {2, "Cursed gate by entrance cleared"},
                         {3, "Dojo gate cleared"},
                     },
                 .npcHasMoreToSay =
                     {
                         {1, "Nameless Man"},
                     },
                 .dialogBits =
                     {
                         {188, "First entrance conversation"},
                     },
             }},
            {MapTypes::AgataForest, {}},
            {MapTypes::MoonCaveEntrance, {}},
            {MapTypes::TakaPass, {}},
            {MapTypes::RyoshimaCoast, {}},
            {MapTypes::Unknown76, {}},
            {MapTypes::NRyoshimaCoast, {}},
            {MapTypes::Kamui, {}},
            {MapTypes::KamuiEzofuji, {}},
            {MapTypes::ShinshuField100yearsago, {}},
            {MapTypes::MoonCaveEntrance100yearsago, {}},
            {MapTypes::Unk82, {}},
            {83, {}},
};

} // namespace okami