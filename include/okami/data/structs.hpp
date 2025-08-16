#pragma once
#include <cstdint>

#include "../animals.hpp"
#include "../bestiarytome.hpp"
#include "../dojotech.hpp"
#include "../fish.hpp"
#include "../movelisttome.hpp"
#include "../shopdata.h"
#include "../straybeads.hpp"
#include "../travelguides.hpp"
#include "../treasures.hpp"
#include "bitfield.hpp"
#include "itemtype.hpp"
#include "maptype.hpp"
#include "wkmath.hpp"

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

    // upper word = category, lower word = weapon
    // 0x Reflector
    //   00 - Divine Retribution
    //   01 - Snarling Beast
    //   02 - Infinity Judge
    //   03 - Trinity Mirror
    //   04 - Solar Flare
    // 1x Glaive
    //   10 - Tsumugari
    //   11 - Seven Strike
    //   12 - Blade of Kusanagi
    //   13 - Eighth Wonder
    //   14 - Thunder Edge
    // 2x Rosary
    //   20 - Devout Beads
    //   21 - Life Beads
    //   22 - Exorcism Beads
    //   23 - Resurrection Beads
    //   24 - Tundra Beads
    // FF None
    uint8_t mainWeapon;
    uint8_t subWeapon;

    // Current Karmic transformation
    // 7 = Karmic Transformer 1
    // 8 = Karmic Transformer 6
    // 9 = Karmic Transformer 5
    // 10 = Karmic Transformer 4
    // 11 = Karmic Transformer 2
    // 12 = Karmic Transformer 9
    // 13 = Karmic Transformer 7
    // 14 = Karmic Transformer 3
    // 15 = Karmic Transformer 8
    uint8_t currentTransformation;
    uint8_t __padding4;

    uint16_t godhood;
    uint16_t __padding5;

    // Bit set is `1 << weaponID` (ID from weapon slots)
    uint64_t weaponsUpgraded;

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
    // maybe actually brush upgrades, need to test modification of BrushData
    uint8_t brushUnknown[64];                     // set from BrushData (+0x8909C0 + 0x80)
    BitField<256> riverOfHeavensRejuvenationBits; // set from BrushData (+0x8909C0 + 0x1F60)

    // From shops
    BitField<32> keyItemsAcquired;
    BitField<32> goldDustsAcquired;

    uint8_t holyArtifactsEquipped[3]; // item id
    uint8_t unk10;                    // padding?
    uint16_t unk11[56];

    BitField<256> mapStateBits[MapTypes::NUM_MAP_TYPES + 1];
    BitField<256> animalsFedBits; // Whether a specific animal group in the world has been fed (globally)
    uint16_t numAnimalsFed[Animals::NUM_ANIMALS];
    BitField<4> wantedListsUnlocked;
    BitField<5> bountiesSlain[4];

    int32_t currentFortune;       // -1 = hidden
    uint32_t currentFortuneFlags; // -1 = hidden

    // unk15[0] 0x00004000 sakuya tree bloomed (fruit reward)
    // unk15[0] 0x08000000 taka pass bloomed (fruit reward)
    // unk15[1] gets set when going to map screen first time with quest marker
    // unk15[1] 0x00200000 Agata ruins quest marked
    uint32_t unk15[3];
    uint32_t unk16;
    uint32_t unk17[4];
    uint32_t unk18;
    uint32_t unk19; // Set to some value when first looking at the map in the menu
    uint32_t unk20;

    BitField<128> logbookViewed;
    BitField<32> fortuneViewed;

    uint32_t unk22[194];

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
    BitField<96> gameProgressionBits;
    BitField<64> animalsFedFirstTime;

    // unk1[0] 0x800 -> backstory finished, intro stage started
    BitField<32> field_34;

    // unk1[1] 0x10000000 -> first Headless Guardian fight
    // unk1[1] 0x04000000 -> bandit spider fight
    // unk1[1] 0x08000000 -> first Ubume fight
    // unk1[1] 0x00000040 -> swallowed by giant fish in N Ryoshima
    BitField<32> field_38;

    BitField<32> brushUpgrades;

    uint32_t gameOverCount;

    // 1 = Controller Vibration: Off
    // 2 = Camera Control: Invert Y Axis
    // 4 = Just saved
    // 6 = Camera Control: Invert X Axis
    // 9 = ??? gives all karmic transformers when set and specific code called
    // 10 = Filter: Light
    // 11 = Filter: Heavy
    // 12 = Aspect Ratio: 4:3
    // 13 = Mini-Games: On
    BitField<32> optionFlags;

    BitField<32> areasRestored;
    uint16_t volumeBGM; // 0 through 127
    uint16_t volumeSE;
    uint16_t volumeVoice;
    uint16_t field_52;
    BitField<BestiaryTome::NUM_BESTIARY_ENTRIES> bestiaryTomeUnlocked;
    BitField<BestiaryTome::NUM_BESTIARY_ENTRIES> bestiaryTomeRead;
    uint8_t unk2;
    uint8_t field_6D; // padding
    uint8_t field_6E; // padding
    uint8_t field_6F; // padding

    // Bits marking visited map locations
    BitField<MapTypes::NUM_MAP_TYPES> areaVisitedFlags;
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

