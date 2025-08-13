#include "gamehooks.h"

#include "aplocationmonitor.h"
#include "archipelagosocket.h"
#include "devdatafinder.h"
#include "logger.h"
#include "okami/memorymap.hpp"
#include "okami/msd.h"
#include "okami/resource.h"
#include "okami/shopdata.h"
#include "shop.h"

#include <MinHook.h>

typedef void(__cdecl *FunctionType)();

static FunctionType Flower_ReturnToMenu = nullptr;
/// @brief Hook for the function called when "return to menu" is selected from
/// an active save
/// @return void
void __cdecl GameHooks::onReturnToMenu()
{
    Flower_ReturnToMenu();
}

static FunctionType Main_FlowerStopOrigin = nullptr;
// TODO: Verify that this is what I think it is
/// @brief Hook for the function called when the game stops
/// @return void
void __cdecl GameHooks::onGameStop()
{
    logInfo("[gamehooks] Cleaning Up...");
    GameHooks::cleanup();
    Main_FlowerStopOrigin();
}

static FunctionType Main_FlowerTickOrigin = nullptr;
/// @brief Hook for the game's mainloop. Runs every frame.
/// @return void
void __cdecl GameHooks::onGameTick()
{
    okami::devDataFinder_OnGameTick();
    // checkBrushes();
    auto &apsocket = ArchipelagoSocket::instance();
    apsocket.poll();
    apsocket.processMainThreadTasks();

    APLocationMonitor::instance().update();

    // call vanilla tick
    Main_FlowerTickOrigin();
}

typedef void(__fastcall *ItemPickupFn)(void *rcx, int edx, int r8);
static ItemPickupFn oItemPickup = nullptr;
void __fastcall GameHooks::onItemPickup(void *MaybeInventoryStruct, int ItemID, int NumItems)
{
    logDebug("[gamehooks] Picked up %d %s", NumItems, okami::ItemTypes::GetName(ItemID));
    if (NumItems > 0)
    {
        logInfo("[gamehooks] Calling APLocationMonitor::onItemPickup with item=%d, quantity=%d", ItemID, NumItems);
        APLocationMonitor::instance().onItemPickup(ItemID, NumItems);
    }

    // Check if we should supress vanilla
    // TODO: determine a better way to tell what to supress
    // if (checkItems(ItemID))
    // {
    //     return;
    // }

    return oItemPickup(MaybeInventoryStruct, ItemID, NumItems);
}

typedef bool(__fastcall *EditBrushesFn)(void *MaybeInventoryStruct, int BitIndex, int Operation);
static EditBrushesFn oEditBrushes = nullptr;
bool __fastcall GameHooks::onBrushEdit(void *MaybeInventoryStruct, int BitIndex, int Operation)
{
    logDebug("[gamehooks] EditBrushes called with 0x%p, 0x%X, 0x%X", MaybeInventoryStruct, BitIndex, Operation);

    // return oEditBrushes(MaybeInventoryStruct, BitIndex, Operation);

    return true;
}

static int64_t(__fastcall *oGetShopVariation)(void *, uint32_t shopNum, char **pszShopTextureName);
static const void *(__fastcall *oGetShopMetadata)(void *, uint32_t shopNum, uint32_t *pNumEvents, char **pszShopTextureName);
static const void *(__fastcall *oLoadRsc)(void *pRscPackage, const char *pszType, uint32_t nIdx);
static okami::ItemShopStock *(__fastcall *ocKibaShop__GetShopStockList)(void *, uint32_t *numItems);

static int64_t __fastcall onGetShopVariation(void *pUnk, uint32_t shopNum, char **pszShopTextureName)
{
    // Call the original metadata function but drop most of its details.
    // This prevents the use of variations (and therefore requiring defining multiple duplicate shops with just 1 item difference)
    uint32_t n;
    oGetShopMetadata(pUnk, shopNum, &n, pszShopTextureName);

    return 0;
}

