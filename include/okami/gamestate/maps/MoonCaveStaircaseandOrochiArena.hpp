#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/MoonCaveStaircaseandOrochiArena.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::MoonCaveStaircaseandOrochiArena
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{2, "Started Orochi battle"},
                                                                   {3, "Started Orochi battle"},
                                                                   {4, "Triggered Orochi intro cutscene"},
                                                                   {5, "Started Orochi battle for real"},
                                                                   {6, "Triggered Susano entrance cutscene"},
                                                                   {7, "Defeated phase 2"},
                                                                   {9, "Being scolded by Issun for trying to leave"},
                                                                   {11, "Scolded by Issun for trying to leave 1st time"},
                                                                   {12, "Issun talking about pit (unflips when not talking, flips for good after pep talk)"},
                                                                   {13, "Got closer to pit (can trigger turn-around pep talk)"},
                                                                   {16, "Moon power for Susano cutscene"},
                                                                   {17, "Drew moon for Susano"},
                                                                   {32, "Started Orochi battle"},
                                                                   {64, "Orochi roaring demonstration"},
                                                                   {65, "First head drunk"},
                                                                   {67, "Orochi's body drunk cutscene"},
                                                                   {68, "First head wasted"},
                                                                   {70, "Spoke to Merchant first time"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {};

const std::unordered_map<unsigned, const char *> areasRestored = {{31, "Entered first time/Restored from Dark head's cursed zone"}};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {{20, "Staircase/Entrance"}};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::MoonCaveStaircaseandOrochiArena
