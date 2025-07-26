#include "devdatafinder.h"

#include "logger.h"
#include "okami/bitfieldmonitor.hpp"
#include "okami/gamestateregistry.h"
#include "okami/memorymap.hpp"

namespace okami
{

namespace
{
bool initialized = false;

std::unique_ptr<BitFieldChangeDetector<86>> globalFlagsMonitor;

// MapState monitors
std::array<std::unique_ptr<BitFieldChangeDetector<96>>, MapTypes::NUM_MAP_TYPES> collectedObjectsMonitors;
std::array<std::unique_ptr<BitFieldChangeDetector<32>>, MapTypes::NUM_MAP_TYPES> commonStatesMonitors;
std::array<std::unique_ptr<BitFieldChangeDetector<96>>, MapTypes::NUM_MAP_TYPES> areasRestoredMonitors;
std::array<std::unique_ptr<BitFieldChangeDetector<32>>, MapTypes::NUM_MAP_TYPES> treesBloomedMonitors;
std::array<std::unique_ptr<BitFieldChangeDetector<32>>, MapTypes::NUM_MAP_TYPES> cursedTreesBloomedMonitors;
std::array<std::unique_ptr<BitFieldChangeDetector<128>>, MapTypes::NUM_MAP_TYPES> fightsCleared;
std::array<std::unique_ptr<BitFieldChangeDetector<64>>, MapTypes::NUM_MAP_TYPES> npcHasMoreToSay;
std::array<std::unique_ptr<BitFieldChangeDetector<64>>, MapTypes::NUM_MAP_TYPES> npcUnknown;
std::array<std::unique_ptr<BitFieldChangeDetector<64>>, MapTypes::NUM_MAP_TYPES> mapsExplored;
std::array<std::unique_ptr<BitFieldChangeDetector<32>>, MapTypes::NUM_MAP_TYPES> field_DC;
std::array<std::unique_ptr<BitFieldChangeDetector<32>>, MapTypes::NUM_MAP_TYPES> field_E0;

// TrackerData monitors
std::unique_ptr<BitFieldChangeDetector<96>> trackerGameProgressionMonitor;
std::unique_ptr<BitFieldChangeDetector<64>> trackerAnimalsFedFirstTimeMonitor;
std::unique_ptr<BitFieldChangeDetector<32>> trackerField34Monitor;
std::unique_ptr<BitFieldChangeDetector<32>> trackerField38Monitor;
std::unique_ptr<BitFieldChangeDetector<32>> trackerBrushUpgradesMonitor;
std::unique_ptr<BitFieldChangeDetector<32>> trackerOptionFlagsMonitor;
std::unique_ptr<BitFieldChangeDetector<32>> trackerAreasRestoredMonitor;

// CollectionData WorldStateData monitors
std::unique_ptr<BitFieldChangeDetector<32>> worldKeyItemsAcquiredMonitor;
std::unique_ptr<BitFieldChangeDetector<32>> worldGoldDustsAcquiredMonitor;
std::array<std::unique_ptr<BitFieldChangeDetector<256>>, MapTypes::NUM_MAP_TYPES + 1> worldMapStateBitsMonitors;
std::unique_ptr<BitFieldChangeDetector<256>> worldAnimalsFedBitsMonitor;

// Non-bitfield data (keep existing manual comparison)
CharacterStats previousStats;
CollectionData previousCollection;
TrackerData previousTracker;

template <typename... Args> void warn(const char *format, Args... args)
{
    std::string newFmt = std::string("[Undocumented] ") + format;
    logWarning(newFmt.c_str(), args...);
}

void onGlobalFlagChange(unsigned int bitIndex, bool oldValue, bool newValue)
{
    const auto &registry = GameStateRegistry::instance();
    const auto &globalDoc = registry.getGlobalConfig().globalGameState;

    if (!globalDoc.contains(bitIndex))
    {
        warn("BitField GlobalGameState index %u was changed from %d to %d", bitIndex, oldValue, newValue);
    }
    else
    {
        logInfo("BitField GlobalGameState index %u (%s) was changed from %d to %d", bitIndex, globalDoc.at(bitIndex).c_str(), oldValue, newValue);
    }
}

void onMapBitFieldChange(int mapId, const std::string &fieldName, const std::unordered_map<unsigned, std::string> &documentation, unsigned int bitIndex,
                         bool oldValue, bool newValue, bool showAlways = false)
{
    std::string mapName = MapTypes::GetName(static_cast<MapTypes::Enum>(mapId));
    std::string fullName = "(" + mapName + ") MapState::" + fieldName;

    if (!documentation.contains(bitIndex))
    {
        warn("BitField %s index %u was changed from %d to %d", fullName.c_str(), bitIndex, oldValue, newValue);
    }
    else if (showAlways)
    {
        logInfo("BitField %s index %u (%s) was changed from %d to %d", fullName.c_str(), bitIndex, documentation.at(bitIndex).c_str(), oldValue, newValue);
    }
}

void onTrackerBitFieldChange(const std::string &fieldName, const std::unordered_map<unsigned, std::string> &documentation, unsigned int bitIndex, bool oldValue,
                             bool newValue, bool showAlways = false)
{
    std::string fullName = "TrackerData::" + fieldName;

    if (!documentation.contains(bitIndex))
    {
        warn("BitField %s index %u was changed from %d to %d", fullName.c_str(), bitIndex, oldValue, newValue);
    }
    else if (showAlways)
    {
        logInfo("BitField %s index %u (%s) was changed from %d to %d", fullName.c_str(), bitIndex, documentation.at(bitIndex).c_str(), oldValue, newValue);
    }
}

void onWorldStateBitFieldChange(const std::string &fieldName, const std::unordered_map<unsigned, std::string> &documentation, unsigned int bitIndex,
                                bool oldValue, bool newValue, bool showAlways = false)
{
    std::string fullName = "WorldStateData::" + fieldName;

    if (!documentation.contains(bitIndex))
    {
        warn("BitField %s index %u was changed from %d to %d", fullName.c_str(), bitIndex, oldValue, newValue);
    }
    else if (showAlways)
    {
        logInfo("BitField %s index %u (%s) was changed from %d to %d", fullName.c_str(), bitIndex, documentation.at(bitIndex).c_str(), oldValue, newValue);
    }
}

void initializeMonitors()
{
    const auto &registry = GameStateRegistry::instance();

    globalFlagsMonitor = std::make_unique<BitFieldChangeDetector<86>>(onGlobalFlagChange);

    // MapState monitors
    for (int mapId = 0; mapId < MapTypes::NUM_MAP_TYPES; mapId++)
    {
        const auto &mapConfig = registry.getMapConfig(static_cast<MapTypes::Enum>(mapId));

        collectedObjectsMonitors[mapId] = std::make_unique<BitFieldChangeDetector<96>>(
            [mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
            { onMapBitFieldChange(mapId, "collectedObjects", mapConfig.collectedObjects, bitIndex, oldValue, newValue); });

        commonStatesMonitors[mapId] = std::make_unique<BitFieldChangeDetector<32>>(
            [mapId](unsigned int bitIndex, bool oldValue, bool newValue)
            {
                const auto &registry = GameStateRegistry::instance();
                const auto &commonStates = registry.getGlobalConfig().commonStates;
                onMapBitFieldChange(mapId, "commonStates", commonStates, bitIndex, oldValue, newValue);
            });

        areasRestoredMonitors[mapId] = std::make_unique<BitFieldChangeDetector<96>>(
            [mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
            { onMapBitFieldChange(mapId, "areasRestored", mapConfig.areasRestored, bitIndex, oldValue, newValue); });

        treesBloomedMonitors[mapId] =
            std::make_unique<BitFieldChangeDetector<32>>([mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
                                                         { onMapBitFieldChange(mapId, "treesBloomed", mapConfig.treesBloomed, bitIndex, oldValue, newValue); });

        cursedTreesBloomedMonitors[mapId] = std::make_unique<BitFieldChangeDetector<32>>(
            [mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
            { onMapBitFieldChange(mapId, "cursedTreesBloomed", mapConfig.cursedTreesBloomed, bitIndex, oldValue, newValue); });

        fightsCleared[mapId] = std::make_unique<BitFieldChangeDetector<128>>(
            [mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
            { onMapBitFieldChange(mapId, "fightsCleared", mapConfig.fightsCleared, bitIndex, oldValue, newValue); });

        npcHasMoreToSay[mapId] = std::make_unique<BitFieldChangeDetector<64>>(
            [mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
            { onMapBitFieldChange(mapId, "npcHasMoreToSay", mapConfig.npcs, bitIndex, oldValue, newValue, true); });

        npcUnknown[mapId] =
            std::make_unique<BitFieldChangeDetector<64>>([mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
                                                         { onMapBitFieldChange(mapId, "npcUnknown", mapConfig.npcs, bitIndex, oldValue, newValue, true); });

        mapsExplored[mapId] =
            std::make_unique<BitFieldChangeDetector<64>>([mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
                                                         { onMapBitFieldChange(mapId, "mapsExplored", mapConfig.mapsExplored, bitIndex, oldValue, newValue); });

        field_DC[mapId] =
            std::make_unique<BitFieldChangeDetector<32>>([mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
                                                         { onMapBitFieldChange(mapId, "field_DC", mapConfig.field_DC, bitIndex, oldValue, newValue); });

        field_E0[mapId] =
            std::make_unique<BitFieldChangeDetector<32>>([mapId, &mapConfig](unsigned int bitIndex, bool oldValue, bool newValue)
                                                         { onMapBitFieldChange(mapId, "field_E0", mapConfig.field_E0, bitIndex, oldValue, newValue); });
    }

    // TrackerData monitors
    trackerGameProgressionMonitor = std::make_unique<BitFieldChangeDetector<96>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const auto &registry = GameStateRegistry::instance();
            const auto &gameProgress = registry.getGlobalConfig().gameProgress;
            onTrackerBitFieldChange("gameProgressionBits", gameProgress, bitIndex, oldValue, newValue, true);
        });

    trackerAnimalsFedFirstTimeMonitor = std::make_unique<BitFieldChangeDetector<64>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const auto &registry = GameStateRegistry::instance();
            const auto &animalsFedFirstTime = registry.getGlobalConfig().animalsFedFirstTime;
            onTrackerBitFieldChange("animalsFedFirstTime", animalsFedFirstTime, bitIndex, oldValue, newValue);
        });

    trackerField34Monitor = std::make_unique<BitFieldChangeDetector<32>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const std::unordered_map<unsigned, std::string> emptyDoc;
            onTrackerBitFieldChange("field_34", emptyDoc, bitIndex, oldValue, newValue);
        });

    trackerField38Monitor = std::make_unique<BitFieldChangeDetector<32>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const std::unordered_map<unsigned, std::string> emptyDoc;
            onTrackerBitFieldChange("field_38", emptyDoc, bitIndex, oldValue, newValue);
        });

    trackerBrushUpgradesMonitor = std::make_unique<BitFieldChangeDetector<32>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const auto &registry = GameStateRegistry::instance();
            const auto &brushUpgrades = registry.getGlobalConfig().brushUpgrades;
            onTrackerBitFieldChange("brushUpgrades", brushUpgrades, bitIndex, oldValue, newValue);
        });

