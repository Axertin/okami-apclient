
#pragma once
template <typename Enum>
class BitfieldFlags
{
    static_assert(std::is_enum_v<Enum>, "BitfieldFlags only works with enum types");
    using Underlying = std::underlying_type_t<Enum>;

public:
    constexpr BitfieldFlags(Underlying raw = 0) : _raw(raw) {}

    /// @brief Check if a flag is set
    /// @param flag flag to check
    /// @return value of the flag
    constexpr bool has(Enum flag) const
    {
        return (_raw & static_cast<Underlying>(flag)) != 0;
    }

    /// @brief Set a flag
    /// @param flag flag to set
    constexpr void set(Enum flag)
    {
        _raw |= static_cast<Underlying>(flag);
    }

    /// @brief Unset a flag
    /// @param flag flag to unset
    constexpr void clear(Enum flag)
    {
        _raw &= ~static_cast<Underlying>(flag);
    }

    /// @brief Get the raw byte of the bitfield
    /// @return the memory byte
    constexpr Underlying raw() const
    {
        return _raw;
    }

private:
    Underlying _raw;
};
