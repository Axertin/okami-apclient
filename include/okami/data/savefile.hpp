#pragma once
#include <cstdint>

#include "structs.hpp"

// mainly only for documentation purposes

namespace okami
{

struct SaveSlot
{
    uint32_t header;        // 00 00 40 40
    uint32_t areaNameStrId; // from +0x79BEB4
    uint64_t checksum;
    uint64_t timeRTC;

    // Loaded to +0xB4DF90
    CharacterStats character;

    // Loaded to +0xB21780
    TrackerData tracked;

    // Loaded to +0xB205D0
    // contains map state bits, some info only loaded into this struct on save,
    // live usage elsewhere
    CollectionData collection;

    // Loaded to +0xB322B0
    BitField<1824> persistentMapBits[MapTypes::NUM_MAP_TYPES]; // mostly dialog

    // Loaded to +0xB36CF0
    BitField<512> issunDialogBits[MapTypes::NUM_MAP_TYPES];
    uint32_t unk1; // padding?

    // Loaded to +0xB21820
    CustomTextures customTextures;
}; // 0x172A0 each

struct SaveFile
{
    SaveSlot slots[30];
};
} // namespace okami
