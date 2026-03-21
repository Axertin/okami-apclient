#include "shops.hpp"

#include <array>
#include <cstring>
#include <list>

#include <okami/itemtype.hpp>
#include <okami/maptype.hpp>
#include <wolf_framework.hpp>

#include "../gamestate_accessors.hpp"
#include "../isocket.h"
#include "../itempatch.hpp"
#include "../rewards/game_items.hpp"
#include "../rewards/reward_types.hpp"
#include "check_types.hpp"

namespace checks
{

// Memory offsets
constexpr uintptr_t SHOP_VARIATION_OFFSET = 0x4420C0;
constexpr uintptr_t LOAD_RSC_OFFSET = 0x1B1770;
constexpr uintptr_t SHOP_METADATA_OFFSET = 0x441E40;
constexpr uintptr_t KIBA_SHOP_GET_STOCK_LIST_OFFSET = 0x43F5A0;
constexpr uintptr_t ITEM_SHOP_PURCHASE_OFFSET = 0x43CA30;
constexpr uintptr_t KIBA_SHOP_PURCHASE_OFFSET = 0x43FD30;
constexpr uintptr_t ITEM_SHOP_SORT_STOCK_OFFSET = 0x441A70;
constexpr uintptr_t ITEM_SHOP_IS_GRAYED_OFFSET = 0x43b6a0;
constexpr uintptr_t ITEM_SHOP_IS_PURCHASED_OFFSET = 0x43bae0;
constexpr uintptr_t ITEM_SHOP_BUY_QTY_DIALOG_OFFSET = 0x43d060;
constexpr uintptr_t EXTERIOR_MAP_ID_OFFSET = 0xB6B240;

// Shop struct offsets (from cShopBase) — validated against decompiled
// FUN_18043ca30 (cItemShop::PurchaseItem) which reads param_1+0x8A/0x8B.
constexpr uintptr_t SHOP_SCROLL_OFFSET = 0x8A;
constexpr uintptr_t SHOP_VISUAL_SELECT_INDEX = 0x8B;
constexpr uintptr_t SHOP_OUTER_STATE = 0x95;
constexpr uintptr_t SHOP_SUB_STATE = 0x96;
constexpr uintptr_t SHOP_INPUT_BUF_PTR = 0x38;
constexpr uintptr_t SHOP_OUTPUT_BUF_PTR = 0x40;
constexpr uintptr_t SHOP_STOCK_PTR = 0x48;

// Static member initialization
ShopMan::GetShopVariationFn ShopMan::originalGetShopVariation_ = nullptr;
ShopMan::LoadRscFn ShopMan::originalLoadRsc_ = nullptr;
ShopMan::GetShopMetadataFn ShopMan::originalGetShopMetadata_ = nullptr;
ShopMan::GetKibaShopStockListFn ShopMan::originalCKibaShop_GetShopStockList_ = nullptr;
ShopMan::ItemShopPurchaseFn ShopMan::originalCItemShop_PurchaseItem_ = nullptr;
ShopMan::KibaShopPurchaseFn ShopMan::originalCKibaShop_PurchaseItem_ = nullptr;
ShopMan::ItemShopSortStockFn ShopMan::originalCItemShop_SortStock_ = nullptr;
ShopMan::IsGrayedOutFn ShopMan::originalIsGrayedOut_ = nullptr;
ShopMan::IsPurchasedFn ShopMan::originalIsPurchased_ = nullptr;
ShopMan::BuyQtyDialogFn ShopMan::originalBuyQtyDialog_ = nullptr;
ShopMan *ShopMan::activeInstance_ = nullptr;

// ============================================================================
// ShopDefinition Implementation
// ============================================================================

ShopDefinition::ShopDefinition() = default;

void ShopDefinition::RebuildISL()
{
    dataISL.clear();
    dataISL.reserve(sizeof(okami::ISLHeader) + sizeof(uint32_t) + sizeof(okami::ItemShopStock) * itemStock.size() + sizeof(okami::SellValueArray));

    okami::ISLHeader header = {"ISL", 1, 0, 0};
    dataISL.append(header);

    uint32_t numItems = static_cast<uint32_t>(itemStock.size());
    dataISL.append(numItems);

    dataISL.append_range(itemStock);
    dataISL.append(sellValues);
}

void ShopDefinition::CheckDirty()
{
    if (dirty)
    {
        RebuildISL();
        dirty = false;
    }
}

const uint8_t *ShopDefinition::GetData()
{
    CheckDirty();
    wolf::logDebug("[ShopMan] GetData() returning %zu bytes at %p", dataISL.size(), dataISL.data());
    return dataISL.data();
}

void ShopDefinition::SetStock(const std::vector<okami::ItemShopStock> &stock)
{
    if (stock.size() <= MaxShopStockSize)
    {
        itemStock = stock;
    }
    else
    {
        wolf::logWarning("[ShopMan] Max stock size in SetStock exceeded");
    }
    dirty = true;
}

void ShopDefinition::AddItem(okami::ItemTypes::Enum item, int32_t cost)
{
    if (itemStock.size() < MaxShopStockSize)
    {
        itemStock.emplace_back(okami::ItemShopStock{item, cost, 0});
    }
    else
    {
        wolf::logWarning("[ShopMan] Max stock size in AddItem exceeded");
    }
    dirty = true;
}

void ShopDefinition::ClearStock()
{
    itemStock.clear();
    dirty = true;
}

void ShopDefinition::SetSellValueOverride(okami::ItemTypes::Enum item, int32_t sellValue)
{
    sellValues[item] = sellValue;
    dirty = true;
}

void ShopDefinition::SetSellValues(const okami::SellValueArray &replacementSellValues)
{
    sellValues = replacementSellValues;
    dirty = true;
}

// ============================================================================
// Data-driven shop registry (21 item shops + 3 demon fang shops)
// ============================================================================

constexpr int NUM_ITEM_SHOPS = 21;

static std::array<ShopDefinition, NUM_ITEM_SHOPS> itemShops;
static std::vector<okami::ItemShopStock> AgataFangShop;
static std::vector<okami::ItemShopStock> ArkOfYamatoFangShop;
static std::vector<okami::ItemShopStock> ImperialPalaceFangShop;

struct ShopMapEntry
{
    okami::MapID::Enum mapId;
    int shopId;
    uint32_t shopNum; // UINT32_MAX = wildcard (any shopNum), otherwise exact match
};

constexpr uint32_t kAnyShopNum = UINT32_MAX;

static constexpr auto kShopMap = std::to_array<ShopMapEntry>({
    {okami::MapID::AgataForestCursed, 0, kAnyShopNum},
    {okami::MapID::AgataForestHealed, 0, kAnyShopNum},
    {okami::MapID::ArkofYamato, 1, kAnyShopNum},
    {okami::MapID::CityCheckpoint, 2, kAnyShopNum},
    {okami::MapID::DragonPalace, 3, kAnyShopNum},
    {okami::MapID::KamikiVillagePostTei, 4, kAnyShopNum},
    {okami::MapID::KamikiVillageCursed, 5, kAnyShopNum},
    {okami::MapID::KamikiVillage, 5, kAnyShopNum},
    {okami::MapID::KamikiVillagePast, 6, kAnyShopNum},
    {okami::MapID::KamuiCursed, 7, kAnyShopNum},
    {okami::MapID::KamuiHealed, 7, kAnyShopNum},
    {okami::MapID::KusaVillage, 8, kAnyShopNum},
    {okami::MapID::MoonCaveInterior, 9, kAnyShopNum},
    {okami::MapID::MoonCaveStaircaseAndOrochiArena, 10, kAnyShopNum},
    {okami::MapID::NRyoshimaCoast, 11, kAnyShopNum},
    {okami::MapID::OniIslandLowerInterior, 12, kAnyShopNum},
    {okami::MapID::Ponctan, 13, kAnyShopNum},
    {okami::MapID::RyoshimaCoastCursed, 14, kAnyShopNum},
    {okami::MapID::RyoshimaCoastHealed, 14, kAnyShopNum},
    {okami::MapID::SasaSanctuary, 15, kAnyShopNum},
    {okami::MapID::SeianCityCommonersQuarter, 16, 0}, // weapon shop
    {okami::MapID::SeianCityCommonersQuarter, 17, 1}, // fish shop
    {okami::MapID::ShinshuFieldCursed, 18, kAnyShopNum},
    {okami::MapID::ShinshuFieldHealed, 18, kAnyShopNum},
    {okami::MapID::TakaPassCursed, 19, kAnyShopNum},
    {okami::MapID::TakaPassHealed, 19, kAnyShopNum},
    {okami::MapID::WawkuShrine, 20, kAnyShopNum},
});

std::optional<int> GetShopIdForMap(uint16_t mapId, uint32_t shopNum)
{
    auto id = static_cast<okami::MapID::Enum>(mapId);

    for (const auto &[map, shop, num] : kShopMap)
    {
        if (map == id && (num == kAnyShopNum || num == shopNum))
            return shop;
    }
    return std::nullopt;
}

static ShopDefinition *GetShopById(int shopId)
{
    if (shopId >= 0 && shopId < NUM_ITEM_SHOPS)
        return &itemShops[static_cast<size_t>(shopId)];
    return nullptr;
}

const void *GetCurrentItemShopData(uint16_t mapId, uint32_t shopNum)
{
    auto shopId = GetShopIdForMap(mapId, shopNum);
    if (shopId)
        return itemShops[static_cast<size_t>(*shopId)].GetData();
    return nullptr;
}

okami::ItemShopStock *GetCurrentDemonFangShopData(uint16_t mapId, uint32_t *pNumItems)
{
    auto mapID = static_cast<okami::MapID::Enum>(mapId);

    switch (mapID)
    {
    case okami::MapID::AgataForestCursed:
    case okami::MapID::AgataForestHealed:
        *pNumItems = static_cast<uint32_t>(AgataFangShop.size());
        return AgataFangShop.data();
    case okami::MapID::ArkofYamato:
        *pNumItems = static_cast<uint32_t>(ArkOfYamatoFangShop.size());
        return ArkOfYamatoFangShop.data();
    case okami::MapID::ImperialPalaceAmmySize:
        *pNumItems = static_cast<uint32_t>(ImperialPalaceFangShop.size());
        return ImperialPalaceFangShop.data();
    default:
        break;
    }

    *pNumItems = 0;
    return nullptr;
}

// ============================================================================
// ShopMan Implementation
// ============================================================================

ShopMan::ShopMan(ISocket &socket, CheckCallback checkCallback) : socket_(socket), checkCallback_(std::move(checkCallback))
{
}

ShopMan::~ShopMan()
{
    shutdown();
}

void ShopMan::initialize()
{
    if (initialized_)
    {
        wolf::logWarning("[ShopMan] Already initialized");
        return;
    }

    activeInstance_ = this;

    // Get shop metadata function pointer (needed by variation hook)
    uintptr_t mainBase = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
    originalGetShopMetadata_ = reinterpret_cast<GetShopMetadataFn>(mainBase + SHOP_METADATA_OFFSET);

    // Hook shop variation to disable variations
    if (!wolf::hookFunction("main.dll", SHOP_VARIATION_OFFSET, reinterpret_cast<void *>(&hookGetShopVariation),
                            reinterpret_cast<void **>(&originalGetShopVariation_)))
    {
        wolf::logError("[ShopMan] Failed to install GetShopVariation hook");
        return;
    }

    // Hook resource loading to inject custom ISL data
    if (!wolf::hookFunction("main.dll", LOAD_RSC_OFFSET, reinterpret_cast<void *>(&hookLoadRsc), reinterpret_cast<void **>(&originalLoadRsc_)))
    {
        wolf::logError("[ShopMan] Failed to install LoadRsc hook");
        return;
    }

    // Hook demon fang shop stock list
    if (!wolf::hookFunction("main.dll", KIBA_SHOP_GET_STOCK_LIST_OFFSET, reinterpret_cast<void *>(&hookCKibaShop_GetShopStockList),
                            reinterpret_cast<void **>(&originalCKibaShop_GetShopStockList_)))
    {
        wolf::logError("[ShopMan] Failed to install CKibaShop_GetShopStockList hook");
        return;
    }

    // Hook item shop purchase for check detection
    if (!wolf::hookFunction("main.dll", ITEM_SHOP_PURCHASE_OFFSET, reinterpret_cast<void *>(&hookCItemShop_PurchaseItem),
                            reinterpret_cast<void **>(&originalCItemShop_PurchaseItem_)))
    {
        wolf::logError("[ShopMan] Failed to install CItemShop_PurchaseItem hook");
        return;
    }

    // Hook demon fang shop purchase for check detection
    if (!wolf::hookFunction("main.dll", KIBA_SHOP_PURCHASE_OFFSET, reinterpret_cast<void *>(&hookCKibaShop_PurchaseItem),
                            reinterpret_cast<void **>(&originalCKibaShop_PurchaseItem_)))
    {
        wolf::logError("[ShopMan] Failed to install CKibaShop_PurchaseItem hook");
        return;
    }

    // Hook item shop stock sort to bypass the game's ordering table filter.
    // The vanilla sort drops items not in a hardcoded 116-entry table, which
    // prevents us from placing arbitrary items in shop slots.
    if (!wolf::hookFunction("main.dll", ITEM_SHOP_SORT_STOCK_OFFSET, reinterpret_cast<void *>(&hookCItemShop_SortStock),
                            reinterpret_cast<void **>(&originalCItemShop_SortStock_)))
    {
        wolf::logError("[ShopMan] Failed to install CItemShop_SortStock hook");
        return;
    }

    // Hook isGrayedOut to control purchasability for AP shops
    if (!wolf::hookFunction("main.dll", ITEM_SHOP_IS_GRAYED_OFFSET, reinterpret_cast<void *>(&hookIsGrayedOut),
                            reinterpret_cast<void **>(&originalIsGrayedOut_)))
    {
        wolf::logError("[ShopMan] Failed to install isGrayedOut hook");
        return;
    }

    // Hook isPurchased to show already-purchased AP slots
    if (!wolf::hookFunction("main.dll", ITEM_SHOP_IS_PURCHASED_OFFSET, reinterpret_cast<void *>(&hookIsPurchased),
                            reinterpret_cast<void **>(&originalIsPurchased_)))
    {
        wolf::logError("[ShopMan] Failed to install isPurchased hook");
        return;
    }

    // Hook buy quantity dialog (state 4) to skip it for AP shops.
    // The vanilla dialog hangs on AP items because canIncrementQty returns 0
    // for maxStack=1 items, which disables ALL input in the quantity selector.
    if (!wolf::hookFunction("main.dll", ITEM_SHOP_BUY_QTY_DIALOG_OFFSET, reinterpret_cast<void *>(&hookBuyQtyDialog),
                            reinterpret_cast<void **>(&originalBuyQtyDialog_)))
    {
        wolf::logError("[ShopMan] Failed to install buyQtyDialog hook");
        return;
    }

    initialized_ = true;
    wolf::logInfo("[ShopMan] Shop hooks installed successfully");
}

void ShopMan::shutdown()
{
    if (!initialized_)
    {
        return;
    }

    activeInstance_ = nullptr;
    initialized_ = false;
}

void ShopMan::reset()
{
    // Clear scouting cache
    scoutedMapId_ = 0;
    scoutedItems_.clear();
    currentShopId_ = -1;
    purchasedChecks_.clear();
}

void ShopMan::scoutShopsForMap(uint16_t mapId)
{
    // Check if already scouted for this map
    if (scoutedMapId_ == mapId)
    {
        // Already attempted scouting for this map (even if it returned empty)
        return;
    }

    // Clear previous cache and mark this map as scouted (even if it fails)
    scoutedItems_.clear();
    scoutedMapId_ = mapId;

    // Gather all shop location IDs for this map
    std::list<int64_t> locationsToScout;
    std::optional<int> lastShopId;

    // Check both shopNum 0 and 1 (for Seian which has 2 shops)
    for (uint32_t shopNum = 0; shopNum <= 1; ++shopNum)
    {
        auto shopId = GetShopIdForMap(mapId, shopNum);
        if (!shopId)
        {
            continue;
        }

        // Skip if we already processed this shop ID (most maps have same shop for both shopNum values)
        if (lastShopId && *lastShopId == *shopId)
        {
            continue;
        }
        lastShopId = *shopId;

        int slotCount = socket_.getSlotConfig().shopSlots;
        for (int slot = 0; slot < slotCount; ++slot)
        {
            locationsToScout.push_back(checks::getShopCheckId(*shopId, slot));
        }
    }

    if (locationsToScout.empty() || !socket_.isConnected())
    {
        return;
    }

    // Scout all locations synchronously
    auto scoutedItems = socket_.scoutLocationsSync(locationsToScout, 0);

    if (scoutedItems.empty())
    {
        return;
    }

    // Store results in cache
    for (const auto &item : scoutedItems)
    {
        scoutedItems_[item.location] = item;
    }
}

void ShopMan::populateShopFromScoutedData(int shopId)
{
    wolf::logDebug("[ShopMan] populateShopFromScoutedData called for shopId=%d", shopId);

    ShopDefinition *shop = GetShopById(shopId);
    if (!shop)
    {
        wolf::logWarning("[ShopMan] Invalid shop ID %d for population", shopId);
        return;
    }

    shop->ClearStock();

    int slotCount = socket_.getSlotConfig().shopSlots;
    wolf::logDebug("[ShopMan] Populating %d slots", slotCount);

    for (int slot = 0; slot < slotCount; ++slot)
    {
        int64_t locationId = checks::getShopCheckId(shopId, slot);
        auto it = scoutedItems_.find(locationId);
        if (it == scoutedItems_.end())
        {
            wolf::logDebug("[ShopMan] No item at slot %d (locationId=%lld), stopping", slot, locationId);
            break;
        }

        const ScoutedItem &scouted = it->second;

        // Convert AP item ID to game item
        okami::ItemTypes::Enum gameItem;

        // Helper to check if a game item is a weapon (divine instruments can't be displayed in shops)
        auto isWeapon = [](int itemId) { return itemId >= okami::ItemTypes::DivineRetribution && itemId <= okami::ItemTypes::ThunderEdge; };

        const int mySlot = socket_.getPlayerSlot();
        const bool isNative = !rewards::isForeignItem(scouted.player, mySlot);

        // Helper to select a native Okami dummy item based on AP classification flags
        auto nativeDummy = [](unsigned flags)
        {
            if (rewards::isTrap(flags))
                return okami::ItemTypes::OkamiTrapItem;
            if (rewards::isProgression(flags))
                return okami::ItemTypes::OkamiProgressionItem;
            return okami::ItemTypes::OkamiStandardItem;
        };

        if (isNative && rewards::game_items::isDirectGameItem(scouted.item))
        {
            int rawItem = rewards::game_items::getItemId(scouted.item);
            if (isWeapon(rawItem))
            {
                // Weapons can't be displayed in item shops - use dummy
                gameItem = nativeDummy(scouted.flags);
                itempatch::registerScoutedItemName(locationId, socket_.getItemName(scouted.item, socket_.getPlayerSlot()));
                wolf::logDebug("[ShopMan] Slot %d: native AP item %lld -> weapon %d, using dummy %d", slot, scouted.item, rawItem, static_cast<int>(gameItem));
            }
            else
            {
                // Displayable native item — use raw type for vanilla icon and name
                gameItem = static_cast<okami::ItemTypes::Enum>(rawItem);
                wolf::logDebug("[ShopMan] Slot %d: native AP item %lld -> game item %d", slot, scouted.item, rawItem);
            }
        }
        else if (isNative && rewards::game_items::isProgressiveWeapon(scouted.item))
        {
            // Progressive weapons always need dummy in shops
            gameItem = nativeDummy(scouted.flags);
            itempatch::registerScoutedItemName(locationId, socket_.getItemName(scouted.item, socket_.getPlayerSlot()));
            wolf::logDebug("[ShopMan] Slot %d: native AP item %lld -> progressive weapon, using dummy %d", slot, scouted.item, static_cast<int>(gameItem));
        }
        else if (isNative)
        {
            // Native brushes, event flags, etc.
            gameItem = nativeDummy(scouted.flags);
            itempatch::registerScoutedItemName(locationId, socket_.getItemName(scouted.item, socket_.getPlayerSlot()));
            wolf::logDebug("[ShopMan] Slot %d: native AP item %lld -> non-game item, using dummy %d (flags=0x%x)", slot, scouted.item,
                           static_cast<int>(gameItem), scouted.flags);
        }
        else
        {
            // Foreign item — select AP dummy type based on classification flags
            if (rewards::isTrap(scouted.flags))
                gameItem = okami::ItemTypes::ForeignTrapItem;
            else if (rewards::isProgression(scouted.flags))
                gameItem = okami::ItemTypes::ForeignProgressionItem;
            else
                gameItem = okami::ItemTypes::ForeignStandardItem;
            itempatch::registerScoutedItemName(locationId, socket_.getItemName(scouted.item, scouted.player));
            wolf::logDebug("[ShopMan] Slot %d: foreign AP item %lld -> dummy type %d (flags=0x%x)", slot, scouted.item, static_cast<int>(gameItem),
                           scouted.flags);
        }

        // TODO: Get actual price from AP data or slot_data
        constexpr int32_t placeholderCost = 100;
        shop->AddItem(gameItem, placeholderCost);
    }

    wolf::logDebug("[ShopMan] Shop population complete");
}

// ============================================================================
// Hook Implementations
// ============================================================================

int64_t __fastcall ShopMan::hookGetShopVariation(void *pUnk, uint32_t shopNum, char **pszShopTextureName)
{
    // Call the original metadata function but drop most of its details.
    // This prevents the use of variations (and therefore requiring defining multiple duplicate shops)
    if (originalGetShopMetadata_)
    {
        uint32_t n;
        originalGetShopMetadata_(pUnk, shopNum, &n, pszShopTextureName);
    }
    return 0;
}

const void *__fastcall ShopMan::hookLoadRsc(void *pRscPackage, const char *pszType, uint32_t nIdx)
{
    if (std::strcmp(pszType, "ISL") == 0)
    {
        wolf::logDebug("[ShopMan] hookLoadRsc called for ISL, nIdx=%u", nIdx);

        // Only inject custom shop data if we can and need to
        if (activeInstance_ && activeInstance_->socket_.isConnected() && activeInstance_->socket_.getSlotConfig().randomizeShops)
        {
            // Get current map ID
            uintptr_t mainBase = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
            auto *currentMapPtr = reinterpret_cast<uint16_t *>(mainBase + EXTERIOR_MAP_ID_OFFSET);
            uint16_t mapId = *currentMapPtr;

            wolf::logDebug("[ShopMan] mapId=0x%04X, randomizeShops=true", mapId);

            // Get shop ID for this map/shopNum combination
            auto shopId = GetShopIdForMap(mapId, nIdx);
            if (shopId)
            {
                wolf::logDebug("[ShopMan] shopId=%d", *shopId);

                // Track current shop for purchase detection
                activeInstance_->currentShopId_ = *shopId;
                itempatch::setCurrentShopId(*shopId);

                // Lazy scouting: scout on first shop access for this map
                activeInstance_->scoutShopsForMap(mapId);

                // Only populate from scouted data if we got results
                if (!activeInstance_->scoutedItems_.empty())
                {
                    wolf::logDebug("[ShopMan] Have %zu scouted items, populating shop", activeInstance_->scoutedItems_.size());
                    activeInstance_->populateShopFromScoutedData(*shopId);

                    const void *pResult = GetCurrentItemShopData(mapId, nIdx);
                    if (pResult != nullptr)
                    {
                        wolf::logDebug("[ShopMan] Returning custom ISL data at %p", pResult);
                        return pResult;
                    }
                    else
                    {
                        wolf::logWarning("[ShopMan] GetCurrentItemShopData returned nullptr!");
                    }
                }
                else
                {
                    wolf::logDebug("[ShopMan] No scouted items, falling through to original");
                }
            }
            else
            {
                wolf::logDebug("[ShopMan] No shop for this map/nIdx, falling through to original");
            }
        }
        else
        {
            wolf::logDebug("[ShopMan] Not injecting: instance=%p, connected=%d, randomize=%d", activeInstance_,
                           activeInstance_ ? activeInstance_->socket_.isConnected() : false,
                           activeInstance_ ? activeInstance_->socket_.getSlotConfig().randomizeShops : false);
        }
    }

    // Fall through to original for unhandled resource types
    if (originalLoadRsc_)
    {
        return originalLoadRsc_(pRscPackage, pszType, nIdx);
    }
    return nullptr;
}

okami::ItemShopStock *__fastcall ShopMan::hookCKibaShop_GetShopStockList(void *pKibaShop, uint32_t *numItems)
{
    // Only inject custom shop data if connected to AP server
    // TODO: Also check slot_data for whether shops are randomized
    if (activeInstance_ && activeInstance_->socket_.isConnected())
    {
        // Get current map ID
        uintptr_t mainBase = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
        auto *currentMapPtr = reinterpret_cast<uint16_t *>(mainBase + EXTERIOR_MAP_ID_OFFSET);
        uint16_t mapId = *currentMapPtr;

        okami::ItemShopStock *pResult = GetCurrentDemonFangShopData(mapId, numItems);
        if (pResult != nullptr)
        {
            return pResult;
        }
    }

    // Fall through to original
    if (originalCKibaShop_GetShopStockList_)
    {
        return originalCKibaShop_GetShopStockList_(pKibaShop, numItems);
    }

    *numItems = 0;
    return nullptr;
}

void __fastcall ShopMan::hookCItemShop_PurchaseItem(void *pShop)
{
    auto *shopBase = reinterpret_cast<uint8_t *>(pShop);
    uint8_t stateBefore = shopBase[SHOP_SUB_STATE];

    // Snapshot inventory count for the selected item before purchase
    uint16_t *inventorySlot = nullptr;
    uint16_t savedCount = 0;
    if (activeInstance_ && activeInstance_->currentShopId_ >= 0 && activeInstance_->socket_.getSlotConfig().randomizeShops)
    {
        uint8_t scrollOffset = shopBase[SHOP_SCROLL_OFFSET];
        uint8_t visualSelectIndex = shopBase[SHOP_VISUAL_SELECT_INDEX];
        int selectedSlot = scrollOffset + visualSelectIndex;
        auto *stockPtr = *reinterpret_cast<uint8_t **>(shopBase + SHOP_STOCK_PTR);
        int32_t itemType = *reinterpret_cast<int32_t *>(stockPtr + selectedSlot * 0x20);

        if (itemType >= 0 && itemType < okami::ItemTypes::NUM_ITEM_TYPES && apgame::collectionData.is_bound())
        {
            inventorySlot = &apgame::collectionData->inventory[itemType];
            savedCount = *inventorySlot;
        }
    }

    if (originalCItemShop_PurchaseItem_)
    {
        originalCItemShop_PurchaseItem_(pShop);
    }

    uint8_t stateAfter = shopBase[SHOP_OUTER_STATE];

    // For AP shops, "No" in the confirmation dialog tries to go back to state 4
    // (quantity dialog). Since we skip state 4 entirely, redirect to state 3
    // (browse) instead to avoid a 4→5→4→5 infinite loop.
    if (stateAfter == 4 && activeInstance_ && activeInstance_->currentShopId_ >= 0 && activeInstance_->socket_.getSlotConfig().randomizeShops)
    {
        shopBase[SHOP_OUTER_STATE] = 3;
    }

    // Send check only when purchase actually completes:
    // state 0x96 transitions 1->0 and 0x95 == 3 (confirmed, not cancelled which goes to 4)
    if (stateBefore == 1 && shopBase[SHOP_SUB_STATE] == 0 && stateAfter == 3 && activeInstance_ && activeInstance_->currentShopId_ >= 0)
    {
        // Restore inventory count to undo the item grant (server sends the real item)
        if (inventorySlot)
            *inventorySlot = savedCount;

        uint8_t scrollOffset = shopBase[SHOP_SCROLL_OFFSET];
        uint8_t visualSelectIndex = shopBase[SHOP_VISUAL_SELECT_INDEX];
        int selectedSlot = scrollOffset + visualSelectIndex;

        int64_t checkId = checks::getShopCheckId(activeInstance_->currentShopId_, selectedSlot);
        activeInstance_->purchasedChecks_.insert(checkId);
        activeInstance_->checkCallback_(checkId);
    }
}

void __fastcall ShopMan::hookCKibaShop_PurchaseItem(void *pShop)
{
    auto *shopBase = reinterpret_cast<uint8_t *>(pShop);
    uint8_t stateBefore = shopBase[SHOP_SUB_STATE];

    // Snapshot inventory count for the selected item before purchase
    uint16_t *inventorySlot = nullptr;
    uint16_t savedCount = 0;
    if (activeInstance_ && activeInstance_->currentShopId_ >= 0 && activeInstance_->socket_.getSlotConfig().randomizeShops)
    {
        uint8_t scrollOffset = shopBase[SHOP_SCROLL_OFFSET];
        uint8_t visualSelectIndex = shopBase[SHOP_VISUAL_SELECT_INDEX];
        int selectedSlot = scrollOffset + visualSelectIndex;
        auto *stockPtr = *reinterpret_cast<uint8_t **>(shopBase + SHOP_STOCK_PTR);
        int32_t itemType = *reinterpret_cast<int32_t *>(stockPtr + selectedSlot * 0x20);

        if (itemType >= 0 && itemType < okami::ItemTypes::NUM_ITEM_TYPES && apgame::collectionData.is_bound())
        {
            inventorySlot = &apgame::collectionData->inventory[itemType];
            savedCount = *inventorySlot;
        }
    }

    if (originalCKibaShop_PurchaseItem_)
    {
        originalCKibaShop_PurchaseItem_(pShop);
    }

    // Kiba shop confirmed state: 0x96 transitions 1->0 and 0x95 == 0 (cancelled is 0x95 == 1)
    if (stateBefore == 1 && shopBase[SHOP_SUB_STATE] == 0 && shopBase[SHOP_OUTER_STATE] == 0 && activeInstance_ && activeInstance_->currentShopId_ >= 0)
    {
        // Restore inventory count to undo the item grant (server sends the real item)
        if (inventorySlot)
            *inventorySlot = savedCount;

        uint8_t scrollOffset = shopBase[SHOP_SCROLL_OFFSET];
        uint8_t visualSelectIndex = shopBase[SHOP_VISUAL_SELECT_INDEX];
        int selectedSlot = scrollOffset + visualSelectIndex;

        int64_t checkId = checks::getShopCheckId(activeInstance_->currentShopId_, selectedSlot);
        activeInstance_->purchasedChecks_.insert(checkId);
        activeInstance_->checkCallback_(checkId);
    }
}

void __fastcall ShopMan::hookCItemShop_SortStock(void *pShop, uint8_t numItems)
{
    // When we're running a randomized shop, bypass the game's ordering table
    // filter and copy items straight through in their original slot order.
    if (activeInstance_ && activeInstance_->currentShopId_ >= 0 && activeInstance_->socket_.getSlotConfig().randomizeShops)
    {
        auto *shopBase = reinterpret_cast<uint8_t *>(pShop);
        auto *inputBuf = *reinterpret_cast<int64_t **>(shopBase + SHOP_INPUT_BUF_PTR);
        auto *outputBuf = *reinterpret_cast<int64_t **>(shopBase + SHOP_OUTPUT_BUF_PTR);

        if (inputBuf && outputBuf)
        {
            // Each entry is 8 bytes: {int32_t itemType, int32_t cost}
            std::memcpy(outputBuf, inputBuf, static_cast<size_t>(numItems) * 8);
            return;
        }
    }

    // Fall through to original sort for non-randomized shops
    if (originalCItemShop_SortStock_)
    {
        originalCItemShop_SortStock_(pShop, numItems);
    }
}

int __fastcall ShopMan::hookIsGrayedOut(void *pShop, int slotIndex)
{
    if (activeInstance_ && activeInstance_->currentShopId_ >= 0 && activeInstance_->socket_.getSlotConfig().randomizeShops)
    {
        int64_t checkId = checks::getShopCheckId(activeInstance_->currentShopId_, slotIndex);

        // Already purchased this AP slot
        if (activeInstance_->purchasedChecks_.contains(checkId))
            return 0; // grayed

        // Check if player can afford it
        auto *shopBase = reinterpret_cast<uint8_t *>(pShop);
        auto *stockPtr = *reinterpret_cast<uint8_t **>(shopBase + SHOP_STOCK_PTR);
        int32_t cost = *reinterpret_cast<int32_t *>(stockPtr + slotIndex * 0x20 + 0x10);

        if (apgame::collectionData.is_bound())
        {
            auto money = static_cast<int32_t>(apgame::collectionData->currentMoney);
            if (money < cost)
                return 0; // can't afford
        }

        return 1; // purchasable
    }

    return originalIsGrayedOut_ ? originalIsGrayedOut_(pShop, slotIndex) : 1;
}

int __fastcall ShopMan::hookIsPurchased(void *pShop, int slotIndex)
{
    if (activeInstance_ && activeInstance_->currentShopId_ >= 0 && activeInstance_->socket_.getSlotConfig().randomizeShops)
    {
        int64_t checkId = checks::getShopCheckId(activeInstance_->currentShopId_, slotIndex);
        return activeInstance_->purchasedChecks_.contains(checkId) ? 1 : 0;
    }

    return originalIsPurchased_ ? originalIsPurchased_(pShop, slotIndex) : 0;
}

void __fastcall ShopMan::hookBuyQtyDialog(void *pShop)
{
    // For AP shops, skip the quantity dialog entirely and advance straight
    // to state 5 (purchase confirmation). The vanilla quantity dialog hangs
    // because canIncrementQty returns 0 for maxStack=1 items, which disables
    // all input including cancel. State 3 already sets qty=1 at offset 0x86.
    if (activeInstance_ && activeInstance_->currentShopId_ >= 0 && activeInstance_->socket_.getSlotConfig().randomizeShops)
    {
        auto *shopBase = reinterpret_cast<uint8_t *>(pShop);
        shopBase[SHOP_OUTER_STATE] = 5; // outer state → 5 (purchase confirmation)
        shopBase[SHOP_SUB_STATE] = 0;   // sub-state → 0 (init)
        return;
    }

    if (originalBuyQtyDialog_)
    {
        originalBuyQtyDialog_(pShop);
    }
}

} // namespace checks
