#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/KamikiVillageGameStart.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::KamikiVillageGameStart
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{13, "Cut Down the Peach quest started"},
                                                                   {16, "Ran into the forcefield towards Kamiki (instant)"},
                                                                   {18, "Identified invisible forcefield towards Kamiki"},
                                                                   {43, "Cut Down the Peach quest ended"},
                                                                   {44, "Peach is cut"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {};

const std::unordered_map<unsigned, const char *> areasRestored = {};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::KamikiVillageGameStart
