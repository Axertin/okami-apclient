#pragma once
#include <cstdint>

namespace okami
{

enum ItemParamFlags
{
    ItmIsFood = 0x100000,
    ItmIsTreasureTome = 0x8000000,
    ItmCantBeRemoved = 0x40000000,
};

struct ItemParam
{
    uint16_t maxCount;
    int16_t value;
    uint32_t flags;
    uint8_t category;
    // 3 bytes padding
};

} // namespace okami
