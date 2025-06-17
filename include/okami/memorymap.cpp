#include "memorymap.hpp"

#include <cstdint>
#include <iostream>

#include "memoryaccessor.hpp"
#include "brushes.hpp"
#include "items.hpp"
#include "maps.hpp"
#include "dojotech.hpp"
#include "straybeads.hpp"
#include "travelguides.hpp"
#include "animals.hpp"
#include "treasures.hpp"
#include "fish.hpp"
#include "acquisition.hpp"

namespace okami
{
    // Base addresses
    uintptr_t MainBase;
    uintptr_t FlowerBase;

    MemoryAccessor<uint32_t> IngameTimeFrames;

    MemoryAccessor<float> AmmyPosX;
    MemoryAccessor<float> AmmyPosY;
    MemoryAccessor<float> AmmyPosZ;
    MemoryAccessor<int16_t> AmmyCurrentHealth;
    MemoryAccessor<int16_t> AmmyMaxHealth;
    MemoryAccessor<uint8_t> HealthUpgradeCount;
    MemoryAccessor<int32_t> AmmyCurrentInk;
    MemoryAccessor<int32_t> AmmyMaxInk;
    MemoryAccessor<int32_t> BrushTargetInk;
    MemoryAccessor<int32_t> BrushCurrentInk;
    MemoryAccessor<uint8_t> InkUpgradeCount;
    MemoryAccessor<uint8_t> CanvasBrushStrokes;
    MemoryAccessor<int16_t> AmmyCurrentFood;
    MemoryAccessor<int16_t> AmmyMaxFood;
    MemoryAccessor<uint8_t> FoodUpgradeCount;
    MemoryAccessor<int16_t> AmmyGodhood;
    MemoryAccessor<uint32_t> AmmyCurrentMoney;
    MemoryAccessor<uint32_t> AmmyDisplayedMoney;
    MemoryAccessor<uint32_t> AmmyMaxMoney; // TODO: Make Array of length 4
    MemoryAccessor<uint32_t> AmmyTotalMoney;
    MemoryAccessor<uint8_t> MoneyUpgradeCount;
    MemoryAccessor<int16_t> AmmyCurrentPraise;
    MemoryAccessor<int16_t> PraiseUpgradeBar;
    MemoryAccessor<int16_t> HealthUpgradeBar;
    MemoryAccessor<int16_t> FoodUpgradeBar;
    MemoryAccessor<int16_t> MoneyUpgradeBar;
    MemoryAccessor<int16_t> InkUpgradeBar;
    MemoryAccessor<int16_t> AmmyTotalPraise;
    MemoryAccessor<int16_t> AmmyCurrentDemonFangs;
    MemoryAccessor<uint16_t> AmmyTotalDemonFangs;

    MemoryAccessor<uint16_t> AstralPouchAcqFlag;

    // Other important things
    MemoryAccessor<uint8_t> LoadingZoneTrigger;

    void *MaybeInventoryStructPtr;

    // Game function pointers
    char *GetSaveDataRootDirectoryFnPtr;
    void *MainFlowerStartupFnPtr;
    void *MainFlowerStopFnPtr;
    void *MainFlowerTickFnPtr;
    void *MainFlowerLoadPtr;
    void *MainFlowerTreasurePickedUpPtr;
    void *MainFlowerItemPickupFnPtr;

    // Overlay structures
    MemoryAccessor<BitfieldFlags<BrushOverlay>> AmmyUsableBrushTechniques;
    MemoryAccessor<BitfieldFlags<BrushOverlay>> AmmyObtainedBrushTechniques;
    MemoryAccessor<BitfieldFlags<DojoOverlay>> AmmyUsableDojoTechniques;
    MemoryAccessor<BitfieldFlags<StrayBeadOverlay>> CollectionStrayBeads;
    MemoryAccessor<BitfieldFlags<TravelGuideOverlay>> CollectionTravelGuides;
    MemoryAccessor<BitfieldFlags<AnimalOverlay>> CollectionAninals;
    MemoryAccessor<BitfieldFlags<FishOverlay>> CollectionFish;
    MemoryAccessor<BitfieldFlags<TreasureOverlay>> CollectionTreasure;
    MemoryAccessor<BitfieldFlags<TreasureOverlay>> CollectionAcquisitions;

