#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/MoonCaveEntrance.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::MoonCaveEntrance
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "First Moon Cave Entrance camera pan cutscene"},
                                                                   {4, "Bringing Kushi to Moon Cave cutscene"},
                                                                   {6, "Issun dialogue after first entered"},
                                                                   {7, "Ran into barrier"}};

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

} // namespace okami::game_state::maps::MoonCaveEntrance
