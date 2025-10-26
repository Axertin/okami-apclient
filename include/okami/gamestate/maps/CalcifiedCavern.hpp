#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/CalcifiedCavern.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::CalcifiedCavern
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "Collected Mask"},
                                                                   {2, "Picked up Thunder Brew"},
                                                                   {3, "Door into Moon Cave unlocked"},
                                                                   {4, "First Cavern camera pan"},
                                                                   {5, "Spawned Mask chest"},
                                                                   {6, "Approached Imp guards with mask"},
                                                                   {7, "Spoke to Imps guarding door first time"},
                                                                   {8, "Rejected first Mask design"},
                                                                   {9, "Finished first Mask design"},
                                                                   {10, "Drawing on Mask"},
                                                                   {12, "Started Black Imp Demon Gate fight"},
                                                                   {13, "Imps started giving random vegetables (end of new dialogue)"},
                                                                   {15, "Black Imp intro cutscene triggered"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {{0, "Chest after Black Imp Demon Gate (Mask)"},
                                                                     {1, "Chest on path to Mask (Holy Bone S)"}};

const std::unordered_map<unsigned, const char *> areasRestored = {{16, "Mask chest area"}};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {{0, "Started Black Imp Demon Gate fight"}, {1, "Cleared Black Imp Demon Gate fight"}};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::CalcifiedCavern