    trackerOptionFlagsMonitor = std::make_unique<BitFieldChangeDetector<32>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const std::unordered_map<unsigned, std::string> emptyDoc;
            onTrackerBitFieldChange("optionFlags", emptyDoc, bitIndex, oldValue, newValue);
        });

    trackerAreasRestoredMonitor = std::make_unique<BitFieldChangeDetector<32>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const auto &registry = GameStateRegistry::instance();
            const auto &areasRestored = registry.getGlobalConfig().areasRestored;
            onTrackerBitFieldChange("areasRestored", areasRestored, bitIndex, oldValue, newValue);
        });

    // WorldStateData monitors
    worldKeyItemsAcquiredMonitor = std::make_unique<BitFieldChangeDetector<32>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const auto &registry = GameStateRegistry::instance();
            const auto &keyItemsFound = registry.getGlobalConfig().keyItemsFound;
            onWorldStateBitFieldChange("keyItemsAcquired", keyItemsFound, bitIndex, oldValue, newValue);
        });

    worldGoldDustsAcquiredMonitor = std::make_unique<BitFieldChangeDetector<32>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const auto &registry = GameStateRegistry::instance();
            const auto &goldDustsFound = registry.getGlobalConfig().goldDustsFound;
            onWorldStateBitFieldChange("goldDustsAcquired", goldDustsFound, bitIndex, oldValue, newValue);
        });

    for (int i = 0; i < MapTypes::NUM_MAP_TYPES + 1; i++)
    {
        worldMapStateBitsMonitors[i] = std::make_unique<BitFieldChangeDetector<256>>(
            [i](unsigned int bitIndex, bool oldValue, bool newValue)
            {
                const auto &registry = GameStateRegistry::instance();
                auto mapType = static_cast<MapTypes::Enum>(i);
                const auto &worldStateBits = registry.getMapConfig(mapType).worldStateBits;
                std::string fieldName = "mapStateBits[" + std::to_string(i) + "] (" + MapTypes::GetName(i) + ")";
                onWorldStateBitFieldChange(fieldName, worldStateBits, bitIndex, oldValue, newValue, true);
            });
    }

    worldAnimalsFedBitsMonitor = std::make_unique<BitFieldChangeDetector<256>>(
        [](unsigned int bitIndex, bool oldValue, bool newValue)
        {
            const auto &registry = GameStateRegistry::instance();
            const auto &animalsFound = registry.getGlobalConfig().animalsFound;
            onWorldStateBitFieldChange("animalsFedBits", animalsFound, bitIndex, oldValue, newValue);
        });
}

