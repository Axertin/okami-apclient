#pragma warning(push, 0)
#include <unordered_set>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#pragma warning(pop)

#include "okami/data/structs.hpp"
#include "okami/memoryaccessor.hpp"

TEST_CASE("BitField basic operations", "[game-data]")
{
    // Create a BitField wrapped in MemoryAccessor for testing
    okami::BitField<32> testBits{};
    okami::MemoryAccessor<okami::BitField<32>> accessor;
    accessor.bind(reinterpret_cast<uintptr_t>(&testBits));

    SECTION("Set and get individual bits")
    {
        accessor->Set(5);

        REQUIRE(accessor->IsSet(5));
        REQUIRE_FALSE(accessor->IsSet(4));
        REQUIRE_FALSE(accessor->IsSet(6));
    }

    SECTION("Multiple bit operations")
    {
        accessor->Set(0);
        accessor->Set(15);
        accessor->Set(31);

        REQUIRE(accessor->IsSet(0));
        REQUIRE(accessor->IsSet(15));
        REQUIRE(accessor->IsSet(31));

        auto indices = accessor->GetSetIndices();
        REQUIRE(indices.size() == 3);
        REQUIRE(std::find(indices.begin(), indices.end(), 0) != indices.end());
        REQUIRE(std::find(indices.begin(), indices.end(), 15) != indices.end());
        REQUIRE(std::find(indices.begin(), indices.end(), 31) != indices.end());
    }

    SECTION("Clear operations")
    {
        accessor->Set(10);
        accessor->Set(20);

        accessor->Clear(10);

        REQUIRE_FALSE(accessor->IsSet(10));
        REQUIRE(accessor->IsSet(20));
    }

    SECTION("HasAnySet functionality")
    {
        REQUIRE_FALSE(accessor->HasAnySet());

        accessor->Set(1);
        REQUIRE(accessor->HasAnySet());

        accessor->Clear(1);
        REQUIRE_FALSE(accessor->HasAnySet());
    }
}

TEST_CASE("BitField size constraints", "[game-data]")
{
    SECTION("Standard map bitfield size")
    {
        // Test that 512-bit fields work correctly
        okami::BitField<512> largeBits{};
        okami::MemoryAccessor<okami::BitField<512>> accessor;
        accessor.bind(reinterpret_cast<uintptr_t>(&largeBits));

        accessor->Set(0);
        accessor->Set(256);
        accessor->Set(511);

        REQUIRE(accessor->IsSet(0));
        REQUIRE(accessor->IsSet(256));
        REQUIRE(accessor->IsSet(511));

        auto indices = accessor->GetSetIndices();
        REQUIRE(indices.size() == 3);
    }
}
