#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

#include <okami/filebuffer.h>
#include <okami/shopdata.h>

// Forward declarations
class ISocket;
struct ScoutedItem;

namespace checks
{

// Hardcoded max shop stock size unless we want to hook the memory allocation for it
// It always allocates this much regardless of how much stock was loaded.
constexpr size_t MaxShopStockSize = 50;

/**
 * @brief Simple shop data definition (no variations as in vanilla).
 *
 * In order to work, shop variations need to also be eliminated via hook.
 */
class ShopDefinition
{
  private:
    FileBuffer dataISL;

    bool dirty = true;
    okami::SellValueArray sellValues = okami::DefaultItemSellPrices;
    std::vector<okami::ItemShopStock> itemStock;

    void RebuildISL();
    void CheckDirty();

  public:
    ShopDefinition();

    // Warning: Do NOT call when shop is currently open.
    const uint8_t *GetData();

    void SetStock(const std::vector<okami::ItemShopStock> &stock);
    void AddItem(okami::ItemTypes::Enum item, int32_t cost);
    void ClearStock();
    void SetSellValueOverride(okami::ItemTypes::Enum item, int32_t sellValue);
    void SetSellValues(const okami::SellValueArray &replacementSellValues);
};

// Shop data lookup functions
void InitializeShopData();
const void *GetCurrentItemShopData(uint16_t mapId, uint32_t shopNum);
okami::ItemShopStock *GetCurrentDemonFangShopData(uint16_t mapId, uint32_t *pNumItems);

// Get shop ID for a given map (returns nullopt if no shop on this map)
[[nodiscard]] std::optional<int> GetShopIdForMap(uint16_t mapId, uint32_t shopNum);

// Get the number of slots for a shop (max items it can hold)
[[nodiscard]] constexpr int GetShopSlotCount([[maybe_unused]] int shopId)
{
    return MaxShopStockSize;
}

/**
 * @brief Manager for shop randomization and purchase detection
 *
 * Follows the ContainerMan pattern for WOLF hook management.
 */
class ShopMan
{
  public:
    using CheckCallback = std::function<void(int64_t)>;

    explicit ShopMan(ISocket &socket, CheckCallback checkCallback);
    ~ShopMan();

    // Non-copyable, non-movable (owns hooks)
    ShopMan(const ShopMan &) = delete;
    ShopMan &operator=(const ShopMan &) = delete;
    ShopMan(ShopMan &&) = delete;
    ShopMan &operator=(ShopMan &&) = delete;

    void initialize();
    void shutdown();
    void reset();

  private:
    // Hook function types
    using GetShopVariationFn = int64_t(__fastcall *)(void *, uint32_t, char **);
    using LoadRscFn = const void *(__fastcall *)(void *, const char *, uint32_t);
    using GetShopMetadataFn = const void *(__fastcall *)(void *, uint32_t, uint32_t *, char **);
    using GetKibaShopStockListFn = okami::ItemShopStock *(__fastcall *)(void *, uint32_t *);
    using ItemShopPurchaseFn = void(__fastcall *)(void *);
    using KibaShopPurchaseFn = void(__fastcall *)(void *);

    // Hook implementations
    static int64_t __fastcall hookGetShopVariation(void *pUnk, uint32_t shopNum, char **pszShopTextureName);
    static const void *__fastcall hookLoadRsc(void *pRscPackage, const char *pszType, uint32_t nIdx);
    static okami::ItemShopStock *__fastcall hookCKibaShop_GetShopStockList(void *pKibaShop, uint32_t *numItems);
    static void __fastcall hookCItemShop_PurchaseItem(void *pShop);
    static void __fastcall hookCKibaShop_PurchaseItem(void *pShop);

    // Original function pointers
    static GetShopVariationFn originalGetShopVariation_;
    static LoadRscFn originalLoadRsc_;
    static GetShopMetadataFn originalGetShopMetadata_;
    static GetKibaShopStockListFn originalCKibaShop_GetShopStockList_;
    static ItemShopPurchaseFn originalCItemShop_PurchaseItem_;
    static KibaShopPurchaseFn originalCKibaShop_PurchaseItem_;

    static ShopMan *activeInstance_;

    // Scouting helpers
    void scoutShopsForMap(uint16_t mapId);
    void populateShopFromScoutedData(int shopId);

    ISocket &socket_;
    CheckCallback checkCallback_;
    bool initialized_ = false;

    // Scouting cache
    uint16_t scoutedMapId_ = 0;
    std::unordered_map<int64_t, ScoutedItem> scoutedItems_; // location -> item

    // Current shop tracking (set when ISL loads, used by purchase hooks)
    int currentShopId_ = -1;
};

} // namespace checks
