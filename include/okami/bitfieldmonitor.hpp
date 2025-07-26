#pragma once
#include <functional>
#include <vector>

#include "okami/data/structs.hpp"

namespace okami
{

/**
 * @brief Lightweight change detector for BitField structures
 *
 * This utility class monitors a BitField for changes and calls a callback
 * when any bits transition from 0->1 or 1->0. It's designed to be shared
 * between the devdatafinder (for research) and AP location monitor (for gameplay).
 *
 * Thread Safety: This class is NOT thread-safe by itself. The caller is responsible
 * for ensuring thread safety if used across multiple threads.
 *
 * @tparam N Size of the BitField to monitor
 */
template <unsigned int N> class BitFieldChangeDetector
{
  public:
    /**
     * @brief Callback function signature for bit changes
     * @param bitIndex The index of the bit that changed (0-based)
     * @param oldValue The previous value of the bit (true/false)
     * @param newValue The new value of the bit (true/false)
     */
    using ChangeCallback = std::function<void(unsigned int bitIndex, bool oldValue, bool newValue)>;

    /**
     * @brief Construct a new BitField change detector
     * @param callback Function to call when bits change
     */
    explicit BitFieldChangeDetector(ChangeCallback callback) : onChange_(callback), initialized_(false)
    {
    }

    /**
     * @brief Update the detector with the current BitField state
     *
     * On first call, this captures the initial state without triggering callbacks.
     * On subsequent calls, it compares against the previous state and calls the
     * callback for any changed bits.
     *
     * @param current The current state of the BitField to check
     */
    void update(const BitField<N> &current)
    {
        if (!initialized_)
        {
            previousState_ = current;
            initialized_ = true;
            return;
        }

        // Find changed bits
        BitField<N> diff = current ^ previousState_;
        std::vector<unsigned> changedIndices = diff.GetSetIndices();

        // Only process the bits that actually changed
        for (unsigned idx : changedIndices)
        {
            bool oldValue = previousState_.IsSet(idx);
            bool newValue = current.IsSet(idx);
            onChange_(idx, oldValue, newValue);
        }

        previousState_ = current;
    }

    /**
     * @brief Reset the detector to uninitialized state
     *
     * After calling this, the next update() call will capture initial state
     * without triggering callbacks. Useful when returning to menu or starting
     * a new game session.
     */
    void reset()
    {
        initialized_ = false;
        // Note: Don't clear previousState_ - let it reinitialize naturally
        // This avoids false positives when memory is in transition
    }

    /**
     * @brief Check if the detector has been initialized with a baseline state
     * @return true if update() has been called at least once
     */
    bool isInitialized() const
    {
        return initialized_;
    }

  private:
    ChangeCallback onChange_;   ///< Callback to invoke on changes
    BitField<N> previousState_; ///< Previous state for comparison
    bool initialized_;          ///< Whether we have a baseline state
};

} // namespace okami
