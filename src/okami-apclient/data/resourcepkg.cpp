#include "okami/resourcepkg.hpp"

#include <algorithm>
#include <fstream>
#include <mutex>
#include <ranges>
#include <span>
#include <string>
#include <vector>

#include "okami/blowfish.hpp"

namespace okami
{

// ---------------------------------------------------------------------------
// Internal FileBuffer helper (not exposed in any public header)
// ---------------------------------------------------------------------------
namespace
{

class FileBuffer
{
  public:
    void append(const auto &value)
    {
        const auto *p = reinterpret_cast<const uint8_t *>(&value);
        m_buf.insert(m_buf.end(), p, p + sizeof(value));
    }

    void append_range(const std::ranges::range auto &container)
    {
        std::ranges::for_each(container, [this](const auto &v) { append(v); });
    }

    void append_bytes(std::size_t count, uint8_t value = 0)
    {
        m_buf.insert(m_buf.end(), count, value);
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return m_buf.size();
    }

    [[nodiscard]] const uint8_t *data() const noexcept
    {
        return m_buf.data();
    }

    [[nodiscard]] std::vector<uint8_t> &get_buffer() noexcept
    {
        return m_buf;
    }

  private:
    std::vector<uint8_t> m_buf;
};

// Ensure BlowFish::Create is called exactly once with the Okami cipher key.
void ensureBlowfishReady()
{
    static std::once_flag s_flag;
    std::call_once(s_flag, []() { Nippon::BlowFish::Create(std::string(Nippon::kCipherKey)); });
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// ResourcePackage implementation
// ---------------------------------------------------------------------------

void ResourcePackage::addEntry(ResourceType type, std::span<const uint8_t> data)
{
    m_entries.push_back({type, std::vector<uint8_t>(data.begin(), data.end())});
}

void ResourcePackage::addBlank()
{
    m_entries.push_back({ResourceType{}, {}});
}

bool ResourcePackage::write(const std::filesystem::path &filename) const
{
    ensureBlowfishReady();

    try
    {
        std::filesystem::create_directories(filename.parent_path());
    }
    catch (...)
    {
        return false;
    }

    FileBuffer result;

    // numElems includes the ROF sentinel (+1)
    const auto numElems = static_cast<uint32_t>(m_entries.size() + 1);
    result.append(numElems);

    // Calculate starting offset: after header counts + all offsets + all type tags
    // Header layout: uint32_t numElems  +  numElems*uint32_t offsets  +  numElems*ResourceType tags
    const uint32_t headerBytes = static_cast<uint32_t>((1 + numElems + numElems) * sizeof(uint32_t));

    std::vector<uint32_t> entryOffsets;
    entryOffsets.reserve(numElems);

    uint32_t offset = headerBytes;
    for (const auto &entry : m_entries)
    {
        entryOffsets.push_back(offset);
        result.append(offset);
        offset += static_cast<uint32_t>(entry.data.size());
    }
    // ROF sentinel offset
    entryOffsets.push_back(offset);
    result.append(offset);

    // Type tags for all entries
    std::ranges::for_each(m_entries, [&result](const Entry &e) { result.append(e.type); });
    // ROF sentinel type tag
    result.append(ResourceType{'R', 'O', 'F', '\0'});

    // Raw data blobs
    std::ranges::for_each(m_entries, [&result](const Entry &e) { result.append_range(e.data); });

    // ROF section: 8-byte magic + 64-bit offsets
    const std::string rofHead = "RUNOFS64";
    result.append_range(rofHead);
    std::ranges::for_each(entryOffsets,
                          [&result](uint32_t o)
                          {
                              const uint64_t o64 = o;
                              result.append(o64);
                          });

    // Pad to 32-byte boundary
    if (const std::size_t rem = result.size() % 32; rem != 0)
        result.append_bytes(32 - rem);

    // Encrypt in-place
    Nippon::BlowFish::Encrypt(result.get_buffer());

    // Write to disk
    try
    {
        std::ofstream out{filename, std::ios::binary};
        if (!out)
            return false;
        out.write(reinterpret_cast<const char *>(result.data()), static_cast<std::streamsize>(result.size()));
        return out.good();
    }
    catch (...)
    {
        return false;
    }
}

} // namespace okami
