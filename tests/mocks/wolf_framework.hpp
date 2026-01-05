#pragma once

#include <algorithm>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
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

// Storage for registered hooks by offset (type-erased as void*)
extern std::unordered_map<uintptr_t, void *> registeredHooks;

// Reset all mock state
void reset();

// Typed trigger - caller specifies function signature
template <typename FnType, typename... Args> void triggerHook(uintptr_t offset, Args &&...args)
{
    auto it = registeredHooks.find(offset);
    if (it != registeredHooks.end())
    {
        auto fn = reinterpret_cast<FnType>(it->second);
        fn(std::forward<Args>(args)...);
    }
}

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

// Mock hook function - stores callback for later triggering
template <typename T, typename U> inline bool hookFunction(const char *module, uintptr_t offset, T hookFn, U *originalFn)
{
    (void)module;
    mock::registeredHooks[offset] = reinterpret_cast<void *>(hookFn);
    *originalFn = nullptr; // No original to call in tests
    return true;
}

// Mock module base - return pointer to mockMemory so offset reads work
inline void *getModuleBase(const char *module)
{
    (void)module;
    // Return pointer to start of mockMemory so module + offset reads work
    // Ensure mockMemory has enough space for typical offset ranges
    return mock::mockMemory.data();
}

// Mock bitfield monitor implementation
struct MockBitfieldMonitor
{
    uintptr_t offset;
    size_t byteCount;
    std::function<void(unsigned int, bool, bool)> callback;
    std::string name;
};

using BitfieldMonitorHandle = MockBitfieldMonitor *;

namespace mock
{
// Track all created monitors
extern std::vector<std::unique_ptr<MockBitfieldMonitor>> bitfieldMonitors;

// Helper to trigger a specific monitor's callback
inline void triggerBitfieldChange(BitfieldMonitorHandle handle, unsigned int bitIndex, bool oldValue, bool newValue)
{
    if (handle && handle->callback)
    {
        handle->callback(bitIndex, oldValue, newValue);
    }
}

// Helper to find monitor by offset (for testing)
inline BitfieldMonitorHandle findMonitorByOffset(uintptr_t offset)
{
    for (auto &m : bitfieldMonitors)
    {
        if (m->offset == offset)
        {
            return m.get();
        }
    }
    return nullptr;
}
} // namespace mock

inline BitfieldMonitorHandle createBitfieldMonitor(const char *module, uintptr_t offset, size_t byteCount,
                                                   std::function<void(unsigned int, bool, bool)> callback, const char *name)
{
    (void)module;
    auto monitor = std::make_unique<MockBitfieldMonitor>();
    monitor->offset = offset;
    monitor->byteCount = byteCount;
    monitor->callback = std::move(callback);
    monitor->name = name ? name : "";
    auto *ptr = monitor.get();
    mock::bitfieldMonitors.push_back(std::move(monitor));
    return ptr;
}

inline void destroyBitfieldMonitor(BitfieldMonitorHandle handle)
{
    auto &monitors = mock::bitfieldMonitors;
    monitors.erase(std::remove_if(monitors.begin(), monitors.end(), [handle](const auto &m) { return m.get() == handle; }), monitors.end());
}

} // namespace wolf
