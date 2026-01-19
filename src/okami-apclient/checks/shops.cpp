#include "shops.hpp"

#include <cstring>
#include <list>

#include <okami/itemtype.hpp>
#include <okami/maptype.hpp>
#include <wolf_framework.hpp>

#include "../isocket.h"
#include "../rewards/game_items.hpp"
#include "check_types.hpp"

namespace checks
{

constexpr int MAX_SHOP_SLOTS = 50;

// Memory offsets
constexpr uintptr_t SHOP_VARIATION_OFFSET = 0x4420C0;
constexpr uintptr_t LOAD_RSC_OFFSET = 0x1B1770;
constexpr uintptr_t SHOP_METADATA_OFFSET = 0x441E40;
constexpr uintptr_t KIBA_SHOP_GET_STOCK_LIST_OFFSET = 0x43F5A0;
constexpr uintptr_t ITEM_SHOP_PURCHASE_OFFSET = 0x43CA30;
constexpr uintptr_t KIBA_SHOP_PURCHASE_OFFSET = 0x43FD30;
constexpr uintptr_t EXTERIOR_MAP_ID_OFFSET = 0xB6B240;

// Shop struct offsets (from cShopBase)
constexpr uintptr_t SHOP_SCROLL_OFFSET = 0x82;
constexpr uintptr_t SHOP_VISUAL_SELECT_INDEX = 0x83;

// Static member initialization
ShopMan::GetShopVariationFn ShopMan::originalGetShopVariation_ = nullptr;
ShopMan::LoadRscFn ShopMan::originalLoadRsc_ = nullptr;
ShopMan::GetShopMetadataFn ShopMan::originalGetShopMetadata_ = nullptr;
ShopMan::GetKibaShopStockListFn ShopMan::originalCKibaShop_GetShopStockList_ = nullptr;
ShopMan::ItemShopPurchaseFn ShopMan::originalCItemShop_PurchaseItem_ = nullptr;
ShopMan::KibaShopPurchaseFn ShopMan::originalCKibaShop_PurchaseItem_ = nullptr;
ShopMan *ShopMan::activeInstance_ = nullptr;

// ============================================================================
// ShopDefinition Implementation
// ============================================================================

ShopDefinition::ShopDefinition()
{
}

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
    return dataISL.data();
}

