#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/CityCheckpoint.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::CityCheckpoint
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "Bringing Down the Bridge ended"},
                                                                   {2, "Speak to Merchant first time"},
                                                                   {7, "Speak to Soldier second time (pre-Moon Cave)"},
                                                                   {8, "Speak to Yoichi first time"},
                                                                   {9, "Speak to Yoichi second time"},
                                                                   {10, "Speak to Mrs. Pine first time"},
                                                                   {12, "Speak to Tea Customer first time"},
                                                                   {27, "Read sign in front of Big Drawbridge"},
                                                                   {28, "Read sign in front of Big Drawbridge"},
                                                                   {35, "Speak to Soldier first time"},
                                                                   {36, "Speak to Yoichi first time (pre-Moon Cave)"},
                                                                   {42, "City Checkpoint quest ended"},
                                                                   {43, "Bringing Down the Bridge quest started"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {{0, "Clover near furthest downstream cliffs"},
                                                                     {3, "Clover in cave behind waterfall"},
                                                                     {4, "Chest behind Merchant (Bull Horns)"},
                                                                     {5, "Chest under wooden ramp [Mother Tree]"},
                                                                     {6, "Buried chest near furthest downstream cliffs (Stray Bead)"},
                                                                     {7, "Buried chest downstream of waterfall (Exorcism Slip S)"},
                                                                     {10, "Chest on fire near waterfall (Stray Bead)"}};

const std::unordered_map<unsigned, const char *> areasRestored = {};

const std::unordered_map<unsigned, const char *> treesBloomed = {{0, "Next to Mrs Pine's tea shop"}};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {{1, "Tea Customer"}, {3, "Soldier"}, {4, "Yoichi"}};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::CityCheckpoint
