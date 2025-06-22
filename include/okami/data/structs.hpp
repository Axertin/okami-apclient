#pragma once
#include <cstdint>

#include "maptype.hpp"
#include "itemtype.hpp"

namespace okami {

  // singleton at +0xB4DF90
  struct CharacterStats {
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

    uint32_t dojoTechniquesUnlocked[2];

    uint8_t unk2;
    uint8_t unk3;
    uint8_t unk4;
    uint8_t __padding4;

    uint16_t godhood;
    uint16_t __padding5;

    uint64_t weaponsUnlocked;

    uint16_t unk5;
    uint16_t unk6;
    uint16_t unk7;
    uint16_t __padding7;

    float x, y, z;  // set from elsewhere
    float u, v, w;  // set from elsewhere
  };

  struct WorldStateData {
    uint32_t timeOfDay;
    uint16_t day;
    uint16_t unk1;  // set from +0xB6B242
    uint16_t unk2;  // set from +0xB6B244
    uint16_t unk3;
    uint32_t unk4;
    uint32_t usableBrushTechniques[2];          // set from BrushData (+0x8909C0 + 0x70)
    uint32_t obtainedBrushTechniques[2];        // set from BrushData (+0x8909C0 + 0x78)
    uint8_t brushUpgrades[64];                  // set from BrushData (+0x8909C0 + 0x80)
    uint32_t riverOfHeavensRejuvinationBits[8]; // set from BrushData (+0x8909C0 + 0x1F60)
    uint32_t unk5[1];
    uint32_t unk6[1];
    uint8_t unk7;
    uint8_t unk8;
    uint8_t unk9;
    uint8_t unk10;
    uint16_t unk11[56];

    uint32_t mapStateBits[MapTypes::NUM_MAP_TYPES + 1][8];
    uint32_t animalsFedBits[5];
    uint32_t unk12;
    uint32_t unk13;
    uint32_t unk14;
    uint32_t unk15[10];
    uint32_t unk16[1];
    uint32_t unk17[4];
    uint16_t numAnimalsFed[20];
    uint32_t unk18;
    uint32_t unk19;
    uint32_t unk20;
    uint32_t unk21[4];

    uint8_t unk22[780];

    uint32_t totalMoney;
    uint16_t demonFangs;
    uint8_t unk23[2];
    uint16_t enemiesKilled;
    uint8_t unk24[30];
  };

  // singleton at +0xB205D0
  struct CollectionData {
    uint16_t numSaves;
    uint16_t currentMapId;  // set from +0xB6B240
    uint16_t lastMapId;     // set from +0xB6B248
    uint8_t unk1;
    uint8_t unk2;
    
    uint32_t currentInk;
    uint32_t maxInk;
    uint32_t currentMoney;
    uint8_t walletUpgrades;
    uint8_t healthUpgrades;
    uint8_t unk3;
    uint8_t unk4;

    uint32_t strayBeadsCollected[4];
    uint32_t travelGuidesCollected;
    uint32_t travelGuidesViewed;
    uint32_t dojoMovesCollected;
    uint32_t dojoMovesViewed;
    uint32_t fishTomesCollected[2];
    uint32_t fishTomesViewed[2];
    uint32_t animalTomesCollected;
    uint32_t animalTomesViewed;
    uint32_t treasureTomesCollected[2];
    uint32_t treasureTomesViewed[2];

    uint16_t inventory[ItemTypes::NUM_ITEM_TYPES];
    WorldStateData world;
  };

  // singleton at +0xB21780
  struct TrackerData {
    uint32_t firstTimeItem[8];
    uint32_t logbookAvailable[3];
    uint8_t unk0;
    uint8_t animalTomeUnlocked[4];
    uint8_t unk1[15];
    uint32_t field_40;
    uint32_t field_44;
    uint32_t field_48;
    uint16_t field_4C;
    uint16_t field_4E;
    uint16_t field_50;
    uint16_t field_52;
    uint32_t bestiaryTomeUnlocked[3];
    uint32_t bestiaryTomeRead[3];
    uint8_t unk2;
    uint8_t field_6D;
    uint8_t field_6E;
    uint8_t field_6F;
    uint32_t field_70[3];
    uint32_t timePlayed;
  };

  // singleton at +0xB21820
  // assuming this is specifically for the things you can free-draw in-game
  // i.e. the face mask for the Moon Cave
  struct CustomTextureBits {
    uint8_t textureBits1[32896];  // vector of hx::Texture
    uint8_t textureBits2[32944];  // vector of hx::Texture
    uint8_t unk2[48];
  };

  // singleton at +0x8909C0
  struct BrushState {
    // TODO chonker
  };
}
