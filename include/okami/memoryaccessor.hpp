#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace okami
{

/**
 * @brief Provides typed access to a specific memory address.
 *
 * @tparam T The type of the memory at the address.
 */
template <typename T> class MemoryAccessor
{
    static_assert(std::is_trivially_copyable_v<T>, "MemoryAccessor requires trivial types");

    uintptr_t address;

  public:
    MemoryAccessor() = default;

    /**
     * @brief Construct a new MemoryAccessor object.
     *
     * @param addr The memory address to access.
     */
    explicit MemoryAccessor(uintptr_t addr) : address(addr)
    {
    }

    /**
     * @brief Bind to an address (like one only known at runtime)
     *
     * @param addr The memory address to access
     */
    void bind(uintptr_t addr)
    {
        address = addr;
    }

    /**
     * @brief Read the value from memory.
     *
     * @return The value stored at the memory location.
     */
    T get() const
    {
        assert(address != 0);
        return *reinterpret_cast<volatile T *>(address);
    }

    /**
     * @brief Access memory directly.
     *
     * @return The pointer to the value stored at the memory location.
     */
    T *get_ptr() const
    {
        assert(address != 0);
        return reinterpret_cast<T *>(address);
    }

    /**
     * @brief Write a value to memory.
     *
     * @param value The value to write.
     */
    void set(const T &value)
    {
        assert(address != 0);
        *reinterpret_cast<volatile T *>(address) = value;
    }

    /// Implicit conversion to T via get().
    operator T() const
    {
        return get();
    }

    /// Assignment operator to write a value.
    MemoryAccessor &operator=(const T &value)
    {
        set(value);
        return *this;
    }

    /// Get the raw address this accessor points to.
    uintptr_t raw() const
    {
        return address;
    }

    /// Implicit memory access via get_ptr().
    T *operator->() const
    {
        return get_ptr();
    }
};

} // namespace okami
