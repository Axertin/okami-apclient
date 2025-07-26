#include <cstdint>
#include <vector>

namespace okami
{
template <unsigned int N> struct BitField
{
    static constexpr unsigned int count = N;
    static constexpr unsigned int array_size = (N + 31) / 32;
    uint32_t values[array_size];

    // Gets the pointer to the word used for testing.
    uint32_t *GetIdxPtr(unsigned int index)
    {
        return &values[index / 32];
    }

    // Creates the 32-bit mask used to test against the word.
    constexpr uint32_t GetIdxMask(unsigned int index) const
    {
        return 0x80000000 >> (index % 32);
    }

    // Checks if a bit has been set
    bool IsSet(unsigned int index) const
    {
        return (values[index / 32] & GetIdxMask(index)) != 0;
    }

    // Checks if a bit is not set
    bool IsClear(unsigned int index) const
    {
        return !IsSet(index);
    }

    // Sets a specific bit
    void Set(unsigned int index)
    {
        values[index / 32] |= GetIdxMask(index);
    }

    // Clears a specific bit
    void Clear(unsigned int index)
    {
        values[index / 32] &= ~GetIdxMask(index);
    }

    BitField<N> operator^(const BitField<N> &other) const
    {
        BitField<N> result = {};
        for (unsigned i = 0; i < array_size; i++)
        {
            result.values[i] = this->values[i] ^ other.values[i];
        }
        return result;
    }

    bool operator!=(const BitField<N> &other) const
    {
        for (unsigned i = 0; i < array_size; i++)
        {
            if (values[i] != other.values[i])
                return true;
        }
        return false;
    }

    // Checks if any bit has been set
    bool HasAnySet() const
    {
        for (unsigned i = 0; i < array_size; i++)
        {
            if (values[i] != 0)
                return true;
        }
        return false;
    }

    // Get the indices for all bits that are set
    std::vector<unsigned> GetSetIndices() const
    {
        std::vector<unsigned> result;
        for (unsigned i = 0; i < N; i++)
        {
            if (IsSet(i))
            {
                result.emplace_back(i);
            }
        }
        return result;
    }

    // Explicit accessor for logging
    constexpr uint32_t word(size_t wordIdx) const
    {
        return wordIdx < array_size ? values[wordIdx] : 0;
    }

    // Sets all values
    void SetAll()
    {
        for (unsigned i = 0; i < array_size; i++)
        {
            values[i] = 0xFFFFFFFF;
        }
    }

    // Clears all values
    void ClearAll()
    {
        for (unsigned i = 0; i < array_size; i++)
        {
            values[i] = 0;
        }
    }
};
} // namespace okami
