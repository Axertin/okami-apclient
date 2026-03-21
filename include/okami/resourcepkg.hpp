#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

namespace okami
{

/// Four-character resource type tag used in Okami .dat packages.
struct ResourceType
{
    char tag[4]{};
    bool operator==(const ResourceType &) const = default;
};
static_assert(sizeof(ResourceType) == 4);

/// Write-only Blowfish-encrypted Okami resource package builder.
///
/// The .dat format (decrypted layout):
///   [uint32_t numEntries+1]                     <- +1 for ROF sentinel
///   [uint32_t offsets[numEntries+1]]             <- byte offsets into decrypted blob
///   [ResourceType typetags[numEntries+1]]        <- last one is "ROF\0"
///   [raw data blobs]
///   ["RUNOFS64"][uint64_t offsets[numEntries+1]] <- ROF section
///   [zero padding to 32-byte boundary]
///   <<< ENTIRE buffer is Blowfish-ECB encrypted >>>
class ResourcePackage
{
  public:
    ResourcePackage() = default;
    ~ResourcePackage() = default;

    ResourcePackage(const ResourcePackage &) = delete;
    ResourcePackage &operator=(const ResourcePackage &) = delete;
    ResourcePackage(ResourcePackage &&) = default;
    ResourcePackage &operator=(ResourcePackage &&) = default;

    /// Add a typed entry with the given raw data bytes.
    void addEntry(ResourceType type, std::span<const uint8_t> data);

    /// Add a blank (zero-length) entry with a null type tag.
    /// Used for the reserved index-0 slot.
    void addBlank();

    /// Number of entries currently in the package (excluding the ROF sentinel).
    [[nodiscard]] std::size_t size() const noexcept
    {
        return m_entries.size();
    }

    /// Encrypt and write the package to the given path.
    /// Creates parent directories as needed.
    /// Returns true on success, false on any I/O or encoding failure.
    [[nodiscard]] bool write(const std::filesystem::path &filename) const;

  private:
    struct Entry
    {
        ResourceType type{};
        std::vector<uint8_t> data{};
    };

    std::vector<Entry> m_entries;
};

} // namespace okami
