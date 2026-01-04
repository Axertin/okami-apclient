#include "wolf_framework.hpp"

#include <okami/bitfield.hpp>
#include <okami/structs.hpp>

namespace wolf::mock
{

std::vector<uint8_t> mockMemory;
std::vector<GiveItemCall> giveItemCalls;
std::vector<std::string> logMessages;
std::vector<std::function<void()>> playStartCallbacks;
std::vector<std::function<void()>> returnToMenuCallbacks;
std::unordered_map<uintptr_t, void *> registeredHooks;

void reset()
{
    mockMemory.clear();
    giveItemCalls.clear();
    logMessages.clear();
    playStartCallbacks.clear();
    returnToMenuCallbacks.clear();
    registeredHooks.clear();
}

void triggerPlayStart()
{
    for (auto &callback : playStartCallbacks)
    {
        callback();
    }
}

void triggerReturnToMenu()
{
    for (auto &callback : returnToMenuCallbacks)
    {
        callback();
    }
}

void reserveMemory(size_t size)
{
    if (mockMemory.capacity() < size)
    {
        mockMemory.reserve(size);
    }
}

} // namespace wolf::mock
