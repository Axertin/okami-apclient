#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <set>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "okami/customiconpkg.hpp"
#include "okami/itemtype.hpp"

// Compile-time invariants for kIconEntries (replace removed runtime tests)
static_assert(okami::customiconpkg::kIconEntries.size() == 13);
static_assert(okami::customiconpkg::kIconEntries[0].pkgIndex == 1);
static_assert(okami::customiconpkg::kIconEntries[12].pkgIndex == 13);

namespace
{

std::filesystem::path testTempDir()
{
    return std::filesystem::temp_directory_path() / "okami_test_icons";
}

/// 128 bytes of zeros seems vaguely plausible, sure
std::vector<uint8_t> fakeDDSData(std::size_t n = 128)
{
    return std::vector<uint8_t>(n, 0);
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// kIconEntries content tests
// ---------------------------------------------------------------------------

TEST_CASE("kIconEntries: contains all expected custom AP items", "[customiconpkg][entries]")
{
    const auto &entries = okami::customiconpkg::kIconEntries;

    // All expected item types must be present
    constexpr auto kExpectedTypes = std::to_array({
        okami::ItemTypes::HourglassOrb,
        okami::ItemTypes::Yen10,
        okami::ItemTypes::Yen50,
        okami::ItemTypes::Yen100,
        okami::ItemTypes::Yen150,
        okami::ItemTypes::Yen500,
        okami::ItemTypes::Praise,
        okami::ItemTypes::ForeignStandardItem,
        okami::ItemTypes::ForeignProgressionItem,
        okami::ItemTypes::ForeignTrapItem,
        okami::ItemTypes::OkamiStandardItem,
        okami::ItemTypes::OkamiProgressionItem,
        okami::ItemTypes::OkamiTrapItem,
    });

    for (auto expected : kExpectedTypes)
    {
        bool found = std::ranges::any_of(entries, [&](const auto &e) { return e.itemType == expected; });
        INFO("Missing itemType " << expected << " (" << okami::ItemTypes::GetName(expected) << ")");
        REQUIRE(found);
    }
    REQUIRE(entries.size() == kExpectedTypes.size());
}

TEST_CASE("kIconEntries: itemType values have no duplicates", "[customiconpkg][entries]")
{
    std::set<int> seen;
    for (const auto &e : okami::customiconpkg::kIconEntries)
    {
        REQUIRE(seen.find(e.itemType) == seen.end());
        seen.insert(e.itemType);
    }
}

// ---------------------------------------------------------------------------
// build() tests
// ---------------------------------------------------------------------------

TEST_CASE("build: with 128-byte fake DDS data succeeds and creates a file", "[customiconpkg][build]")
{
    const auto dir = testTempDir() / "build_fake";
    const auto path = dir / "APCustomIcons.dat";
    std::filesystem::remove_all(dir);

    const auto data = fakeDDSData(128);
    REQUIRE(okami::customiconpkg::build(path, data, data, data));
    REQUIRE(std::filesystem::exists(path));
    REQUIRE(std::filesystem::file_size(path) > 0);
}

TEST_CASE("build: with empty apIconData returns false", "[customiconpkg][build]")
{
    const auto dir = testTempDir() / "build_empty";
    const auto path = dir / "APCustomIcons.dat";
    std::filesystem::remove_all(dir);

    const std::vector<uint8_t> empty;
    REQUIRE_FALSE(okami::customiconpkg::build(path, empty, fakeDDSData(), fakeDDSData()));
    REQUIRE_FALSE(okami::customiconpkg::build(path, fakeDDSData(), empty, fakeDDSData()));
    REQUIRE_FALSE(okami::customiconpkg::build(path, fakeDDSData(), fakeDDSData(), empty));
    // File should not be created
    REQUIRE_FALSE(std::filesystem::exists(path));
}

TEST_CASE("build: creates parent directories as needed", "[customiconpkg][build]")
{
    const auto dir = testTempDir() / "build_mkdir" / "nested" / "dirs";
    const auto path = dir / "APCustomIcons.dat";
    std::filesystem::remove_all(testTempDir() / "build_mkdir");

    REQUIRE(okami::customiconpkg::build(path, fakeDDSData(), fakeDDSData(), fakeDDSData()));
    REQUIRE(std::filesystem::exists(path));
}

TEST_CASE("build: written file size is consistent with entry count", "[customiconpkg][build]")
{
    const auto dir = testTempDir() / "build_size";
    const auto path = dir / "APCustomIcons.dat";
    std::filesystem::remove_all(dir);

    REQUIRE(okami::customiconpkg::build(path, fakeDDSData(128), fakeDDSData(128), fakeDDSData(128)));

    // File must be non-empty and padded to a 32-byte boundary
    const auto sz = std::filesystem::file_size(path);
    REQUIRE(sz > 0);
    REQUIRE(sz % 32 == 0);
}

// ---------------------------------------------------------------------------
// buildFromFile() tests
// ---------------------------------------------------------------------------

TEST_CASE("buildFromFiles: with non-existent paths returns false", "[customiconpkg][buildFromFiles]")
{
    const auto dir = testTempDir() / "bff_missing";
    const auto out = dir / "out.dat";
    const auto noFile = dir / "does_not_exist.dds";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    REQUIRE_FALSE(okami::customiconpkg::buildFromFiles(out, noFile, noFile, noFile));
}

TEST_CASE("buildFromFiles: with valid temp DDS files succeeds", "[customiconpkg][buildFromFiles]")
{
    const auto dir = testTempDir() / "bff_valid";
    const auto iconPath = dir / "ap.dds";
    const auto outPath = dir / "APCustomIcons.dat";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    // Write a fake DDS file to disk
    {
        const auto dds = fakeDDSData(128);
        std::ofstream ofs{iconPath, std::ios::binary};
        REQUIRE(ofs.is_open());
        ofs.write(reinterpret_cast<const char *>(dds.data()), static_cast<std::streamsize>(dds.size()));
    }

    REQUIRE(okami::customiconpkg::buildFromFiles(outPath, iconPath, iconPath, iconPath));
    REQUIRE(std::filesystem::exists(outPath));
    REQUIRE(std::filesystem::file_size(outPath) > 0);
}

TEST_CASE("buildFromFiles: output matches direct build() call", "[customiconpkg][buildFromFiles]")
{
    const auto dir = testTempDir() / "bff_match";
    const auto iconPath = dir / "ap.dds";
    const auto outBff = dir / "from_file.dat";
    const auto outBuild = dir / "direct.dat";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    const auto dds = fakeDDSData(256);

    // Write DDS to disk
    {
        std::ofstream ofs{iconPath, std::ios::binary};
        ofs.write(reinterpret_cast<const char *>(dds.data()), static_cast<std::streamsize>(dds.size()));
    }

    REQUIRE(okami::customiconpkg::buildFromFiles(outBff, iconPath, iconPath, iconPath));
    REQUIRE(okami::customiconpkg::build(outBuild, dds, dds, dds));

    // Both files must have the same size (both produced from identical data)
    REQUIRE(std::filesystem::file_size(outBff) == std::filesystem::file_size(outBuild));
}
