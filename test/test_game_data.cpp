#pragma warning(push, 0)
#include <unordered_set>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#pragma warning(pop)

#include "okami/data/structs.hpp"
#include "okami/devdatamapdata.h"
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

TEST_CASE("Map data structure integrity", "[game-data]")
{
    SECTION("All documented maps exist in enum")
    {
        // Verify that every map in our documentation exists
        for (const auto &[mapId, mapDesc] : okami::mapDataDesc)
        {
            // NUM_MAP_TYPES seems to be 1 fewer than the actual number of maps - blame the '06 devs I guess
            REQUIRE(mapId <= okami::MapTypes::NUM_MAP_TYPES);
        }
    }

    SECTION("Kamiki Village has expected data")
    {
        auto it = okami::mapDataDesc.find(okami::MapTypes::KamikiVillage);
        REQUIRE(it != okami::mapDataDesc.end());

        const auto &desc = it->second;
        REQUIRE_FALSE(desc.worldStateBits.empty());
        REQUIRE_FALSE(desc.collectedObjects.empty());
        REQUIRE_FALSE(desc.treesBloomed.empty());

        // Verify some known mappings exist
        REQUIRE(desc.worldStateBits.count(1) > 0);   // "In god zone by the tree"
        REQUIRE(desc.collectedObjects.count(0) > 0); // First chest
    }

    SECTION("No duplicate bit indices within categories")
    {
        for (const auto &[mapId, mapDesc] : okami::mapDataDesc)
        {
            // Check for duplicate keys in worldStateBits
            std::unordered_set<unsigned> seenBits;
            for (const auto &[bitIndex, description] : mapDesc.worldStateBits)
            {
                REQUIRE(seenBits.find(bitIndex) == seenBits.end());
                seenBits.insert(bitIndex);
            }

            // Check for duplicate keys in collectedObjects
            std::unordered_set<unsigned> seenObjects;
            for (const auto &[objIndex, description] : mapDesc.collectedObjects)
            {
                REQUIRE(seenObjects.find(objIndex) == seenObjects.end());
                seenObjects.insert(objIndex);
            }
        }
    }

    SECTION("All descriptions are non-empty")
    {
        for (const auto &[mapId, mapDesc] : okami::mapDataDesc)
        {
            for (const auto &[bitIndex, description] : mapDesc.worldStateBits)
            {
                REQUIRE_FALSE(description.empty());
            }

            for (const auto &[objIndex, description] : mapDesc.collectedObjects)
            {
                // REQUIRE_FALSE(description.empty()); //TODO: Add descriptions to every object, then re-enable this check
            }

            for (const auto &[treeIndex, description] : mapDesc.treesBloomed)
            {
                REQUIRE_FALSE(description.empty());
            }
        }
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

    SECTION("Bit indices within documented ranges")
    {
        for (const auto &[mapId, mapDesc] : okami::mapDataDesc)
        {
            for (const auto &[bitIndex, description] : mapDesc.worldStateBits)
            {
                // Assuming 512-bit fields for map state
                REQUIRE(bitIndex < 512);
            }
        }
    }
}
