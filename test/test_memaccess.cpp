#include <catch2/catch_test_macros.hpp>

#include "okami/brushes.hpp"
#include "okami/memoryaccessor.hpp"
#include "okami/memorymap.hpp"

TEST_CASE("MemoryAccessor reads and writes scalar types", "[memory]")
{
    uint32_t simulatedMemory = 0;

    okami::MemoryAccessor<uint32_t> accessor(
        reinterpret_cast<uintptr_t>(&simulatedMemory));

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

TEST_CASE("MemoryAccessor::raw returns bound address", "[memory]")
{
    uint32_t dummy = 0;
    auto addr = reinterpret_cast<uintptr_t>(&dummy);

    okami::MemoryAccessor<uint32_t> accessor(addr);
    REQUIRE(accessor.raw() == addr);
}

TEST_CASE("MemoryAccessor deals with BitFields", "[memory]")
{
    uint8_t dummy[4] = {0};
    auto addr = reinterpret_cast<uintptr_t>(&dummy);

    okami::MemoryAccessor<okami::BitField<32>> accessor(addr);

    accessor->Set(0x4);

    REQUIRE(accessor->HasAnySet() == true);
    REQUIRE(accessor->GetSetIndices().size() == 1);
    REQUIRE(accessor->GetSetIndices()[0] == 0x4);
}
