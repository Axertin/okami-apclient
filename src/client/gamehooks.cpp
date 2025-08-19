#include "gamehooks.h"

#include <algorithm>
#include <unordered_set>

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
static okami::ItemShopStock *(__fastcall *ocKibaShop__GetShopStockList)(void *, uint32_t *numItems);

static int64_t __fastcall onGetShopVariation(void *pUnk, uint32_t shopNum, char **pszShopTextureName)
{
    // Call the original metadata function but drop most of its details.
    // This prevents the use of variations (and therefore requiring defining multiple duplicate shops with just 1 item difference)
    uint32_t n;
    oGetShopMetadata(pUnk, shopNum, &n, pszShopTextureName);
    return 0;
}

static const void *(__fastcall *oLoadRsc)(void *pRscPackage, const char *pszType, uint32_t nIdx);
static const void *__fastcall onLoadRsc(void *pRscPackage, const char *pszType, uint32_t nIdx)
{
    if (strcmp(pszType, "ISL") == 0)
    {
        // nIdx is the shop number for the map, only 1 for Seian Fish Shop
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
    return oLoadRsc(pRscPackage, pszType, nIdx);
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
template <class T> void CreateHook(uintptr_t base, uintptr_t offset, T *pDetour)
{
    MH_CreateHook(reinterpret_cast<void *>(base + offset), reinterpret_cast<LPVOID>(pDetour), nullptr);
}

template <class T> void CreateMainHook(uintptr_t offset, T *pDetour, T **ppOriginal)
{
    CreateHook(okami::MainBase, offset, pDetour, ppOriginal);
}
template <class T> void CreateMainHook(uintptr_t offset, T *pDetour)
{
    CreateHook(okami::MainBase, offset, pDetour);
}

template <class T> void GetFnAddress(T &storage, uintptr_t base, uintptr_t offset)
{
    storage = reinterpret_cast<T>(base + offset);
}

template <class T> void GetMainFnAddress(T &storage, uintptr_t offset)
{
    GetFnAddress(storage, okami::MainBase, offset);
}

static void *(__fastcall *oLoadRscIdx)(void *pPkg, uint32_t idx);

static hx::Texture *(__fastcall *oCItemShop_GetItemIcon)(okami::cItemShop *pShop, int item);
hx::Texture *__fastcall GetItemIcon(okami::cItemShop *pShop, int item)
{
    if (item == 0 || !pShop->pIconsRsc)
    {
        // original call still needed as it returns a blank graphic from sgpCore20BinRsc
        return oCItemShop_GetItemIcon(pShop, item);
    }

    // First item in a package is index 1
    return reinterpret_cast<hx::Texture *>(oLoadRscIdx(pShop->pIconsRsc, item + 1));
}

static void *(__fastcall *oLoadResourcePackageAsync)(void *pFilesystem, const char *pszFilename, void **pOutputRscData, void *pHeap, int32_t, int32_t, int32_t,
                                                     int32_t);
void *__fastcall onLoadResourcePackageAsync(void *pFilesystem, const char *pszFilename, void **pOutputRscData, void *pHeap, int32_t a5, int32_t a6, int32_t a7,
                                            int32_t a8)
{
    // Intercept item shop icons
    if (pszFilename && std::strcmp(pszFilename, "id/ItemShopBuyIcon.dat") == 0)
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
static uint32_t TestItemDescID = 0;

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

        // Vanilla items with missing strings
        Core20MSD.OverrideString(okami::ItemTypes::HourglassOrb + ItemStrBaseID, "Hourglass Orb");
        Core20MSD.OverrideString(okami::ItemTypes::Yen10 + ItemStrBaseID, "10 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Yen50 + ItemStrBaseID, "50 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Yen100 + ItemStrBaseID, "100 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Yen150 + ItemStrBaseID, "150 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Yen500 + ItemStrBaseID, "500 Yen");
        Core20MSD.OverrideString(okami::ItemTypes::Praise + ItemStrBaseID, "Praise");

        // Test item string
        TestItemTextID = Core20MSD.AddString("Heinermann's Morph Ball");
        TestItemDescID = Core20MSD.AddString("This item seems important.");

        msdInitialized = true;
    }
    *ppCore20MSD = Core20MSD.GetData();
}

static std::unordered_set<uint16_t> ArchipelagoItemTypes = {
    okami::ItemTypes::ArchipelagoTestItem1, okami::ItemTypes::ArchipelagoTestItem2, okami::ItemTypes::ArchipelagoTestItem3,
    okami::ItemTypes::ArchipelagoTestItem4, okami::ItemTypes::ArchipelagoTestItem5, okami::ItemTypes::ArchipelagoTestItem6,
    okami::ItemTypes::ArchipelagoTestItem7,
};

constexpr uint8_t MaxVisibleSlots = 4;

static void(__fastcall *oSetUIStrId)(void *gui, uint8_t ctrlId, uint16_t strId);

/*
TODO:
  Vanilla items that have missing descriptions:
  - Ruins Key
  - Oddly Shaped Turnip
  - Hourglass Orb
  - 10/50/100/150/500 Yen
  - Praise
  - Vista of the Gods
  - Spirit Globe S + Ink Pot
*/
uint32_t __fastcall onCItemShop_UpdatePurchaseList(okami::cItemShop *pShop)
{
    // Rewrite - don't call original
    pShop->numSlots = pShop->numItemSlots;
    pShop->numVisibleSlots = std::min(pShop->numSlots, MaxVisibleSlots);

    for (uint32_t i = 0; i < pShop->numSlots; i++)
    {
        int32_t itemType = pShop->itemStockList[i].itemType;
        pShop->shopSlots[i].itemType = itemType;

        // TODO: This is where we choose a different icon and text for the same item type
        if (ArchipelagoItemTypes.contains(itemType))
        {
            pShop->shopSlots[i].pIcon = GetItemIcon(pShop, okami::ItemTypes::ArchipelagoTestItem1);
            pShop->shopSlots[i].itemNameStrId = TestItemTextID;
        }
        else
        {
            pShop->shopSlots[i].pIcon = GetItemIcon(pShop, itemType);
            pShop->shopSlots[i].itemNameStrId = itemType + 294;
        }

        // Decide the max count for purchases: 0 / #
        pShop->shopSlots[i].maxCount = 1;

        pShop->shopSlots[i].currentCount = okami::AmmyCollections->inventory[itemType];
        pShop->shopSlots[i].itemCost = pShop->itemStockList[i].cost;
    }
    return pShop->numItemSlots;
}

// Brings up the confirmation dialog for a purchase
static void(__fastcall *oCItemShop_PurchaseItem)(okami::cItemShop *pShop);
static void __fastcall onCItemShop_PurchaseItem(okami::cItemShop *pShop)
{
    // Use these to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();
    // int32_t variation = pShop->shopStockVariation;

    // The index of the item in the shop
    int selIdx = pShop->scrollOffset + pShop->visualSelectIndex;

    // Item type, do something different if it's "the" Archipelago Item
    int itemType = pShop->shopSlots[selIdx].itemType;

    // TODO replace/detect actual purchase
    if (ArchipelagoItemTypes.contains(itemType))
    {
        // Wipe the item from inventory to detect whether it gets purchased or not
        okami::AmmyCollections->inventory[itemType] = 0;

        oCItemShop_PurchaseItem(pShop);

        if (okami::AmmyCollections->inventory[itemType] != 0)
        {
            // TODO: TRIGGER ARCHIPELAGO PURCHASE OF SHOP SLOT HERE (send send send!)
            // TODO: Also mark a flag in save file somewhere to note that this has been purchased
            // Get AP id from mapID, variation, and selIdx

            okami::AmmyCollections->inventory[itemType] = 0;
        }
    }
    else
    {
        oCItemShop_PurchaseItem(pShop);
    }
}

static bool(__fastcall *oCItemShop_IsSoldOut)(okami::cItemShop *pShop, uint32_t shopIdx);
static bool __fastcall onCItemShop_IsSoldOut(okami::cItemShop *pShop, uint32_t shopIdx)
{
    // Use these to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();
    // int32_t variation = pShop->shopStockVariation;

    // TODO: return true when AP item is marked as obtained (received)

    return oCItemShop_IsSoldOut(pShop, shopIdx);
}

static bool(__fastcall *oCItemShop_IsPurchasable)(okami::cItemShop *pShop, uint32_t shopIdx);
static bool __fastcall onCItemShop_IsPurchasable(okami::cItemShop *pShop, uint32_t shopIdx)
{
    // Use these to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();
    // int32_t variation = pShop->shopStockVariation;

    // TODO: return false when AP item is marked as obtained (received)

    // false = greyed
    return oCItemShop_IsPurchasable(pShop, shopIdx);
}

static void(__fastcall *oCItemShop_ShopInteractUpdate)(okami::cItemShop *pShop);
static void __fastcall onCItemShop_ShopInteractUpdate(okami::cItemShop *pShop)
{
    // For default desc add 0x2000 to item type
    oCItemShop_ShopInteractUpdate(pShop);

    uint8_t selIdx = pShop->scrollOffset + pShop->visualSelectIndex;
    if (selIdx == 0) // TODO testing only
    {
        oSetUIStrId(&pShop->gui, 11, TestItemDescID);
    }
}

uint32_t __fastcall onCKibaShop_UpdatePurchaseList(okami::cKibaShop *pShop)
{
    // Rewrite - don't call original
    pShop->numSlots = pShop->numShopItems;
    pShop->numVisibleSlots = std::min(pShop->numSlots, MaxVisibleSlots);

    for (uint32_t i = 0; i < pShop->numSlots; i++)
    {
        int32_t itemType = pShop->itemStockList[i].itemType;
        pShop->shopSlots[i].itemType = itemType;

        // TODO: This is where we choose a different icon and text for the same item type
        if (ArchipelagoItemTypes.contains(itemType))
        {
            // TODO FIXME need to load the big item icon list elsewhere
            // pShop->shopSlots[i].pIcon = GetItemIcon(pShop, okami::ItemTypes::ArchipelagoTestItem1);
            pShop->shopSlots[i].pIcon = nullptr;
            pShop->shopSlots[i].itemNameStrId = TestItemTextID;
        }
        else
        {
            // TODO FIXME need to load the big item icon list elsewhere
            // pShop->shopSlots[i].pIcon = GetItemIcon(pShop, itemType);
            pShop->shopSlots[i].pIcon = nullptr;
            pShop->shopSlots[i].itemNameStrId = itemType + 294;
        }

        // Decide the max count for purchases: 0 / #
        pShop->shopSlots[i].maxCount = 1;

        pShop->shopSlots[i].currentCount = okami::AmmyCollections->inventory[itemType];
        pShop->shopSlots[i].itemCost = pShop->itemStockList[i].cost;
    }
    return pShop->numShopItems;
}

// Brings up the confirmation dialog for a purchase
static void(__fastcall *oCKibaShop_PurchaseItem)(okami::cKibaShop *pShop);
static void __fastcall onCKibaShop_PurchaseItem(okami::cKibaShop *pShop)
{
    // Use this to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();

    // The index of the item in the shop
    int selIdx = pShop->scrollOffset + pShop->visualSelectIndex;

    // Item type, do something different if it's "the" Archipelago Item
    int itemType = pShop->shopSlots[selIdx].itemType;

    // TODO replace/detect actual purchase
    if (ArchipelagoItemTypes.contains(itemType))
    {
        // Wipe the item from inventory to detect whether it gets purchased or not
        okami::AmmyCollections->inventory[itemType] = 0;

        oCKibaShop_PurchaseItem(pShop);

        if (okami::AmmyCollections->inventory[itemType] != 0)
        {
            // TODO: TRIGGER ARCHIPELAGO PURCHASE OF SHOP SLOT HERE (send send send!)
            // TODO: Also mark a flag in save file somewhere to note that this has been purchased
            // Get AP id from mapID, variation, and selIdx

            okami::AmmyCollections->inventory[itemType] = 0;
        }
    }
    else
    {
        oCKibaShop_PurchaseItem(pShop);
    }
}

static bool(__fastcall *oCKibaShop_IsSoldOut)(okami::cKibaShop *pShop, uint32_t shopIdx);
static bool __fastcall onCKibaShop_IsSoldOut(okami::cKibaShop *pShop, uint32_t shopIdx)
{
    // Use these to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();

    // TODO: return true when AP item is marked as obtained (received)

    return oCKibaShop_IsSoldOut(pShop, shopIdx);
}

static bool(__fastcall *oCKibaShop_IsPurchasable)(okami::cKibaShop *pShop, uint32_t shopIdx);
static bool __fastcall onCKibaShop_IsPurchasable(okami::cKibaShop *pShop, uint32_t shopIdx)
{
    // Use these to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();

    // TODO: return false when AP item is marked as obtained (received)

    // false = greyed
    return oCKibaShop_IsPurchasable(pShop, shopIdx);
}

static void(__fastcall *oCKibaShop_ShopInteractUpdate)(okami::cKibaShop *pShop);
static void __fastcall onCKibaShop_ShopInteractUpdate(okami::cKibaShop *pShop)
{
    // For default desc add 0x200D to item type
    oCKibaShop_ShopInteractUpdate(pShop);

    uint8_t selIdx = pShop->scrollOffset + pShop->visualSelectIndex;
    if (selIdx == 0) // TODO testing only
    {
        oSetUIStrId(&pShop->gui, 11, TestItemDescID);
    }
}

uint32_t __fastcall onCSkillShop_UpdatePurchaseList(okami::cSkillShop *pShop)
{
    // Rewrite - don't call original
    pShop->numSlots = pShop->numSkillSlots;
    pShop->numVisibleSlots = std::min(pShop->numSlots, MaxVisibleSlots);

    for (uint32_t i = 0; i < pShop->numSlots; i++)
    {
        int32_t skillType = pShop->skillList[i].skillId;

        // TODO: This is where we choose a different icon and text for the same item type
        pShop->shopSlots[i].itemCost = pShop->skillList[i].cost;
        pShop->shopSlots[i].pIcon = nullptr;
        // 0x2000 is a context specific offset
        // anything 0x2000 + n here is found in id/idskillshop.idd -> bin TBL -> PAC -> MSD
        // but that also means we can just use the existing global MSD modifications with a low ID
        pShop->shopSlots[i].itemNameStrId = skillType + 0x2000;
    }
    return pShop->numSkillSlots;
}

// Brings up the confirmation dialog for a purchase
static void(__fastcall *oCSkillShop_PurchaseSkill)(okami::cSkillShop *pShop);
static void __fastcall onCSkillShop_PurchaseSkill(okami::cSkillShop *pShop)
{
    // Use this to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();

    // The index of the item in the shop
    // int selIdx = pShop->scrollOffset + pShop->visualSelectIndex;

    // Item type, do something different if it's "the" Archipelago Item
    // int skillType = pShop->skillList[selIdx].skillId;

    // TODO replace/detect actual purchase

    oCSkillShop_PurchaseSkill(pShop);
}

static bool(__fastcall *oCSkillShop_IsSkillNotLearned)(okami::cSkillShop *pShop, uint32_t shopIdx);
static bool __fastcall onCSkillShop_IsSkillNotLearned(okami::cSkillShop *pShop, uint32_t shopIdx)
{
    // Determines how the entry gets drawn
    // Use these to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();

    // TODO: return false when AP item is marked as obtained (received)

    return oCSkillShop_IsSkillNotLearned(pShop, shopIdx);
}

static bool(__fastcall *oCSkillShop_IsSoldOut)(okami::cSkillShop *pShop, uint32_t shopIdx);
static bool __fastcall onCSkillShop_IsSoldOut(okami::cSkillShop *pShop, uint32_t shopIdx)
{
    // Use these to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();

    // TODO: return true when AP item is marked as obtained (received)

    return oCSkillShop_IsSoldOut(pShop, shopIdx);
}

static bool(__fastcall *oCSkillShop_IsPurchasable)(okami::cSkillShop *pShop, uint32_t shopIdx);
static bool __fastcall onCSkillShop_IsPurchasable(okami::cSkillShop *pShop, uint32_t shopIdx)
{
    // Use these to get the Archipelago shop ID
    // uint16_t mapID = okami::ExteriorMapID.get();

    // TODO: return false when AP item is marked as obtained (received)

    // false = greyed
    return oCSkillShop_IsPurchasable(pShop, shopIdx);
}

static void(__fastcall *oCSkillShop_ShopInteractUpdate)(okami::cSkillShop *pShop);
static void __fastcall onCSkillShop_ShopInteractUpdate(okami::cSkillShop *pShop)
{
    // For default desc add 0x2017 to skill type
    oCSkillShop_ShopInteractUpdate(pShop);

    uint8_t selIdx = pShop->scrollOffset + pShop->visualSelectIndex;
    if (selIdx == 0) // TODO testing only
    {
        oSetUIStrId(&pShop->gui, 11, TestItemDescID);
    }
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

    CreateMainHook(0x4B6240, &onReturnToMenu, &Flower_ReturnToMenu);
    CreateMainHook(0x4B6230, &onGameStop, &Main_FlowerStopOrigin);
    CreateMainHook(0x4B63B0, &onGameTick, &Main_FlowerTickOrigin);
    CreateMainHook(0x4965D0, &onItemPickup, &oItemPickup);
    CreateMainHook(0x17C270, &onBrushEdit, &oEditBrushes);

    CreateMainHook(0x4420C0, &onGetShopVariation, &oGetShopVariation);
    CreateMainHook(0x1B1770, &onLoadRsc, &oLoadRsc);
    CreateMainHook(0x1AFC90, &onLoadResourcePackageAsync, &oLoadResourcePackageAsync);
    CreateMainHook(0x1412B0, &onGXTextureManager_GetNumEntries, &oGXTextureManager_GetNumEntries);
    CreateMainHook(0x1C9510, &onLoadCore20MSD, &oLoadCore20MSD);

    // Item shop
    CreateMainHook(0x43E250, &onCItemShop_UpdatePurchaseList);
    CreateMainHook(0x43CA30, &onCItemShop_PurchaseItem, &oCItemShop_PurchaseItem);
    CreateMainHook(0x43BAE0, &onCItemShop_IsSoldOut, &oCItemShop_IsSoldOut);
    CreateMainHook(0x43B6A0, &onCItemShop_IsPurchasable, &oCItemShop_IsPurchasable);
    CreateMainHook(0x43C6F0, &onCItemShop_ShopInteractUpdate, &oCItemShop_ShopInteractUpdate);

    // Demon fang shop
    CreateMainHook(0x43F5A0, &onCKibaShop__GetShopStockList, &ocKibaShop__GetShopStockList);
    CreateMainHook(0x440380, &onCKibaShop_UpdatePurchaseList);
    CreateMainHook(0x43FD30, &onCKibaShop_PurchaseItem, &oCKibaShop_PurchaseItem);
    CreateMainHook(0x43F440, &onCKibaShop_IsSoldOut, &oCKibaShop_IsSoldOut);
    CreateMainHook(0x43F2F0, &onCKibaShop_IsPurchasable, &oCKibaShop_IsPurchasable);
    CreateMainHook(0x43FA90, &onCKibaShop_ShopInteractUpdate, &oCKibaShop_ShopInteractUpdate);

    // Skill shop
    CreateMainHook(0x4431B0, &onCSkillShop_UpdatePurchaseList);
    CreateMainHook(0x442570, &onCSkillShop_IsSkillNotLearned, &oCSkillShop_IsSkillNotLearned);
    CreateMainHook(0x442C40, &onCSkillShop_PurchaseSkill, &oCSkillShop_PurchaseSkill);
    CreateMainHook(0x4423C0, &onCSkillShop_IsSoldOut, &oCSkillShop_IsSoldOut);
    CreateMainHook(0x4421C0, &onCSkillShop_IsPurchasable, &oCSkillShop_IsPurchasable);
    CreateMainHook(0x442A50, &onCSkillShop_ShopInteractUpdate, &oCSkillShop_ShopInteractUpdate);

    GetMainFnAddress(oCItemShop_GetItemIcon, 0x43BDA0);
    GetMainFnAddress(oGetShopMetadata, 0x441E40);
    GetMainFnAddress(oLoadRscIdx, 0x1B16C0);
    GetMainFnAddress(oSetUIStrId, 0x1B6CA0);
    GetMainFnAddress(ppCore20MSD, 0x9C11B0);

    MH_EnableHook(MH_ALL_HOOKS);

    logInfo("[gamehooks] Game hooks initialized.");

    // TODO: TEMPORARY for testing, move to archipelago connect when it works
    InitializeShopData();
}

void GameHooks::cleanup()
{
}
