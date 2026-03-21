#include "okami/customiconpkg.hpp"

#include <cstring>
#include <fstream>
#include <vector>

#include "okami/blowfish.hpp"
#include "okami/resourcepkg.hpp"

namespace okami::customiconpkg
{

namespace
{

struct RawEntry
{
    ResourceType type{};
    std::vector<uint8_t> data{};
};

/// Decrypt and parse all entries from an Okami Blowfish DAT package.
/// Returns an empty vector on any read or format error.
static std::vector<RawEntry> readPackageEntries(const std::filesystem::path &path)
{
    // BlowFish::Create is idempotent with the same key — safe to call multiple times.
    Nippon::BlowFish::Create(std::string(Nippon::kCipherKey));

    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f)
        return {};
    const auto sz = static_cast<std::size_t>(f.tellg());
    if (sz < sizeof(uint32_t))
        return {};
    f.seekg(0);
    std::vector<uint8_t> buf(sz);
    f.read(reinterpret_cast<char *>(buf.data()), static_cast<std::streamsize>(sz));
    if (!f.good())
        return {};

    Nippon::BlowFish::Decrypt(buf);

    uint32_t numElems = 0;
    std::memcpy(&numElems, buf.data(), sizeof(numElems));
    if (numElems < 1)
        return {};

    const uint32_t actual = numElems - 1;                          // ROF sentinel is the last element
    const std::size_t hdrSize = sizeof(uint32_t)                   // numElems
                                + numElems * sizeof(uint32_t)      // offsets[]
                                + numElems * sizeof(ResourceType); // type tags[]
    if (buf.size() < hdrSize)
        return {};

    const auto *offsets = reinterpret_cast<const uint32_t *>(buf.data() + sizeof(uint32_t));
    const auto *types = reinterpret_cast<const ResourceType *>(buf.data() + sizeof(uint32_t) + numElems * sizeof(uint32_t));

    std::vector<RawEntry> result;
    result.reserve(actual);
    for (uint32_t i = 0; i < actual; ++i)
    {
        const uint32_t start = offsets[i];
        const uint32_t end = offsets[i + 1]; // next offset (or ROF sentinel offset)
        if (start > end || end > static_cast<uint32_t>(buf.size()))
            return {};
        RawEntry e;
        e.type = types[i];
        e.data.assign(buf.begin() + start, buf.begin() + end);
        result.push_back(std::move(e));
    }
    return result;
}

} // anonymous namespace

bool build(const std::filesystem::path &outPath, std::span<const uint8_t> standardData, std::span<const uint8_t> progressionData,
           std::span<const uint8_t> trapData)
{
    if (standardData.empty() || progressionData.empty() || trapData.empty())
        return false;

    const std::array<std::span<const uint8_t>, 3> ddsBuffers = {standardData, progressionData, trapData};

    ResourcePackage pkg;
    pkg.addBlank(); // index-0 reserved

    for (const auto &e : kIconEntries)
        pkg.addEntry(ResourceType{'D', 'D', 'S', '\0'}, ddsBuffers[static_cast<size_t>(e.ddsIndex)]);

    return pkg.write(outPath);
}

bool buildFromFiles(const std::filesystem::path &outPath, const std::filesystem::path &standardPath, const std::filesystem::path &progressionPath,
                    const std::filesystem::path &trapPath)
{
    auto readFile = [](const std::filesystem::path &p) -> std::vector<uint8_t>
    {
        std::ifstream ifs{p, std::ios::binary};
        if (!ifs)
            return {};
        return {std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{}};
    };

    const auto std_data = readFile(standardPath);
    const auto prog_data = readFile(progressionPath);
    const auto trap_data = readFile(trapPath);

    return build(outPath, std_data, prog_data, trap_data);
}

bool buildCombinedFromFiles(const std::filesystem::path &outPath, const std::filesystem::path &vanillaPath, const std::filesystem::path &standardPath,
                            const std::filesystem::path &progressionPath, const std::filesystem::path &trapPath, int &vanillaCount)
{
    vanillaCount = 0;

    const auto vanillaEntries = readPackageEntries(vanillaPath);
    if (vanillaEntries.empty())
        return false;
    vanillaCount = static_cast<int>(vanillaEntries.size());

    auto readFile = [](const std::filesystem::path &p) -> std::vector<uint8_t>
    {
        std::ifstream ifs{p, std::ios::binary};
        if (!ifs)
            return {};
        return {std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{}};
    };

    const auto std_data = readFile(standardPath);
    const auto prog_data = readFile(progressionPath);
    const auto trap_data = readFile(trapPath);

    if (std_data.empty() || prog_data.empty() || trap_data.empty())
        return false;

    const std::array<std::span<const uint8_t>, 3> ddsBuffers = {std_data, prog_data, trap_data};

    ResourcePackage pkg;

    // Copy vanilla entries at their original indices (0..vanillaCount-1)
    for (const auto &e : vanillaEntries)
        pkg.addEntry(e.type, e.data);

    // Append custom entries at indices vanillaCount..vanillaCount+kIconEntries.size()-1
    for (const auto &e : kIconEntries)
        pkg.addEntry(ResourceType{'D', 'D', 'S', '\0'}, ddsBuffers[static_cast<size_t>(e.ddsIndex)]);

    return pkg.write(outPath);
}

} // namespace okami::customiconpkg
