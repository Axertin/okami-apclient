#include <catch2/catch_test_macros.hpp>
#include "okami/bitfieldflags.hpp"
#include <set>

enum class TestFlag : uint8_t
{
    A = 0,  // byte 0, bit 0
    B = 1,  // byte 0, bit 1
    C = 7,  // byte 0, bit 7
    D = 8,  // byte 1, bit 0
    E = 15, // byte 1, bit 7
    F = 23  // byte 2, bit 7
};

constexpr size_t TestBytes = 3; // 24 bits
using TestFlags = okami::BitfieldFlagsStorage<TestFlag, TestBytes>;

TEST_CASE("BitfieldFlagsStorage - Copy and Equality", "[bitfield]")
{
    TestFlags flags1;
    TestFlags flags2;

    flags1.clearAll();
    flags2.clearAll();

    flags1.set(TestFlag::B);
    flags1.set(TestFlag::F);

    REQUIRE_FALSE(flags1.isEqual(flags2.view()));

    flags2.copyFrom(flags1.view());
    REQUIRE(flags2.isEqual(flags1.view()));
    REQUIRE(flags2.has(TestFlag::B));
    REQUIRE(flags2.has(TestFlag::F));
}

TEST_CASE("BitfieldFlagsStorage - diffBits correctness", "[bitfield]")
{
    using Storage = okami::BitfieldFlagsStorage<TestFlag, TestBytes>;

    SECTION("Diff with BitfieldFlags view")
    {
        Storage prev;
        Storage current;

        prev.clearAll();
        current.clearAll();

        prev.set(TestFlag::A);
        prev.set(TestFlag::B);
        prev.set(TestFlag::D);

        current.set(TestFlag::B); // same
        current.set(TestFlag::C); // new
        current.set(TestFlag::D); // same
        current.set(TestFlag::F); // new

        auto diff = prev.diffBits(current.view());
        std::set<TestFlag> diffSet{diff.begin(), diff.end()};

        REQUIRE(diffSet.size() == 3);
        REQUIRE(diffSet.find(TestFlag::A) != diffSet.end());
        REQUIRE(diffSet.find(TestFlag::C) != diffSet.end());
        REQUIRE(diffSet.find(TestFlag::F) != diffSet.end());
    }

    SECTION("Diff with another BitfieldFlagsStorage")
    {
        Storage s1;
        Storage s2;

        s1.clearAll();
        s2.clearAll();

        s1.set(TestFlag::A);
        s1.set(TestFlag::C);
        s1.set(TestFlag::F);

        s2.set(TestFlag::A); // same
        s2.set(TestFlag::D); // new

        auto diff = s1.diffBits(s2);
        std::set<TestFlag> diffSet{diff.begin(), diff.end()};

        REQUIRE(diffSet.size() == 3);
        REQUIRE(diffSet.find(TestFlag::C) != diffSet.end());
        REQUIRE(diffSet.find(TestFlag::D) != diffSet.end());
        REQUIRE(diffSet.find(TestFlag::F) != diffSet.end());
    }

    SECTION("Default-constructed storages are equal")
    {
        Storage s1, s2;
        REQUIRE(s1.isEqual(s2));
    }

    SECTION("Identical modifications result in equality")
    {
        Storage s1, s2;
        s1.set(TestFlag::A);
        s2.set(TestFlag::A);
        REQUIRE(s1.isEqual(s2));
    }

    SECTION("Different modifications result in inequality")
    {
        Storage s1, s2;
        s1.set(TestFlag::A);
        s2.set(TestFlag::B);
        REQUIRE_FALSE(s1.isEqual(s2));
    }

    SECTION("Subsequent matching operations restore equality")
    {
        Storage s1, s2;
        s1.set(TestFlag::A);
        s2.set(TestFlag::A);
        s1.set(TestFlag::B);
        s2.set(TestFlag::B);
        REQUIRE(s1.isEqual(s2));
    }

    SECTION("Clearing a bit in one breaks equality")
    {
        Storage s1, s2;
        s1.set(TestFlag::A);
        s2.set(TestFlag::A);
        s1.clear(TestFlag::A);
        REQUIRE_FALSE(s1.isEqual(s2));
    }
}
