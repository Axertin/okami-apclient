#pragma once

#include <cstdint>
#include <string>

namespace itempatch
{
/// Install the GetNumEntries hook so the texture manager allocates enough
/// texture slots for all custom item icons when it initialises during
/// flower_startup.  Must be called from EarlyGameInit (before flower_startup).
void initializeEarly();

/// Install game hooks for icon texture expansion, icon loading, and MSD string
/// patching. Must be called after apgame::initialize() and before patchItemParams().
void initialize();

/// Patch the in-game ItemParam array to ensure all item types have valid
/// category and maxCount values, preventing shop crashes.
/// Must be called after apgame::initialize().
void patchItemParams();

/// Set the current shop ID used by the slot array builder hook.
/// Must be called in hookLoadRsc immediately before returning custom ISL data,
/// so that s_currentShopId is valid when FUN_18043E250 fires.
void setCurrentShopId(int shopId);

/// Register a scouted item name for a specific AP location.
/// Idempotent: calling again for the same location is a no-op.
/// Safe to call at any time — no dependency on MSD load state.
void registerScoutedItemName(int64_t locationId, const std::string &name);

/// Resolve an AP dummy item's per-slot custom name from the current shop context.
/// Returns the compiled MSD string for the selected slot's scouted item, or
/// nullptr if the strId is not an AP dummy item or no custom name is registered.
/// This is the core logic called by hookGetMSDString — exposed for testability.
const uint16_t *resolveApItemName(uint16_t strId);

/// Set the shop object pointer directly. In production this is captured by
/// hookBuildSlotArray; this setter enables unit tests to inject a mock shop buffer.
void setShopPointer(void *pShop);

/// Clear all shop context (shop ID, shop pointer). Call when leaving a shop
/// to prevent stale lookups in hookGetMSDString.
void clearShopContext();

/// Reset all dynamic state (custom strings, location index, shop context).
/// For use in tests to prevent cross-test pollution.
void resetState();
} // namespace itempatch
