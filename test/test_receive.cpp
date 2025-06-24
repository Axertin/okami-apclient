#include <catch2/catch_test_macros.hpp>

#include "okami/brushes.hpp"
#include "okami/items.hpp"
#include "okami/memoryaccessor.hpp"
#include "okami/memorymap.hpp"
#include "receive.h"

TEST_CASE("receiveAPItem properly sets brush flags", "[receive]")
{
    uint8_t mockUsable[4] = {0};
    uint8_t mockObtained[4] = {0};

    okami::AmmyUsableBrushes.bind(reinterpret_cast<uintptr_t>(&mockUsable));
    okami::AmmyObtainedBrushes.bind(reinterpret_cast<uintptr_t>(&mockObtained));

    receiveAPItem(0x103);

    REQUIRE(okami::AmmyUsableBrushes->IsSet(0x3));
    REQUIRE(okami::AmmyObtainedBrushes->IsSet(0x3));
}
