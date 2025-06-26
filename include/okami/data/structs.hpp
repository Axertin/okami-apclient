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
    uint8_t unk1;
    uint8_t __padding1;
    uint16_t currentPraise;
    uint16_t totalPraise;
    uint8_t __padding2;
    uint8_t __padding3;

    BitField<DojoTechs::NUM_DOJO_TECHS> dojoTechniquesUnlocked;

    uint32_t unk1b;
    uint8_t unk2;
    uint8_t unk3;
    uint8_t unk4;
    uint8_t __padding4;

    uint16_t godhood;
    uint16_t __padding5;

    BitField<64> weaponsUnlocked;

    uint16_t unk5;
    uint16_t unk6;
    uint16_t unk7;
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
    BitField<64> usableBrushTechniques;           // set from BrushData (+0x8909C0 + 0x70)
    BitField<64> obtainedBrushTechniques;         // set from BrushData (+0x8909C0 + 0x78)
    uint8_t brushUpgrades[64];                    // set from BrushData (+0x8909C0 + 0x80)
    BitField<256> riverOfHeavensRejuvenationBits; // set from BrushData (+0x8909C0 + 0x1F60)
    uint32_t unk5;
    uint32_t unk6;
    uint8_t unk7;
    uint8_t unk8;
    uint8_t unk9;
    uint8_t unk10;
    uint16_t unk11[56];

    BitField<256> mapStateBits[MapTypes::NUM_MAP_TYPES + 1];
    BitField<256> animalsFedBits; // Whether a specific animal group in the
                                  // world has been fed (globally)
    uint16_t numAnimalsFed[Animals::NUM_ANIMALS];
    // unk15[8] gets set when going to map screen first time with quest marker
    uint32_t unk15[10]; // wanted lists here
    uint32_t unk16;
    uint32_t unk17[4];
    uint32_t unk18;
    uint32_t unk19; // Set to some value when first looking at the map in the menu
    uint32_t unk20;
    uint32_t unk21[4]; // logbook entry viewed
    // unk21[0] 0x80000000 = Destroy the Boulder! viewed
    // unk21[0] 0x40000000 = Guardian Sapling Trouble viewed
    // unk21[0] 0x20000000 = Secret of Hana Valley viewed
    // unk21[0] 0x10000000 = Revive the Guardian Saplings viewed
    // unk21[0] 0x8000000 = Sacred Tree Reborn viewed
    // unk21[0] 0x4000000 = Shinshu Adventure viewed
    // unk21[2] 0x80 = Cut Down the Peach viewed

    uint8_t unk22[780];

    uint32_t totalMoney;
    uint32_t demonFangs;
    uint32_t enemiesKilled;
    uint8_t unk24[28];
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
    BitField<64> unknown;
    uint32_t unk1[3];
    // unk1[0] 0x800 -> backstory finished, intro stage started

    uint32_t field_40;
    uint32_t field_44; // changed in save menu

    // 0x40000000 = completed cut down peach journal?
    // 0x20000000 = completed Secret of Hana Valley / started Revive the Guardian Saplings
    // 0x10000000 = completed Revive the Guardian Saplings
    uint32_t field_48;
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
    // field_70[2] 0x1000000 - entering shinshu field first time
    // field_70[0] 0x08000000 - Secret of Hana Valley added
    uint32_t field_70[3];
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
    // TODO chonker
};

struct MapState
{
    uint32_t user[32];             // custom data differs per map
    BitField<96> buriedObjects;    // set if dug up
    BitField<96> collectedObjects; // set if chest or other object collected
    BitField<32> field_98;
    uint32_t timeOfDay;         // Usually synced with world time
    BitField<96> areasRestored; // whether certain map areas are restored
    BitField<32> treesBloomed;
    BitField<32> field_B0;
    BitField<128> hellGatesCleared;
    BitField<64> npcHasMoreToSay;
    BitField<64> npcUnknown; // Related to MoreToSay but never seen it set
    BitField<64> field_D4;
    BitField<32> field_DC;
    BitField<32> field_E0;
};
} // namespace okami
