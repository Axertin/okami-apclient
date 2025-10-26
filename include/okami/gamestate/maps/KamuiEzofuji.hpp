#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/KamuiEzofuji.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::KamuiEzofuji
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{3, "Take Lika to Ezofuji ended; Oki's Unauthorized Act started"},
                                                                   {6, "Oki's Unauthorized Act ended; Squatters of Ezofuji ended; Rising Yamato quest started"},
                                                                   {52, "Examined altar by entrance"}};

const std::unordered_map<unsigned, const char *> userIndices = {};

const std::unordered_map<unsigned, const char *> collectedObjects = {{0, "Power slash 3 cave rock broken"},
                                                                     {1, "Clover outside power slash 3 cave"},
                                                                     {2, "Diamond rock outside power slash 3 cave cut"},
                                                                     {6, "Chest by entrance ([Another Civilization])"},
                                                                     {10, "Buried chest east part of high north ledge (Stray Bead)"},
                                                                     {11, "Clover east part of high north ledge"},
                                                                     {12, "Clover in hard ground on north ledge left"},
                                                                     {13, "Buried chest in hard ground on north ledge (Stray Bead)"},
                                                                     {14, "Clover in hard ground on north ledge right"},
                                                                     {15, "Clover on lower north ledge"},
                                                                     {16, "Chest on high north ledge (Sun Fragment)"}};

const std::unordered_map<unsigned, const char *> areasRestored = {};

const std::unordered_map<unsigned, const char *> treesBloomed = {};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {};

const std::unordered_map<unsigned, const char *> npcs = {};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::KamuiEzofuji
