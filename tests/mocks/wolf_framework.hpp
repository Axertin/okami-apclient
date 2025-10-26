#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

// Mock implementation of wolf_framework.hpp for testing
// Only implements what's needed for item handler testing
// Note: okami headers will be included by the files that include this mock

namespace wolf
{

// Mock memory storage
namespace mock
{
// Storage for mock memory regions
extern std::vector<uint8_t> mockMemory;

// Track calls to wolf::giveItem
struct GiveItemCall
{
    int itemId;
    int count;
};
extern std::vector<GiveItemCall> giveItemCalls;

// Track log calls
extern std::vector<std::string> logMessages;

// Store lifecycle callbacks for testing
extern std::vector<std::function<void()>> playStartCallbacks;
extern std::vector<std::function<void()>> returnToMenuCallbacks;

// Reset all mock state
void reset();

// Trigger callbacks for testing
void triggerPlayStart();
void triggerReturnToMenu();

// Reserve memory to prevent reallocation during accessor creation
void reserveMemory(size_t size);
} // namespace mock

// Mock MemoryAccessor
template <typename T> class MemoryAccessor
{
  private:
    T *ptr_;

  public:
    MemoryAccessor() : ptr_(nullptr)
    {
    }

    explicit MemoryAccessor(const char *module, uintptr_t offset)
    {
        // Allocate mock memory for this accessor
        if (mock::mockMemory.size() < offset + sizeof(T))
        {
            mock::mockMemory.resize(offset + sizeof(T));
        }
        ptr_ = reinterpret_cast<T *>(&mock::mockMemory[offset]);
        // Zero-initialize
        std::memset(ptr_, 0, sizeof(T));
    }

    T *operator->()
    {
        return ptr_;
    }
    const T *operator->() const
    {
        return ptr_;
    }
    T &operator*()
    {
        return *ptr_;
    }
    const T &operator*() const
    {
        return *ptr_;
    }

    T *get()
    {
        return ptr_;
    }
    const T *get() const
    {
        return ptr_;
    }
};

// Mock wolf framework functions
inline void giveItem(int itemId, int count)
{
    mock::giveItemCalls.push_back({itemId, count});
}

inline void logInfo(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    mock::logMessages.push_back(std::string("[INFO] ") + buffer);
}

inline void logDebug(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    mock::logMessages.push_back(std::string("[DEBUG] ") + buffer);
}

inline void logWarning(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    mock::logMessages.push_back(std::string("[WARNING] ") + buffer);
}

inline void logError(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    mock::logMessages.push_back(std::string("[ERROR] ") + buffer);
}

// Mock game lifecycle hooks
inline void onPlayStart(std::function<void()> callback)
{
    mock::playStartCallbacks.push_back(callback);
}

inline void onReturnToMenu(std::function<void()> callback)
{
    mock::returnToMenuCallbacks.push_back(callback);
}

inline bool onItemPickupBlocking(std::function<bool(int, int)> callback)
{
    // No-op in tests
    return true;
}

// Mock bitfield monitor types (unused in tests)
using BitfieldMonitorHandle = void *;

inline BitfieldMonitorHandle createBitfieldMonitor(const char *, uintptr_t, size_t, std::function<void(unsigned int, bool, bool)>, const char *)
{
    return nullptr;
}

inline void destroyBitfieldMonitor(BitfieldMonitorHandle)
{
    // No-op
}

} // namespace wolf
