
#pragma once
template <typename Enum>
class FlagEdgeDetector
{
    static_assert(std::is_enum_v<Enum>, "FlagEdgeDetector only works with enums.");
    using Flags = BitfieldFlags<Enum>;

public:
    /// @brief Poll state of flage
    /// @param newState read flags
    void update(const Flags &newState)
    {
        _previous = _current;
        _current = newState;
    }

    /// @brief detect the assertion of a flag
    /// @param flag flag to detect on
    /// @return true if the flag transitioned from 0 -> 1 since last update
    bool risingEdge(Enum flag) const
    {
        return !_previous.has(flag) && _current.has(flag);
    }

    /// @brief detect the deassertion of a flag
    /// @param flag flag to detect on
    /// @return true if the flag transitioned from 1 -> 0 since last update
    bool fallingEdge(Enum flag) const
    {
        return _previous.has(flag) && !_current.has(flag);
    }

    /// @brief detect change in a flag
    /// @param flag flag to detect on
    /// @return true if the flag transitioned from either 1 -> 0 or 0 -> 1 since last update
    bool changed(Enum flag) const
    {
        return _previous.has(flag) != _current.has(flag);
    }

    /// @brief Reset state memory
    void reset()
    {
        _previous = Flags{};
        _current = Flags{};
    }

private:
    Flags _previous{};
    Flags _current{};
};
