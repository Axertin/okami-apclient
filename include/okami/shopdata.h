#pragma once
#include <array>

#include "okami/itemtype.hpp"

namespace okami
{

using SellValueArray = std::array<std::int32_t, ItemTypes::NUM_ITEM_TYPES>;

extern SellValueArray DefaultItemSellPrices;
extern SellValueArray DefaultTakaPassItemSellPrices;
extern SellValueArray DefaultSeianFishShopItemSellPrices;

#pragma pack(push, 1)
struct ISLHeader
{
    char magic[4];
    std::int32_t variations;
    std::int32_t unk2;
    std::int32_t unk3;
};

struct ItemShopStock
{
    std::int32_t itemType;
    std::int32_t cost;
    std::int32_t unk;
};

struct ShopEntry
{
    std::int32_t num;
    ItemShopStock stock[1];
};
#pragma pack(pop)

} // namespace okami
