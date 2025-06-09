#include <catch2/catch_test_macros.hpp>
#include "okami/bitfieldflags.hpp"
#include "okami/memoryaccessor.hpp"
#include "okami/brushes.hpp"

TEST_CASE("BrushOverlay flags can be read from mock memory", "[memory]")
{
    uint8_t mockMemory[4] = {
        0b00010100, // greensprout and bloom
        0b00000010, // thunderbolt
        0, 0};

    okami::MemoryAccessor<okami::BitfieldFlags<okami::BrushOverlay>> accessor;
    accessor.bind(reinterpret_cast<uintptr_t>(mockMemory), 4);

    auto brush = accessor.get();

    REQUIRE(brush.has(okami::BrushOverlay::greensprout));
    REQUIRE(brush.has(okami::BrushOverlay::bloom));
    REQUIRE(brush.has(okami::BrushOverlay::thunderbolt));
    REQUIRE_FALSE(brush.has(okami::BrushOverlay::icestorm));
}
