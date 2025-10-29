#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/Kamui.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::Kamui
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "Shinshu's Rolling Thunder ended; Subzero Zone started"},
                                                                   {3, "Subzero Zone ended; Cold Comfort started; Squatters of Ezofuji started"},
                                                                   {6, "Yoichi the Philosopher ended"},
                                                                   {19, "Yoichi the Philosopher started"},
                                                                   {60, "Spoke to Merchant"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {
    {18, "Clover on sapling ledge"}, {42, "Bombed cracked rock on sapling ledge"}, {43, "Bombed double-crack to cherry bomb 3 cave"}};

const std::unordered_map<unsigned, const char *> areasRestored = {};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::Kamui