static const void *__fastcall onLoadRsc(void *pRscPackage, const char *pszType, uint32_t nIdx)
{
    if (strcmp(pszType, "ISL") == 0)
    {
        const void *pResult = GetCurrentItemShopData(nIdx);
        if (pResult == nullptr)
        {
            pResult = oLoadRsc(pRscPackage, pszType, nIdx);
        }
        return pResult;
    }
    else if (strcmp(pszType, "SSL") == 0)
    {
        // Skill shop
        // TODO
        return oLoadRsc(pRscPackage, pszType, nIdx);
    }
    else
    {
        return oLoadRsc(pRscPackage, pszType, nIdx);
    }
}

static okami::ItemShopStock *__fastcall onCKibaShop__GetShopStockList(void *pKibaShop, uint32_t *numItems)
{
    okami::ItemShopStock *pResult = GetCurrentDemonFangShopData(numItems);
    if (pResult == nullptr)
    {
        pResult = ocKibaShop__GetShopStockList(pKibaShop, numItems);
    }
    return pResult;
}

void GameHooks::giveItem(int ItemID, int NumItems)
{
    oItemPickup(okami::MaybeInventoryStructPtr, ItemID, NumItems);
}

template <class T> void CreateHook(uintptr_t base, uintptr_t offset, T *pDetour, T **ppOriginal)
{
    MH_CreateHook(reinterpret_cast<void *>(base + offset), reinterpret_cast<LPVOID>(pDetour), reinterpret_cast<LPVOID *>(ppOriginal));
}

static void *(__fastcall *oLoadRscIdx)(void *pPkg, uint32_t idx);
static void *(__fastcall *oCItemShop_GetItemIcon)(okami::cItemShop *pShop, int item);
void *__fastcall onCItemShop_GetItemIcon(okami::cItemShop *pShop, int item)
{
    if (item == 0 || !pShop->pIconsRsc)
    {
        // original call still needed as it returns a blank graphic from sgpCore20BinRsc
        return oCItemShop_GetItemIcon(pShop, item);
    }

    // First item in a package is index 1
    return oLoadRscIdx(pShop->pIconsRsc, item + 1);
}

static void(__fastcall *oCItemShop_SortInventory)(okami::cShopBase *pShop, uint8_t numSlots);
void __fastcall onCItemShop_SortInventory(okami::cShopBase *pShop, uint8_t numSlots)
{
    // Enforces specific item ordering. Side effect of filtering out items that are "not valid".
    // We don't care about either, and can use our shop definition ordering.
    for (uint32_t i = 0; i < numSlots; i++)
    {
        pShop->inventorySorted[i] = pShop->inventory[i];
    }
}

static void *(__fastcall *oLoadResourcePackageAsync)(void *pFilesystem, const char *pszFilename, void **pOutputRscData, void *pHeap, int32_t, int32_t, int32_t,
                                                     int32_t);
void *__fastcall onLoadResourcePackageAsync(void *pFilesystem, const char *pszFilename, void **pOutputRscData, void *pHeap, int32_t a5, int32_t a6, int32_t a7,
                                            int32_t a8)
{
    // Intercept item shop icons
    if (pszFilename && strcmp(pszFilename, "id/ItemShopBuyIcon.dat") == 0)
    {
        pszFilename = "archipelago/ItemPackage.dat";
    }

    return oLoadResourcePackageAsync(pFilesystem, pszFilename, pOutputRscData, pHeap, a5, a6, a7, a8);
}

static int64_t(__fastcall *oGXTextureManager_GetNumEntries)(void *pTextureManager, int32_t texGroup);
int64_t __fastcall onGXTextureManager_GetNumEntries(void *pTextureManager, int32_t texGroup)
{
    if (texGroup == 4)
    {
        // Originally was 128 but we have more than 256 icons
        // return 300; // FIXME: uncomment when injection on suspend is finished
    }
    return oGXTextureManager_GetNumEntries(pTextureManager, texGroup);
}

// WARN: Update ppCore20MSD every time this changes
//       Do NOT change during gameplay!
// also maybe move logic out of here to elsewhere
static okami::MSDManager Core20MSD;
static bool msdInitialized = false;
constexpr uint32_t ItemStrBaseID = 294;

