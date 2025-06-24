#include <cstdint>
#include <vector>

namespace okami
{
  template <unsigned int N>
  struct BitField
  {
    static constexpr unsigned int const_array_size = (N + 31) / 32;
    uint32_t values[const_array_size];

    // Checks if a bit has been set
    bool IsSet(unsigned int index) const
    {
      return (values[index / 32] & (0x80000000 >> (index % 32))) != 0;
    }

    // Checks if a bit is not set
    bool IsClear(unsigned int index) const
    {
      return !IsSet(index);
    }

    // Sets a specific bit
    void Set(unsigned int index)
    {
      values[index / 32] |= (0x80000000 >> (index % 32));
    }

    // Clears a specific bit
    void Clear(unsigned int index)
    {
      values[index / 32] &= ~(0x80000000 >> (index % 32));
    }

    BitField<N> operator^(const BitField<N> &other) const
    {
      BitField<N> result = {};
      for (unsigned i = 0; i < const_array_size; i++)
      {
        result.values[i] = this->values[i] ^ other.values[i];
      }
      return result;
    }

    // Checks if any bit has been set
    bool HasAnySet() const
    {
      for (unsigned i = 0; i < const_array_size; i++)
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
      return wordIdx < const_array_size ? values[wordIdx] : 0;
    }
  };
}
