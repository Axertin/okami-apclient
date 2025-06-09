#pragma once
#include <cstdint>
#include <type_traits>
#include <cassert>

namespace okami
{
    /**
     * @brief A view over an in-memory bitfield represented by contiguous bytes.
     *
     * The enum values must represent the *bit index* within the whole bitfield,
     * not just `1 << n` as in classic bitmasks. For example, if you would use
     * `1 << 3` to mask your bit, you would instead use `3`
     *
     * @tparam Enum An enum class with an integral underlying type.
     */
    template <typename Enum>
    class BitfieldFlags
    {
        static_assert(std::is_enum_v<Enum>, "BitfieldFlags requires an enum type.");
        using Underlying = std::underlying_type_t<Enum>;
        static_assert(std::is_integral_v<Underlying>, "Enum must have integral underlying type.");

        volatile uint8_t *_base;
        size_t _size; // size in bytes

    public:
        /**
         * @brief Construct a BitfieldFlags view over a contiguous memory region.
         *
         * @param base Pointer to the first byte of the bitfield in memory.
         * @param size Number of contiguous bytes to cover.
         */
        constexpr BitfieldFlags(volatile uint8_t *base, size_t size)
            : _base(base), _size(size)
        {
            assert(base != nullptr && size > 0);
        }

        /**
         * @brief Check if a bit is set.
         *
         * @param flag The bit index enum.
         * @return true if the bit is set, false otherwise.
         */
        bool has(Enum flag) const
        {
            size_t index = static_cast<Underlying>(flag);
            size_t byte = index / 8;
            uint8_t bit = static_cast<uint8_t>(1u << (index % 8));
            assert(byte < _size);
            return (_base[byte] & bit) != 0;
        }

        /**
         * @brief Set a specific bit.
         *
         * @param flag The bit index enum.
         */
        void set(Enum flag)
        {
            size_t index = static_cast<Underlying>(flag);
            size_t byte = index / 8;
            uint8_t bit = static_cast<uint8_t>(1u << (index % 8));
            assert(byte < _size);
            _base[byte] |= bit;
        }

        /**
         * @brief Clear a specific bit.
         *
         * @param flag The bit index enum.
         */
        void clear(Enum flag)
        {
            size_t index = static_cast<Underlying>(flag);
            size_t byte = index / 8;
            uint8_t bit = static_cast<uint8_t>(1u << (index % 8));
            assert(byte < _size);
            _base[byte] &= ~bit;
        }

        /**
         * @brief Toggle a specific bit.
         *
         * @param flag The bit index enum.
         */
        void toggle(Enum flag)
        {
            size_t index = static_cast<Underlying>(flag);
            size_t byte = index / 8;
            uint8_t bit = static_cast<uint8_t>(1u << (index % 8));
            assert(byte < _size);
            _base[byte] ^= bit;
        }

        /**
         * @brief Read the raw value of a specific byte within the range.
         *
         * @param byteOffset Index of the byte (0 to size - 1).
         * @return The raw byte value.
         */
        uint8_t rawByte(size_t byteOffset) const
        {
            assert(byteOffset < _size);
            return _base[byteOffset];
        }

        /**
         * @brief Get the number of bytes in the bitfield.
         *
         * @return Size in bytes.
         */
        size_t size() const noexcept { return _size; }

        /**
         * @brief Get the raw pointer to the base of the bitfield.
         *
         * @return volatile uint8_t* to the start.
         */
        volatile uint8_t *data() const noexcept { return _base; }
    };

} // namespace okami
