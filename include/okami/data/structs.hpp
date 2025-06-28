#pragma once
#include <cstdint>

#include "../animals.hpp"
#include "../bestiarytome.hpp"
#include "../dojotech.hpp"
#include "../fish.hpp"
#include "../movelisttome.hpp"
#include "../straybeads.hpp"
#include "../travelguides.hpp"
#include "../treasures.hpp"
#include "bitfield.hpp"
#include "itemtype.hpp"
#include "maptype.hpp"

namespace hx
{
struct Texture
{
    uint32_t type;
    uint32_t field_4;
    void *pTexture2D;
    uint32_t count;
    uint8_t field_14;
    // 3 bytes padding
};
} // namespace hx

namespace okami
{

// singleton at +0xB4DF90
struct CharacterStats
{
    uint16_t currentHealth;
    uint16_t maxHealth;
    uint16_t currentFood;
    uint16_t maxFood;
    uint16_t unk1;
    uint16_t currentPraise;
    uint16_t totalPraise;
    uint16_t __padding1;

    BitField<DojoTechs::NUM_DOJO_TECHS> dojoTechniquesUnlocked;

    uint32_t unk1b;

    // 0x00 - Divine Retribution
    // 0x01 - Snarling Beast
    // 0x20 - Devout Beads
    uint8_t mainWeapon;
    uint8_t subWeapon;
    uint8_t unk4;
    uint8_t __padding4;

    uint16_t godhood;
    uint16_t __padding5;

    BitField<64> weaponsUpgraded;

    uint16_t vengeanceSlipTimer;
    uint16_t attackIncreaseTimer;
    uint16_t defenseIncreaseTimer;
    uint16_t __padding7;

    float x, y, z; // set from elsewhere
    float u, v, w; // set from elsewhere
};

struct WorldStateData
{
    uint32_t timeOfDay;
    uint16_t day;
    uint16_t unk1; // set from +0xB6B242
    uint16_t unk2; // set from +0xB6B244
    uint16_t unk3;
    uint32_t unk4;
    BitField<64> usableBrushTechniques;   // set from BrushData (+0x8909C0 + 0x70)
    BitField<64> obtainedBrushTechniques; // set from BrushData (+0x8909C0 + 0x78)
    // All entries are 1 by default.
    // Indices 12 and 25 match number of cherry bombs that can be created.
    // Nothing changes for power slash 2.
    uint8_t brushUnknown[64];                     // set from BrushData (+0x8909C0 + 0x80)
    BitField<256> riverOfHeavensRejuvenationBits; // set from BrushData (+0x8909C0 + 0x1F60)

    BitField<32> keyItemsAcquired;
    BitField<32> goldDustsAcquired;

    uint8_t holyArtifactsEquipped[3]; // item id
    uint8_t unk10;                    // padding?
    uint16_t unk11[56];

    BitField<256> mapStateBits[MapTypes::NUM_MAP_TYPES + 1];
    BitField<256> animalsFedBits; // Whether a specific animal group in the
                                  // world has been fed (globally)
    uint16_t numAnimalsFed[Animals::NUM_ANIMALS];
    BitField<4> wantedListsUnlocked;
    BitField<5> bountiesSlain[4];
    // unk15[0] = Current fortune index, -1 = hidden
    // unk15[1] = Current fortune flags, -1 = hidden
    // unk15[2] 0x00004000 sakuya tree bloomed (fruit reward)
    // unk15[2] 0x08000000 taka pass bloomed (fruit reward)
    // unk15[3] gets set when going to map screen first time with quest marker
    // unk15[3] 0x00200000 Agata ruins quest marked
    uint32_t unk15[5];
    uint32_t unk16;
    uint32_t unk17[4];
    uint32_t unk18;
    uint32_t unk19; // Set to some value when first looking at the map in the menu
    uint32_t unk20;

    BitField<128> logbookViewed;

    //
    // unk22[0] 0x80000000 - first fortune viewed
    // unk22[0] 0x40000000 - second fortune viewed
    // unk22[0] 0x20000000 - third fortune viewed
    uint32_t unk22[195];

    uint32_t totalMoney;
    uint32_t totalDemonFangs; // not what you have for spending
    uint32_t totalEnemiesKilled;
    uint32_t unk24[7];
};

// singleton at +0xB205D0
struct CollectionData
{
    uint16_t numSaves;
    uint16_t currentMapId; // set from +0xB6B240
    uint16_t lastMapId;    // set from +0xB6B248
    uint8_t unk1;
    uint8_t unk2;

