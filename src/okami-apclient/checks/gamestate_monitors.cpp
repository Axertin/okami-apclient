#include "gamestate_monitors.hpp"

#include <okami/maptype.hpp>
#include <okami/offsets.hpp>
#include <okami/structs.hpp>

#include "check_types.hpp"

namespace checks
{

wolf::BitfieldMonitorHandle createGameProgressMonitor(CheckCallback cb)
{
    constexpr uintptr_t addr = okami::main::trackerData + offsetof(okami::TrackerData, gameProgressionBits);
    constexpr size_t byteCount = (96 + 7) / 8; // 96 bits = 12 bytes

    return wolf::createBitfieldMonitor(
        "main.dll", addr, byteCount,
        [cb](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            if (!oldValue && newValue)
            {
                cb(getGameProgressCheckId(static_cast<int>(bitIndex)));
            }
        },
        "GameProgress monitor");
}

wolf::BitfieldMonitorHandle createGlobalFlagsMonitor(CheckCallback cb)
{
    constexpr uintptr_t addr = okami::main::globalGameStateFlags;
    constexpr size_t byteCount = (86 + 7) / 8; // 86 bits = 11 bytes

    return wolf::createBitfieldMonitor(
        "main.dll", addr, byteCount,
        [cb](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            if (!oldValue && newValue)
            {
                cb(getGlobalFlagCheckId(static_cast<int>(bitIndex)));
            }
        },
        "GlobalFlags monitor");
}

std::vector<wolf::BitfieldMonitorHandle> createWorldStateMonitors(CheckCallback cb)
{
    constexpr size_t numMapTypes = static_cast<size_t>(okami::MapTypes::NUM_MAP_TYPES);
    constexpr size_t byteCount = (256 + 7) / 8; // 256 bits = 32 bytes

    std::vector<wolf::BitfieldMonitorHandle> handles;
    handles.reserve(numMapTypes);

    for (size_t mapId = 0; mapId < numMapTypes; mapId++)
    {
        uintptr_t addr = okami::main::collectionData + offsetof(okami::CollectionData, world) + offsetof(okami::WorldStateData, mapStateBits) +
                         (mapId * sizeof(okami::BitField<256>));

        handles.push_back(wolf::createBitfieldMonitor(
            "main.dll", addr, byteCount,
            [cb, mapId](unsigned int bitIndex, bool oldValue, bool newValue)
            {
                if (!oldValue && newValue)
                {
                    cb(getWorldStateCheckId(static_cast<int>(mapId), static_cast<int>(bitIndex)));
                }
            },
            "WorldState monitor"));
    }

    return handles;
}

std::vector<wolf::BitfieldMonitorHandle> createCollectedObjectMonitors(CheckCallback cb)
{
    constexpr size_t numMapTypes = static_cast<size_t>(okami::MapTypes::NUM_MAP_TYPES);
    constexpr size_t byteCount = (96 + 7) / 8; // 96 bits = 12 bytes

    std::vector<wolf::BitfieldMonitorHandle> handles;
    handles.reserve(numMapTypes);

    for (size_t mapId = 0; mapId < numMapTypes; mapId++)
    {
        uintptr_t addr = okami::main::mapData + (mapId * sizeof(okami::MapState)) + offsetof(okami::MapState, collectedObjects);

        handles.push_back(wolf::createBitfieldMonitor(
            "main.dll", addr, byteCount,
            [cb, mapId](unsigned int bitIndex, bool oldValue, bool newValue)
            {
                if (!oldValue && newValue)
                {
                    cb(getCollectedObjectCheckId(static_cast<int>(mapId), static_cast<int>(bitIndex)));
                }
            },
            "CollectedObjects monitor"));
    }

    return handles;
}

std::vector<wolf::BitfieldMonitorHandle> createAreaRestoredMonitors(CheckCallback cb)
{
    constexpr size_t numMapTypes = static_cast<size_t>(okami::MapTypes::NUM_MAP_TYPES);
    constexpr size_t byteCount = (96 + 7) / 8; // 96 bits = 12 bytes

    std::vector<wolf::BitfieldMonitorHandle> handles;
    handles.reserve(numMapTypes);

    for (size_t mapId = 0; mapId < numMapTypes; mapId++)
    {
        uintptr_t addr = okami::main::mapData + (mapId * sizeof(okami::MapState)) + offsetof(okami::MapState, areasRestored);

        handles.push_back(wolf::createBitfieldMonitor(
            "main.dll", addr, byteCount,
            [cb, mapId](unsigned int bitIndex, bool oldValue, bool newValue)
            {
                if (!oldValue && newValue)
                {
                    cb(getAreaRestoredCheckId(static_cast<int>(mapId), static_cast<int>(bitIndex)));
                }
            },
            "AreasRestored monitor"));
    }

    return handles;
}

} // namespace checks
