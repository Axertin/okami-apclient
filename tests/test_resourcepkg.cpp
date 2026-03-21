#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "okami/blowfish.hpp"
#include "okami/resourcepkg.hpp"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace
{

std::filesystem::path testTempDir()
{
    return std::filesystem::temp_directory_path() / "okami_test";
}

/// Decrypt a file written by ResourcePackage::write() and return raw bytes.
std::vector<uint8_t> decryptFile(const std::filesystem::path &path)
{
    std::ifstream ifs{path, std::ios::binary};
    REQUIRE(ifs.is_open());

    std::vector<uint8_t> data{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{}};
    Nippon::BlowFish::Create(std::string(Nippon::kCipherKey));
    Nippon::BlowFish::Decrypt(data);
    return data;
}

/// Minimal valid DDS header (128 bytes with "DDS " magic).
std::vector<uint8_t> makeFakeDDS(std::size_t totalSize = 128)
{
    std::vector<uint8_t> dds(totalSize, 0);
    // DDS magic: 'D','D','S',' '
    dds[0] = 'D';
    dds[1] = 'D';
    dds[2] = 'S';
    dds[3] = ' ';
    return dds;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// ResourceType tests
// ---------------------------------------------------------------------------

TEST_CASE("ResourceType: equality operator compares tags", "[resourcepkg][ResourceType]")
{
    okami::ResourceType a{'D', 'D', 'S', '\0'};
    okami::ResourceType b{'D', 'D', 'S', '\0'};
    okami::ResourceType c{'R', 'O', 'F', '\0'};

    REQUIRE(a == b);
    REQUIRE_FALSE(a == c);
}

// ---------------------------------------------------------------------------
// ResourcePackage construction / entry management
// ---------------------------------------------------------------------------

TEST_CASE("ResourcePackage: addBlank increments size", "[resourcepkg]")
{
    okami::ResourcePackage pkg;
    pkg.addBlank();
    REQUIRE(pkg.size() == 1);

    pkg.addBlank();
    REQUIRE(pkg.size() == 2);
}

TEST_CASE("ResourcePackage: addEntry increments size", "[resourcepkg]")
{
    okami::ResourcePackage pkg;
    const auto dds = makeFakeDDS();
    pkg.addEntry({'D', 'D', 'S', '\0'}, dds);
    REQUIRE(pkg.size() == 1);

    pkg.addEntry({'D', 'D', 'S', '\0'}, dds);
    REQUIRE(pkg.size() == 2);
}

// ---------------------------------------------------------------------------
// ResourcePackage write / encryption
// ---------------------------------------------------------------------------

TEST_CASE("ResourcePackage: write creates a file", "[resourcepkg][write]")
{
    const auto dir = testTempDir() / "write_creates";
    const auto path = dir / "test.dat";
    std::filesystem::remove_all(dir);

    okami::ResourcePackage pkg;
    pkg.addBlank();
    pkg.addEntry({'D', 'D', 'S', '\0'}, makeFakeDDS());

    REQUIRE(pkg.write(path));
    REQUIRE(std::filesystem::exists(path));
    REQUIRE(std::filesystem::file_size(path) > 0);
}

TEST_CASE("ResourcePackage: written file is encrypted (first 4 bytes are not 'DDS ')", "[resourcepkg][write]")
{
    const auto dir = testTempDir() / "encrypt_check";
    const auto path = dir / "test.dat";
    std::filesystem::remove_all(dir);

    okami::ResourcePackage pkg;
    pkg.addBlank();
    const auto dds = makeFakeDDS();
    pkg.addEntry({'D', 'D', 'S', '\0'}, dds);

    REQUIRE(pkg.write(path));

    std::ifstream ifs{path, std::ios::binary};
    REQUIRE(ifs.is_open());
    std::array<char, 4> magic{};
    ifs.read(magic.data(), 4);

    // The first 4 bytes of an encrypted file must NOT be "DDS " or the uint32_t numElems
    // in plaintext form (which would be 0x00000002 little-endian = {2,0,0,0}).
    // Blowfish-ECB will produce something else.
    const bool looksPlaintext = (magic[0] == 'D' && magic[1] == 'D' && magic[2] == 'S' && magic[3] == ' ');
    REQUIRE_FALSE(looksPlaintext);
}

TEST_CASE("ResourcePackage: decrypted header has correct numEntries", "[resourcepkg][write][decrypt]")
{
    const auto dir = testTempDir() / "decrypt_header";
    const auto path = dir / "test.dat";
    std::filesystem::remove_all(dir);

    okami::ResourcePackage pkg;
    pkg.addBlank();                                     // index 0
    pkg.addEntry({'D', 'D', 'S', '\0'}, makeFakeDDS()); // index 1
    pkg.addEntry({'D', 'D', 'S', '\0'}, makeFakeDDS()); // index 2

    REQUIRE(pkg.write(path));

    const auto decrypted = decryptFile(path);
    REQUIRE(decrypted.size() >= 4);

    // First uint32_t: numElems = numEntries + 1 (for ROF sentinel)
    uint32_t numElems = 0;
    std::memcpy(&numElems, decrypted.data(), sizeof(numElems));
    REQUIRE(numElems == 4); // 3 user entries + 1 ROF sentinel
}

TEST_CASE("ResourcePackage: decrypted file contains RUNOFS64 magic", "[resourcepkg][write][decrypt]")
{
    const auto dir = testTempDir() / "rof_magic";
    const auto path = dir / "test.dat";
    std::filesystem::remove_all(dir);

    okami::ResourcePackage pkg;
    pkg.addBlank();
    pkg.addEntry({'D', 'D', 'S', '\0'}, makeFakeDDS());

    REQUIRE(pkg.write(path));

    const auto decrypted = decryptFile(path);

    // Search for "RUNOFS64" anywhere in the decrypted bytes
    const std::string magic = "RUNOFS64";
    const auto it = std::search(decrypted.begin(), decrypted.end(), magic.begin(), magic.end());
    REQUIRE(it != decrypted.end());
}

TEST_CASE("ResourcePackage: ROF sentinel type tag is 'ROF\\0'", "[resourcepkg][write][decrypt]")
{
    const auto dir = testTempDir() / "rof_tag";
    const auto path = dir / "test.dat";
    std::filesystem::remove_all(dir);

    okami::ResourcePackage pkg;
    pkg.addBlank();                                     // entry 0
    pkg.addEntry({'D', 'D', 'S', '\0'}, makeFakeDDS()); // entry 1

    REQUIRE(pkg.write(path));

    const auto decrypted = decryptFile(path);
    REQUIRE(decrypted.size() >= 4);

    // numElems
    uint32_t numElems = 0;
    std::memcpy(&numElems, decrypted.data(), sizeof(numElems));
    REQUIRE(numElems == 3); // 2 entries + ROF sentinel

    // Type tags section starts at: 4 (numElems) + numElems*4 (offsets) = 4 + 12 = 16
    const std::size_t tagBase = sizeof(uint32_t) + numElems * sizeof(uint32_t);

    // Last type tag (ROF sentinel) is at tagBase + (numElems-1)*4
    const std::size_t rofTagOffset = tagBase + (numElems - 1) * sizeof(uint32_t);
    REQUIRE(decrypted.size() >= rofTagOffset + 4);

    REQUIRE(decrypted[rofTagOffset + 0] == 'R');
    REQUIRE(decrypted[rofTagOffset + 1] == 'O');
    REQUIRE(decrypted[rofTagOffset + 2] == 'F');
    REQUIRE(decrypted[rofTagOffset + 3] == '\0');
}

TEST_CASE("ResourcePackage: write returns false for unwritable path", "[resourcepkg][write]")
{
    // Use a path where the parent is a file, not a directory
    const auto dir = testTempDir() / "not_a_dir_file";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir.parent_path());
    {
        // Create a regular file at 'dir' so create_directories will fail
        std::ofstream block{dir};
        block << "block";
    }

    const auto badPath = dir / "sub" / "test.dat";

    okami::ResourcePackage pkg;
    pkg.addBlank();
    // write() should fail because it can't create_directories through a file
    REQUIRE_FALSE(pkg.write(badPath));
}

TEST_CASE("ResourcePackage: decrypted offsets are monotonically increasing", "[resourcepkg][write][decrypt]")
{
    const auto dir = testTempDir() / "offsets_check";
    const auto path = dir / "test.dat";
    std::filesystem::remove_all(dir);

    const auto dds = makeFakeDDS(256);

    okami::ResourcePackage pkg;
    pkg.addBlank();
    pkg.addEntry({'D', 'D', 'S', '\0'}, dds);
    pkg.addEntry({'D', 'D', 'S', '\0'}, dds);

    REQUIRE(pkg.write(path));

    const auto decrypted = decryptFile(path);
    REQUIRE(decrypted.size() >= 4);

    uint32_t numElems = 0;
    std::memcpy(&numElems, decrypted.data(), sizeof(numElems));
    // 3 entries + 1 ROF = 4
    REQUIRE(numElems == 4);

    // Read offsets (numElems uint32_t values starting at byte 4)
    std::vector<uint32_t> offsets(numElems);
    for (uint32_t i = 0; i < numElems; ++i)
        std::memcpy(&offsets[i], decrypted.data() + 4 + i * 4, sizeof(uint32_t));

    // Offsets must be non-decreasing (blank entries have 0 bytes so two adjacent
    // entries can share the same start offset).
    for (uint32_t i = 1; i < numElems; ++i)
        REQUIRE(offsets[i] >= offsets[i - 1]);

    // Entries with real data (entries 1 and 2, each 256 bytes) must have strictly
    // increasing offsets relative to entry 0 (blank).
    REQUIRE(offsets[1] == offsets[0]); // blank entry: 0 bytes
    REQUIRE(offsets[2] > offsets[1]);  // first DDS entry: 256 bytes
    REQUIRE(offsets[3] > offsets[2]);  // second DDS entry: 256 bytes
}

TEST_CASE("ResourcePackage: zero-entry write has ROF-only sentinel", "[resourcepkg]")
{
    // A package with no user entries should produce numElems == 1 (just the ROF sentinel).
    okami::ResourcePackage pkg;
    // pkg.size() == 0
    REQUIRE(pkg.size() == 0);

    const auto tmpDir = std::filesystem::temp_directory_path() / "okami_rp_zero_test";
    std::filesystem::create_directories(tmpDir);
    const auto outPath = tmpDir / "zero.dat";
    REQUIRE(pkg.write(outPath));

    // Decrypt and check numElems == 1
    std::ifstream f(outPath, std::ios::binary);
    const auto fileSize = std::filesystem::file_size(outPath);
    std::vector<uint8_t> buf(fileSize);
    f.read(reinterpret_cast<char *>(buf.data()), fileSize);
    Nippon::BlowFish::Create(std::string(Nippon::kCipherKey));
    Nippon::BlowFish::Decrypt(buf);

    uint32_t numElems{};
    std::memcpy(&numElems, buf.data(), sizeof(numElems));
    REQUIRE(numElems == 1); // only the ROF sentinel
}

TEST_CASE("ResourcePackage: ROF 64-bit offsets match 32-bit header offsets", "[resourcepkg]")
{
    const std::vector<uint8_t> fakeDds(64, 0xAB);
    okami::ResourcePackage pkg;
    pkg.addBlank();
    pkg.addEntry(okami::ResourceType{'D', 'D', 'S', '\0'}, fakeDds);

    const auto tmpDir = std::filesystem::temp_directory_path() / "okami_rp_rof_test";
    std::filesystem::create_directories(tmpDir);
    const auto outPath = tmpDir / "rof.dat";
    REQUIRE(pkg.write(outPath));

    // Decrypt
    std::ifstream f(outPath, std::ios::binary);
    const auto fileSize = std::filesystem::file_size(outPath);
    std::vector<uint8_t> buf(fileSize);
    f.read(reinterpret_cast<char *>(buf.data()), fileSize);
    Nippon::BlowFish::Create(std::string(Nippon::kCipherKey));
    Nippon::BlowFish::Decrypt(buf);

    // Parse header
    const uint32_t *p32 = reinterpret_cast<const uint32_t *>(buf.data());
    uint32_t numElems = p32[0]; // 3 = 1 blank + 1 DDS + 1 ROF
    REQUIRE(numElems == 3);

    // Collect 32-bit offsets from header (indices 1..numElems)
    std::vector<uint32_t> offsets32(numElems);
    for (uint32_t i = 0; i < numElems; ++i)
        offsets32[i] = p32[1 + i];

    // Find "RUNOFS64" in the buffer
    const std::string magic = "RUNOFS64";
    auto it = std::search(buf.begin(), buf.end(), magic.begin(), magic.end());
    REQUIRE(it != buf.end());

    // 64-bit offsets immediately follow the magic
    const uint64_t *p64 = reinterpret_cast<const uint64_t *>(&*(it + 8));
    for (uint32_t i = 0; i < numElems; ++i)
    {
        uint64_t o64{};
        std::memcpy(&o64, reinterpret_cast<const uint8_t *>(p64) + i * sizeof(uint64_t), sizeof(o64));
        REQUIRE(o64 == static_cast<uint64_t>(offsets32[i]));
    }
}
