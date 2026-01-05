#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include <wolf_framework.hpp>

namespace checks
{

using CheckCallback = std::function<void(int64_t)>;

/**
 * @brief Create a monitor for game progress bitfield changes
 *
 * Monitors the gameProgressionBits field in TrackerData.
 * Triggers callback with check ID when bits change 0->1.
 *
 * @param cb Callback to invoke with computed check ID
 * @return Handle to the created monitor (caller must destroy)
 */
wolf::BitfieldMonitorHandle createGameProgressMonitor(CheckCallback cb);

/**
 * @brief Create a monitor for global flags bitfield changes
 *
 * Monitors the global flags at 0xB6B2AC.
 * Triggers callback with check ID when bits change 0->1.
 *
 * @param cb Callback to invoke with computed check ID
 * @return Handle to the created monitor (caller must destroy)
 */
wolf::BitfieldMonitorHandle createGlobalFlagsMonitor(CheckCallback cb);

/**
 * @brief Create monitors for world state bitfield changes (one per map)
 *
 * Monitors mapStateBits for each map type.
 * Triggers callback with check ID when bits change 0->1.
 *
 * @param cb Callback to invoke with computed check ID
 * @return Vector of handles (one per map type, caller must destroy all)
 */
std::vector<wolf::BitfieldMonitorHandle> createWorldStateMonitors(CheckCallback cb);

/**
 * @brief Create monitors for collected object bitfield changes (one per map)
 *
 * Monitors collectedObjects field in MapState for each map.
 * Triggers callback with check ID when bits change 0->1.
 *
 * @param cb Callback to invoke with computed check ID
 * @return Vector of handles (one per map type, caller must destroy all)
 */
std::vector<wolf::BitfieldMonitorHandle> createCollectedObjectMonitors(CheckCallback cb);

/**
 * @brief Create monitors for area restoration bitfield changes (one per map)
 *
 * Monitors areasRestored field in MapState for each map.
 * Triggers callback with check ID when bits change 0->1.
 *
 * @param cb Callback to invoke with computed check ID
 * @return Vector of handles (one per map type, caller must destroy all)
 */
std::vector<wolf::BitfieldMonitorHandle> createAreaRestoredMonitors(CheckCallback cb);

} // namespace checks
