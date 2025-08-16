#include <array>
#include <cstdint>
#include <iostream>

#include "shop.h"

ShopDefinition::ShopDefinition()
{
}

void ShopDefinition::RebuildISL()
{
    this->dataISL.clear();
    this->dataISL.reserve(sizeof(okami::ISLHeader) + sizeof(std::uint32_t) + sizeof(okami::ItemShopStock) * this->itemStock.size() +
                          sizeof(okami::SellValueArray));

    okami::ISLHeader header = {"ISL", 1};
    this->dataISL.append(header);

    std::uint32_t numItems = this->itemStock.size();
    this->dataISL.append(numItems);

    this->dataISL.append_range(this->itemStock);
    this->dataISL.append(this->sellValues);
}

void ShopDefinition::CheckDirty()
{
    if (this->dirty)
    {
        this->RebuildISL();
        this->dirty = false;
    }
}

const std::uint8_t *ShopDefinition::GetData()
{
    this->CheckDirty();
    return this->dataISL.data();
}

void ShopDefinition::SetStock(const std::vector<okami::ItemShopStock> &stock)
{
    if (stock.size() < MaxShopStockSize)
    {
        this->itemStock = stock;
    }
    else
    {
        std::cerr << "Max stock size in SetStock exceeded" << std::endl;
    }
    this->dirty = true;
}

void ShopDefinition::AddItem(okami::ItemTypes::Enum item, std::int32_t cost)
{
    if (this->itemStock.size() < MaxShopStockSize)
    {
        this->itemStock.emplace_back(okami::ItemShopStock{item, cost});
    }
    else
    {
        std::cerr << "Max stock size in AddItem exceeded" << std::endl;
    }
    this->dirty = true;
}

void ShopDefinition::ClearStock()
{
    this->itemStock.clear();
    this->dirty = true;
}

void ShopDefinition::SetSellValueOverride(okami::ItemTypes::Enum item, std::int32_t sellValue)
{
    this->sellValues[item] = sellValue;
    this->dirty = true;
}

void ShopDefinition::SetSellValues(const okami::SellValueArray &replacementSellValues)
{
    this->sellValues = replacementSellValues;
    this->dirty = true;
}
