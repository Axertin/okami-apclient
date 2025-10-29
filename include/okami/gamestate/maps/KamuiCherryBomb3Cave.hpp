#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/KamuiCherryBomb3Cave.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::KamuiCherryBomb3Cave
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "Interacted with pool"}, {2, "Obtained Cherry Bomb 3"}};

const std::unordered_map<unsigned, const char *> userIndices = {{0, "Money donated"}};

const std::unordered_map<unsigned, const char *> collectedObjects = {
    {0, "Post-donation chest ([Cherry Bomb 3])"}, {1, "Clover middle"}, {2, "Clover right"}, {3, "Clover left"}};

const std::unordered_map<unsigned, const char *> areasRestored = {};

const std::unordered_map<unsigned, const char *> treesBloomed = {{0, "Left near"}, {1, "Right near"}, {2, "Left far"}, {3, "Right far"}};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::KamuiCherryBomb3Cave
