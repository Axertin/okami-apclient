#pragma once
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <array>
#include <vector>
#include <algorithm>

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
            if (byte >= _size)
                return 0;
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

    template <typename Enum, size_t NumBytes>
    class BitfieldFlagsStorage
    {
        static_assert(std::is_enum_v<Enum>, "BitfieldFlagsStorage requires an enum type.");
        using Underlying = std::underlying_type_t<Enum>;
        static_assert(std::is_integral_v<Underlying>, "Enum must have integral underlying type.");

        std::array<uint8_t, NumBytes> _buffer{};

    public:
        constexpr BitfieldFlagsStorage() noexcept = default;

        /**
         * @brief Access a mutable BitfieldFlags view of the internal buffer.
         */
        BitfieldFlags<Enum> view()
        {
            return BitfieldFlags<Enum>(_buffer.data(), NumBytes);
        }

        /**
         * @brief Access a const BitfieldFlags view of the internal buffer.
         */
        BitfieldFlags<Enum> view() const
        {
            return BitfieldFlags<Enum>(const_cast<volatile uint8_t *>(_buffer.data()), NumBytes);
        }

        /**
         * @brief Copy content from an external BitfieldFlags view.
         */
        void copyFrom(const BitfieldFlags<Enum> &other)
        {
            assert(other.size() == NumBytes && "Mismatched bitfield sizes in copyFrom.");
            std::copy_n(other.data(), NumBytes, _buffer.data());
        }

        /**
         * @brief Copy content from an external BitfieldFlagsStorage.
         */
        void copyFrom(const BitfieldFlagsStorage<Enum, NumBytes> &other)
        {
            assert(other.size() == NumBytes && "Mismatched bitfield sizes in copyFrom.");
            _buffer = other.data();
        }

        /**
         * @brief Compare this storage to another BitfieldFlags view.
         */
        bool isEqual(const BitfieldFlags<Enum> &other) const
        {
            assert(other.size() == NumBytes && "Mismatched bitfield sizes in isEqual.");
            return std::equal(_buffer.begin(), _buffer.end(), other.data());
        }

        /**
         * @brief Compare this storage to another BitfieldFlagsStorage
         */
        bool isEqual(const BitfieldFlagsStorage<Enum, NumBytes> &other) const
        {
            assert(other.size() == NumBytes && "Mismatched bitfield sizes in isEqual.");
            return _buffer == other.data();
        }

        /**
         * @brief Return a list of flags that differ between this storage and another view.
         */
        std::vector<Enum> diffBits(const BitfieldFlagsStorage<Enum, NumBytes> &other) const
        {
            std::vector<Enum> differences;
            const auto &otherBuffer = other.data();

            for (size_t byteIndex = 0; byteIndex < NumBytes; ++byteIndex)
            {
                uint8_t xorResult = _buffer[byteIndex] ^ otherBuffer[byteIndex];
                if (xorResult == 0)
                    continue;

                for (uint8_t bit = 0; bit < 8; ++bit)
                {
                    if (xorResult & (1u << bit))
                    {
                        size_t bitIndex = byteIndex * 8 + bit;
                        differences.push_back(static_cast<Enum>(bitIndex));
                    }
                }
            }

            return differences;
        }

        /**
         * @brief Return a list of flags that differ between this storage and another view.
         */
        std::vector<Enum> diffBits(const BitfieldFlags<Enum> &other) const
        {
            assert(other.size() == NumBytes && "Mismatched bitfield sizes in diffBits.");

            std::vector<Enum> differences;
            size_t totalBits = NumBytes * 8;

            for (size_t bitIndex = 0; bitIndex < totalBits; ++bitIndex)
            {
                size_t byte = bitIndex / 8;
                uint8_t mask = static_cast<uint8_t>(1u << (bitIndex % 8));

                uint8_t selfByte = _buffer[byte];
                uint8_t otherByte = other.rawByte(byte);

                if ((selfByte & mask) != (otherByte & mask))
                {
                    differences.push_back(static_cast<Enum>(bitIndex));
                }
            }
            return differences;
        }

        /**
         * @brief Clear all bits.
         */
        void clearAll() noexcept
        {
            _buffer.fill(0);
        }

        /**
         * @brief Set all bits.
         */
        void setAll() noexcept
        {
            _buffer.fill(0xFF);
        }

        /**
         * @brief Set a specific flag bit.
         */
        void set(Enum flag)
        {
            size_t index = static_cast<Underlying>(flag);
            size_t byte = index / 8;
            uint8_t bit = static_cast<uint8_t>(1u << (index % 8));
            assert(byte < NumBytes);
            _buffer[byte] |= bit;
        }

        /**
         * @brief Clear a specific flag bit.
         */
        void clear(Enum flag)
        {
            size_t index = static_cast<Underlying>(flag);
            size_t byte = index / 8;
            uint8_t bit = static_cast<uint8_t>(1u << (index % 8));
            assert(byte < NumBytes);
            _buffer[byte] &= ~bit;
        }

        /**
         * @brief Toggle a specific flag bit.
         */
        void toggle(Enum flag)
        {
            size_t index = static_cast<Underlying>(flag);
            size_t byte = index / 8;
            uint8_t bit = static_cast<uint8_t>(1u << (index % 8));
            assert(byte < NumBytes);
            _buffer[byte] ^= bit;
        }

        /**
         * @brief Check if a specific flag bit is set.
         */
        bool has(Enum flag) const
        {
            size_t index = static_cast<Underlying>(flag);
            size_t byte = index / 8;
            uint8_t bit = static_cast<uint8_t>(1u << (index % 8));
            if (byte >= NumBytes)
                return false;
            return (_buffer[byte] & bit) != 0;
        }

        /**
         * @brief Get the number of bits stored.
         */
        static constexpr size_t bitCount() noexcept
        {
            return NumBytes * 8;
        }

        /**
         * @brief Get the number of bytes used.
         */
        static constexpr size_t size() noexcept
        {
            return NumBytes;
        }

        /**
         * @brief Access the raw buffer.
         */
        constexpr const std::array<uint8_t, NumBytes> &data() const noexcept
        {
            return _buffer;
        }
    };

} // namespace okami
