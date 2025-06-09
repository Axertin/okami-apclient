
#pragma once
#include <cstdint>
#include "memoryaccessor.hpp"

namespace okami
{
    inline MemoryAccessor<uint16_t> InventoryItemID;
    inline MemoryAccessor<uint16_t> InventoryStringID;
    inline MemoryAccessor<uint8_t> InventoryFirstItemOffset;
    inline MemoryAccessor<uint8_t> InventoryColumn;
    inline MemoryAccessor<uint8_t> InventorySelectedItemRow;
    inline MemoryAccessor<int16_t> AmmyCurrentSunFragments;
    inline MemoryAccessor<int16_t> InventoryItemQualities; // 256-element Array
}
