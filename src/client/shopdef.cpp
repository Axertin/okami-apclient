#include "shop.h"

#include <array>
#include <cstdint>

ShopDefinition::ShopDefinition()
{
}

void ShopDefinition::RebuildISL()
{
    this->dataISL.clear();
    this->dataISL.reserve(sizeof(okami::ISLHeader) + sizeof(std::uint32_t) + sizeof(okami::ItemShopStock) * this->itemStock.size() +
                          sizeof(okami::SellValueArray));

    auto appendData = [&](auto &value)
    {
        std::uint8_t *valBytes = reinterpret_cast<std::uint8_t *>(&value);
        this->dataISL.insert(this->dataISL.end(), valBytes, valBytes + sizeof(value));
    };

    okami::ISLHeader header = {"ISL", 1};
    appendData(header);

    std::uint32_t numItems = this->itemStock.size();
    appendData(numItems);

    for (okami::ItemShopStock &item : this->itemStock)
    {
        appendData(item);
    }

    appendData(this->sellValues);
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
    this->itemStock = stock;
    this->dirty = true;
}

void ShopDefinition::AddItem(okami::ItemTypes::Enum item, std::int32_t cost)
{
    this->itemStock.emplace_back(okami::ItemShopStock{item, cost});
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

void ShopDefinition::SetSellValues(okami::SellValueArray sellValues)
{
    this->sellValues = sellValues;
    this->dirty = true;
}
