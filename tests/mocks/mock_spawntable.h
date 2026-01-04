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
     * @brief Add a container entry with item stored directly in spawn_data
     *
     * Creates a single container entry at the specified index.
     * The item_id is stored directly in spawn_data->item_id (matching actual game behavior).
     *
     * @param containerIdx Index for the container entry (0-127)
     * @param item_id Item ID stored in the container's spawn_data
     * @param flags Entry flags (bit 0 = enabled)
     * @return Reference to builder for chaining
     */
    SpawnTableBuilder &addContainer(int containerIdx, uint8_t item_id, uint16_t flags = 1)
    {
        if (containerIdx < 0 || containerIdx >= 128)
        {
            return *this;
        }

        // Create container data with item_id directly
        okami::ContainerData data{};
        data.item_id = item_id;
        data.item_type = 0x0A;
        containerData_.push_back(data);
        entryToDataIndex_[containerIdx] = containerData_.size() - 1;

        // Set up container entry
        auto &entry = table_.entries[containerIdx];
        entry.flags = flags;
        entry.spawn_type_1 = 1; // Container/chest type

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