void ShopDefinition::SetStock(const std::vector<okami::ItemShopStock> &stock)
{
    if (stock.size() < MaxShopStockSize)
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
// Shop Instances (21 regular + 3 demon fang)
// ============================================================================

static ShopDefinition AgataForestShop;
static ShopDefinition ArkOfYamatoShop;
static ShopDefinition CityCheckpointShop;
static ShopDefinition DragonPalaceShop;
static ShopDefinition KamikiPostTeiShop;
static ShopDefinition KamikiShop;
static ShopDefinition KamikiPastShop;
static ShopDefinition KamuiShop;
static ShopDefinition KusaShop;
static ShopDefinition MoonCaveInteriorShop;
static ShopDefinition MoonCaveStaircaseShop;
static ShopDefinition NRyoshimaShop;
static ShopDefinition OniIslandInteriorShop;
static ShopDefinition PonctanShop;
static ShopDefinition RyoshimaShop;
static ShopDefinition SasaShop;
static ShopDefinition SeianWeaponShop;
static ShopDefinition SeianFishShop;
static ShopDefinition ShinshuShop;
static ShopDefinition TakaPassShop;
static ShopDefinition WawkuShrineShop;

static std::vector<okami::ItemShopStock> AgataFangShop;
static std::vector<okami::ItemShopStock> ArkOfYamatoFangShop;
static std::vector<okami::ItemShopStock> ImperialPalaceFangShop;

const void *GetCurrentItemShopData(uint16_t mapId, uint32_t shopNum)
{
    auto mapID = static_cast<okami::MapID::Enum>(mapId);

    switch (mapID)
    {
    case okami::MapID::AgataForestCursed:
    case okami::MapID::AgataForestHealed:
        return AgataForestShop.GetData();
    case okami::MapID::ArkofYamato:
        return ArkOfYamatoShop.GetData();
    case okami::MapID::CityCheckpoint:
        return CityCheckpointShop.GetData();
    case okami::MapID::DragonPalace:
        return DragonPalaceShop.GetData();
    case okami::MapID::KamikiVillagePostTei:
        return KamikiPostTeiShop.GetData();
    case okami::MapID::KamikiVillageCursed:
    case okami::MapID::KamikiVillage:
        return KamikiShop.GetData();
    case okami::MapID::KamikiVillagePast:
        return KamikiPastShop.GetData();
    case okami::MapID::KamuiCursed:
    case okami::MapID::KamuiHealed:
        return KamuiShop.GetData();
    case okami::MapID::KusaVillage:
        return KusaShop.GetData();
    case okami::MapID::MoonCaveInterior:
        return MoonCaveInteriorShop.GetData();
    case okami::MapID::MoonCaveStaircaseAndOrochiArena:
        return MoonCaveStaircaseShop.GetData();
    case okami::MapID::NRyoshimaCoast:
        return NRyoshimaShop.GetData();
    case okami::MapID::OniIslandLowerInterior:
        return OniIslandInteriorShop.GetData();
    case okami::MapID::Ponctan:
        return PonctanShop.GetData();
    case okami::MapID::RyoshimaCoastCursed:
    case okami::MapID::RyoshimaCoastHealed:
        return RyoshimaShop.GetData();
    case okami::MapID::SasaSanctuary:
        return SasaShop.GetData();
    case okami::MapID::SeianCityCommonersQuarter:
        switch (shopNum)
        {
        case 0:
            return SeianWeaponShop.GetData();
        case 1:
            return SeianFishShop.GetData();
        }
        break;
    case okami::MapID::ShinshuFieldCursed:
    case okami::MapID::ShinshuFieldHealed:
        return ShinshuShop.GetData();
    case okami::MapID::TakaPassCursed:
    case okami::MapID::TakaPassHealed:
        return TakaPassShop.GetData();
    case okami::MapID::WawkuShrine:
        return WawkuShrineShop.GetData();
    default:
        break;
    }
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

std::optional<int> GetShopIdForMap(uint16_t mapId, uint32_t shopNum)
{
    auto mapID = static_cast<okami::MapID::Enum>(mapId);

    switch (mapID)
    {
    case okami::MapID::AgataForestCursed:
    case okami::MapID::AgataForestHealed:
        return 0;
    case okami::MapID::ArkofYamato:
        return 1;
    case okami::MapID::CityCheckpoint:
        return 2;
    case okami::MapID::DragonPalace:
        return 3;
    case okami::MapID::KamikiVillagePostTei:
        return 4;
    case okami::MapID::KamikiVillageCursed:
    case okami::MapID::KamikiVillage:
        return 5;
    case okami::MapID::KamikiVillagePast:
        return 6;
    case okami::MapID::KamuiCursed:
    case okami::MapID::KamuiHealed:
        return 7;
    case okami::MapID::KusaVillage:
        return 8;
    case okami::MapID::MoonCaveInterior:
        return 9;
    case okami::MapID::MoonCaveStaircaseAndOrochiArena:
        return 10;
    case okami::MapID::NRyoshimaCoast:
        return 11;
    case okami::MapID::OniIslandLowerInterior:
        return 12;
    case okami::MapID::Ponctan:
        return 13;
    case okami::MapID::RyoshimaCoastCursed:
    case okami::MapID::RyoshimaCoastHealed:
        return 14;
    case okami::MapID::SasaSanctuary:
        return 15;
    case okami::MapID::SeianCityCommonersQuarter:
        return (shopNum == 0) ? 16 : 17; // 16 = weapon, 17 = fish
    case okami::MapID::ShinshuFieldCursed:
    case okami::MapID::ShinshuFieldHealed:
        return 18;
    case okami::MapID::TakaPassCursed:
    case okami::MapID::TakaPassHealed:
        return 19;
    case okami::MapID::WawkuShrine:
        return 20;
    default:
        return std::nullopt;
    }
}

// Get ShopDefinition pointer by shop ID
static ShopDefinition *GetShopById(int shopId)
{
    switch (shopId)
    {
    case 0:
        return &AgataForestShop;
    case 1:
        return &ArkOfYamatoShop;
    case 2:
        return &CityCheckpointShop;
    case 3:
        return &DragonPalaceShop;
    case 4:
        return &KamikiPostTeiShop;
    case 5:
        return &KamikiShop;
    case 6:
        return &KamikiPastShop;
    case 7:
        return &KamuiShop;
    case 8:
        return &KusaShop;
    case 9:
        return &MoonCaveInteriorShop;
    case 10:
        return &MoonCaveStaircaseShop;
    case 11:
        return &NRyoshimaShop;
    case 12:
        return &OniIslandInteriorShop;
    case 13:
        return &PonctanShop;
    case 14:
        return &RyoshimaShop;
    case 15:
        return &SasaShop;
    case 16:
        return &SeianWeaponShop;
    case 17:
        return &SeianFishShop;
    case 18:
        return &ShinshuShop;
    case 19:
        return &TakaPassShop;
    case 20:
        return &WawkuShrineShop;
    default:
        return nullptr;
    }
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

    // Check both shopNum 0 and 1 (for Seian which has 2 shops)
    for (uint32_t shopNum = 0; shopNum <= 1; ++shopNum)
    {
        auto shopId = GetShopIdForMap(mapId, shopNum);
        if (!shopId)
        {
            continue;
        }

        int slotCount = socket_.getSlotConfig().shopSlots;
        for (int slot = 0; slot < slotCount; ++slot)
        {
            locationsToScout.push_back(checks::getShopCheckId(*shopId, slot));
        }
    }

    if (locationsToScout.empty())
    {
        wolf::logDebug("[ShopMan] No shops found for map 0x%04X", mapId);
        return;
    }

    // Check if still connected before scouting
    if (!socket_.isConnected())
    {
        wolf::logWarning("[ShopMan] Cannot scout - not connected to server");
        return;
    }

    wolf::logDebug("[ShopMan] Scouting %zu locations for map 0x%04X", locationsToScout.size(), mapId);

    // Scout all locations synchronously
    // This may fail if the server doesn't have these locations defined
    auto scoutedItems = socket_.scoutLocationsSync(locationsToScout, 0);

    if (scoutedItems.empty())
    {
        wolf::logDebug("[ShopMan] Scouting returned no results (server may not have shop locations)");
        return;
    }

    // Store results in cache
    for (const auto &item : scoutedItems)
    {
        scoutedItems_[item.location] = item;
    }

    wolf::logInfo("[ShopMan] Received %zu scouted items for map 0x%04X", scoutedItems.size(), mapId);
}

void ShopMan::populateShopFromScoutedData(int shopId)
{
    ShopDefinition *shop = GetShopById(shopId);
    if (!shop)
    {
        wolf::logWarning("[ShopMan] Invalid shop ID %d for population", shopId);
        return;
    }

    shop->ClearStock();

    int slotCount = socket_.getSlotConfig().shopSlots;
    for (int slot = 0; slot < slotCount; ++slot)
    {
        int64_t locationId = checks::getShopCheckId(shopId, slot);
        auto it = scoutedItems_.find(locationId);
        if (it == scoutedItems_.end())
        {
            // No item at this slot - stop here (sparse slots not supported yet)
            break;
        }

        const ScoutedItem &scouted = it->second;

        // Convert AP item ID to game item
        okami::ItemTypes::Enum gameItem;
        if (rewards::game_items::isDirectGameItem(scouted.item))
        {
            gameItem = static_cast<okami::ItemTypes::Enum>(rewards::game_items::getItemId(scouted.item));
        }
        else if (rewards::game_items::isProgressiveWeapon(scouted.item))
        {
            auto nextItem = rewards::game_items::getNextItemToGrant(scouted.item);
            if (nextItem)
            {
                gameItem = static_cast<okami::ItemTypes::Enum>(*nextItem);
            }
            else
            {
                // At max stage, use placeholder
                gameItem = okami::ItemTypes::ArchipelagoTestItem1;
            }
        }
        else
        {
            // Non-game items (brushes, event flags, etc.) use placeholder
            gameItem = okami::ItemTypes::ArchipelagoTestItem1;
        }

        // TODO: Get actual price from AP data or slot_data
        constexpr int32_t placeholderCost = 100;
        shop->AddItem(gameItem, placeholderCost);
    }

    wolf::logDebug("[ShopMan] Populated shop %d with items from scouted data", shopId);
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
        // Only inject custom shop data if connected to AP server
        // TODO: Also check slot_data for whether shops are randomized
        if (activeInstance_ && activeInstance_->socket_.isConnected())
        {
            // Get current map ID
            uintptr_t mainBase = reinterpret_cast<uintptr_t>(wolf::getModuleBase("main.dll"));
            auto *currentMapPtr = reinterpret_cast<uint16_t *>(mainBase + EXTERIOR_MAP_ID_OFFSET);
            uint16_t mapId = *currentMapPtr;

            // Get shop ID for this map/shopNum combination
            auto shopId = GetShopIdForMap(mapId, nIdx);
            if (shopId)
            {
                // Track current shop for purchase detection
                activeInstance_->currentShopId_ = *shopId;

                // Lazy scouting: scout on first shop access for this map
                activeInstance_->scoutShopsForMap(mapId);

                // Only populate from scouted data if we got results
                // If scouting returned nothing (server doesn't have shop locations), fall back to vanilla
                if (!activeInstance_->scoutedItems_.empty())
                {
                    // Populate shop from scouted data
                    activeInstance_->populateShopFromScoutedData(*shopId);

                    // Return the ISL data
                    const void *pResult = GetCurrentItemShopData(mapId, nIdx);
                    if (pResult != nullptr)
                    {
                        wolf::logDebug("[ShopMan] Injecting custom ISL data for map 0x%04X shop %u (shopId=%d)", mapId, nIdx, *shopId);
                        return pResult;
                    }
                }
                else
                {
                    wolf::logDebug("[ShopMan] No scouted items for map 0x%04X, using vanilla shop", mapId);
                }
            }
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
            wolf::logDebug("[ShopMan] Injecting custom demon fang shop data for map 0x%04X", mapId);
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
    if (activeInstance_ && activeInstance_->currentShopId_ >= 0)
    {
        // Read selected item index from shop struct
        auto *shopBase = reinterpret_cast<uint8_t *>(pShop);
        uint8_t scrollOffset = shopBase[SHOP_SCROLL_OFFSET];
        uint8_t visualSelectIndex = shopBase[SHOP_VISUAL_SELECT_INDEX];
        int selectedSlot = scrollOffset + visualSelectIndex;

        // Calculate check ID and send it
        int64_t checkId = checks::getShopCheckId(activeInstance_->currentShopId_, selectedSlot);
        wolf::logInfo("[ShopMan] Item shop purchase: shop=%d slot=%d checkId=%lld", activeInstance_->currentShopId_, selectedSlot, checkId);

        activeInstance_->checkCallback_(checkId);
    }

    // Call original to complete the purchase
    if (originalCItemShop_PurchaseItem_)
    {
        originalCItemShop_PurchaseItem_(pShop);
    }
}

void __fastcall ShopMan::hookCKibaShop_PurchaseItem(void *pShop)
{
    if (activeInstance_ && activeInstance_->currentShopId_ >= 0)
    {
        // Read selected item index from shop struct (same offsets as item shop)
        auto *shopBase = reinterpret_cast<uint8_t *>(pShop);
        uint8_t scrollOffset = shopBase[SHOP_SCROLL_OFFSET];
        uint8_t visualSelectIndex = shopBase[SHOP_VISUAL_SELECT_INDEX];
        int selectedSlot = scrollOffset + visualSelectIndex;

        // Calculate check ID and send it
        int64_t checkId = checks::getShopCheckId(activeInstance_->currentShopId_, selectedSlot);
        wolf::logInfo("[ShopMan] Demon fang shop purchase: shop=%d slot=%d checkId=%lld", activeInstance_->currentShopId_, selectedSlot, checkId);

        activeInstance_->checkCallback_(checkId);
    }

    // Call original to complete the purchase
    if (originalCKibaShop_PurchaseItem_)
    {
        originalCKibaShop_PurchaseItem_(pShop);
    }
}

} // namespace checks
