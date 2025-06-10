#include <catch2/catch_test_macros.hpp>
#include "okami/bitfieldflags.hpp"
#include "okami/memoryaccessor.hpp"
#include "okami/brushes.hpp"

TEST_CASE("MemoryAccessor reads and writes scalar types", "[memory]")
{
    uint32_t simulatedMemory = 0;

    okami::MemoryAccessor<uint32_t> accessor(reinterpret_cast<uintptr_t>(&simulatedMemory));

    // Write a value
    accessor.set(123456);
    REQUIRE(simulatedMemory == 123456);

    // Read the value back
    uint32_t value = accessor.get();
    REQUIRE(value == 123456);

    // Operator overload
    accessor = 789;
    REQUIRE(simulatedMemory == 789);
    REQUIRE(static_cast<uint32_t>(accessor) == 789);
}

TEST_CASE("MemoryAccessor::bind changes the memory address", "[memory]")
{
    uint32_t first = 111;
    uint32_t second = 222;

    okami::MemoryAccessor<uint32_t> accessor;
    accessor.bind(reinterpret_cast<uintptr_t>(&first));
    REQUIRE(accessor.get() == 111);

    accessor.bind(reinterpret_cast<uintptr_t>(&second));
    REQUIRE(accessor.get() == 222);
}

TEST_CASE("BrushOverlay flags can be read from mock memory", "[memory]")
{
    uint8_t mockMemory[4] = {
        0b00010100, // greensprout and bloom
        0b00000010, // thunderbolt
        0, 0};

    okami::MemoryAccessor<okami::BitfieldFlags<okami::BrushOverlay>> accessor;
    accessor.bind<4>(reinterpret_cast<uintptr_t>(mockMemory));

    auto brush = accessor.get();

    REQUIRE(brush.has(okami::BrushOverlay::greensprout));
    REQUIRE(brush.has(okami::BrushOverlay::bloom));
    REQUIRE(brush.has(okami::BrushOverlay::thunderbolt));
    REQUIRE_FALSE(brush.has(okami::BrushOverlay::icestorm));
}

TEST_CASE("MemoryAccessor::raw returns bound address", "[memory]")
{
    uint32_t dummy = 0;
    auto addr = reinterpret_cast<uintptr_t>(&dummy);

    okami::MemoryAccessor<uint32_t> accessor(addr);
    REQUIRE(accessor.raw() == addr);
}

TEST_CASE("BitfieldFlags ignores bits outside allocated range", "[memory]")
{
    uint8_t mem[2] = {0xFF, 0xFF};
    okami::MemoryAccessor<okami::BitfieldFlags<okami::BrushOverlay>> accessor;
    accessor.bind<1>(reinterpret_cast<uintptr_t>(mem)); // only bind 1 byte

    auto flags = accessor.get();

    // All enum values in byte 0 should be set
    REQUIRE(flags.has(okami::BrushOverlay::greensprout));
    REQUIRE(flags.has(okami::BrushOverlay::bloom));
    REQUIRE_FALSE(flags.has(okami::BrushOverlay::thunderbolt)); // second byte
}
