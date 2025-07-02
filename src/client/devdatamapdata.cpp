#include "devdatamapdata.h"

#include <unordered_map>

#include "okami/data/maptype.hpp"

namespace okami
{
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
                         // 19 - entered a cave map
                         {16, "Remaining Warriors quest ended; Orbs Reunited quest started"},
                         {22, "Taka Pass sapling bloomed"},

                         {31, "Festival quest ended; City Checkpoint quest started; To the Capital! quest started"},

                         {38, "Near a mermaid warp"},

                         // Saplings bloomed
                         {40, "Hana Valley sapling bloomed"},
                         {41, "Shinshu Field sapling bloomed"},
                         {42, "Agata Forest sapling bloomed"},
                         {44, "Ryoshima Coast sapling bloomed"},

                         // Mermaid springs
                         {46, "Shinshu Field mermaid spring unlocked"},
                         {47, "Agata Forest mermaid spring unlocked"},
                         {48, "Taka Pass mermaid spring unlocked"},
                         {49, "Sasa Sanctuary mermaid spring unlocked"},
                         {50, "Ryoshima Coast mermaid spring unlocked"},
                         {51, "N.Ryoshima Coast mermaid spring unlocked"},
                         {52, "Dragon Palace mermaid spring unlocked"},
                         {53, "Kamui mermaid spring unlocked"},

                         {63, "Shinshu Adventure quest ended"},

