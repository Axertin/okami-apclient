#include <catch2/catch_test_macros.hpp>
#include "okami/bitfieldflags.hpp"
#include "okami/memoryaccessor.hpp"
#include "okami/memorymap.hpp"
#include "okami/brushes.hpp"
#include "okami/items.hpp"
#include "receive.h"

TEST_CASE("receiveAPItem properly sets brush flags", "[receive]")
{
    uint8_t MockUsable[4] = {0};
    uint8_t MockObtained[4] = {0};

    okami::AmmyObtainedBrushTechniques.bind<4>(reinterpret_cast<uintptr_t>(MockObtained));
    okami::AmmyUsableBrushTechniques.bind<4>(reinterpret_cast<uintptr_t>(MockUsable));

    receiveAPItem(0x103);

    REQUIRE(MockUsable[0] == 0b00001000); // dot_trees
    REQUIRE(MockUsable[1] == 0);
    REQUIRE(MockUsable[2] == 0);
    REQUIRE(MockUsable[3] == 0);

    REQUIRE(MockObtained[0] == 0b00001000); // dot_trees
    REQUIRE(MockObtained[1] == 0);
    REQUIRE(MockObtained[2] == 0);
    REQUIRE(MockObtained[3] == 0);
}