static uint32_t TestItemTextID = 0;
static void(__fastcall *oLoadCore20MSD)(void *pMsgStruct);
static const void **ppCore20MSD;
void __fastcall onLoadCore20MSD(void *pMsgStruct)
{
    oLoadCore20MSD(pMsgStruct);
    if (!*ppCore20MSD)
        return;

    if (!msdInitialized)
    {
        Core20MSD.ReadMSD(*ppCore20MSD);
        Core20MSD.OverrideString(okami::ItemTypes::HourglassOrb + ItemStrBaseID, "Hourglass Orb");
        Core20MSD.OverrideString(okami::ItemTypes::Yen10 + ItemStrBaseID, "10 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Yen50 + ItemStrBaseID, "50 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Yen100 + ItemStrBaseID, "100 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Yen150 + ItemStrBaseID, "150 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Yen500 + ItemStrBaseID, "500 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Praise + ItemStrBaseID, "Praise");

        TestItemTextID = Core20MSD.AddString("Heinermann's Morph Ball");

        msdInitialized = true;
    }
    *ppCore20MSD = Core20MSD.GetData();
}

static uint16_t(__fastcall *oGetItemNameStrId)(void *, uint16_t item);
uint16_t __fastcall onGetItemNameStrId(void *, uint16_t item)
{
    if (item == okami::ItemTypes::Unused_52)
    {
        return TestItemTextID;
    }
    return item + 294;
}

/**
 * @brief Setup game-related hooks
 *
 */
void GameHooks::setup()
{
    logInfo("[gamehooks] Initializing MinHook...");
    if (MH_Initialize() != MH_OK)
    {
        logError("[gamehooks] Failed to initialize MinHook!");
        return;
    }

    MH_CreateHook(okami::MainFlowerStartupFnPtr, reinterpret_cast<LPVOID>(&onReturnToMenu), reinterpret_cast<LPVOID *>(&Flower_ReturnToMenu));
    MH_CreateHook(okami::MainFlowerStopFnPtr, reinterpret_cast<LPVOID>(&onGameStop), reinterpret_cast<LPVOID *>(&Main_FlowerStopOrigin));
    MH_CreateHook(okami::MainFlowerTickFnPtr, reinterpret_cast<LPVOID>(&onGameTick), reinterpret_cast<LPVOID *>(&Main_FlowerTickOrigin));
    MH_CreateHook(okami::MainFlowerItemPickupFnPtr, reinterpret_cast<LPVOID>(&onItemPickup), reinterpret_cast<LPVOID *>(&oItemPickup));
    MH_CreateHook(okami::EditBrushesFnPtr, reinterpret_cast<LPVOID>(&onBrushEdit), reinterpret_cast<LPVOID *>(&oEditBrushes));

    CreateHook(okami::MainBase, 0x4420C0, &onGetShopVariation, &oGetShopVariation);
    CreateHook(okami::MainBase, 0x1B1770, &onLoadRsc, &oLoadRsc);
    CreateHook(okami::MainBase, 0x43F5A0, &onCKibaShop__GetShopStockList, &ocKibaShop__GetShopStockList);
    CreateHook(okami::MainBase, 0x43BDA0, &onCItemShop_GetItemIcon, &oCItemShop_GetItemIcon);
    CreateHook(okami::MainBase, 0x1AFC90, &onLoadResourcePackageAsync, &oLoadResourcePackageAsync);
    CreateHook(okami::MainBase, 0x441A70, &onCItemShop_SortInventory, &oCItemShop_SortInventory);
    CreateHook(okami::MainBase, 0x1412B0, &onGXTextureManager_GetNumEntries, &oGXTextureManager_GetNumEntries);
    CreateHook(okami::MainBase, 0x1C9510, &onLoadCore20MSD, &oLoadCore20MSD);
    CreateHook(okami::MainBase, 0x496F40, &onGetItemNameStrId, &oGetItemNameStrId);

    oGetShopMetadata = reinterpret_cast<decltype(oGetShopMetadata)>(okami::MainBase + 0x441E40);
    oLoadRscIdx = reinterpret_cast<decltype(oLoadRscIdx)>(okami::MainBase + 0x1B16C0);
    ppCore20MSD = reinterpret_cast<const void **>(okami::MainBase + 0x9C11B0);

    MH_EnableHook(MH_ALL_HOOKS);

    logInfo("[gamehooks] Game hooks initialized.");

    // TODO: TEMPORARY for testing, move to archipelago connect when it works
    InitializeShopData();
}

void GameHooks::cleanup()
{
}
