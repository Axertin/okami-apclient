#pragma once

#include <okami/structs.hpp>
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
extern BitFieldAccessor<32> keyItemsAcquired;
extern BitFieldAccessor<32> goldDustsAcquired;
extern BitFieldAccessor<32> brushUpgrades;
extern Accessor<okami::CollectionData> collectionData;
extern Accessor<okami::WorldStateData> worldStateData;
extern Accessor<okami::TrackerData> trackerData;

// Initialize all accessors
// Must be called during mod startup before receiving any items
void initialize();

} // namespace apgame
