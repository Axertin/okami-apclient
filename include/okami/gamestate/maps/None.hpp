#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/None.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::None
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{3, "Doing Constellation"},
                                                                   {9, "Carrying Susano"},
                                                                   {10, "Celestial Brush Locked"},
                                                                   {16, "Remaining Warriors quest ended; Orbs Reunited quest started"},
                                                                   {18, "Defeated Orochi"},
                                                                   {19, "Entered Cave"},
                                                                   {22, "Taka Pass Sapling Healed"},
                                                                   {31, "Festival quest ended; City Checkpoint quest started; To the Capital! quest started"},
                                                                   {38, "Near a mermaid warp"},
                                                                   {40, "Hana Valley sapling Healed"},
                                                                   {41, "Shinshu Field sapling Healed"},
                                                                   {42, "Agata Forest sapling Healed"},
                                                                   {44, "Ryoshima Coast sapling Healed"},
                                                                   {46, "Shinshu Field mermaid spring unlocked"},
                                                                   {47, "Agata Forest mermaid spring unlocked"},
                                                                   {48, "Taka Pass mermaid spring unlocked"},
                                                                   {49, "Sasa Sanctuary mermaid spring unlocked"},
                                                                   {50, "Ryoshima Coast mermaid spring unlocked"},
                                                                   {51, "N.Ryoshima Coast mermaid spring unlocked"},
                                                                   {52, "Dragon Palace mermaid spring unlocked"},
                                                                   {53, "Kamui mermaid spring unlocked"},
                                                                   {62, "Waka 1 defeated"},
                                                                   {63, "Shinshu Adventure quest ended"},
                                                                   {64, "Fishing in Agata Forest"},
                                                                   {70, "Divine Springs tutorial (Power slash 2)"},
                                                                   {71, "Donated to Divine Spring first time (Power slash 2)"},
                                                                   {72, "Bandit Spider defeated"},
                                                                   {84, "First dojo cutscene trigger"},
                                                                   {97, "Fishing tutorial shown"}};

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

} // namespace okami::game_state::maps::None
