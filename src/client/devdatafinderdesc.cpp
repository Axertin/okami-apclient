
#include "devdatafinderdesc.h"

#include <string>
#include <unordered_map>

#include "okami/data/structs.hpp"

namespace okami
{
const std::unordered_map<unsigned, std::string> animalsFedDesc = {
    {1, "Hana Valley campfire - Monkey"},
    {2, "Hana Valley mural - Boar Piglet"},
    {3, "Kamiki village - Chicken"},
    {4, "Kamiki village entrance - Hare"},
    {5, "Kamiki village Hayabusa - Dog"},
    {6, "Kamiki village hill - Sparrow"},
    {7, "Kamiki village pond - Sparrow"},
    {8, "Kamiki village farm - Sparrow"},
    {10, "Shinshu Field gate area by guardian tree - Hare"},
    {11, "Shinshu Field kiln - Hare"},
    {12, "Shinshu Field north catwalk field - Hare"},
    {13, "Shinshu Field east catwalk field - Hare"},
    {14, "Shinshu Field grass patch by entrance - Boar Piglet"},
    {19, "Agata Forest Ume - Dog"},
    {20, "Agata Forest grass patch in front of forest hut - Hare"},
    {21, "Agata Forest near dungeon entrance along path - Sparrow"},
    {22, "Agata Forest by waterfall - Monkey"},
    {23, "Agata Forest Hitoshio Spring - Deer"},
    {25, "Agata Forest Taka Pass exit - Hare"},
    {26, "Agata Forest forest hut - Nightingale"},
    {32, "Taka Pass mermaid spring - Deer"},
    {33, "Taka Pass sapling field path by cave - Horse"},
    {34, "Taka Pass right of Sasa exit - Sparrow"},
    {35, "Taka Pass field between Cutters path and moles - Boar Piglet"},
    {36, "Taka Pass right of Checkpoint exit - Fox"},
    {37, "Shinshu Field Agata Forest Port - Boar"},
    {38, "Shinshu Field dojo - Pig"},
    {39, "Shinshu Field east path - Hare"},
    {40, "Shinshu Field near Tama's - Sparrow"},
    {41, "Shinshu Field near dojo - Horse"},
    {42, "Agata Forest dungeon entrance - Deer"},
    {43, "Agata Forest in front of dungeon entrance by water - Deer"},
    {44, "Agata Forest path to dungeon from Kiba closer to dungeon - Deer"},
    {45, "Agata Forest path to dungeon from Kiba closer to Kiba - Deer"},
    {46, "Agata Forest Madame Fawn's house - Hare"},
    {47, "Agata Forest by Kiba - Sparrow"},
    {48, "Agata Forest sapling - Hare"},
    {49, "Taka Pass bamboo field by origin mirror - Tiger"},
    {50, "Taka Pass near Checkpoint exit - Deer"},
    {51, "Taka Pass Kusa path turn - Monkey"},
    {52, "Taka Pass right side of Cutters path junction - Dog"},
    {53, "Taka Pass intersection to Cutters House entrance - Dog"},
    {54, "Taka Pass path near sapling - Deer"},
    {55, "Taka Pass by Sasa Gate sign - Horse"},
    {56, "Taka Pass in sapling cave - Fox"},
    {79, "Ryoshima Coast front of water cave - Cow"},
    {81, "Ryoshima Coast by graveyard - Fox"},
    {82, "Ryoshima Coast by entrance origin mirror - Dog"},
    {84, "Ryoshima Coast right tri-cursed grass - Boar"},
    {85, "Ryoshima Coast beach front right of merchant - Monkey"},
    {86, "Ryoshima Coast north ryoshima gate - Horse"},
    {86, "Ryoshima Coast outside Seian gate - Hare"},
    {88, "Ryoshima Coast behind bell - Hare"},
    {89, "Ryoshima Coast dojo island - Monkey"},
    {90, "Taka Pass sapling - Nightingale"},
    {100, "Ryoshima Coast Inaba - Hare"},
    {106, "Kamui in front of Wali's house - Sheep"},
    {107, "Kamui bottom of sapling ledge - Bear"},
    {112, "Kamui north area under the bridge - Hare"},
    {114, "Kamui (Ezofuji) lower north ledge - Sheep"},
    {116, "Kamui (Ezofuji) outside power slash 3 cave - Monkey"},
    {117, "Kamui (Ezofuji) west part along path - Bear"},
    {119, "Kamui (Ezofuji) southwest part along path - Raccoon"},
    {137, "Ryoshima Coast middle tri-cursed grass - Boar"},
    {138, "Ryoshima Coast nook by entrance to Seian - Cow"},
    {139, "Ryoshima Coast end of dock - Cat"},
    {140, "Ryoshima Coast sapling - Nightingale"},
    {141, "Ryoshima Coast left tri-cursed grass - Pig"},
    {160, "Taka Pass Stray Bead 26 cave - Monkey"},
};

const std::unordered_map<unsigned, std::string> keyItemsFoundDesc = {
    {6, "Pinwheel"},
};

const std::unordered_map<unsigned, std::string> goldDustsFoundDesc = {
    {0, "Agata Forest Merchant 1"},
    {1, "Agata Forest Merchant 2"},
    {2, "Kusa Village Merchant 1"},
    {3, "Kusa Village Merchant 2"},
};

const std::unordered_map<unsigned, std::string> brushUpgradesDesc = {
    {0, "Power Slash 2"},
    {6, "Cherry Bomb 2"},
    {10, "Power Slash 3"},
    {11, "Cherry Bomb 3"},
};

const std::unordered_map<unsigned, std::string> commonStatesDesc = {
    {30, "All trees bloomed"},
    {31, "Visited"},
};
const std::unordered_map<unsigned, std::string> areasRestoredDesc = {
    {1, "Kamiki Village"}, {2, "Hana Valley"}, {3, "Shinshu Field"}, {4, "Tsuta Ruins"}, {5, "Agata Forest"}, {6, "Taka Pass"},
};

// logbookAvailable
const std::unordered_map<unsigned, std::string> gameProgressDesc = {
    {2, "Battle started after talking with merchant"},
    {3, "Yomigami gives rejuvenation"},
    {4, "Tachigami gives power slash"},
    {6, "After gaining sunrise and entering battle"},
    {7, "After picking up susano first time"},
    {9, "Guardian Sapling Trouble journal added"},
    {10, "Konohana shuffle started"},
    {11, "Sakuya tree bloomed"},
    {12, "lily pad constellation appears"},
    {13, "finished lily pad constellation"},
    {14, "finished lily pad tutorial"},
    {15, "spider queen defeated"},
    {16, "spoke with susano after mural battle in Hana Valley"},
    {17, "mural battle (new enemy) in Hana Valley"},
    {18, "restored mural in Hana Valley"},
    {19, "restored sapling and shown constellation Hana Valley"},
    {20, "greensprout obtained"},
    {24, "revived Shinshu sapling"},
    {26, "Vine constellation shown"},
    {27, "Vine constellation completed"},
    {28, "Bud Ogre enemy introduced"},
    {29, "Spider queen combat started"},
    {30, "tsuta ruins pond room battle triggered"},
    {31, "cherry bomb constellation"},
    {32, "received cherry bomb"},
    {36, "river of heavens stars twinkling sets this"},
    {37, "boulder slash minigame failed"},
    {41, "Mrs Orange gave Cherry Cake for quest"},
    {42, "cave of nagi constellation sets this"},
    {45, "boulder removed"},
    {46, "waka fight"},
    {47, "waka story time"},
    {50, "log minigame started"},
    {51, "got ruins key"},
    {52, "Ume rescued from spider queen, post-dialogue"},
    {53, "Hana Valley bear defeated"},
    {60, "first fish demon encountered"},
    {61, "waka 2 fight started"},
    {62, "waka 2 fight finished"},
};

const std::unordered_map<unsigned, std::string> animalsFedFirstTimeDesc = {
    {16, "Hare"},      {17, "Dog"},   {19, "Sparrow"}, {20, "Monkey"},      {21, "Boar Piglet"},  {22, "Boar"},  {23, "Hayabusa (dog)"},
    {24, "Ume (dog)"}, {31, "Deer"},  {33, "Bear"},    {34, "Pig"},         {37, "Tiger"},        {38, "Horse"}, {39, "Cat"},
    {40, "Cow"},       {41, "Sheep"}, {42, "Raccoon"}, {43, "Nightingale"}, {44, "Inaba (hare)"},
};

} // namespace okami
