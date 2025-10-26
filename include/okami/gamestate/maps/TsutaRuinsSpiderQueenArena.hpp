#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/TsutaRuinsSpiderQueenArena.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::TsutaRuinsSpiderQueenArena
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "Post-defeat ask to save; Ume is Lost ended; A Son's Determination started"},
                                                                   {2, "Fall area triggered"},
                                                                   {3, "Spider Queen defeated"},
                                                                   {4, "Golden gate tutorial trigger"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {};

const std::unordered_map<unsigned, const char *> areasRestored = {};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {
    {1, "Entrance"},        {2, "Right side room"},      {3, "Greenway room 1F"}, {4, "Pond room 1F"},
    {5, "Hallway room 2F"}, {6, "Left side room"},       {7, "Gloomy room 2F"},   {8, "Hell gates room 2F"},
    {9, "Center room"},     {10, "Deep mirror room 3F"}, {11, "Boss room 3F"},    {12, "Origin mirror room 2F"}};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::TsutaRuinsSpiderQueenArena
