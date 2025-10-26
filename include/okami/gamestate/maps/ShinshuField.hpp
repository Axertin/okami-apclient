#pragma once
#include <unordered_map>

// Auto-generated from src/devtools/game-data/maps/ShinshuField.yml
// Do not edit manually - regenerate with scripts/generate_gamestate_headers.py

namespace okami::game_state::maps::ShinshuField
{

const std::unordered_map<unsigned, const char *> worldStateBits = {{2, "Entered Shinshu field first time"},
                                                                   {3, "Entered cursed zone for first time"},
                                                                   {4, "Returned to Shinshu with Bloom"},
                                                                   {5, "Bloom Guardian Sapling"},
                                                                   {6, "Dialog trigger for distant guardian sapling"},
                                                                   {8, "Lifted curse zone"},
                                                                   {9, "Lifted curse zone"},
                                                                   {11, "Spoke to Mika second time (bountry trigger enabled)"},
                                                                   {12, "Encountered bounty tutorial"},
                                                                   {13, "Cursed grass by Agata Port restored"},
                                                                   {14, "Cursed grass by entrance restored"},
                                                                   {15, "Nameless Man's Kiln restored"},
                                                                   {17, "Left Moon Cave post-Orochi"},
                                                                   {18, "Crack by cat statue blown up"},
                                                                   {19, "Crack by Agata Forest Port blown up"},
                                                                   {20, "Crack to Agata Forest blown up"},
                                                                   {21, "Issun stops you from going back to Kamiki"},
                                                                   {22, "You choose to leave Shinshu"},
                                                                   {23, "Read notice to return to Tama's at night"},
                                                                   {24, "Heavy Sleeper quest started"},
                                                                   {25, "Inspected Agata Forest Port sign"},
                                                                   {26, "Encountered bounty tutorial"},
                                                                   {28, "Trigger for first fish demon fight"},
                                                                   {29, "Gate tutorial trigger"},
                                                                   {30, "First gate cleared, finish tutorial trigger"},
                                                                   {31, "Tama failed first time"},
                                                                   {32, "Tama failed second time"},
                                                                   {33, "Finished cherry bomb crack tutorial; Heavy Sleeper ended"},
                                                                   {35, "Triggered constellation"},
                                                                   {36, "Completed constellation"},
                                                                   {38, "Storm before Kamiki Festival begun"},
                                                                   {39, "Cherry bomb crack tutorial started"},
                                                                   {40, "Crack by Tama's house blown up"},
                                                                   {42, "Issun re-explaining we should help Tama out"},
                                                                   {44, "Brush assist for cherry bomb crack tutorial triggered"},
                                                                   {45, "Crack by Tama's house blown up"},
                                                                   {46, "Dojo gate cleared"},
                                                                   {50, "Interacted with crack in the wall"},
                                                                   {51, "Crack by Tama's house blown up"},
                                                                   {52, "Nameless Man's Kiln restored"},
                                                                   {53, "Spoke to Nameless Man to get first vase"},
                                                                   {54, "First vase placed"},
                                                                   {59, "Transitioned after placing first vase"},
                                                                   {64, "Vase grabbable item active"},
                                                                   {66, "Ida began running again"},
                                                                   {68, "Racing Ida"},
                                                                   {69, "Spoke to Ida before first race"},
                                                                   {70, "Finished first Ida race"},
                                                                   {71, "Transitioned after finishing first Ida race"},
                                                                   {85, "Spoke to Kushi before Moon Cave"},
                                                                   {86, "Kushi's Decision quest ended; Kushi's Feelings started"},
                                                                   {87, "Kushi's Feelings ended; Festival quest started"},
                                                                   {90, "Monument cutscene in catwalk field"},
                                                                   {92, "Encountered bounty tutorial"},
                                                                   {93, "Spoke to Ida first time"},
                                                                   {94, "Gave Traveler's Charm to Ida"},
                                                                   {97, "Spoke to Nameless Man intro (cursed) 1"},
                                                                   {98, "Spoke to Nameless Man intro (cursed) 2; Secret of Hana Valley quest started"},
                                                                   {99, "Spoke to Nameless Man post-bloom (cursed)"},
                                                                   {101, "Spoke to Nameless Man at night 1"},
                                                                   {102, "Spoke to Nameless Man at night 2"},
                                                                   {103, "Nameless Man finished giving rice (resets next day)"},
                                                                   {104, "Spoke to Nameless Man post-kiln 1"},
                                                                   {107, "Spoke to Mika intro"},
                                                                   {108, "Spoke to Mika second time"},
                                                                   {109, "Cleared bounty tutorial"},
                                                                   {116, "Spoke to Mika with Kushi"},
                                                                   {121, "Talked with Tama 1"},
                                                                   {122, "Talked with Tama 2"},
                                                                   {123, "Finished talking with Tama"},
                                                                   {124, "Spoke to Tama post-bomb"},
                                                                   {133, "Spoke to Onigiri Sensei first time"},
                                                                   {135, "Dojo opened"},
                                                                   {137, "Spoke to Kushi trying to move sake barrel"},
                                                                   {138, "Spoke to Merchant"},
                                                                   {139, "Spoke to Merchant during pre-festival storm"}};

const std::unordered_map<unsigned, const char *> userIndices = {{0, "Number of times Nameless Man gave you rice"}};

const std::unordered_map<unsigned, const char *> collectedObjects = {{0, "Buried chest between 3 bushes behind merchant (Wooden Bear)"},
                                                                     {1, "Chest behind sapling (Traveler's Charm)"},
                                                                     {2, "Buried chest by dock near Tama's (Vase)"},
                                                                     {3, "Buried chest right side of ramp to moon cave (Stray Bead)"},
                                                                     {4, "Chest hidden by crack near Agata Forest Port (Coral Fragment)"},
                                                                     {5, "Buried chest behind Dojo between bushes (Stray Bead)"},
                                                                     {6, "Chest near entrance from cursed gate (Vengeance Slip)"},
                                                                     {7, "Chest buried between 3 bushes near sapling (Stray Bead)"},
                                                                     {8, "Chest buried between bushes behind kiln (Stray Bead)"},
                                                                     {9, "Chest behind crack at Tama's (Rat Statue)"},
                                                                     {10, "Chest behind crack at cat statue (Exorcism Slip S)"},
                                                                     {11, "Clover behind merchant"},
                                                                     {12, "Clover east side off cliff from path"},
                                                                     {13, "Buried chest under leaves at Yama's house (Bull Horn)"},
                                                                     {14, "Clover in stone near entrance"},
                                                                     {15, "Clover in stone near upper Agata exit"},
                                                                     {16, "Chest top of moon shrine arch right side (Golden Peach)"},
                                                                     {17, "Clover in east catwalk field"},
                                                                     {18, "Buried chest between bushes in east catwalk field (Kutani Pottery)"},
                                                                     {19, "Sapling chest ([Fleeing Battle])"},
                                                                     {20, "Agata Forest port chest ([Feeding])"},
                                                                     {21, "Catwalk field monument chest (Crystal)"},
                                                                     {22, "Clover in north catwalk field"},
                                                                     {24, "Buried chest between bushes in north catwalk field (Pearl)"},
                                                                     {25, "Chest outside Tama's house ([Legend of Orochi])"},
                                                                     {26, "Buried chest under burning leaves at dojo (Godly Charm)"}};

const std::unordered_map<unsigned, const char *> areasRestored = {
    {16, "Gate area by tree"},   {17, "Nameless Man's Kiln"}, {18, "Cursed gate by entrance"},  {19, "Dojo"},
    {20, "North catwalk field"}, {21, "East catwalk field"},  {22, "Cursed grass by entrance"}, {23, "Cursed grass by Agata Port"},
    {31, "Main area restored"}};

const std::unordered_map<unsigned, const char *> treesBloomed = {{1, "Left side from entrance tree 1"},    {2, "Left side from entrance tree 2"},
                                                                 {3, "Right side from entrance tree 1"},   {4, "Right side from entrance tree 2"},
                                                                 {5, "Right side from entrance tree 3"},   {6, "Outside Tama's house"},
                                                                 {7, "Near stairs by Tama's house"},       {8, "Right side of ramp to moon cave 1"},
                                                                 {9, "Right side of ramp to moon cave 2"}, {10, "Left side from entrance tree 3"}};

const std::unordered_map<unsigned, const char *> cursedTreesBloomed = {};

const std::unordered_map<unsigned, const char *> fightsCleared = {
    {0, "Gate by sapling"},          {1, "Gate by Nameless Man"},    {2, "Cursed gate by entrance"},         {3, "Dojo gate"},
    {4, "North catwalk field gate"}, {5, "East catwalk field gate"}, {71, "Izo the String Cutter (bounty)"}, {74, "Fish demon fight"}};

const std::unordered_map<unsigned, const char *> npcs = {{1, "Nameless Man"}, {2, "Mika"}, {3, "Tama"}};

const std::unordered_map<unsigned, const char *> mapsExplored = {};

const std::unordered_map<unsigned, const char *> field_DC = {};

const std::unordered_map<unsigned, const char *> field_E0 = {};

} // namespace okami::game_state::maps::ShinshuField
