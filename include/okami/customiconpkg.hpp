#pragma once

#include <array>
#include <filesystem>
#include <span>
#include <string_view>

#include "okami/itemtype.hpp"

namespace okami::customiconpkg
{

/// Entry mapping an ItemType enum value to its 1-based package index.
/// Index 0 is always a blank reserved entry.
struct IconEntry
{
    int itemType;
    int pkgIndex;
    int ddsIndex; // 0=standard, 1=progression, 2=trap
};

/// All custom AP items that receive the AP icon. Indices are 1-based.
inline constexpr auto kIconEntries = std::to_array<IconEntry>({
    // Existing entries (vanilla items that lack shop icons):
    {okami::ItemTypes::HourglassOrb, 1, 0},
    {okami::ItemTypes::Yen10, 2, 0},
    {okami::ItemTypes::Yen50, 3, 0},
    {okami::ItemTypes::Yen100, 4, 0},
    {okami::ItemTypes::Yen150, 5, 0},
    {okami::ItemTypes::Yen500, 6, 0},
    {okami::ItemTypes::Praise, 7, 0},
    // Foreign dummy types (other player's items):
    {okami::ItemTypes::ForeignStandardItem, 8, 0},    // standard AP icon
    {okami::ItemTypes::ForeignProgressionItem, 9, 1}, // progression recolor
    {okami::ItemTypes::ForeignTrapItem, 10, 2},       // trap recolor
    // Okami-native dummy types (local player's non-displayable items):
    {okami::ItemTypes::OkamiStandardItem, 11, 0},
    {okami::ItemTypes::OkamiProgressionItem, 12, 1},
    {okami::ItemTypes::OkamiTrapItem, 13, 2},
});

/// Filename of the custom icon package, relative to the mod directory.
inline constexpr std::string_view kPackageFilename = "icons/APCustomIcons.dat";

/// Filename used to request the package via LoadRscPkgAsync.
/// Must match the path written under data_pc/ so the virtual FS tree can resolve it.
inline constexpr std::string_view kPackageResourceName = "archipelago/customicons.dat";

/// Build the encrypted .dat package from three distinct DDS blobs (standard, progression, trap).
/// All three spans must be non-empty. Returns true on success.
[[nodiscard]] bool build(const std::filesystem::path &outPath, std::span<const uint8_t> standardData, std::span<const uint8_t> progressionData,
                         std::span<const uint8_t> trapData);

/// Reads three DDS files from disk and calls build().
[[nodiscard]] bool buildFromFiles(const std::filesystem::path &outPath, const std::filesystem::path &standardPath, const std::filesystem::path &progressionPath,
                                  const std::filesystem::path &trapPath);

/// Build a combined package: all entries from the encrypted vanilla package at
/// vanillaPath, followed by the custom DDS icons.  The combined package is written
/// to outPath using the same Blowfish format.
/// vanillaCount receives the number of vanilla entries (= 0-based index of the
/// first custom entry in the combined package).
/// Returns true on success; false if the vanilla package cannot be read or DDS
/// files are missing.
[[nodiscard]] bool buildCombinedFromFiles(const std::filesystem::path &outPath, const std::filesystem::path &vanillaPath,
                                          const std::filesystem::path &standardPath, const std::filesystem::path &progressionPath,
                                          const std::filesystem::path &trapPath, int &vanillaCount);

} // namespace okami::customiconpkg