// Keep existing manual comparison for non-BitField data
template <class T> void compareInt(const char *name, T old, T current)
{
    if (current != old)
    {
        warn("%s was changed from %08X to %08X", name, static_cast<uint32_t>(old), static_cast<uint32_t>(current));
    }
}

void comparePreviousStats()
{
    CharacterStats &current = *AmmyStats.get_ptr();
    CharacterStats &old = previousStats;

    compareInt("CharacterStats::unk1", old.unk1, current.unk1);
    compareInt("CharacterStats::unk1b", old.unk1b, current.unk1b);
}

void comparePreviousCollection()
{
    CollectionData &current = *AmmyCollections.get_ptr();
    CollectionData &old = previousCollection;

    compareInt("CollectionData::unk1", old.unk1, current.unk1);
    compareInt("CollectionData::unk2", old.unk2, current.unk2);
    compareInt("CollectionData::unk3", old.unk3, current.unk3);
    compareInt("CollectionData::unk4", old.unk4, current.unk4);

    // Non-BitField WorldStateData comparisons
    compareInt("WorldStateData::unk1", old.world.unk1, current.world.unk1);
    compareInt("WorldStateData::unk2", old.world.unk2, current.world.unk2);
    compareInt("WorldStateData::unk3", old.world.unk3, current.world.unk3);
    compareInt("WorldStateData::unk4", old.world.unk4, current.world.unk4);
    compareInt("WorldStateData::unk10", old.world.unk10, current.world.unk10);

    for (unsigned i = 0; i < 56; i++)
    {
        std::string name = std::string("WorldStateData::unk11[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk11[i], current.world.unk11[i]);
    }

    compareInt("WorldStateData::currentFortuneFlags", old.world.currentFortuneFlags, current.world.currentFortuneFlags);

    for (unsigned i = 0; i < 3; i++)
    {
        std::string name = std::string("WorldStateData::unk15[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk15[i], current.world.unk15[i]);
    }

    compareInt("WorldStateData::unk16", old.world.unk16, current.world.unk16);

    for (unsigned i = 0; i < 4; i++)
    {
        std::string name = std::string("WorldStateData::unk17[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk17[i], current.world.unk17[i]);
    }

    compareInt("WorldStateData::unk18", old.world.unk18, current.world.unk18);
    compareInt("WorldStateData::unk19", old.world.unk19, current.world.unk19);
    compareInt("WorldStateData::unk20", old.world.unk20, current.world.unk20);

    for (unsigned i = 0; i < 194; i++)
    {
        std::string name = std::string("WorldStateData::unk22[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk22[i], current.world.unk22[i]);
    }

    for (unsigned i = 0; i < 7; i++)
    {
        std::string name = std::string("WorldStateData::unk24[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk24[i], current.world.unk24[i]);
    }
}

void compareTrackerData()
{
    TrackerData &current = *AmmyTracker.get_ptr();
    TrackerData &old = previousTracker;

    compareInt("TrackerData::field_52", old.field_52, current.field_52);
    compareInt("TrackerData::unk2", old.unk2, current.unk2);
    compareInt("TrackerData::field_6D", old.field_6D, current.field_6D);
    compareInt("TrackerData::field_6E", old.field_6E, current.field_6E);
    compareInt("TrackerData::field_6F", old.field_6F, current.field_6F);
}

void comparePreviousMapData()
{
    const auto &registry = GameStateRegistry::instance();
    auto &current = *MapData.get_ptr();
    auto &old = *MapData.get_ptr();

    for (unsigned i = 0; i < MapTypes::NUM_MAP_TYPES; i++)
    {
        auto mapType = static_cast<MapTypes::Enum>(i);
        const auto &mapConfig = registry.getMapConfig(mapType);
        std::string mapNamePrefix = std::string("(") + MapTypes::GetName(i) + ") ";

        for (unsigned j = 0; j < 32; j++)
        {
            if (mapConfig.userIndices.contains(j))
                continue;

            std::string name = mapNamePrefix + std::string("MapState::user[") + std::to_string(j) + "]";
            compareInt(name.c_str(), old[i].user[j], current[i].user[j]);
        }

        compareInt((mapNamePrefix + "MapState::timeOfDay").c_str(), old[i].timeOfDay, current[i].timeOfDay);
    }
}

void saveNonBitfieldData()
{
    previousStats = *AmmyStats.get_ptr();
    previousCollection = *AmmyCollections.get_ptr();
    previousTracker = *AmmyTracker.get_ptr();
}

} // namespace

void devDataFinder_OnGameTick()
{
    if (!initialized)
    {
        initializeMonitors();
        saveNonBitfieldData();
        initialized = true;
        return;
    }

    // Update all BitField monitors
    globalFlagsMonitor->update(*GlobalGameStateFlags.get_ptr());

    const auto &mapData = *MapData.get_ptr();
    for (int mapId = 0; mapId < MapTypes::NUM_MAP_TYPES; mapId++)
    {
        const auto &mapState = mapData[mapId];

        collectedObjectsMonitors[mapId]->update(mapState.collectedObjects);
        commonStatesMonitors[mapId]->update(mapState.commonStates);
        areasRestoredMonitors[mapId]->update(mapState.areasRestored);
        treesBloomedMonitors[mapId]->update(mapState.treesBloomed);
        cursedTreesBloomedMonitors[mapId]->update(mapState.cursedTreesBloomed);
        fightsCleared[mapId]->update(mapState.fightsCleared);
        npcHasMoreToSay[mapId]->update(mapState.npcHasMoreToSay);
        npcUnknown[mapId]->update(mapState.npcUnknown);
        mapsExplored[mapId]->update(mapState.mapsExplored);
        field_DC[mapId]->update(mapState.field_DC);
        field_E0[mapId]->update(mapState.field_E0);
    }

    const auto &trackerData = *AmmyTracker.get_ptr();
    trackerGameProgressionMonitor->update(trackerData.gameProgressionBits);
    trackerAnimalsFedFirstTimeMonitor->update(trackerData.animalsFedFirstTime);
    trackerField34Monitor->update(trackerData.field_34);
    trackerField38Monitor->update(trackerData.field_38);
    trackerBrushUpgradesMonitor->update(trackerData.brushUpgrades);
    trackerOptionFlagsMonitor->update(trackerData.optionFlags);
    trackerAreasRestoredMonitor->update(trackerData.areasRestored);

    const auto &collectionData = *AmmyCollections.get_ptr();
    worldKeyItemsAcquiredMonitor->update(collectionData.world.keyItemsAcquired);
    worldGoldDustsAcquiredMonitor->update(collectionData.world.goldDustsAcquired);
    worldAnimalsFedBitsMonitor->update(collectionData.world.animalsFedBits);

    for (int i = 0; i < MapTypes::NUM_MAP_TYPES + 1; i++)
    {
        worldMapStateBitsMonitors[i]->update(collectionData.world.mapStateBits[i]);
    }

    // Manual comparisons for non-BitField data
    comparePreviousStats();
    comparePreviousCollection();
    compareTrackerData();
    comparePreviousMapData();

    saveNonBitfieldData();
}

} // namespace okami
