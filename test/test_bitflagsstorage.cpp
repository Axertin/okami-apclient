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

    auto v1 = flags1.view();
    auto v2 = flags2.view();

    flags1.clearAll();
    flags2.clearAll();

    v1.set(TestFlag::B);
    v1.set(TestFlag::F);

    REQUIRE_FALSE(flags1.isEqual(flags2.view()));

    flags2.copyFrom(v1);
    REQUIRE(flags2.isEqual(flags1.view()));
    REQUIRE(v2.has(TestFlag::B));
    REQUIRE(v2.has(TestFlag::F));
}

TEST_CASE("BitfieldFlagsStorage - diffBits correctness", "[bitfield]")
{
    TestFlags prev;
    TestFlags current;

    auto vprev = prev.view();
    auto vcur = current.view();

    prev.clearAll();
    current.clearAll();

    vprev.set(TestFlag::A);
    vprev.set(TestFlag::B);
    vprev.set(TestFlag::D);

    vcur.set(TestFlag::B); // same
    vcur.set(TestFlag::C); // different
    vcur.set(TestFlag::D); // same
    vcur.set(TestFlag::F); // different

    auto diff = prev.diffBits(vcur);
    auto diffSet = std::set<TestFlag>{diff.begin(), diff.end()};

    REQUIRE(diffSet.size() == 3);
    REQUIRE(diffSet.find(TestFlag::A) != diffSet.end());
    REQUIRE(diffSet.find(TestFlag::C) != diffSet.end());
    REQUIRE(diffSet.find(TestFlag::F) != diffSet.end());
}