                         //{62, "Waka 1 defeated"}, // gets cleared after story time
                         // 65 - Agata fishing related, caught big fish?
                         {64, "Fishing in Agata Forest"},
                         {70, "Divine Springs tutorial (Power slash 2)"},
                         {71, "Donated to Divine Spring first time (Power slash 2)"},
                         {72, "Bandit Spider defeated"},
                         {84, "First dojo cutscene trigger"},
                         {97, "Fishing tutorial shown"},
                     },
             }},
            {MapTypes::KamikiVillageGameStart,
             {
                 .worldStateBits =
                     {
                         {13, "Cut Down the Peach quest started"},
                         {16, "Ran into the forcefield towards Kamiki (instant)"},
                         // 15 and 17 get set when Issun starts conversation, 15 16 and 17 all get cleared after the convo
                         {18, "Identified invisible forcefield towards Kamiki"},
                         {43, "Cut Down the Peach quest ended"},
                         {44, "Peach is cut"},
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
             }},
            {MapTypes::KamikiVillage,
             {
                 .worldStateBits =
                     {
                         {1, "In god zone by the tree"},
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
                         {17, "Finished battle with Merchant enemies, entrance area restored; Destroy the Boulder! quest started"},
                         {18, "Water wheel rejuvenation applied"},
                         {19, "Water wheel repaired"},
                         {20, "Trigger convo near Susano's house snoring"},
                         {21, "All trees bloomed"},
                         {23, "Went to lookout while cursed"},
                         {24, "Went to lookout after interacting with statues"},
                         {25, "Expecting to draw the sun"},
                         {27, "Issun calls out to Sakuya trigger (bloom-fest)"},
                         {29, "Interacted with Mr Orange at night"},
                         {30, "Interacted with Mr Orange at night 2"},
                         {31, "Interacted with Mr Orange at night 3"},
                         // 13, 26, 164 - drew the sun, village to be uncursed
                         {33, "Interacted with Mr Orange first time"},
                         {34, "Interacted with Mr Orange second time"},
                         {35, "Interacted with Mr Orange third time"},
                         {36, "Returned to Kamiki after leaving post-bloom"},
                         {38, "Spoke with Mr Orange at night (bloom-fest) 1"},
                         {39, "Spoke with Mr Orange at night (bloom-fest) 2"},
                         {40, "Spoke with Mr Orange at night (bloom-fest) 3"},
                         // 51, 52 - set after restoring laundry pole
                         // 53, 54 - set after drying the laundry
                         {57, "Interacted with Mrs Orange at night"},
                         {58, "Interacted with Mrs Orange first time"},
                         {59, "Interacted with Mrs Orange second time"},
                         {60, "Interacted with Mrs Orange post-laundry pole 1"},
                         {61, "Interacted with Mrs Orange at night post-quest"},
                         {62, "Spoke with Mrs Orange at night (bloom-fest)"},
                         {64, "Spoke with Mrs Orange post-bloom"},
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
                         {93, "Spoke with Kushi (bloom-fest) 1"},
                         {94, "Spoke with Kushi (bloom-fest) 2; Mr. Orange's Dance quest started"},
                         {96, "Spoke with Kushi post-bloom"},
                         {99, "Interacted with Mushi first time"},
                         {101, "Interacted with Mushi at night"},
                         {102, "Spoke with Mushi (bloom-fest)"},
                         {111, "Interacted with Mushi's Mama first time"},
                         {112, "Interacted with Mushi's Mama second time"},
                         {114, "Interacted with Mushi's Mama at night"},
                         {120, "Merchant post-battle state"},
                         {127, "Beat komuso enemies within 10s"},
                         {128, "Komuso tells how many trees remaining; Make the Trees Bloom started"},
                         {129, "Spoke with Komuso post-bloom"},
                         {134, "Interacted with Camille"},
                         {135, "Interacted with Camille 2"},
                         {136, "Spoke with Camille post-bloom"},
                         {149, "Spoke with Sakuya post-bloom"},
                         {153, "Iron training rock slashed"},
                         // 154 - slash minigame missed 1st dummy
                         {155, "Boulder removed; Destroy the Boulder! quest ended; Guardian Sapling Trouble started"},
                         {156, "Slash big boulder attempt made"},
                         {157, "Konohana shuffle succeeded; Make the Trees Bloom ended; Mr. Orange's Dance ended; Sacred Tree Reborn ended; Have Water Lily, "
                               "Will Travel started"},
                         {158, "Konohana shuffle attempt 2"},
                         {160, "Hasugami constellation completed"},
                         {161, "Left Kamiki after bloom"},
                         {162, "Hasugami constellation completed"},
                         {163, "Lily pad tutorial complete"},
                         {165, "First seed pack acquired"},
                         {172, "Fight With Hayabusa quest ended"},
                         {173, "Powerful Brew quest ended; Kushi's Decision quest started"},
                         {184, "Digging Record started"},
                         {185, "Digging Record ended; Fight With Hayabusa quest started"},
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
                     },
                 .collectedObjects =
                     {
                         {0, "Buried chest on hill (Traveler's Charm)"},
                         {1, "Buried chest on big lily pad island right (Inkfinity Stone)"},
                         {2, "Buried chest on big lily pad island left (Stray Bead)"},
                         {3, "Chest on the ledge behind Mrs Orange (Exorcism Slip S)"},
                         {4, "Chest on lookout scaffold upper (Glass Beads)"},
                         {5, "Chest on lookout scaffold lower (Stray Bead)"},
                         {6, "Chest on farm (Dragonfly Bead)"},
                         {7, "Chest in river by bridge (further) (Rabbit Statue)"},
                         {8, "Chest in Mr Orange's house (Coral Fragment)"},
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
                         {20, "Chest in water by waterfall (Vase)"},
                         {21, "Chest in river by bridge (closer) (Glass Beads)"},
                         {22, "Kushi's House Bale of Rice 1"},
                         {23, "Kushi's House Bale of Rice 1"},
                         {24, "Kushi's House Bale of Rice 1"},
                         {28, "Small lily pad island chest (Sun Fragment)"},
                         {29, "Solar orb in the pond 1"},
                         {30, "Solar orb in the pond 2"},
                         {31, "Solar orb in the pond 3"},
                         {32, "Solar orb in the pond 4"},
                         {33, "Clover on vine cliff"},
                         {34, "Chest behind rock on island (Dragonfly Bead)"},
                         {35, "Clover on big lily pad island"},
                         {36, "Chest on higher vine cliff (Stray Bead)"},
                         {37, "Buried chest on island (Wooden Bear)"},
                         {38, "Chest by pond (Feedbag (Seeds))"},
                         {80, "Vista of the Gods given to Susano"}, // does this belong here?
                         {81, "Sapling reward fruit cut down 1 (Bull Horn)"},
                         {83, "Sapling reward fruit cut down 3 (Golden Peach)"},
                     },
                 .areasRestored =
                     {
                         {16, "Pond by lookout"},
                         {17, "Merchant battle/Entrance area"},
                         {31, "Kamiki Village"},
                     },
                 .treesBloomed =
                     {
                         {0, "Lookout pond tree 3"},
                         {1, "Lookout pond tree 1"},
                         {2, "Lookout pond tree 4"},
                         {3, "Lookout pond tree 2"},
                         {4, "Ramp tree 1"},
                         {5, "Front of Kushi's"},
                         {6, "Behind Kushi's"},
                         {7, "Mr Orange's house"},
                         {8, "By Mrs Orange clothes line"},
                         {9, "Bridge entrance side"},
                         {10, "Ramp tree 2"},
                         {11, "Ramp tree 3"},
                         {12, "Ramp tree 4"},
                         {13, "Bridg village side"},
                         {14, "Susano's house"},
                     },
                 .npcs =
                     {
                         {0, "Mr Orange"},
                         {1, "Mrs Orange"},
                         {2, "Susano"},
                         {3, "Kushi"},
                         {4, "Mushi"},
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
                         {41, "Secret of Hana Valley quest ended, Revive the Guardian Saplings quest started"},
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
                         {78, "Sun can be drawn on mural"},
                         {80, "Constellation pulse"},
                         {82, "Hana Valley restoration process"},
                         {83, "cursed tree related"},
                         {85, "Interacting with constellation"},
                     },
                 .collectedObjects =
                     {
                         {0, "Clover on cliff"},
                         {1, "Buried chest just after campfire (Stray Bead)"},
                         {2, "Hidden buried chest by entrance on beach (Coral Fragment)"},
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
                 .cursedTreesBloomed =
                     {
                         {0, "Cursed tree closest to mural"},
                         {1, "Cursed tree closest to campfire"},
                         {2, "Cursed tree by waterfall"},
                         {3, "Cursed tree by bridge"},
                     },
             }},
            {MapTypes::TsutaRuins,
             {
                 .worldStateBits =
                     {
                         {1, "Ball in greenway room de-petrified"},
                         {2, "Triggered battle for enemy bud ogre"},
                         {3, "Ball in greenway room placed in slot"},
                         {4, "Pond room battle triggered"},
                         {5, "Pond room battle cleared"},
                         {6, "Pond room sunshine added"},
                         {7, "Issun trigger at entrance path 2F"},
                         {8, "Blockhead defeated"},
                         {9, "Blockhead defeated"},
                         {10, "Blockhead defeated"},
                         {11, "Left room 2F door unlocked"},
                         {12, "Bridge above entrance restored"},
                         {13, "Hell gates room defeated"},
                         {14, "Hell gates room sunshine added"},
                         {15, "Poison pots destroyed"},
                         {16, "Center room right side cursed grass restored"},
                         {17, "Center room back side cursed grass restored"},
                         {18, "Center room left side cursed grass restored"},
                         {19, "Center room right side cursed grass restored"},
                         {20, "Center room back side cursed grass restored"},
                         {21, "Center room left side cursed grass restored"},
                         {22, "Vine tutorial completed"},
                         {23, "Constellation completed"},
                         {25, "Entrance 4F lid lifted"},
                         {26, "Issun pre-boss room trigger"},
                         {30, "Constellation completed"},
                         {31, "Vine tutorial brush hint overlay"},
                         {32, "Vine used first time"},
                         {33, "Entrance room path 2F collapsing"},
                         {34, "Entered inner ruins"},
                         {35, "Blockhead introduced"},
                         {36, "Issun asks if you really want to leave"},
                         {48, "Issun commented on ruins restoration"},
                         {56, "Pond room 2F crack bombed"},
                         {57, "Pond room 2F hidden wall bombed"},
                         {64, "Bud ogre stunned tutorial"},
                         {67, "Bud ogre stunned tutorial"},
                         {80, "Intro triggered"},
                         {81, "View scene triggered in greenway room by waterfall"},
                         {82, "View scene triggered at entrance path 2F"},
                         {83, "View scene triggered post-vine at entrance 2F center"},
                         {84, "View scene triggered at hell gates room"},
                     },
                 .collectedObjects =
                     {
                         {0, "Greenway room bulb lower level (Incense Burner)"},
                         {1, "Blockhead room 2F bulb behind bomb wall (Stray Bead)"},
                         {2, "Pond room bulb (Vengeance Slip)"},
                         {3, "Clover pond room 2F ledge"},
                         {4, "Gloomy room bulb (Exorcism Slip S)"},
                         {5, "Clover entrance room 2F just outside origin mirror room"},
                         {6, "Clover entrance room middle"},
                         {7, "Clover entrance room left side island"},
                         {8, "Entrance bulb (Traveler's Charm)"},
                         {9, "Entrance room bulb right side (Steel Soul Sake)"},
                         {10, "Clover right side room on island"},
                         {11, "Clover above greenway room entrance"},
                         {12, "Greenway room bulb by stone ball (Steel Fist Sake)"},
                         {13, "Entrance room bulb left side (Exorcism Slip S)"},
                         {14, "Clover at greenway room's highest point"},
                         {15, "Bulb at greenway room's highest point (Stray Bead)"},
                         {16, "Clover main room right side ledge"},
                         {17, "Entrance chest ([Enhancing Divinity])"},
                         {18, "Blockhead room 2F bulb (Golden Peach)"},
                         {19, "Clover blockhead room 2F left"},
                         {20, "Clover blockhead room 2F right"},
                         {21, "Boss hallway chest left ([Godhood Tips])"},
                         {22, "Boss hallway chest right (Holy Bone S)"},
                         {23, "Hell gates room bulb left (Lacquerware Set)"},
                         {24, "Hell gates room bulb right (Holy Bone S)"},
                         {25, "Hell gates room bulb behind waterfall bombable wall (Stray Bead)"},
                         {26, "Entrance room chest right side (Vase)"},
                         {27, "Hell gates room chest (Tsuta Ruins Map)"},
                         {28, ""},
                     },

                 .areasRestored =
                     {
                         {16, "Hell gates room"},
                         {17, "Pond room restored"},
                         {18, "Center room right side cursed grass"},
                         {19, "Center room back side cursed grass"},
                         {20, "Center room left side cursed grass"},
                         {21, "Tsuta Ruins restored"},
                         {22, "Hell gates room mushrooms"},
                         {23, "Left room"},
                         {26, "Center room fully"},
                     },
                 .cursedTreesBloomed =
                     {
                         {0, "Cursed tree by entrance right side bloomed"},
                         {1, "First cursed tree in right side room"},
                         {2, "Second cursed tree in right side room"},
                         {3, "Cursed tree entrance room left side island bloomed"},
                     },
                 .fightsCleared =
                     {
                         {1, "Gloomy room slip"},
                     },
                 .mapsExplored =
                     {
                         {1, "Entrance"},
                         {2, "Right side room"},
                         {3, "Greenway room 1F"},
                         {4, "Pond room 1F"},
                         {5, "Hallway room 2F"},
                         {6, "Left side room"},
                         {7, "Gloomy room 2F"},
                         {8, "Hell gates room 2F"},
                         {9, "Center room"},
                         {10, "Deep mirror room 3F"},
                         {11, "Boss room 3F"},
                         {12, "Origin mirror room 2F"},
                     },
             }},
            {MapTypes::CityCheckpoint,
             {
                 .worldStateBits =
                     {
                         {1, "Bringing Down the Bridge ended"},
                         {42, "City Checkpoint quest ended"},
                         {43, "Bringing Down the Bridge quest started"},
                     },
             }},
            {MapTypes::TsutaRuinsSpiderQueenArena,
             {
                 .worldStateBits =
                     {
                         {1, "Post-defeat ask to save; Ume is Lost ended; A Son's Determination started"},
                         {2, "Fall area triggered"},
                         {3, "Spider Queen defeated"},
                         {4, "Golden gate tutorial trigger"},
                     },
                 .mapsExplored =
                     {
                         {1, "Entrance"},
                         {2, "Right side room"},
                         {3, "Greenway room 1F"},
                         {4, "Pond room 1F"},
                         {5, "Hallway room 2F"},
                         {6, "Left side room"},
                         {7, "Gloomy room 2F"},
                         {8, "Hell gates room 2F"},
                         {9, "Center room"},
                         {10, "Deep mirror room 3F"},
                         {11, "Boss room 3F"},
                         {12, "Origin mirror room 2F"},
                     },
             }},
            {MapTypes::GaleShrine, {}},
            {MapTypes::KusaVillage,
             {
                 .worldStateBits =
                     {
                         {1, "Mysterious Windmill quest ended; Canine Warrior Hunt quest started"},
                         {11, "Canine Warrior Hunt ended; Remaining Warriors started"},
                         {27, "Defeat Crimson Helm! ended; Powerful Brew started"},
                         {28, "Spoke to Gourd Farmer"},
                         {29, "Spoke to Gourd Farmer part 2; Animal Hating Couple quest started"},
                         {33, "Divine Wind's Cessation started"},
                         {41, "Spoke to Merchant"},
                         {78, "Orbs Reunited quest ended; Divine Wind's Cessation ended; Defeat Crimson Helm! started"},
                     },
                 .npcs =
                     {
                         {0, "Gourd Farmer"},
                     },
             }},
            {MapTypes::SasaSanctuary,
             {
                 .worldStateBits =
                     {
                         {1, "Entry dialogue trigger"},
                         {5, "Kaguya's Memories ended"},
                         {9, "Spring No More ended"},
                         {12, "Rejection quest started"},
                         {17, "Blocked Path ended"},
                         {35, "Blocked Path started"},
                         {61, "Spring No More started"},
                     },
             }},
            {MapTypes::AgataForestMadameFawnsHouse,
             {
                 .worldStateBits =
                     {
                         {1, "Spoke with Madame Fawn 1"},
                         {2, "Spoke with Madame Fawn 2"},
                         {3, "First fortune told"},
                         {5, "Came in from back door"},
                         {6, "Came in from back door"},
                     },
                 .collectedObjects =
                     {
                         {0, "Chest (Stray Bead)"},
                     },
             }},
            {MapTypes::DiggingMinigame,
             {
                 .worldStateBits =
                     {
                         {1, "Made it to end (show shining block)"},
                         {3, "Explanation shown"},
                     },
             }},
            {MapTypes::OnigiriDojoLessonRoom, {}},
            {MapTypes::GaleShrineCrimsonHelmArena, {}},
            {MapTypes::CalcifiedCavern, {}},
            {MapTypes::MoonCaveInterior,
             {
                 .worldStateBits =
                     {
                         {2, "Orochi's Appetizer quest started"},
                         {26, "Orochi's Appetizer quest ended"},
                     },
             }},
            {MapTypes::MoonCaveStaircaseandOrochiArena, {}},
            {MapTypes::RyoshimaCoastBanditSpiderArena,
             {
                 .worldStateBits =
                     {
                         {1, "Area restored"},
                         {2, "Spider defeated"},
                     },
                 .collectedObjects =
                     {
                         {0, "Area chest (Sun Fragment)"},
                     },
                 .areasRestored =
                     {
                         {16, "Area restored"},
                     },
             }},
            {MapTypes::TakaPassStrayBead26Cave,
             {

                 .collectedObjects =
                     {
                         {0, "Middle chest (Stray Bead)"},
                         {1, "Left chest (Godly Charm)"},
                         {2, "Right chest (Crystal)"},
                         {3, "Clover right"},
                         {4, "Clover left"},
                         {5, "Clover middle"},
                     },
                 .treesBloomed =
                     {
                         {0, "Left near"},
                         {1, "Right near"},
                         {2, "Left far"},
                         {3, "Right far"},
                     },
             }},
            {MapTypes::KamuiCherryBomb3Cave,
             {
                 .worldStateBits =
                     {
                         {1, "Interacted with pool"},
                         {2, "Obtained Cherry Bomb 3"},
                     },
                 .userIndices =
                     {
                         {0, "Money donated"},
                     },
                 .collectedObjects =
                     {
                         {0, "Post-donation chest ([Cherry Bomb 3])"},
                         {1, "Clover middle"},
                         {2, "Clover right"},
                         {3, "Clover left"},
                     },
                 .treesBloomed =
                     {
                         {0, "Left near"},
                         {1, "Right near"},
                         {2, "Left far"},
                         {3, "Right far"},
                     },
             }},
            {MapTypes::NRyoshimaCoastPowerSlash2Cave,
             {
                 .worldStateBits =
                     {
                         {1, "Interacted with pool"},
                         {2, "Obtained Power Slash 2"},
                     },
                 .userIndices =
                     {
                         {0, "Money donated"},
                     },
                 .collectedObjects =
                     {
                         {0, "Post-donation chest ([Power Slash 2])"},
                         {1, "Clover middle"},
                         {2, "Clover right"},
                         {3, "Clover left"},
                     },
                 .treesBloomed =
                     {
                         {0, "Left near"},
                         {1, "Right near"},
                         {2, "Left far"},
                         {3, "Right far"},
                     },
             }},
            {MapTypes::NRyoshimaCoastCherryBomb2Cave,
             {
                 .worldStateBits =
                     {
                         {1, "Interacted with pool"},
                         {2, "Obtained Cherry Bomb 2"},
                     },
                 .userIndices =
                     {
                         {0, "Money donated"},
                     },
                 .collectedObjects =
                     {
                         {0, "Post-donation chest ([Cherry Bomb 2])"},
                         {1, "Clover middle"},
                         {2, "Clover right"},
                         {3, "Clover left"},
                     },
                 .treesBloomed =
                     {
                         {0, "Left near"},
                         {1, "Right near"},
                         {2, "Left far"},
                         {3, "Right far"},
                     },
             }},
            {MapTypes::KamuiEzofujiPowerSlash3Cave,
             {
                 .worldStateBits =
                     {
                         {1, "Interacted with pool"},
                         {2, "Obtained Power Slash 3"},
                     },
                 .userIndices =
                     {
                         {0, "Money donated"},
                     },
                 .collectedObjects =
                     {
                         {0, "Post-donation chest ([Power Slash 3])"},
                         {1, "Clover middle"},
                         {2, "Clover right"},
                         {3, "Clover left"},
                     },
                 .treesBloomed =
                     {
                         {0, "Left near"},
                         {1, "Right near"},
                         {2, "Left far"},
                         {3, "Right far"},
                     },
             }},
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
             }},
            {MapTypes::SeianCityAristocraticQtr,
             {
                 .worldStateBits =
                     {
                         {7, "Fishing for Trouble ended; Living Sword started"},
                         {9, "Living Sword ended"},
                         {30, "Watery Wants quest started"},
                         {31, "Watery Wants quest ended"},
                         {33, "Fishing for Trouble started"},
                         {40, "Wanted: Prayer Slips quest started"},
                         {43, "Wanted: Prayer Slips quest ended; Sunken Treasure quest started; Fox Rods started"},
                         {102, "Tight Security quest started"},
                     },
             }},
            {MapTypes::SeianCityCommonersQtr,
             {
                 .worldStateBits =
                     {
                         {17, "Carpenter's Project ended"},
                         {26, "Kaguya's Memories started"},
                         {37, "A Tiny Problem started"},
                         {69, "To the Capital! quest ended; Capital Under Misty Siege started"},
                         {70, "Capital Under Misty Siege ended; Himiko, Queen of Nippon started"},
                         {113, "Carpenter's Project started"},
                     },
             }},
            {MapTypes::SeianCityAristocraticQtrHimikosPalace,
             {
                 .worldStateBits =
                     {
                         {2, "Tight Security ended"},
                         {4, "Fox Rods 2 quest ended"},
                         {6, "Himiko, Queen of Nippon ended; Land of the Dragonians started; Himiko's Motive started"},
                         {7, "Himiko's Motive ended; Oni Island's Whereabouts started"},
                     },
             }},
            {MapTypes::DragonPalace,
             {
                 .worldStateBits =
                     {
                         {2, "The Amazing Orca ended"},
                         {97, "Land of the Dragonians ended; Into the Dragon started"},
                         {154, "Wada's Request ended; Bad Feeling Confirmed started"},
                     },
             }},
            {MapTypes::InsidetheDragon,
             {
                 .worldStateBits =
                     {
                         {12, "Fox Rods 2 quest started"},
                         {33, "Into the Dragon ended; Wada's Request started"},
                     },
             }},
            {MapTypes::SunkenShip,
             {
                 .worldStateBits =
                     {
                         {1, "Fox Rods quest ended"},
                         {3, "Sunken Treasure quest ended"},
                     },
             }},
            {MapTypes::ImperialPalaceAmmySize,
             {
                 .worldStateBits =
                     {
                         {4, "Kaguya's Lament ended"},
                         {9, "A Tiny Problem ended"},
                         {13, "A Suspicious Emperor started"},
                     },
             }},
            {MapTypes::ImperialPalaceIssunSize,
             {
                 .worldStateBits =
                     {
                         {19, "Kaguya's Lament quest started"},
                     },
             }},
            {MapTypes::OniIslandLowerInterior, {}},
            {MapTypes::OniIslandNinetailsArena,
             {
                 .worldStateBits =
                     {
                         {1, "Lone Wolf vs Evil ended"},
                     },
             }},
            {MapTypes::CatcallTower,
             {
                 .worldStateBits =
                     {
                         {12, "Catcall Tower quest ended; The Power of Catwalk started"},
                     },
             }},
            {MapTypes::ImperialPalaceBlightArena,
             {
                 .worldStateBits =
                     {
                         {2, "A Suspicious Emperor quest ended"},
                     },
             }},
            {MapTypes::SeianCityCommonersQtrKimonoShop, {}},
            {MapTypes::OniIslandExterior, {}},
            {MapTypes::OniIslandUpperInterior, {}},
            {MapTypes::OniIslandSidescroller, {}},
            {MapTypes::Wepkeer,
             {
                 .worldStateBits =
                     {
                         {3, "Cold Comfort ended; Icy Wep'keer started"},
                         {17, "Icy Wep'keer ended; Lika is Missing! started"},
                         {18, "Kai as Guide started"},
                         {25, "Lika is Missing! ended; Forest of Yoshpet started"},
                     },
             }},
            {MapTypes::KamikiVillage100yearsago,
             {
                 .worldStateBits =
                     {
                         {2, "Spirit Gate Ho! ended; Beyond the Gate started"},
                         {7, "Beyond the Gate ended; Kamiki a Century Ago started"},
                         {8, "Kamiki a Century Ago ended; To the Moon Cave! started"},
                         {1, "To the Moon Cave! ended"},
                     },
             }},
            {MapTypes::WawkuShrine, {}},
            {MapTypes::WawkuShrineNechkuArena, {}},
            {MapTypes::Ponctan,
             {
                 .worldStateBits =
                     {
                         {7, "Gate of Misfortune ended; Open Wide the Spirit Gate started"},
                         {15, "Poncle Village Ponc'tan ended; Gate of Misfortune started"},
                     },
             }},
            {MapTypes::MoonCave100yearsago,
             {
                 .worldStateBits =
                     {
                         {1, "Enter Ishaku started"},
                     },
             }},
            {MapTypes::ArkOfYamato, {}},
            {MapTypes::ArkOfYamatoSpiderQueenArena, {}},
            {MapTypes::ArkOfYamatoOrochiArena, {}},
            {MapTypes::ArkOfYamatoBlightArena, {}},
            {MapTypes::ArkOfYamatoNinetailsArena, {}},
            {MapTypes::ArkOfYamatoCrimsonHelmArena, {}},
            {MapTypes::PonctanMrsSealsHouse, {}},
            {MapTypes::Yoshpet,
             {
                 .worldStateBits =
                     {
                         {7, "Kai as Guide ended; Kai Goes Deep started"},
                         {17, "Kai Goes Deep ended; Poncle Village Ponc'tan started"},
                         {32, "Open Wide the Spirit Gate ended; Spirit Gate Ho! started"},
                     },
             }},
            {MapTypes::InnerYoshpet,
             {
                 .worldStateBits =
                     {
                         {2, "Enter Ishaku ended; Forest of Yoshpet ended; Take Lika to Ezofuji started"},
                     },
             }},
            {MapTypes::ArkOfYamatoYamiArena, {}},
            {MapTypes::WepkeerSquare, {}},
            {MapTypes::WawkuShrineLechkuAndNechkuArena, {}},
            {MapTypes::FishingwithBenkeionSeianBridge, {}},
            {MapTypes::FishingwithBenkeinexttoHimikosPalace, {}},
            {MapTypes::FishingwithKokariInAgata,
             {
                 .worldStateBits =
                     {
                         {2, "Set on first entry"},
                         {4, "First ever bite tutorial triggered"},
                     },
             }},
            {MapTypes::FishingwithKokariInKamui, {}},
            {MapTypes::Unknown70, {}},
            {MapTypes::FishingwithFishermaninNRyo, {}},
            {MapTypes::ShinshuField,
             {
                 .worldStateBits =
                     {
                         {2, "Entered Shinshu field first time"},
                         {3, "Entered cursed zone for first time"},
                         {4, "Returned to Shinshu with Bloom"},
                         {5, "Sapling bloomed; Revive the Guardian Saplings ended; Guardian Sapling Trouble ended; Shinshu Adventure started; Sacred Tree "
                             "Reborn started"},
                         {6, "Dialog trigger for distant guardian sapling"},
                         {8, "Lifted curse zone"},
                         {9, "Lifted curse zone"},
                         {11, "Spoke to Mika second time (bountry trigger enabled)"},
                         {12, "Encountered bounty tutorial"},
                         {13, "Cursed grass by Agata Port restored"},
                         {14, "Cursed grass by entrance restored"},
                         {15, "Nameless Man's Kiln restored"},
                         {18, "Crack by cat statue blown up"},
                         {19, "Crack by Agata Forest Port blown up"},
                         {20, "Crack to Agata Forest blown up"},
                         {21, "Issun stops you from going back to Kamiki"},
                         {22, "You choose to leave Shinshu"},
                         {23, "Read notice to return to Tama's at night"},
                         {24, "Heavy Sleeper quest started"},
                         {25, "Inspected Agata Forest Port sign"},
                         {26, "Encountered bounty tutorial"},
                         {28, "Trigger for first fish demon fight"},
                         {29, "Gate tutorial trigger"},
                         {30, "First gate cleared, finish tutorial trigger"},
                         {31, "Tama failed first time"},
                         {32, "Tama failed second time"},
                         {33, "Finished cherry bomb crack tutorial; Heavy Sleeper ended"},
                         {35, "Triggered constellation"},
                         {36, "Completed constellation"},
                         {39, "Cherry bomb crack tutorial started"},
                         {40, "Crack by Tama's house blown up"},
                         {42, "Issun re-explaining we should help Tama out"},
                         {44, "Brush assist for cherry bomb crack tutorial triggered"},
                         {45, "Crack by Tama's house blown up"},
                         {46, "Dojo gate cleared"},
                         {50, "Interacted with crack in the wall"},
                         {51, "Crack by Tama's house blown up"},
                         {52, "Nameless Man's Kiln restored"},
                         {66, "Ida ran off"},
                         {86, "Kushi's Decision quest ended; Kushi's Feelings started"},
                         {87, "Kushi's Feelings ended; Festival quest started"},
                         {90, "Monument cutscene in catwalk field"},
                         {92, "Encountered bounty tutorial"},
                         {93, "Spoke to Ida first time"},
                         {94, "Ida ran off"},
                         {97, "Spoke to Nameless Man intro (cursed) 1"},
                         {98, "Spoke to Nameless Man intro (cursed) 2; Secret of Hana Valley quest started"},
                         {99, "Spoke to Nameless Man post-bloom (cursed)"},
                         {101, "Spoke to Nameless Man at night 1"},
                         {102, "Spoke to Nameless Man at night 2"},
                         {103, "Nameless Man finished giving rice (resets next day)"},
                         {104, "Spoke to Nameless Man post-kiln 1"},
                         {107, "Spoke to Mika intro"},
                         {108, "Spoke to Mika second time"},
                         {109, "Cleared bounty tutorial"},
                         {121, "Talked with Tama 1"},
                         {122, "Talked with Tama 2"},
                         {123, "Finished talking with Tama"},
                         {124, "Spoke to Tama post-bomb"},
                         {133, "Spoke to Onigiri Sensei first time"},
                         {135, "Dojo opened"},
                         {138, "Spoke to Merchant"},
                     },
                 .userIndices =
                     {
                         {0, "Number of times Nameless Man gave you rice"},
                     },
                 .collectedObjects =
                     {
                         {0, "Buried chest between 3 bushes behind merchant (Wooden Bear)"},
                         {1, "Chest behind sapling (Traveler's Charm)"},
                         {2, "Buried chest by dock near Tama's (Vase)"},
                         {3, "Buried chest right side of ramp to moon cave (Stray Bead)"},
                         {4, "Chest hidden by crack near Agata Forest Port (Coral Fragment)"},
                         {5, "Buried chest behind Dojo between bushes (Stray Bead)"},
                         {6, "Chest near entrance from cursed gate (Vengeance Slip)"},
                         {7, "Chest buried between 3 bushes near sapling (Stray Bead)"},
                         {8, "Chest buried between bushes behind kiln (Stray Bead)"},
                         {9, "Chest behind crack at Tama's (Rat Statue)"},
                         {10, "Chest behind crack at cat statue (Exorcism Slip S)"},
                         {11, "Clover behind merchant"},
                         {12, "Clover east side off cliff from path"},
                         {13, "Buried chest under leaves at Yama's house (Bull Horn)"},
                         {14, "Clover in hard ground near entrance"},
                         {16, "Chest top of moon shrine arch right side (Golden Peach)"},
                         {17, "Clover in east catwalk field"},
                         {18, "Buried chest between bushes in east catwalk field (Kutani Pottery)"},
                         {19, "Sapling chest ([Fleeing Battle])"},
                         {20, "Agata Forest port chest ([Feeding])"},
                         {21, "Catwalk field monument chest (Crystal)"},
                         {22, "Clover in north catwalk field"},
                         {24, "Buried chest between bushes in north catwalk field (Pearl)"},
                         {25, "Chest outside Tama's house ([Legend of Orochi])"},
                         {26, "Buried chest under burning leaves at dojo (Godly Charm)"},
                     },
                 .areasRestored =
                     {
                         {16, "Gate area by tree"},
                         {17, "Nameless Man's Kiln"},
                         {18, "Cursed gate by entrance"},
                         {19, "Dojo"},
                         {20, "North catwalk field"},
                         {21, "East catwalk field"},
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
                         {6, "Outside Tama's house"},
                         {7, "Near stairs by Tama's house"},
                         {8, "Right side of ramp to moon cave 1"},
                         {9, "Right side of ramp to moon cave 2"},
                         {10, "Left side from entrance tree 3"},
                     },
                 .fightsCleared =
                     {
                         {0, "Gate by sapling"},
                         {1, "Gate by Nameless Man"},
                         {2, "Cursed gate by entrance"},
                         {3, "Dojo gate"},
                         {4, "North catwalk field gate"},
                         {5, "East catwalk field gate"},
                         {71, "Izo the String Cutter (bounty)"},
                         {74, "Fish demon fight"},
                     },
                 .npcs =
                     {
                         {1, "Nameless Man"},
                         {2, "Mika"},
                         {3, "Tama"},
                     },
             }},
            {MapTypes::AgataForest,
             {
                 .worldStateBits =
                     {
                         {1, "Have Water Lily, Will Travel quest ended"},
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
                         {33, "Kushi's Distress quest ended"},
                         {35, "Ume's Lost Again ended; Ume the Meal started"},
                         {38, "Set when near Hitoshio Spring"},
                         {41, "Ume the Meal ended"},
                         {47, "Crack on dungeon side cliff blown up"},
                         {48, "Spoke with Karude intro 1"},
                         {49, "Spoke with Karude intro 2; Hard Working Son started"},
                         {50, "Spoke with Karude post-rescue 1"},
                         {51, "Spoke with Karude post-rapids"},
                         {55, "Spoke with Kokari first time"},
                         {56, "Spoke with Kokari second time; The Lost Key started; Ume is Lost started"},
                         {57, "Spoke with Kokari third time"},
                         {58, "Spoke with Kokari at night"},
                         {59, "Bashed Kokari while crying"},
                         {63, "Ruins key spawns; The Lost Key ended"},
                         {64, "Spoke with Kokari post-log minigame"},
                         {65, "Spoke with Kokari post-Taka Pass"},
                         {66, "End fishing without catching any fish"},
                         {67, "Ume's Lost Again started"},
                         {71, "Spoke with Susano first time"},
                         {72, "Kushi's Distress quest started"},
                         {73, "Susano has no more food"},
                         {75, "Spoke with merchant"},
                         {77, "Spoke with Kiba"},
                         {78, "Spoke with Kiba 2"},
                         {86, "Interacted with sleepy bear first time"},
                         {87, "Interacted with sapling"},
                         {88, "Interacted with dungeon entrance first time"},
                         {90, "Interacted with rapids first time"},
                     },
                 .userIndices =
                     {
                         // 0 - upper word = flags, lower byte = number of times susano gave food
                     },
                 .collectedObjects =
                     {
                         {0, "Bulb on giant rock island sapling side (Holy Bone S)"},
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
                         {36, "Chest at dungeon entrance ([Enhancing Weapons])"},
                     },
                 .areasRestored =
                     {
                         {16, "Cursed grass behind Hitoshio Spring waterfall"},
                         {17, "Cursed grass by forest hut"},
                         {18, "Hell gate in front of dungeon"},
                         {19, "Hell gate area near waterfall (releases nut)"},
                         {31, "Main area"},
                     },
                 .fightsCleared =
                     {
                         {0, "Gate in front of dungeon"},
                         {8, "Gate near waterfall"},
                     },
                 .npcs =
                     {
                         {0, "Karude"},
                         {1, "Kokari"},
                     },
             }},
            {MapTypes::MoonCaveEntrance, {}},
            {MapTypes::TakaPass,
             {
                 .worldStateBits =
                     {
                         {1, "Hard Working Son quest ended"},
                         {2, "Crack to tree bombed"},
                         {3, "Crack to tree bombed"},
                         {4, "Waka 2 triggered"},
                         {5, "Waka 2 fight triggered"},
                         {6, "Waka 2 defeated; Mysterious Windmill started"},
                         {7, "Mrs Cutter is chasing you"},
                         {8, "Bashed Mrs Cutter"},
                         {13, "Triggered Cutters house view pan"},
                         {14, "Bombed Cutters roof"},
                         {17, "Rejection quest ended; Animal Hating Couple quest ended"},
                         {19, "Sapling bloomed"},
                         {20, "Sapling bloomed"},
                         {22, "Sapling finished restoring area dialogue"},
                         {23, "Mermaid spring restored"},
                         {24, "Sapling bridge repaired"},
                         {25, "Spoke to Tea Master first time"},
                         {26, "Spoke to Tea Master post-restoration"},
                         {27, "Returned Tea Master's teacup from Moley (Golden Mushroom)"},
                         {28, "Spoke to Tea Customer first time"},
                         {29, "Spoke to Tea Customer post-restoration"},
                         {35, "Spoke to Sasa Traveler"},
                         {38, "Spoke to Bingo first time"},
                         {39, "Spoke to Bingo after removing leaves"},
                         {40, "Spoke to Bingo second time after removing leaves"},
                         {41, "Leaf pile by Bingo removed"},
                         {43, "Understood Bingo's digging explanation"},
                         {44, "Finished digging minigame"},
                         {45, "Spoke to Bingo post-minigame success"},
                         {47, "Spoke to Spring Girl post-spring restoration"},
                         {48, "Spoke to Merchant first time"},
                         {50, "Moley first dialogue triggered"},
                         {51, "Moley game active"},
                         {52, "Moley hit 1 time"},
                         {53, "Moley hit 2 times"},
                         {54, "Teacup Moley stole spawns"},
                         {58, "Moley minigame completed"},
                         {61, "Barrier to stray bead cave dug out"},
                     },
                 .collectedObjects =
                     {
                         {1, "Chest on Cutters' house (Glass Beads)"}, // req vine
                         {2, "Clover on ledge above Cutters spout"},
                         {3, "Chest on right banner pole (Golden Peach)"}, // req vine
                         {4, "Second fire chest in sapling path (Stray Bead)"},
                         {5, "Fire chest in sapling path (Crystal)"},
                         {6, "Clover in sapling cave"},
                         {7, "Buried chest behind Cutters house by spout (Stray Bead)"},
                         {8, "Buried chest between 2 rocks by mermaid spring"},
                         {12, "Buried chest by Tea Master stall"},
                         {13, "Buried chest by Checkpoint exit (Stray Bead)"},
                         {14, "Buried chest behind right banner pole by tree (Holy Bone S)"},
                         {15, "Leaf buried chest in sapling field by cave (Incense Burner)"},
                         {16, "Leaf buried chest by sapling (Vengeance Slip)"},
                         {17, "Leaf buried chest behind Cutters house (Lacquerware Set)"},
                         {18, "Leaf buried chest by mermaid spring (Stray Bead)"},
                         {21, "Clover on catwalk ledge by start of Kusa path"},
                         {22, "Buried chest under leaf pile by entrance (Vase)"},
                         {23, "Buried chest under leaf pile by entrance origin mirror (Lacquerware Set)"},
                         {24, "Buried chest under leaf pile field between Cutters path and moles (Coral Fragment)"},
                         {25, "Leaf buried chest just off sapling field path bottom of ledge (Pearl)"},
                     },
                 .areasRestored =
                     {
                         {16, "Mermaid spring"},
                         {17, "Bamboo field by entrance origin mirror"},
                         {18, "Bamboo field near Checkpoint exit"},
                         {31, "Taka Pass"},
                     },
                 .treesBloomed =
                     {
                         {0, "Path from sapling 1"},
                         {1, "Path from sapling 2"},
                         {2, "Path from sapling 3"},
                         {3, "Left of Checkpoint exit"},
                         {4, "Right of Checkpoint exit"},
                         {5, "Behind right banner pole"},
                         {6, "Right of Sasa exit 1"},
                         {7, "Right of Sasa exit 2"},
                         {8, "Right of Sasa exit 3"},
                         {9, "Just outside stray bead cave"},
                     },
                 .fightsCleared =
                     {
                         {11, "Gate by mermaid spring"},
                         {12, "Gate in field by entrance origin mirror"},
                         {13, "Gate by Checkpoint exit"},
                     },
                 .npcs =
                     {
                         {3, "Tea Master"},
                         {4, "Tea Customer"},
                         {5, "Sasa Traveler"},
                         {6, "Bingo"},
                     },
                 .mapsExplored =
                     {
                         {1, "Cave to tree"},
                     },
             }},
            {MapTypes::RyoshimaCoast,
             {
                 .worldStateBits =
                     {
                         {2, "First entrance dialogue"},
                         {3, "Sapling bloomed"},
                         {4, "Triggered Headless Guardian fight"},
                         {5, "Crack to water cave in cursed Ryoshima bombed"},
                         {6, "Water cave second pond filled"},
                         {7, "Water cave third pond filled"},
                         {8, "Sapling pond filled"},
                         {9, "Triggered sapling view cutscene"},
                         {10, "Sapling grown"},
                         {11, "Triggered restoring right tri-cursed grass"},
                         {12, "Triggered restoring left tri-cursed grass"},
                         {13, "Triggered restoring middle tri-cursed grass"},
                         {15, "Post-restoration"},
                         {16, "Post-restoration"},
                         {18, "Triggered Ubume fight"},
                         {20, "Inaba returned to Animal Lover (Stray Bead)"},
                         {33, "Lookout tower climb attempt"},
                         {35, "Lookout tower climb attempt"},
                         {49, "Bad Feeling Confirmed ended"},
                         {53, "Spoke with Animal Lover first time"},
                         {55, "Spoke with large soldier at N Ryo gate"},
                         {59, "Spoke with skinny soldier at N Ryo gate"},
                         {70, "Spoke with merchant"},
                         {73, "Hard ground to Bandit Spider arena broken"},
                     },
                 .collectedObjects =
                     {
                         {1, "Clover in front of Madame Fawn's cave"},
                         {3, "Clover at lookout by sunken ship"},
                         {6, "Buried chest on dojo island hard ground (Stray Bead)"},
                         {12, "Clover by entrance to Seian hard ground"},
                         {19, "Chest on dojo island (Exorcism Slip S)"},
                         {20, "Chest at end of dock (Dragonfly Bead)"},
                         {21, "Clover right of dock shore"},
                         {22, "Buried chest at entrance to Seian (Crystal)"},
                         {23, "Chest outside entrance to Seian (Steel Fist Sake)"},
                         {38, "Clam in front of merchant (Holy Bone S)"},                 // bashable
                         {40, "Clam at outermost part of beach (Coral Fragment)"},        // bombable
                         {41, "Clam between dock and outermost part (Traveler's Charm)"}, // bombable
                         {42, "Clam left of dock shore (Dragonfly Bead)"},                // bombable
                         {43, "Clam right of dock shore (Pearl)"},                        // bombable
                         {44, "Clam to right of lookout (Glass Beads)"},                  // bashable
                         {45, "Clam below lookout (Mermaid Coin)"},                       // deep
                     },
                 .areasRestored =
                     {
                         {16, "Right tri-cursed grass"},
                         {17, "Left tri-cursed grass"},
                         {18, "Middle tri-cursed grass"},
                         {19, "Gate area just outside Seian exit"},
                         {20, "N Ryoshima exit mermaid pond"},
                         {31, "Ryoshima Coast"},
                     },
                 .fightsCleared =
                     {
                         {0, "Gate outside Seian exit"}, // also used for Headless Guardian intro (cursed)
                         {1, "Gate by N Ryoshima exit"},
                         {18, "Ubume intro"},
                     },
             }},
            {MapTypes::Unknown76, {}},
            {MapTypes::NRyoshimaCoast,
             {
                 .worldStateBits =
                     {
                         {31, "Emissary From the Deep ended; The Amazing Orca started"},
                         {34, "Otohime's Test started"},
                         {42, "The Power of Catwalk ended; Otohime's Test ended; Twin Whirlpools started"},
                         {45, "Twin Whirlpools ended"},
                         {50, "Oni Island's Whereabouts ended; Lone Wolf vs Evil started"},
                         {53, "Shinshu's Rolling Thunder started"},
                         {62, "Catcall Tower started"},
                         {67, "Emissary From the Deep started"},
                         {91, "Cherry bomb 2 cave crack bombed"},
                     },
             }},
            {MapTypes::Kamui,
             {
                 .worldStateBits =
                     {
                         {1, "Shinshu's Rolling Thunder ended; Subzero Zone started"},
                         {3, "Subzero Zone ended; Cold Comfort started; Squatters of Ezofuji started"},
                         {6, "Yoichi the Philosopher ended"},
                         {19, "Yoichi the Philosopher started"},
                         {60, "Spoke to Merchant"},
                     },
                 .collectedObjects =
                     {
                         {18, "Clover on sapling ledge"},
                         {42, "Bombed cracked rock on sapling ledge"},
                         {43, "Bombed double-crack to cherry bomb 3 cave"},
                     },
             }},
            {MapTypes::KamuiEzofuji,
             {
                 .worldStateBits =
                     {
                         {3, "Take Lika to Ezofuji ended; Oki's Unauthorized Act started"},
                         {6, "Oki's Unauthorized Act ended; Squatters of Ezofuji ended; Rising Yamato quest started"},
                         {52, "Examined altar by entrance"},
                     },
                 .collectedObjects =
                     {
                         {0, "Power slash 3 cave rock broken"},
                         {1, "Clover outside power slash 3 cave"},
                         {2, "Diamond rock outside power slash 3 cave cut"},
                         {6, "Chest by entrance ([Another Civilization])"},
                         {10, "Buried chest east part of high north ledge (Stray Bead)"},
                         {11, "Clover east part of high north ledge"},
                         {12, "Clover in hard ground on north ledge left"},
                         {13, "Buried chest in hard ground on north ledge (Stray Bead)"},
                         {14, "Clover in hard ground on north ledge right"},
                         {15, "Clover on lower north ledge"},
                         {16, "Chest on high north ledge (Sun Fragment)"},
                     },
             }},
            {MapTypes::ShinshuField100yearsago, {}},
            {MapTypes::MoonCaveEntrance100yearsago, {}},
            {MapTypes::Unk82, {}},
            {83, {}},
};

}
