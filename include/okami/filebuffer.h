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
        const uint8_t *valBytes = reinterpret_cast<const uint8_t *>(&value);
        this->buffer.insert(this->buffer.end(), valBytes, valBytes + sizeof(value));
    }

    void append_range(const std::ranges::range auto &container)
    {
        for (const auto &v : container)
        {
            this->append(v);
        }
    }

    void append_bytes(size_t count, uint8_t value = 0)
    {
        this->buffer.insert(this->buffer.end(), count, value);
    }

    size_t size() const
    {
        return this->buffer.size();
    }

    void clear()
    {
        this->buffer.clear();
    }

    void reserve(size_t count)
    {
        this->buffer.reserve(count);
    }

    const uint8_t *data() const
    {
        return this->buffer.data();
    }

    std::vector<uint8_t> &get_buffer()
    {
        return this->buffer;
    }
};
