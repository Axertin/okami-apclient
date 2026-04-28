#pragma once

#include <array>

#include <okami/itemparam.hpp>
#include <okami/itemtype.hpp>
#include <okami/structs.hpp>
#include <okami/warp.hpp>
#include <wolf_framework.hpp>

namespace apgame
{

// Type aliases for convenience
template <size_t N> using BitFieldAccessor = wolf::MemoryAccessor<okami::BitField<N>>;

template <typename T> using Accessor = wolf::MemoryAccessor<T>;

// Game state accessors
// These are initialized at runtime with proper memory addresses
extern BitFieldAccessor<64> usableBrushTechniques;
extern BitFieldAccessor<64> obtainedBrushTechniques;
// BrushData source: WorldStateData copies above are propagated from these bits,
// so any external grant must mirror the write here too or it will be wiped on
// the next sync.
extern BitFieldAccessor<64> usableBrushesSource;
extern BitFieldAccessor<64> obtainedBrushesSource;
extern BitFieldAccessor<32> keyItemsAcquired;
extern BitFieldAccessor<32> goldDustsAcquired;
extern BitFieldAccessor<32> brushUpgrades;
extern Accessor<okami::CollectionData> collectionData;
extern Accessor<okami::WorldStateData> worldStateData;
extern Accessor<okami::TrackerData> trackerData;

// Warp system accessors
extern Accessor<okami::WarpData> warpData;
extern Accessor<uint8_t> mapLoadFlags;

// Item parameter table accessor
extern Accessor<std::array<okami::ItemParam, okami::ItemTypes::NUM_ITEM_TYPES>> itemParams;

// Initialize all accessors
// Must be called during mod startup before receiving any items
void initialize();

} // namespace apgame
