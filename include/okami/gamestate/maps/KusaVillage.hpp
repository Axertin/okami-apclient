#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/KusaVillage.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::KusaVillage
{

const std::unordered_map<unsigned, const char *> worldStateBits = {
    {1, "Mysterious Windmill quest ended; Canine Warrior Hunt quest started"},
    {2, "Outside-Kusa dog hunt started"},
    {3, "Glowing bamboo cut"},
    {4, "Chi freed from glowing bamboo"},
    {5, "Area by flowers sign restored"},
    {6, "Tei is revealed"},
    {7, "Fed Chi"},
    {8, "Fed Ko"},
    {9, "Fed Rei"},
    {10, "Fed Shin"},
    {11, "Canine Warrior Hunt ended; Remaining Warriors started"},
    {12, "Defeated Tei"},
    {15, "Issun text talking to Chi"},
    {16, "Revealed Shin in cursed patch"},
    {18, "Entered after fighting all dogs"},
    {19, "Defeated Blue Imp fight"},
    {20, "Defeated Tei"},
    {21, "Curse cleared"},
    {22, "Area by flowers sign restored"},
    {24, "Completed Komuso's Challenge"},
    {25, "Issun introduces Kusa's curse"},
    {26, "Issun told you to return after feeding 4 dogs"},
    {27, "Defeat Crimson Helm! ended; Powerful Brew started"},
    {28, "Spoke to Gourd Farmer"},
    {29, "Spoke to Gourd Farmer part 2; Animal Hating Couple quest started"},
    {30, "Spoke to Gourd Farmer after fighting all dogs"},
    {31, "Susano cutscene triggered"},
    {32, "Spoke to Mr Bamboo first time"},
    {33, "Divine Wind's Cessation started"},
    {35, "Spoke to Princess Fuse with Satomi Orbs"},
    {36, "Spoke to Princess Fuse in house after removing curse"},
    {38, "Komuso able to summon battle"},
    {39, "Yellow Well Woman ready to say 2nd part of gossip"},
    {40, "Pink Well Woman ready to say 2nd part of gossip"},
    {41, "Spoke to Merchant"},
    {42, "Spoke to Merchant post-Crimson Helm"},
    {43, "Spoke to Mrs Plum first time"},
    {44, "Spoke to Mrs Plum post-Crimson Helm"},
    {45, "Spoke to Mrs Plum after fighting all dogs"},
    {46, "Left Inn with Haruka's Revenge List"},
    {47, "Spoke to Haruka after completing her Revenge List"},
    {49, "Spoke to Flower Girl first time"},
    {50, "Spoke to Flower Girl after restoring flowers"},
    {51, "Spoke to Flower Girl post-Crimson Helm"},
    {52, "Spoke to Guard first time"},
    {53, "Spoke to Guard post-Crimson Helm"},
    {54, "Spoke to Tei first time"},
    {60, "Used Galestorm on well windmill"},
    {61, "Used Galestorm on well windmill"},
    {62, "Heard first Princess Fuse dialog after Blue Imps"},
    {64, "Princess Fuse is inside her house (cleared when outside)"},
    {67, "Transitioned to nighttime"},
    {68, "Bonked Inn washroom door with Haruka"},
    {69, "Haruka moves to her room"},
    {72, "Entered Gale Shrine"},
    {73, "Fight triggered"},
    {74, "Talking to a Canine Warrior after feeding them"},
    {76, "Defeated Blockhead"},
    {77, "Being scolded by Issun for trying to leave early"},
    {78, "Orbs Reunited quest ended; Divine Wind's Cessation ended; Defeat Crimson Helm! started"},
    {80, "Crack by Komuso bombed"},
    {81, "Interacted with glowing bamboo"},
    {83, "Spoke to Flower Girl after restoring well"},
    {86, "Interacted with glowing bamboo"},
    {87, "Glowing bamboo cut"},
    {90, "Chi's Satomi Orb added to circle"},
    {91, "Ko's Satomi Orb added to circle"},
    {92, "Rei's Satomi Orb added to circle"},
    {93, "Shin's Satomi Orb added to circle"},
    {94, "Tei's Satomi Orb added to circle"},
    {95, "Hayabusa's Satomi Orb added to circle"},
    {96, "Ume's Satomi Orb added to circle"},
    {97, "Take's Satomi Orb added to circle"},
    {98, "Satomi Orbs left Princess Fuse"},
    {99, "Interacting with Chi (cleared when not)"},
    {100, "Interacting with Ko (cleared when not)"},
    {101, "Interacting with Rei (cleared when not)"},
    {102, "Interacting with Shin (cleared when not)"},
    {105, "Crack by Komuso bombed"},
    {106, "Near Princess Fuse (cursed)"},
    {109, "Spoke to yellow Well Woman post-Crimson Helm"},
    {110, "Spoke to pink Well Woman post-Crimson Helm"},
    {112, "Kusa Village introduction cutscene"},
    {113, "Tei fight started"},
    {115, "Spoke to Blockhead first time"},
    {116, "Triggered Issun text when trying to leave early"}};

const std::unordered_map<unsigned, const char *> userIndices = {{0, "Warrior dogs met flags"}};

const std::unordered_map<unsigned, const char *> collectedObjects = {{0, "Clover on right in room past Blockhead"},
                                                                     {1, "Clover on left in room past Blockhead"},
                                                                     {2, "Chest in room past Blockhead (Bull Horn)"},
                                                                     {3, "Chest under awning past Koi flag bridge (Sun Fragment)"},
                                                                     {4, "Clover behind where Susano is sleeping"},
                                                                     {5, "Clover above ladder on path to Mr Bamboo's"},
                                                                     {7, "Chest past Koi flag bridge (Stray Bead)"},
                                                                     {8, "Buried chest in Mr Bamboo's house (Stray Bead)"},
                                                                     {9, "Buried chest by shallow pond outside Princess Fuse's (Stray Bead)"},
                                                                     {10, "Clover outside Gale Shrine"},
                                                                     {11, "Buried chest on path to Gale Shrine (Incense Burner)"},
                                                                     {12, "Daruma doll in Mrs Plum's inn (Exorcism Slip S)"},
                                                                     {14, "Chest in shallow pond right of Princess Fuse's (Dragonfly Bead)"},
                                                                     {15, "Chest in shallow pond left of Princess Fuse's (Lacquerware Set)"}};

const std::unordered_map<unsigned, const char *> areasRestored = {{16, "In front of water the flowers sign"}};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {{0, "Gourd Farmer"}, {3, "Princess Fuse"}, {7, "Flower Girl"}, {9, "Guard"}, {11, "Haruka"}};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::KusaVillage
