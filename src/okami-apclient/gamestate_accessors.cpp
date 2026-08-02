#include "gamestate_accessors.hpp"

#include <okami/offsets.hpp>

namespace apgame
{

// Define storage for accessors
Accessor<okami::CharacterStats> characterStats;
BitFieldAccessor<64> usableBrushTechniques;
BitFieldAccessor<64> obtainedBrushTechniques;
BitFieldAccessor<64> usableBrushesSource;
BitFieldAccessor<64> obtainedBrushesSource;
BitFieldAccessor<32> keyItemsAcquired;
BitFieldAccessor<32> goldDustsAcquired;
BitFieldAccessor<32> brushUpgrades;
BitFieldAccessor<86> globalGameState;
Accessor<okami::CollectionData> collectionData;
Accessor<okami::WorldStateData> worldStateData;
Accessor<okami::TrackerData> trackerData;

// Item parameter table accessor
Accessor<std::array<okami::ItemParam, okami::ItemTypes::NUM_ITEM_TYPES>> itemParams;

void initialize()
{
    // CharacterStats is at 0xB4DF90
    characterStats = Accessor<okami::CharacterStats>("main.dll", okami::main::characterStats);

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

    // BrushData source: the WorldStateData copies above are propagated from these
    // bits during the game's sync. Granting must mirror writes here.
    usableBrushesSource = BitFieldAccessor<64>("main.dll", okami::main::usableBrushes);
    obtainedBrushesSource = BitFieldAccessor<64>("main.dll", okami::main::obtainedBrushes);

    // Key items from WorldStateData
    keyItemsAcquired = BitFieldAccessor<32>("main.dll", worldStateDataAddr + offsetof(okami::WorldStateData, keyItemsAcquired));
    goldDustsAcquired = BitFieldAccessor<32>("main.dll", worldStateDataAddr + offsetof(okami::WorldStateData, goldDustsAcquired));

    // Brush upgrades from TrackerData
    brushUpgrades = BitFieldAccessor<32>("main.dll", trackerDataAddr + offsetof(okami::TrackerData, brushUpgrades));

    // Global game state flags (loading-screen bit 16, menu bit 17, etc.)
    globalGameState = BitFieldAccessor<86>("main.dll", okami::main::globalGameStateFlags);

    // Item parameter table
    constexpr uintptr_t itemParamsAddr = 0x7AB220;
    itemParams = Accessor<std::array<okami::ItemParam, okami::ItemTypes::NUM_ITEM_TYPES>>("main.dll", itemParamsAddr);

    wolf::logInfo("[apgame] Game state accessors initialized");
}

bool isInLoadingPhase()
{
    if (!globalGameState.is_bound())
        return false;
    return globalGameState->IsSet(16);
}

} // namespace apgame