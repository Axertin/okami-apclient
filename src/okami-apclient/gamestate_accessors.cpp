#include "gamestate_accessors.hpp"

namespace apgame
{

// Define storage for accessors
BitFieldAccessor<64> usableBrushTechniques;
BitFieldAccessor<64> obtainedBrushTechniques;
BitFieldAccessor<32> keyItemsAcquired;
BitFieldAccessor<32> goldDustsAcquired;
BitFieldAccessor<32> brushUpgrades;
Accessor<okami::CollectionData> collectionData;
Accessor<okami::WorldStateData> worldStateData;
Accessor<okami::TrackerData> trackerData;

// Warp system accessors
Accessor<okami::WarpData> warpData;
Accessor<uint8_t> mapLoadFlags;

void initialize()
{
    // CollectionData is at 0xB205D0
    constexpr uintptr_t collectionDataAddr = 0xB205D0;
    collectionData = Accessor<okami::CollectionData>("main.dll", collectionDataAddr);

    // WorldStateData is embedded in CollectionData
    constexpr uintptr_t worldStateDataAddr = collectionDataAddr + offsetof(okami::CollectionData, world);
    worldStateData = Accessor<okami::WorldStateData>("main.dll", worldStateDataAddr);

    // TrackerData is at 0xB21780
    constexpr uintptr_t trackerDataAddr = 0xB21780;
    trackerData = Accessor<okami::TrackerData>("main.dll", trackerDataAddr);

    // Brush techniques from WorldStateData
    usableBrushTechniques = BitFieldAccessor<64>("main.dll", worldStateDataAddr + offsetof(okami::WorldStateData, usableBrushTechniques));
    obtainedBrushTechniques = BitFieldAccessor<64>("main.dll", worldStateDataAddr + offsetof(okami::WorldStateData, obtainedBrushTechniques));

    // Key items from WorldStateData
    keyItemsAcquired = BitFieldAccessor<32>("main.dll", worldStateDataAddr + offsetof(okami::WorldStateData, keyItemsAcquired));
    goldDustsAcquired = BitFieldAccessor<32>("main.dll", worldStateDataAddr + offsetof(okami::WorldStateData, goldDustsAcquired));

    // Brush upgrades from TrackerData
    brushUpgrades = BitFieldAccessor<32>("main.dll", trackerDataAddr + offsetof(okami::TrackerData, brushUpgrades));

    // Warp system accessors
    warpData = Accessor<okami::WarpData>("main.dll", okami::main::warpData);
    mapLoadFlags = Accessor<uint8_t>("main.dll", okami::main::mapLoadFlags);

    wolf::logInfo("[apgame] Game state accessors initialized");
}

} // namespace apgame