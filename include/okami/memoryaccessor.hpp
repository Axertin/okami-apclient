#pragma once
#include <cstdint>
#include <type_traits>
#include <cassert>
#include "bitfieldflags.hpp"

#include <iostream>

namespace okami
{

    /**
     * @brief Provides typed access to a specific memory address.
     *
     * @tparam T The type of the memory at the address.
     */
    template <typename T>
    class MemoryAccessor
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
        explicit MemoryAccessor(uintptr_t addr)
            : address(addr) {}

        /**
         * @brief Bind to an address (like one only known at runtime)
         *
         * @param addr The memory address to access
         */
        void bind(uintptr_t addr) { address = addr; }

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
        T* get_ptr() const
        {
            assert(address != 0);
            return reinterpret_cast<T*>(address);
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
        operator T() const { return get(); }

        /// Assignment operator to write a value.
        MemoryAccessor &operator=(const T &value)
        {
            set(value);
            return *this;
        }

        /// Get the raw address this accessor points to.
        uintptr_t raw() const { return address; }

        /// Implicit memory access via get_ptr().
        T* operator->() const {
            return get_ptr();
        }
    };

    template <typename Enum>
    class MemoryAccessor<BitfieldFlags<Enum>>
    {
        static_assert(std::is_enum_v<Enum>, "BitfieldFlags requires an enum type");
        uintptr_t base = 0;
        size_t width = 0;

    public:
        MemoryAccessor() = default;

        template <size_t Bytes>
        void bind(uintptr_t addr)
        {
            static_assert(Bytes > 0, "Bitfield must be bound to at least one byte!");
            assert(addr != 0 && "Attempted to bind to null pointer!");
            base = addr;
            width = Bytes;
        }

        BitfieldFlags<Enum> get() const
        {
            assert(base != 0 && "Attempted to get unbound memory!");
            return BitfieldFlags<Enum>(reinterpret_cast<volatile uint8_t *>(base), width);
        }

        bool isBound() const
        {
            return base != 0;
        }

        void clear(Enum flag)
        {
            assert(base != 0 && "Attempted to clear unbound memory!");
            BitfieldFlags<Enum>(reinterpret_cast<volatile uint8_t *>(base), width).clear(flag);
        }

        void set(Enum flag)
        {
            assert(base != 0 && "Attempted to set unbound memory!");
            BitfieldFlags<Enum>(reinterpret_cast<volatile uint8_t *>(base), width).set(flag);
        }

        uintptr_t raw() const { return base; }
    };

} // namespace okami
