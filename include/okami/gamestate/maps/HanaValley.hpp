#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/HanaValley.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::HanaValley
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{1, "Hit campfire group trigger"},
                                                                   {2, "Campfire group cleared for good"},
                                                                   {3, "Campfire group dancing"},
                                                                   {4, "In campfire group battle"},
                                                                   {5, "Triggered cutscene for cursed tree"},
                                                                   {6, "Triggered cutscene for stunned cursed tree"},
                                                                   {7, "Triggered mural battle"},
                                                                   {8, "Mural battle started"},
                                                                   {9, "Mural battle cleared"},
                                                                   {10, "Mural battle cleared"},
                                                                   {11, "Mural battle cleared"},
                                                                   {12, "Currently talking with Susano"},
                                                                   {13, "Triggered panning to mural"},
                                                                   {15, "Spawned sun on mural"},
                                                                   {16, "Spawned sun on mural"},
                                                                   {17, "Susano ran off"},
                                                                   {18, "Triggered sapling room cutscene (first time)"},
                                                                   {19, "Triggered Susano slash minigame cutscene"},
                                                                   {21, "Freed crystal"},
                                                                   {23, "Crystal placed into position"},
                                                                   {25, "Crystal placed into position"},
                                                                   {26, "Triggered convo after crystal placed"},
                                                                   {27, "Getting mural explanation again"},
                                                                   {30, "Sun spawned for sapling"},
                                                                   {31, "Constellation completed"},
                                                                   {35, "Sapling bloomed"},
                                                                   {36, "Constellation completed"},
                                                                   {37, "Sapling bloomed"},
                                                                   {38, "Cursed grass trigger"},
                                                                   {39, "Finished cursed grass intro"},
                                                                   {40, "Restoring cursed grass"},
                                                                   {41, "Secret of Hana Valley quest ended, Revive the Guardian Saplings quest started"},
                                                                   {42, "Restored campfire group area for good"},
                                                                   {43, "Restored campfire group area (cursed)"},
                                                                   {44, "Mural battle cleared for good"},
                                                                   {45, "Restoring cursed grass"},
                                                                   {47, "Post-campfire battle dialogue triggered"},
                                                                   {48, "Triggering sun cutscene without crystal"},
                                                                   {50, "Issun scolding for trying to leave"},
                                                                   {51, "Set during distrub bear's sleep convo"},
                                                                   {53, "Issun constellation explanation if messing around"},
                                                                   {55, "Explanation for bloom"},
                                                                   {57, "Set while Issun explains cursed grass"},
                                                                   {58, "Being given Greensprout Tips scroll"},
                                                                   {60, "cursed tree related"},
                                                                   {61, "Examined the mural"},
                                                                   {62, "Examined sapling"},
                                                                   {65, "Examined the mural"},
                                                                   {66, "Spawned sun on mural"},
                                                                   {68, "Examined sapling"},
                                                                   {71, "Crystal is on fountain"},
                                                                   {75, "Campfire lit/group active"},
                                                                   {76, "Cursed grass restored"},
                                                                   {77, "All trees restored (reward chest)"},
                                                                   {78, "Lighting change toggle"},
                                                                   {80, "Constellation pulse"},
                                                                   {82, "Hana Valley restoration process"},
                                                                   {83, "In cursed tree introduction cutscene"},
                                                                   {85, "Interacting with constellation"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {{0, "Clover on cliff"},
                                                                     {1, "Buried chest just after campfire (Stray Bead)"},
                                                                     {2, "Hidden buried chest by entrance on beach (Coral Fragment)"},
                                                                     {3, "Clover by entrance"},
                                                                     {4, "Island clover"},
                                                                     {5, "Chest near entrance (Traveler's Charm)"},
                                                                     {6, "Island chest ([Digging Tips])"},
                                                                     {80, "Bloom reward chest (Sun Fragment)"}};

const std::unordered_map<unsigned, const char *> areasRestored = {{16, "Campfire area"}, {18, "Mural room"}, {20, "Cursed grass"}, {28, "Sapling room"}};

const std::unordered_map<unsigned, const char *> treesBloomed = {{0, "First tree by entrance"},
                                                                 {1, "Second tree by entrance"},
                                                                 {2, "First tree from campfire"},
                                                                 {3, "Second tree from campfire"},
                                                                 {4, "Dead tree by bridge"},
                                                                 {5, "Campfire right side 1"},
                                                                 {6, "Island tree"},
                                                                 {7, "Campfire right side 2"},
                                                                 {8, "Campfire right side 3"},
                                                                 {9, "Campfire left side 1"},
                                                                 {10, "Campfire left side 2"},
                                                                 {11, "Campfire left side 3"}};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {
    {0, "Cursed tree closest to mural"}, {1, "Cursed tree closest to campfire"}, {2, "Cursed tree by waterfall"}, {3, "Cursed tree by bridge"}};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::HanaValley
