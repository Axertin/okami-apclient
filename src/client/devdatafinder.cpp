#include <string>
#include <unordered_map>

#include "devdatafinderdesc.h"
#include "logger.h"
#include "okami/data/maptype.hpp"
#include "okami/data/structs.hpp"
#include "okami/memorymap.hpp"

namespace
{
bool initialized = false;

okami::CharacterStats previousStats;
okami::CollectionData previousCollection;
okami::TrackerData previousTracker;
std::array<okami::BitField<1824>, okami::MapTypes::NUM_MAP_TYPES> previousMapBits;
std::array<okami::BitField<512>, okami::MapTypes::NUM_MAP_TYPES> previousDialogBits;

template <typename... Args> void warn(const char *format, Args... args)
{
    std::string newFmt = std::string("[Undocumented] ") + format;
    logWarning(newFmt.c_str(), args...);
}

template <unsigned N>
void compareBitfield(const char *name, okami::BitField<N> &old, okami::BitField<N> &current, const std::unordered_map<unsigned, const char *> &documentation)
{
    okami::BitField<N> diff = current ^ old;
    std::vector<unsigned> diffIndices = diff.GetSetIndices();
    for (auto idx : diffIndices)
    {
        if (!documentation.contains(idx))
        {
            warn("BitField %s index %u was changed from %d to %d", name, idx, old.IsSet(idx), current.IsSet(idx));
        }
    }
}

void compareInt(const char *name, int old, int current)
{
    if (current != old)
    {
        warn("%s was changed from %d to %d", name, old, current);
    }
}

void comparePreviousStats()
{
    okami::CharacterStats &current = *okami::AmmyStats.get_ptr();
    okami::CharacterStats &old = previousStats;

    compareInt("CharacterStats::unk1", old.unk1, current.unk1);
    compareInt("CharacterStats::unk1b", old.unk1b, current.unk1b);
    compareInt("CharacterStats::unk2", old.unk2, current.unk2);
    compareInt("CharacterStats::unk3", old.unk3, current.unk3);
    compareInt("CharacterStats::unk4", old.unk4, current.unk4);
    compareInt("CharacterStats::unk5", old.unk5, current.unk5);
    compareInt("CharacterStats::unk6", old.unk6, current.unk6);
    compareInt("CharacterStats::unk7", old.unk7, current.unk7);
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
    compareInt("WorldStateData::unk5", old.world.unk5, current.world.unk5);
    compareInt("WorldStateData::unk6", old.world.unk6, current.world.unk6);
    compareInt("WorldStateData::unk7", old.world.unk7, current.world.unk7);
    compareInt("WorldStateData::unk8", old.world.unk8, current.world.unk8);
    compareInt("WorldStateData::unk9", old.world.unk9, current.world.unk9);
    compareInt("WorldStateData::unk10", old.world.unk10, current.world.unk10);
    for (unsigned i = 0; i < 56; i++)
    {
        std::string name = std::string("WorldStateData::unk11[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk11[i], current.world.unk11[i]);
    }

    for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES + 1; i++)
    {
        std::string name = std::string("WorldStateData::mapStateBits[") + std::to_string(i) + "] (" + okami::MapTypes::GetName(i) + ")";
        compareBitfield(name.c_str(), old.world.mapStateBits[i], current.world.mapStateBits[i], worldStateMapBitsDesc[i]);
    }
    compareBitfield("WorldStateData::animalsFedBits", old.world.animalsFedBits, current.world.animalsFedBits, animalsFedDesc);

    for (unsigned i = 0; i < 10; i++)
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
    for (unsigned i = 0; i < 4; i++)
    {
        std::string name = std::string("WorldStateData::unk21[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk21[i], current.world.unk21[i]);
    }
    for (unsigned i = 0; i < 780; i++)
    {
        std::string name = std::string("WorldStateData::unk22[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk22[i], current.world.unk22[i]);
    }

    for (unsigned i = 0; i < 28; i++)
    {
        std::string name = std::string("WorldStateData::unk24[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.world.unk24[i], current.world.unk24[i]);
    }
}

void compareTrackerData()
{
    okami::TrackerData &current = *okami::AmmyTracker.get_ptr();
    okami::TrackerData &old = previousTracker;

    compareBitfield("TrackerData::logbookAvailable", old.logbookAvailable, current.logbookAvailable, tracker1Desc);
    compareBitfield("TrackerData::unknown", old.unknown, current.unknown, tracker2Desc);

    for (unsigned i = 0; i < 4; i++)
    {
        std::string name = std::string("TrackerData::unk1[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.unk1[i], current.unk1[i]);
    }
    compareInt("TrackerData::field_40", old.field_40, current.field_40);
    compareInt("TrackerData::field_44", old.field_44, current.field_44);
    compareInt("TrackerData::field_48", old.field_48, current.field_48);
    compareInt("TrackerData::field_4C", old.field_4C, current.field_4C);
    compareInt("TrackerData::field_4E", old.field_4E, current.field_4E);
    compareInt("TrackerData::field_50", old.field_50, current.field_50);
    compareInt("TrackerData::field_52", old.field_52, current.field_52);
    compareInt("TrackerData::unk2", old.unk2, current.unk2);
    compareInt("TrackerData::field_6D", old.field_6D, current.field_6D);
    compareInt("TrackerData::field_6E", old.field_6E, current.field_6E);
    compareInt("TrackerData::field_6F", old.field_6F, current.field_6F);

    for (unsigned i = 0; i < 3; i++)
    {
        std::string name = std::string("TrackerData::field_70[") + std::to_string(i) + "]";
        compareInt(name.c_str(), old.field_70[i], current.field_70[i]);
    }
}

void comparePreviousMapBits()
{
    auto &current = *okami::MapBits.get_ptr();
    auto &old = previousMapBits;

    for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES; i++)
    {
        std::string name = std::string("MapBits[") + std::to_string(i) + "] (" + okami::MapTypes::GetName(i) + ")";
        compareBitfield(name.c_str(), old[i], current[i], mapBitsDesc[i]);
    }
}

void comparePreviousDialogBits()
{
    auto &current = *okami::DialogBits.get_ptr();
    auto &old = previousDialogBits;

    for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES; i++)
    {
        std::string name = std::string("IssunBits[") + std::to_string(i) + "] (" + okami::MapTypes::GetName(i) + ")";
        compareBitfield(name.c_str(), old[i], current[i], dialogBitsDesc[i]);
    }
}

void saveData()
{
    previousStats = *okami::AmmyStats.get_ptr();
    previousCollection = *okami::AmmyCollections.get_ptr();
    previousTracker = *okami::AmmyTracker.get_ptr();
    previousMapBits = *okami::MapBits.get_ptr();
    previousDialogBits = *okami::DialogBits.get_ptr();
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
    comparePreviousMapBits();
    comparePreviousDialogBits();

    saveData();
}
