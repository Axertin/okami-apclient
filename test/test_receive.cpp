#include <catch2/catch_test_macros.hpp>

#include "okami/brushes.hpp"
#include "okami/items.hpp"
#include "okami/memoryaccessor.hpp"
#include "okami/memorymap.hpp"
#include "receive.h"

TEST_CASE("receiveAPItem properly sets brush flags", "[receive]")
{
    okami::CollectionData mockMem = {0};

    okami::AmmyCollections.bind(reinterpret_cast<uintptr_t>(&mockMem));

    receiveAPItem(0x103);

    REQUIRE(mockMem.world.usableBrushTechniques.IsSet(0x3));
    REQUIRE(mockMem.world.obtainedBrushTechniques.IsSet(0x3));
}
