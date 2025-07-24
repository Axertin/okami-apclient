#include <array>
#include <cstdint>
#include <vector>

#include "okami/shopdata.h"

/***
 * Simple shop data definition (no variations as in vanilla).
 * In order to work, shop variations need to also be eliminated.
 */
class ShopDefinition
{
  private:
    std::vector<std::uint8_t> dataISL;

    bool dirty = true;
    okami::SellValueArray sellValues = okami::DefaultItemSellPrices;
    std::vector<okami::ItemShopStock> itemStock;

    void RebuildISL();
    void CheckDirty();

  public:
    ShopDefinition();

    // Warning: Do NOT call when shop is currently open.
    const std::uint8_t *GetData();

    void SetStock(const std::vector<okami::ItemShopStock> &stock);
    void AddItem(okami::ItemTypes::Enum item, std::int32_t cost);
    void ClearStock();
    void SetSellValueOverride(okami::ItemTypes::Enum item, std::int32_t sellValue);
    void SetSellValues(okami::SellValueArray sellValues);
};