    uint32_t currentInk;
    uint32_t maxInk;
    uint32_t currentMoney;
    uint8_t walletUpgrades;
    uint8_t healthUpgrades;
    uint8_t unk3;
    uint8_t unk4;

    BitField<StrayBeads::NUM_STRAY_BEADS> strayBeadsCollected;
    BitField<TravelGuides::NUM_TRAVEL_GUIDES> travelGuidesCollected;
    BitField<TravelGuides::NUM_TRAVEL_GUIDES> travelGuidesViewed;
    BitField<MoveListTome::NUM_MOVE_LIST_ENTRIES> dojoMovesCollected; // tome only
    BitField<MoveListTome::NUM_MOVE_LIST_ENTRIES> dojoMovesViewed;
    BitField<FishTome::NUM_FISH_ENTRIES> fishTomesCollected;
    BitField<FishTome::NUM_FISH_ENTRIES> fishTomesViewed;
    BitField<Animals::NUM_ANIMALS> animalTomesCollected;
    BitField<Animals::NUM_ANIMALS> animalTomesViewed;
    BitField<Treasures::NUM_TREASURES> treasureTomesCollected;
    BitField<Treasures::NUM_TREASURES> treasureTomesViewed;

    uint16_t inventory[ItemTypes::NUM_ITEM_TYPES];
    WorldStateData world;
};

// singleton at +0xB21780
struct TrackerData
{
    BitField<ItemTypes::NUM_ITEM_TYPES> firstTimeItem;
    BitField<96> logbookAvailable;
    BitField<64> animalsFedFirstTime;

    // unk1[0] 0x800 -> backstory finished, intro stage started
    BitField<32> field_34;

    // unk1[1] 0x10000000 -> first Headless Guardian fight
    // unk1[1] 0x04000000 -> bandit spider fight
    // unk1[1] 0x08000000 -> first Ubume fight
    // unk1[1] 0x00000040 -> swallowed by giant fish in N Ryoshima
    BitField<32> field_38;

    BitField<32> brushUpgrades;

    uint32_t field_40;
    uint32_t field_44; // changed in save menu

    BitField<32> areasRestored;
    uint16_t field_4C;
    uint16_t field_4E;
    uint16_t field_50;
    uint16_t field_52;
    BitField<BestiaryTome::NUM_BESTIARY_ENTRIES> bestiaryTomeUnlocked;
    BitField<BestiaryTome::NUM_BESTIARY_ENTRIES> bestiaryTomeRead;
    uint8_t unk2;
    uint8_t field_6D;
    uint8_t field_6E;
    uint8_t field_6F;

    // Bits marking visited map locations
    BitField<MapTypes::NUM_MAP_TYPES> mapLocationsRevealed;
    uint32_t timePlayed;
};

struct CustomTexture
{
    uint8_t textureBits[0x8000]; // 16 bits per pixel for 256x256 image
    uint32_t colors1[16];
    uint32_t colors2[16];
    hx::Texture texture[2];
};

// singleton at +0xB21820
// assuming this is specifically for the things you can free-draw in-game
// i.e. the face mask for the Moon Cave
struct CustomTextures
{
    CustomTexture texture1;
    CustomTexture texture2;
};

// singleton at +0x8909C0
struct BrushState
{
    // not worth doing for this mod, but contains the active brush related elements such as unlocked and upgraded brushes
};

struct MapState
{
    uint32_t user[32];             // custom data differs per map
    BitField<96> unburiedObjects;  // set if dug up, same id as collectedObjects
    BitField<96> collectedObjects; // set if chest or other object collected

    BitField<32> commonStates; // Commonly used flags for all maps

    uint32_t timeOfDay;         // Usually synced with world time
    BitField<96> areasRestored; // whether certain map areas are restored, 31 = entire map restored
    BitField<32> treesBloomed;
    BitField<32> cursedTreesBloomed;
    BitField<128> fightsCleared;
    BitField<64> npcHasMoreToSay;
    BitField<64> npcUnknown; // Related to MoreToSay
    BitField<64> mapsExplored;
    BitField<32> field_DC;
    BitField<32> field_E0;
};
} // namespace okami
