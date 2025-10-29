#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/Ponctan.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::Ponctan
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{7, "Gate of Misfortune ended; Open Wide the Spirit Gate started"},
                                                                   {15, "Poncle Village Ponc'tan ended; Gate of Misfortune started"}};

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

} // namespace okami::game_state::maps::Ponctan
