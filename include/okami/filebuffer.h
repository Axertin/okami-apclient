#pragma once

#include <cstdint>
#include <ranges>
#include <vector>

class FileBuffer
{
  private:
    std::vector<uint8_t> buffer;

  public:
    void append(const auto &value)
    {
        const auto *valBytes = reinterpret_cast<const uint8_t *>(&value);
        buffer.insert(buffer.end(), valBytes, valBytes + sizeof(value));
    }

    void append_range(const std::ranges::range auto &container)
    {
        for (const auto &v : container)
        {
            append(v);
        }
    }

    void append_bytes(size_t count, uint8_t value = 0)
    {
        buffer.insert(buffer.end(), count, value);
    }

    [[nodiscard]] size_t size() const noexcept
    {
        return buffer.size();
    }

    void clear()
    {
        buffer.clear();
    }

    void reserve(size_t count)
    {
        buffer.reserve(count);
    }

    [[nodiscard]] const uint8_t *data() const noexcept
    {
        return buffer.data();
    }

    [[nodiscard]] std::vector<uint8_t> &get_buffer() noexcept
    {
        return buffer;
    }
};
