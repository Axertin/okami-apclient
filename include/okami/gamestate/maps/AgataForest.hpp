#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/AgataForest.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::AgataForest
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "Have Water Lily, Will Travel quest ended"},
                                                                   {2, "Sapling of Agata Forest quest started"},
                                                                   {3, "Restored sapling; Sapling of Agata Forest ended"},
                                                                   {4, "Triggered hut dialogue"},
                                                                   {8, "Crack to sapling blown up"},
                                                                   {10, "Area restored"},
                                                                   {11, "Divine instrument tutorial triggered"},
                                                                   {12, "Waka cutscene triggered"},
                                                                   {14, "Interacted with giant bud"},
                                                                   {15, "Dungeon entrance cutscene triggered"},
                                                                   {16, "Dungeon entrance opened"},
                                                                   {17, "Sleepy bear gave praise for giant seed"},
                                                                   {18, "Sleepy bear now standing on giant seed"},
                                                                   {19, "Sleepy bear gave praise for giant cabbage"},
                                                                   {20, "Sleepy bear now standing on giant cabbage"},
                                                                   {21, "Sleepy bear now standing on giant hive (completed)"},
                                                                   {22, "Cursed grass behind Hitoshio Spring rejuvenated"},
                                                                   {23, "Rejuvenated cursed grass by forest hut"},
                                                                   {24, "Kokari stole ruins key"},
                                                                   {26, "Spoke with Kokari while he's got ruins key"},
                                                                   {27, "Ume is rescued"},
                                                                   {28, "Log minigame started"},
                                                                   {29, "Log bridge placed; A Son's Determination ended"},
                                                                   {31, "Spoke with Kushi before filling barrel"},
                                                                   {32, "Started filling Kushi's sake barrel"},
                                                                   {33, "Kushi's Distress quest ended"},
                                                                   {34, "Canine Tracker appeared"},
                                                                   {35, "Ume's Lost Again ended; Ume the Meal started"},
                                                                   {39, "Triggered guide for Crescent tutorial (failed twice)"},
                                                                   {40, "Failed crescent tutorial"},
                                                                   {41, "Ume the Meal ended"},
                                                                   {42, "Ume reappeared"},
                                                                   {43, "Ume is able to be fought"},
                                                                   {44, "Defeated Ume"},
                                                                   {45, "Transitioned after defeating Ume"},
                                                                   {47, "Crack on dungeon side cliff blown up"},
                                                                   {48, "Spoke with Karude intro 1"},
                                                                   {49, "Spoke with Karude intro 2; Hard Working Son started"},
                                                                   {50, "Spoke with Karude post-rescue 1"},
                                                                   {51, "Spoke with Karude post-Taka Pass rejuvenation"},
                                                                   {52, "Spoke with Karude post-Kusa"},
                                                                   {55, "Spoke with Kokari first time"},
                                                                   {56, "Spoke with Kokari second time; The Lost Key started; Ume is Lost started"},
                                                                   {57, "Spoke with Kokari third time"},
                                                                   {58, "Spoke with Kokari at night"},
                                                                   {59, "Bashed Kokari while crying"},
                                                                   {63, "Ruins key spawns; The Lost Key ended"},
                                                                   {64, "Spoke with Kokari post-log minigame"},
                                                                   {65, "Spoke with Kokari post-Taka Pass rejuvenation"},
                                                                   {66, "End fishing without catching any fish"},
                                                                   {67, "Ume's Lost Again started"},
                                                                   {68, "Spoke with Kokari directly before Whopper fishing"},
                                                                   {71, "Spoke with Susano first time"},
                                                                   {72, "Kushi's Distress quest started"},
                                                                   {73, "Susano has no more food"},
                                                                   {75, "Spoke with merchant"},
                                                                   {77, "Spoke with Kiba"},
                                                                   {78, "Spoke with Kiba 2"},
                                                                   {81, "Spoke with Kushi 2nd time before filling up barrel"},
                                                                   {82, "Spoke with Kushi 3rd time before filling up barrel"},
                                                                   {83, "Spoke with Kushi while trying to lift barrel"},
                                                                   {86, "Interacted with sleepy bear first time"},
                                                                   {87, "Interacted with sapling"},
                                                                   {88, "Interacted with dungeon entrance first time"},
                                                                   {90, "Interacted with rapids first time"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {{0, "Bulb on giant rock island sapling side (Holy Bone S)"},
                                                                     {1, "Bulb on northwstern most island (Glass Beads)"},
                                                                     {2, "Bulb on giant rock island other side (Lacquerware Set)"},
                                                                     {3, "Bulb on island closest to waterfall (Steel Fist Sake)"},
                                                                     {4, "Bulb by forest hut closer (Wooden Bear)"},
                                                                     {5, "Bulb by forest hut further (Exorcism Slip S)"},
                                                                     {6, "Bulb by waterfall (Inkfinity Stone)"},
                                                                     {7, "Bulb by fortune cave entrance (Traveler's Charm)"},
                                                                     {8, "Bulb on middle island (Coral Fragment)"},
                                                                     {9, "Clover at Hitoshio Spring hard digging spot"},
                                                                     {10, "Bulb at grass behind Hitoshio Spring waterfall (Incense Burner)"},
                                                                     {11, "Sapling chest (Devout Beads)"},
                                                                     {12, "Buried chest on dungeon side cliff (Stray Bead)"},
                                                                     {13, "Cave fire chest left (Bull Horn)"},
                                                                     {14, "Cave fire chest middle (Stray Bead)"},
                                                                     {15, "Cave fire chest right (Holy Bone M)"},
                                                                     {16, "Chest at Hitoshio Spring vine (Stray Bead)"},
                                                                     {17, "Vine to tree above island by waterfall (Stray Bead)"},
                                                                     {18, "Vine to tree above rock island (Bull Horn)"},
                                                                     {19, "Chest above spout in cave (Lacquerware Set)"},
                                                                     {20, "Clover behind sapling"},
                                                                     {21, "Buried chest right of ramp outside fortune cave (Dragonfly Bead)"},
                                                                     {22, "Buried chest behind forest hut (Stray Bead)"},
                                                                     {23, "Buried chest on middle island (Steel Soul Sake)"},
                                                                     {24, "Iron rock by rapids destroyed"},
                                                                     {25, "Clover under iron rock by rapids"},
                                                                     {26, "Clover under diamond rock in cave"},
                                                                     {27, "Diamond rock in cave destroyed"},
                                                                     {28, "Clover in dungeon side cliff cave"},
                                                                     {29, "Buried chest under leaf pile on entrance ramp (Exorcism Slip M)"},
                                                                     {30, "Buried chest under leaves by upper Shinshu exit (Pearl)"},
                                                                     {31, "Buried chest under leaves by Taka Pass exit (Holy Bone M)"},
                                                                     {32, "Buried chest in hard ground in front of Tsuta Ruins (Bull Statue)"},
                                                                     {33, "Chest by Taka Pass exit ([Ink Bullet Tips])"},
                                                                     {35, "Chest by Kiba ([Battle Tips])"},
                                                                     {36, "Chest at dungeon entrance ([Enhancing Weapons])"}};

const std::unordered_map<unsigned, const char *> areasRestored = {{16, "Cursed grass behind Hitoshio Spring waterfall"},
                                                                  {17, "Cursed grass by forest hut"},
                                                                  {18, "Hell gate in front of dungeon"},
                                                                  {19, "Hell gate area near waterfall (releases nut)"},
                                                                  {31, "Main area"}};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {{0, "Gate in front of dungeon"}, {8, "Gate near waterfall"}, {11, "Canine Warrior Ume"}};

const std::unordered_map<unsigned, const char *> npcs = {{0, "Karude"}, {1, "Kokari"}, {4, "Kushi"}};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::AgataForest
