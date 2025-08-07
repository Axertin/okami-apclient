#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace okami
{
#pragma pack(push, 1)
struct MSDHeader
{
    uint32_t numEntries;
    uint64_t offsets[1];
};
#pragma pack(pop)

/**
 * @brief Used to modify MSD files by adding or replacing strings, and getting the new MSD data.
 */
class MSDManager
{
  private:
    std::vector<std::vector<uint16_t>> strings;

    bool dirty = false;
    std::vector<uint8_t> compiledMSD;

    void MakeDirty();
    void Rebuild();

    static std::vector<uint16_t> CompileString(const std::string &str);

  public:
    MSDManager();

    /**
     * @brief Reads original MSD content and prepares it for alteration.
     *
     * @param pData Pointer to MSD data.
     */
    void ReadMSD(const void *pData);

    /**
     * @brief Adds a string to this MSD file.
     *
     * @param str ASCII string to add.
     * @return uint32_t String index for future remapping in-game.
     */
    uint32_t AddString(const std::string &str);

    /**
     * @brief Overrides an existing string, for example giving a proper name to items that normally would never be displayed. The original index will be
     * retained.
     *
     * @param index Original MSD index.
     * @param str String to set.
     */
    void OverrideString(uint32_t index, const std::string &str);

    /**
     * @brief Retrieves the replacement MSD data to pass back to Okami.
     *
     * @warning Data becomes invalidated when MSD is modified after this call.
     *
     * @return const std::vector<uint8_t>& MSD data.
     */
    const std::vector<uint8_t> &GetData();

    /**
     * @brief Retreives number of strings in this MSD.
     *
     * @return size_t number of strings
     */
    size_t Size() const;
};
} // namespace okami