struct DummyVirtTable
{
    void *vtable;
};

struct cObjGui
{
    int64_t field_0;
    void *pUiElements;
    int64_t field_10;
    void *pUiIdxNum;
    int32_t numUiElements;
    int32_t field_24;
    int32_t field_28;
    char field_2C;
    char field_2D;
    char field_2E;
    char field_2F;
};

struct ShopInventory
{
    uint32_t itemType;
    uint32_t cost;
};

struct ShopSlotData
{
    int32_t itemType;
    int32_t field_4;
    hx::Texture *pIcon;
    int32_t itemCost;
    int16_t itemNameStrId;
    int16_t maxCount;
    int16_t currentCount;
    int16_t field_1A;
    int32_t field_1C;
};

struct cShopBase : public DummyVirtTable
{
    cObjGui gui;
    ShopInventory *inventory;
    ShopInventory *inventorySorted;
    ShopSlotData *shopSlots;
    const char *pszShopNpcImgFile;
    void *pShopRscPkg;
    void *pIconsRsc;
    void *pShopNPCImg;
    hx::Texture *pShopNPCImgTexture;
    char hasShopNPCTexture;
    char field_71; // probably padding
    char field_72;
    char field_73;
    int32_t shopIdRsc;
    int32_t shopNPCTextureRsc;
    char field_7C;
    char field_7D;
    int16_t purchaseQuantity;
    uint8_t numSlots;
    uint8_t numVisibleSlots;
    char scrollOffset;
    char visualSelectIndex;
    char field_84;
    char field_85;
    char field_86;
    char field_87;
    int32_t field_88;
    char interactSeqState;
    char actionState;
    char actionSubstate;
    char field_8F;
};

struct cItemShop : cShopBase
{
    ItemShopStock *itemStockList;
    int32_t *itemSellCosts;
    int32_t iconRef;
    int32_t shopStockVariation;
    float field_B0;
    float field_B4;
    float field_B8;
    char field_BC;
    char shopModeSel;
    char field_BE;
    uint8_t numItemSlots;
    char field_C0;
    char field_C1;
    char field_C2;
    char field_C3;
    int32_t inputController;
    int32_t lastPressedController;
    int32_t field_CC;
};

struct cKibaShop : cShopBase
{
    ItemShopStock *itemStockList;
    uint32_t numShopItems;
    int32_t field_A4;
    float field_A8;
    float field_AC;
    float field_B0;
    char field_B4;
    char field_B5;
    char field_B6;
    char field_B7;
    int32_t inputController;
    int32_t lastPressedController;
};

struct SkillShopStock
{
    uint16_t skillId;
    uint16_t field_2;
    int32_t cost;
    int32_t field_8;
    int32_t field_C;
};

struct cSkillShop : cShopBase
{
    SkillShopStock *skillList;
    int32_t field_A0;
    int32_t purchasedSkillId;
    float field_A8;
    float field_AC;
    float field_B0;
    uint8_t numSkillSlots;
    uint8_t field_B5[3];
    int32_t inputController;
    int32_t lastPressedController;
};

// Don't know about name, relationships, or size
// This is pl00 I think?
struct cAmmyModel
{
    void *vtable;
    int32_t whatever[14];
    wk::math::cVec savedPos;
    wk::math::cMatrix mtx;
    int32_t field_90[4];
    wk::math::cVec *pSpawnPosition;
    wk::math::cVec *pPosition;
    // lots more below but we don't care
};

struct ItemParam
{
    uint16_t maxCount;
    int16_t value;
    uint32_t flags;
    uint8_t category;
    uint8_t padding[3];
};
} // namespace okami
