#pragma once
#include <cstdint>

#include "maptype.hpp"
#include "itemtype.hpp"
#include "bitfield.hpp"

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
}

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

    BitField<64> dojoTechniquesUnlocked;

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
    BitField<256> riverOfHeavensRejuvinationBits; // set from BrushData (+0x8909C0 + 0x1F60)
    uint32_t unk5[1];
    uint32_t unk6[1];
    uint8_t unk7;
    uint8_t unk8;
    uint8_t unk9;
    uint8_t unk10;
    uint16_t unk11[56];

    BitField<256> mapStateBits[MapTypes::NUM_MAP_TYPES + 1];
    BitField<256> animalsFedBits; // Whether a specific animal group in the world has been fed (globally)
    uint16_t numAnimalsFed[20];
    uint32_t unk15[10]; // wanted lists here
    uint32_t unk16[1];
    uint32_t unk17[4];
    uint32_t unk18;
    uint32_t unk19;
    uint32_t unk20;
    uint32_t unk21[4];

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

    BitField<128> strayBeadsCollected;
    BitField<32> travelGuidesCollected;
    BitField<32> travelGuidesViewed;
    BitField<32> dojoMovesCollected;
    BitField<32> dojoMovesViewed;
    BitField<64> fishTomesCollected;
    BitField<64> fishTomesViewed;
    BitField<32> animalTomesCollected;
    BitField<32> animalTomesViewed;
    BitField<64> treasureTomesCollected;
    BitField<64> treasureTomesViewed;

    uint16_t inventory[ItemTypes::NUM_ITEM_TYPES];
    WorldStateData world;
  };

  // singleton at +0xB21780
  struct TrackerData
  {
    BitField<256> firstTimeItem;
    BitField<96> logbookAvailable;
    BitField<32> animalTomeUnlocked;
    uint32_t unk1[4];
    uint32_t field_40;
    uint32_t field_44;
    uint32_t field_48;
    uint16_t field_4C;
    uint16_t field_4E;
    uint16_t field_50;
    uint16_t field_52;
    BitField<96> bestiaryTomeUnlocked;
    BitField<96> bestiaryTomeRead;
    uint8_t unk2;
    uint8_t field_6D;
    uint8_t field_6E;
    uint8_t field_6F;
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
}
