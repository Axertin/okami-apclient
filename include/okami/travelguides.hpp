#pragma once
#include <cstdint>
#include "bitfieldflags.hpp"

namespace okami
{
    enum class TravelGuideByte1 : uint8_t
    {
        power_slash_3 = 1 << 2,        // ID 30
        tribe_of_the_moon = 1 << 3,    // ID 29
        cherry_bomb_3 = 1 << 4,        // ID 28
        another_civilization = 1 << 5, // ID 27
        thunderstorm_tech = 1 << 6,    // ID 26
        waterspout_tech_2 = 1 << 7,    // ID 25
    };

    enum class TravelGuideByte2 : uint8_t
    {
        waterspout_tech_1 = 1 << 0, // ID 24
        cherry_bomb_2 = 1 << 1,     // ID 23
        power_slash_2 = 1 << 2,     // ID 22
        galestorm_tech = 1 << 3,    // ID 21
        mark_of_kabegami = 1 << 4,  // ID 20
        inferno_tech = 1 << 5,      // ID 19
        northern_land = 1 << 6,     // ID 18
        holy_artifacts = 1 << 7,    // ID 17
    };

    enum class TravelGuideByte3 : uint8_t
    {
        veil_of_mist_tech = 1 << 0,  // ID 16
        celestial_envoy = 1 << 1,    // ID 15
        land_of_the_gods = 1 << 2,   // ID 14
        mother_tree = 1 << 3,        // ID 13
        brush_tips = 1 << 4,         // ID 12
        ink_bullet_tips = 1 << 5,    // ID 11
        godhood_tips = 1 << 6,       // ID 10
        enhancing_divinity = 1 << 7, // ID 9
    };

    enum class TravelGuideByte4 : uint8_t
    {
        battle_tips = 1 << 0,       // ID 8
        enhancing_weapons = 1 << 1, // ID 7
        legend_of_orochi = 1 << 2,  // ID 6
        feeding = 1 << 3,           // ID 5
        fleeing_battle = 1 << 4,    // ID 4
        digging_tips = 1 << 5,      // ID 3
        greensprout_tips = 1 << 6,  // ID 2
        travel_tips = 1 << 7,       // ID 1
    };

#pragma pack(push, 1)
    struct TravelGuideOverlay
    {
        BitfieldFlags<TravelGuideByte1> byte1;
        BitfieldFlags<TravelGuideByte2> byte2;
        BitfieldFlags<TravelGuideByte3> byte3;
        BitfieldFlags<TravelGuideByte4> byte4;
    };
#pragma pack(pop)

    inline TravelGuideOverlay *CollectionTravelGuides;
}
