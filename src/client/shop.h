#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "okami/filebuffer.h"
#include "okami/shopdata.h"

// Hardcoded max shop stock size unless we want to hook the memory allocation for it
// It always allocates this much regardless of how much stock was loaded.
constexpr size_t MaxShopStockSize = 50;

/***
 * Simple shop data definition (no variations as in vanilla).
 * In order to work, shop variations need to also be eliminated.
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

void InitializeShopData();
const void *GetCurrentItemShopData(uint32_t shopNum);
okami::ItemShopStock *GetCurrentDemonFangShopData(uint32_t *pNumItems);
