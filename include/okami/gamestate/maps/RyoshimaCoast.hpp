#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/RyoshimaCoast.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::RyoshimaCoast
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{2, "First entrance dialogue"},
                                                                   {3, "Sapling bloomed"},
                                                                   {4, "Triggered Headless Guardian fight"},
                                                                   {5, "Crack to water cave in cursed Ryoshima bombed"},
                                                                   {6, "Water cave second pond filled"},
                                                                   {7, "Water cave third pond filled"},
                                                                   {8, "Sapling pond filled"},
                                                                   {9, "Triggered sapling view cutscene"},
                                                                   {10, "Sapling grown"},
                                                                   {11, "Triggered restoring right tri-cursed grass"},
                                                                   {12, "Triggered restoring left tri-cursed grass"},
                                                                   {13, "Triggered restoring middle tri-cursed grass"},
                                                                   {15, "Post-restoration"},
                                                                   {16, "Post-restoration"},
                                                                   {18, "Triggered Ubume fight"},
                                                                   {20, "Inaba returned to Animal Lover (Stray Bead)"},
                                                                   {33, "Lookout tower climb attempt"},
                                                                   {35, "Lookout tower climb attempt"},
                                                                   {49, "Bad Feeling Confirmed ended"},
                                                                   {53, "Spoke with Animal Lover first time"},
                                                                   {55, "Spoke with large soldier at N Ryo gate"},
                                                                   {59, "Spoke with skinny soldier at N Ryo gate"},
                                                                   {70, "Spoke with merchant"},
                                                                   {73, "Hard ground to Bandit Spider arena broken"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {{1, "Clover in front of Madame Fawn's cave"},
                                                                     {3, "Clover at lookout by sunken ship"},
                                                                     {6, "Buried chest on dojo island hard ground (Stray Bead)"},
                                                                     {12, "Clover by entrance to Seian hard ground"},
                                                                     {19, "Chest on dojo island (Exorcism Slip S)"},
                                                                     {20, "Chest at end of dock (Dragonfly Bead)"},
                                                                     {21, "Clover right of dock shore"},
                                                                     {22, "Buried chest at entrance to Seian (Crystal)"},
                                                                     {23, "Chest outside entrance to Seian (Steel Fist Sake)"},
                                                                     {38, "Clam in front of merchant (Holy Bone S)"},
                                                                     {40, "Clam at outermost part of beach (Coral Fragment)"},
                                                                     {41, "Clam between dock and outermost part (Traveler's Charm)"},
                                                                     {42, "Clam left of dock shore (Dragonfly Bead)"},
                                                                     {43, "Clam right of dock shore (Pearl)"},
                                                                     {44, "Clam to right of lookout (Glass Beads)"},
                                                                     {45, "Clam below lookout (Mermaid Coin)"}};

const std::unordered_map<unsigned, const char *> areasRestored = {{16, "Right tri-cursed grass"},       {17, "Left tri-cursed grass"},
                                                                  {18, "Middle tri-cursed grass"},      {19, "Gate area just outside Seian exit"},
                                                                  {20, "N Ryoshima exit mermaid pond"}, {31, "Ryoshima Coast"}};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {{0, "Gate outside Seian exit"}, {1, "Gate by N Ryoshima exit"}, {18, "Ubume intro"}};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::RyoshimaCoast
