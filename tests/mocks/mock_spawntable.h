#pragma once

#include <cstring>
#include <map>
#include <vector>

#include <okami/spawntable.h>

namespace mock
{

/**
 * @brief Builder for creating test SpawnTable data
 *
 * Creates a SpawnTable with ContainerData entries for testing hooks.
 * The builder owns the ContainerData storage so pointers remain valid.
 */
class SpawnTableBuilder
{
  public:
    SpawnTableBuilder()
    {
        std::memset(&table_, 0, sizeof(table_));
        table_.header.initialized = 1;
        table_.header.capacity = 128;
    }

    /**
     * @brief Add a spawn table entry with container data
     *
     * @param index Entry index (0-127)
     * @param x Container X position
     * @param y Container Y position
     * @param z Container Z position
     * @param item_id Initial item ID in container
     * @param flags Entry flags (bit 0 = enabled)
     * @return Reference to builder for chaining
     */
    SpawnTableBuilder &addEntry(int index, int16_t x, int16_t y, int16_t z, uint8_t item_id = 0, uint16_t flags = 1)
    {
        if (index < 0 || index >= 128)
        {
            return *this;
        }

        // Create container data
        okami::ContainerData data{};
        data.position_x = x;
        data.position_y = y;
        data.position_z = z;
        data.item_id = item_id;
        data.item_type = 0x0A; // Standard item directory
        containerData_.push_back(data);

        // Track which entry uses which data (don't store pointer yet - vector may grow)
        entryToDataIndex_[index] = containerData_.size() - 1;

        // Set up entry (pointer will be fixed in build())
        auto &entry = table_.entries[index];
        entry.flags = flags;
        entry.spawn_type_1 = 1; // Chest path

        table_.header.count++;

        return *this;
    }

    /**
     * @brief Get the built spawn table
     *
     * Note: The returned reference is valid as long as the builder exists.
     * ContainerData pointers in entries point to builder-owned storage.
     * Call this only after all addEntry() calls are complete.
     */
    okami::SpawnTable &build()
    {
        // Fix up pointers now that vector is stable
        for (const auto &[entryIndex, dataIndex] : entryToDataIndex_)
        {
            table_.entries[entryIndex].spawn_data = &containerData_[dataIndex];
        }
        return table_;
    }

    /**
     * @brief Reset builder for reuse
     */
    void reset()
    {
        std::memset(&table_, 0, sizeof(table_));
        table_.header.initialized = 1;
        table_.header.capacity = 128;
        containerData_.clear();
        entryToDataIndex_.clear();
    }

  private:
    okami::SpawnTable table_{};
    std::vector<okami::ContainerData> containerData_;
    std::map<int, size_t> entryToDataIndex_;
};

} // namespace mock