    /**
     * @brief Initialize game variable pointers and overlays.
     * Must be called after `okami::MainBase` is assigned.
     */
    void initVariables()
    {
        IngameTimeFrames.bind(okami::MainBase + 0xB217FC);

        // AmmyPosX.bind(okami::MainBase + 0xB6B2D0 + 0xA8 + 0);
        // AmmyPosY.bind(okami::MainBase + 0xB6B2D0 + 0xA8 + 4);
        // AmmyPosZ.bind(okami::MainBase + 0xB6B2D0 + 0xA8 + 8);
        AmmyPosX.bind(okami::MainBase + 0xB1EC10 + 0); // probably read-only copies
        AmmyPosY.bind(okami::MainBase + 0xB1EC10 + 4); // probably read-only copies
        AmmyPosZ.bind(okami::MainBase + 0xB1EC10 + 8); // probably read-only copies

        AmmyCurrentHealth.bind(okami::MainBase + 0xB4DF90);
        AmmyMaxHealth.bind(okami::MainBase + 0xB4DF92);
        HealthUpgradeCount.bind(okami::MainBase + 0xB205E5);

        AmmyCurrentInk.bind(okami::MainBase + 0xB205D8);
        AmmyMaxInk.bind(okami::MainBase + 0xB205DC);
        BrushTargetInk.bind(okami::MainBase + 0x8928A4);
        BrushCurrentInk.bind(okami::MainBase + 0x8928A8);
        InkUpgradeCount.bind(okami::MainBase + 0xB1F208);
        CanvasBrushStrokes.bind(okami::MainBase + 0x888C58);

        AmmyCurrentFood.bind(okami::MainBase + 0xB4DF94);
        AmmyMaxFood.bind(okami::MainBase + 0xB4DF96);
        FoodUpgradeCount.bind(okami::MainBase + 0xB1F207);

        AmmyGodhood.bind(okami::MainBase + 0xB4DFAC);

        AmmyCurrentMoney.bind(okami::MainBase + 0xB205E0);
        AmmyDisplayedMoney.bind(okami::MainBase + 0xB1CFE4);
        AmmyMaxMoney.bind(okami::MainBase + 0x6B22A8);
        AmmyTotalMoney.bind(okami::MainBase + 0xB21758);
        MoneyUpgradeCount.bind(okami::MainBase + 0xB205E4);

        AmmyCurrentPraise.bind(okami::MainBase + 0xB4DF9A);
        AmmyTotalPraise.bind(okami::MainBase + 0xB4DF9C);
        AmmyCurrentDemonFangs.bind(okami::MainBase + 0xB2066E);
        AmmyTotalDemonFangs.bind(okami::MainBase + 0xB2175C);

        PraiseUpgradeBar.bind(okami::MainBase + 0xB1F1F4);
        HealthUpgradeBar.bind(okami::MainBase + 0xB1F1F6);
        FoodUpgradeBar.bind(okami::MainBase + 0xB1F1F8);
        MoneyUpgradeBar.bind(okami::MainBase + 0xB1F1FA);
        InkUpgradeBar.bind(okami::MainBase + 0xB1F1FC);

        InventoryItemID.bind(okami::MainBase + 0x7A9814);
        InventoryStringID.bind(okami::MainBase + 0x9C148E);
        // InventoryFirstItemOffset.bind(okami::MainBase + 0xB1F401);
        // InventoryColumn.bind(okami::MainBase + 0xB1F402);
        // InventorySelectedItemRow.bind(okami::MainBase + 0xB1F403);
        AmmyCurrentSunFragments.bind(okami::MainBase + 0xB2063A);
        InventoryItemQualities.bind(okami::MainBase + 0xB20630);

        ExteriorMapID.bind(okami::MainBase + 0xB6B240);
        CurrentMapID.bind(okami::MainBase + 0xB65E74);
        VestigialMapID1.bind(okami::MainBase + 0xB4F0B4);
        VestigialMapID2.bind(okami::MainBase + 0xB6B246);

        // Overlay memory helpers
        AmmyUsableBrushTechniques.bind<4>(okami::MainBase + 0x890A30);
        AmmyObtainedBrushTechniques.bind<4>(okami::MainBase + 0x890A38);
        AmmyUsableDojoTechniques.bind<4>(okami::MainBase + 0xB4DFA0);
        CollectionStrayBeads.bind<32>(okami::MainBase + 0xB205E8);
        CollectionTravelGuides.bind<4>(okami::MainBase + 0xB205F8);
        CollectionAninals.bind<4>(okami::MainBase + 0xB20618);
        CollectionFish.bind<4>(okami::MainBase + 0xB20608);
        CollectionTreasure.bind<4>(okami::MainBase + 0xB20620);
        CollectionAcquisitions.bind<4>(okami::MainBase + 0xB21780);

        // TODO: MoveList

        AstralPouchAcqFlag.bind(okami::MainBase + 0xB2063C);

        LoadingZoneTrigger.bind(okami::MainBase + 0xB6B2AF);

        MaybeInventoryStructPtr = reinterpret_cast<void *>(okami::MainBase + 0xB66670);
    }

    /**
     * @brief Initialize major engine function pointers from known okami::MainBase offsets.
     */
    void initFunctions()
    {
        GetSaveDataRootDirectoryFnPtr = reinterpret_cast<char *>(okami::FlowerBase + 0x22F7);
        MainFlowerStartupFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4B6240);
        MainFlowerStopFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4B6230);
        MainFlowerTickFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4B63B0);
        MainFlowerLoadPtr = reinterpret_cast<void *>(okami::MainBase + 0x4390A0);
        MainFlowerTreasurePickedUpPtr = reinterpret_cast<void *>(okami::MainBase + 0x436AE0);
        MainFlowerItemPickupFnPtr = reinterpret_cast<void *>(okami::MainBase + 0x4965D0);
    }
} // namespace okami
