#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/KamikiVillage100yearsago.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::KamikiVillage100yearsago
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "To the Moon Cave! ended"},
                                                                   {2, "Spirit Gate Ho! ended; Beyond the Gate started"},
                                                                   {7, "Beyond the Gate ended; Kamiki a Century Ago started"},
                                                                   {8, "Kamiki a Century Ago ended; To the Moon Cave! started"}};

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

} // namespace okami::game_state::maps::KamikiVillage100yearsago
