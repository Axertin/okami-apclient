#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/RiverOfTheHeavens.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::RiverOfTheHeavens
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{20, "Save point tutorial triggered"},
                                                                   {21, "Breaking pots tutorial triggered"},
                                                                   {24, "Wall jump tutorial triggered"},
                                                                   {25, "Post-wall jump triggered"},
                                                                   {27, "Broken bridge cutscene triggered"},
                                                                   {28, "River of heavens area plaque scene triggered"},
                                                                   {29, "Stars on hill cutscene trigger"},
                                                                   {37, "Area introduction trigger when coming back down the hill from constellation"},
                                                                   {48, "Constellation pulsing (set to 1 then 0 every time it pulses)"},
                                                                   {49, "Broken bridge restored"},
                                                                   {52, "River can be rejuvinated (only set when close enough)"},
                                                                   {53, "Astral pouch cutscene triggered"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {{0, "First chest (Holy Bone S)"}, {1, "Second chest (Astral Pouch)"}};

const std::unordered_map<unsigned, const char *> areasRestored = {};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::RiverOfTheHeavens
