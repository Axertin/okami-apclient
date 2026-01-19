#include <catch2/catch_test_macros.hpp>

#include "version_utils.h"

using version_utils::Version;
using C = version_utils::Compatibility;

TEST_CASE("parseVersion parses valid semver strings", "[version_utils]")
{
    SECTION("Basic version")
    {
        auto v = version_utils::parseVersion("1.2.3");
        REQUIRE(v.has_value());
        CHECK(v->major == 1);
        CHECK(v->minor == 2);
        CHECK(v->patch == 3);
    }

    SECTION("Multi-digit components")
    {
        auto v = version_utils::parseVersion("10.20.30");
        REQUIRE(v.has_value());
        CHECK(v->major == 10);
        CHECK(v->minor == 20);
        CHECK(v->patch == 30);
    }

    SECTION("Pre-release suffix stripped")
    {
        auto v = version_utils::parseVersion("0.6.0-dev.21");
        REQUIRE(v.has_value());
        CHECK(v->major == 0);
        CHECK(v->minor == 6);
        CHECK(v->patch == 0);
    }

    SECTION("Build metadata stripped")
    {
        auto v = version_utils::parseVersion("2.1.3+build.456");
        REQUIRE(v.has_value());
        CHECK(v->major == 2);
        CHECK(v->minor == 1);
        CHECK(v->patch == 3);
    }

    SECTION("Both pre-release and build metadata")
    {
        auto v = version_utils::parseVersion("1.0.0-alpha+001");
        REQUIRE(v.has_value());
        CHECK(v->major == 1);
        CHECK(v->minor == 0);
        CHECK(v->patch == 0);
    }
}

TEST_CASE("parseVersion rejects invalid strings", "[version_utils]")
{
    CHECK_FALSE(version_utils::parseVersion("").has_value());
    CHECK_FALSE(version_utils::parseVersion("1").has_value());
    CHECK_FALSE(version_utils::parseVersion("1.2").has_value());
    CHECK_FALSE(version_utils::parseVersion("a.b.c").has_value());
    CHECK_FALSE(version_utils::parseVersion(".1.2").has_value());
    CHECK_FALSE(version_utils::parseVersion("1..2").has_value());
    CHECK_FALSE(version_utils::parseVersion("1.2.").has_value());
}

TEST_CASE("checkCompatibility: pre-1.0 versions treat minor as breaking", "[version_utils]")
{
    // In semver, 0.x.y is "initial development" - any change can break the API
    // We treat minor version differences as breaking changes

    // Same 0.minor = compatible (patch doesn't matter)
    CHECK(version_utils::checkCompatibility({0, 6, 0}, {0, 6, 0}) == C::Compatible);
    CHECK(version_utils::checkCompatibility({0, 6, 99}, {0, 6, 0}) == C::Compatible);

    // Different 0.minor = breaking, even if client is "newer"
    CHECK(version_utils::checkCompatibility({0, 7, 0}, {0, 6, 0}) == C::MajorMismatch);
    CHECK(version_utils::checkCompatibility({0, 5, 0}, {0, 6, 0}) == C::MajorMismatch);
}

TEST_CASE("checkCompatibility: post-1.0 allows backwards-compatible minor upgrades", "[version_utils]")
{
    // Standard semver: minor versions add features but don't break existing ones
    // Client with higher minor has superset of features = compatible

    CHECK(version_utils::checkCompatibility({1, 5, 0}, {1, 3, 0}) == C::Compatible);
    CHECK(version_utils::checkCompatibility({1, 3, 0}, {1, 3, 0}) == C::Compatible);

    // Client with lower minor is missing features server expects
    CHECK(version_utils::checkCompatibility({1, 2, 0}, {1, 3, 0}) == C::ClientTooOld);
}

TEST_CASE("checkCompatibility: major version changes are always breaking", "[version_utils]")
{
    // Major version bump = intentional breaking change
    CHECK(version_utils::checkCompatibility({2, 0, 0}, {1, 9, 9}) == C::MajorMismatch);
    CHECK(version_utils::checkCompatibility({1, 0, 0}, {2, 0, 0}) == C::MajorMismatch);

    // Crossing the 0.x -> 1.x boundary is also a major change
    CHECK(version_utils::checkCompatibility({1, 0, 0}, {0, 9, 0}) == C::MajorMismatch);
    CHECK(version_utils::checkCompatibility({0, 9, 0}, {1, 0, 0}) == C::MajorMismatch);
}
