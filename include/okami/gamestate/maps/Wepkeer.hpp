#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/Wepkeer.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::Wepkeer
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{3, "Cold Comfort ended; Icy Wep'keer started"},
                                                                   {17, "Icy Wep'keer ended; Lika is Missing! started"},
                                                                   {18, "Kai as Guide started"},
                                                                   {25, "Lika is Missing! ended; Forest of Yoshpet started"}};

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

} // namespace okami::game_state::maps::Wepkeer
