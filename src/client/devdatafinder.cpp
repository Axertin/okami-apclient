#include <string>
#include <unordered_map>

#include "devdatafinderdesc.h"
#include "devdatamapdata.h"
#include "logger.h"
#include "okami/data/maptype.hpp"
#include "okami/data/structs.hpp"
#include "okami/memorymap.hpp"

namespace okami
{
namespace
{
bool initialized = false;

okami::CharacterStats previousStats;
okami::CollectionData previousCollection;
okami::TrackerData previousTracker;
std::array<okami::MapState, okami::MapTypes::NUM_MAP_TYPES> previousMapData;

const std::unordered_map<unsigned int, std::string> emptyMapDesc{};

template <typename... Args> void warn(const char *format, Args... args)
{
    std::string newFmt = std::string("[Undocumented] ") + format;
    logWarning(newFmt.c_str(), args...);
}

template <unsigned N>
void compareBitfield(const char *name, okami::BitField<N> &old, okami::BitField<N> &current, const std::unordered_map<unsigned, std::string> &documentation,
                     bool showAlways = false)
{
    okami::BitField<N> diff = current ^ old;
    std::vector<unsigned> diffIndices = diff.GetSetIndices();
    for (auto idx : diffIndices)
    {
        if (!documentation.contains(idx))
        {
            warn("BitField %s index %u was changed from %d to %d", name, idx, old.IsSet(idx), current.IsSet(idx));
        }
        else if (showAlways)
        {
            logInfo("BitField %s index %u (%s) was changed from %d to %d", name, idx, documentation.at(idx).c_str(), old.IsSet(idx), current.IsSet(idx));
        }
    }
}

template <class T> void compareInt(const char *name, T old, T current);

template <> void compareInt<uint32_t>(const char *name, uint32_t old, uint32_t current)
{
    if (current != old)
    {
        warn("%s was changed from %08X to %08X", name, old, current);
    }
}

template <> void compareInt<uint16_t>(const char *name, uint16_t old, uint16_t current)
{
    if (current != old)
    {
        warn("%s was changed from %04X to %04X", name, old, current);
    }
}

template <> void compareInt<uint8_t>(const char *name, uint8_t old, uint8_t current)
{
    if (current != old)
    {
        warn("%s was changed from %02X to %02X", name, old, current);
    }
}

void comparePreviousStats()
{
    okami::CharacterStats &current = *okami::AmmyStats.get_ptr();
    okami::CharacterStats &old = previousStats;

    compareInt("CharacterStats::unk1", old.unk1, current.unk1);
    compareInt("CharacterStats::unk1b", old.unk1b, current.unk1b);
}

void comparePreviousCollection()
{
    okami::CollectionData &current = *okami::AmmyCollections.get_ptr();
    okami::CollectionData &old = previousCollection;

    compareInt("CollectionData::unk1", old.unk1, current.unk1);
    compareInt("CollectionData::unk2", old.unk2, current.unk2);
    compareInt("CollectionData::unk3", old.unk3, current.unk3);
    compareInt("CollectionData::unk4", old.unk4, current.unk4);

    compareInt("WorldStateData::unk1", old.world.unk1, current.world.unk1);
    compareInt("WorldStateData::unk2", old.world.unk2, current.world.unk2);
    compareInt("WorldStateData::unk3", old.world.unk3, current.world.unk3);
    compareInt("WorldStateData::unk4", old.world.unk4, current.world.unk4);
    compareBitfield("WorldStateData::keyItemsAcquired", old.world.keyItemsAcquired, current.world.keyItemsAcquired, keyItemsFoundDesc);
    compareBitfield("WorldStateData::goldDustsAcquired", old.world.goldDustsAcquired, current.world.goldDustsAcquired, goldDustsFoundDesc);
    compareInt("WorldStateData::unk10", old.world.unk10, current.world.unk10);
    for (unsigned i = 0; i < 56; i++)
    {
        std::string name = std::string("WorldStateData::unk11[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk11[i], current.world.unk11[i]);
    }

    for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES + 1; i++)
    {
        std::string name = std::string("WorldStateData::mapStateBits[") + std::to_string(i) + "] (" + okami::MapTypes::GetName(i) + ")";
        compareBitfield(name.c_str(), old.world.mapStateBits[i], current.world.mapStateBits[i], mapDataDesc.at(i).worldStateBits, true);
    }
    compareBitfield("WorldStateData::animalsFedBits", old.world.animalsFedBits, current.world.animalsFedBits, animalsFedDesc);

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
    okami::TrackerData &current = *okami::AmmyTracker.get_ptr();
    okami::TrackerData &old = previousTracker;

    compareBitfield("TrackerData::gameProgressionBits", old.gameProgressionBits, current.gameProgressionBits, gameProgressDesc, true);
    compareBitfield("TrackerData::animalsFedFirstTime", old.animalsFedFirstTime, current.animalsFedFirstTime, animalsFedFirstTimeDesc);

    compareBitfield("TrackerData::field_34", old.field_34, current.field_34, emptyMapDesc);
    compareBitfield("TrackerData::field_38", old.field_38, current.field_38, emptyMapDesc);
    compareBitfield("TrackerData::brushUpgrades", old.brushUpgrades, current.brushUpgrades, brushUpgradesDesc);
    compareBitfield("TrackerData::optionFlags", old.optionFlags, current.optionFlags, emptyMapDesc);

    compareBitfield("TrackerData::areasRestored", old.areasRestored, current.areasRestored, areasRestoredDesc);
    compareInt("TrackerData::field_52", old.field_52, current.field_52);
    compareInt("TrackerData::unk2", old.unk2, current.unk2);
    compareInt("TrackerData::field_6D", old.field_6D, current.field_6D);
    compareInt("TrackerData::field_6E", old.field_6E, current.field_6E);
    compareInt("TrackerData::field_6F", old.field_6F, current.field_6F);
}

void comparePreviousMapData()
{
    auto &current = *okami::MapData.get_ptr();
    auto &old = previousMapData;
    std::string name;

    for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES; i++)
    {
        std::string mapNamePrefix = std::string("(") + okami::MapTypes::GetName(i) + ") ";
        for (unsigned j = 0; j < 32; j++)
        {
            if (mapDataDesc.at(i).userIndices.contains(j))
                continue;

            name = mapNamePrefix + std::string("MapState::user[") + std::to_string(j) + "]";
            compareInt(name.c_str(), old[i].user[j], current[i].user[j]);
        }

        name = mapNamePrefix + std::string("MapState::collectedObjects");
        compareBitfield(name.c_str(), old[i].collectedObjects, current[i].collectedObjects, mapDataDesc.at(i).collectedObjects);

        name = mapNamePrefix + std::string("MapState::commonStates");
        compareBitfield(name.c_str(), old[i].commonStates, current[i].commonStates, commonStatesDesc);

        name = mapNamePrefix + std::string("MapState::areasRestored");
        compareBitfield(name.c_str(), old[i].areasRestored, current[i].areasRestored, mapDataDesc.at(i).areasRestored);

        name = mapNamePrefix + std::string("MapState::treesBloomed");
        compareBitfield(name.c_str(), old[i].treesBloomed, current[i].treesBloomed, mapDataDesc.at(i).treesBloomed);

        name = mapNamePrefix + std::string("MapState::cursedTreesBloomed");
        compareBitfield(name.c_str(), old[i].cursedTreesBloomed, current[i].cursedTreesBloomed, mapDataDesc.at(i).cursedTreesBloomed);

        name = mapNamePrefix + std::string("MapState::fightsCleared");
        compareBitfield(name.c_str(), old[i].fightsCleared, current[i].fightsCleared, mapDataDesc.at(i).fightsCleared);

        name = mapNamePrefix + std::string("MapState::npcHasMoreToSay");
        compareBitfield(name.c_str(), old[i].npcHasMoreToSay, current[i].npcHasMoreToSay, mapDataDesc.at(i).npcs, true);

        name = mapNamePrefix + std::string("MapState::npcUnknown");
        compareBitfield(name.c_str(), old[i].npcUnknown, current[i].npcUnknown, mapDataDesc.at(i).npcs, true);

        name = mapNamePrefix + std::string("MapState::mapsExplored");
        compareBitfield(name.c_str(), old[i].mapsExplored, current[i].mapsExplored, mapDataDesc.at(i).mapsExplored);

        name = mapNamePrefix + std::string("MapState::field_DC");
        compareBitfield(name.c_str(), old[i].field_DC, current[i].field_DC, mapDataDesc.at(i).field_DC);

        name = mapNamePrefix + std::string("MapState::field_E0");
        compareBitfield(name.c_str(), old[i].field_E0, current[i].field_E0, mapDataDesc.at(i).field_E0);
    }
}

void saveData()
{
    previousStats = *okami::AmmyStats.get_ptr();
    previousCollection = *okami::AmmyCollections.get_ptr();
    previousTracker = *okami::AmmyTracker.get_ptr();
    previousMapData = *okami::MapData.get_ptr();
}
} // namespace

void devDataFinder_OnGameTick()
{
    if (!initialized)
    {
        saveData();
        initialized = true;
        return;
    }
    comparePreviousStats();
    comparePreviousCollection();
    compareTrackerData();
    comparePreviousMapData();

    saveData();
}
} // namespace okami