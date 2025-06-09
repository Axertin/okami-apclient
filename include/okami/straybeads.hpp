#pragma once
#include <cstdint>
#include "bitfieldflags.hpp"

namespace okami
{
    /**
     * @brief Enum representing all stray beads by bit index across 4 bytes.
     *
     * Bit values represent contiguous bits starting from bit 0 (lowest bit of byte 0),
     * up through bit 127 (highest bit of byte 15).
     */
    enum class StrayBeadOverlay : uint8_t
    {
        // Byte 0 (offset +0)
        sasa_3 = 0,
        sasa_2 = 1,
        sasa_1 = 2,
        kusa_3 = 3,
        kusa_2 = 4,
        kusa_1 = 5,
        taka_6 = 6,
        taka_5 = 7,

        // Byte 1 (offset +1)
        taka_4 = 8,
        taka_3 = 9,
        taka_2 = 10,
        taka_1 = 11,
        tsuta_3 = 12,
        tsuta_2 = 13,
        tsuta_1 = 14,
        agata_6 = 15,

        // Byte 2 (offset +2)
        agata_5 = 16,
        agata_4 = 17,
        agata_3 = 18,
        agata_2 = 19,
        agata_1 = 20,
        hana_valley = 21,
        shinshu_6 = 22,
        shinshu_5 = 23,

        // Byte 3 (offset +3)
        shinshu_4 = 24,
        shinshu_3 = 25,
        shinshu_2 = 26,
        shinshu_1 = 27,
        kamiki_3 = 28,
        kamiki_2 = 29,
        kamiki_1 = 30,
        cave_of_nagi = 31,

        // Byte 4 (offset +4)
        nryoshima_5 = 32,
        nryoshima_4 = 33,
        nryoshima_3 = 34,
        nryoshima_2 = 35,
        nryoshima_1 = 36,
        imperial_palace = 37,
        sunken_ship_2 = 38,
        sunken_ship_1 = 39,

        // Byte 5 (offset +5)
        seian_aq_6 = 40,
        seian_aq_5 = 41,
        seian_aq_4 = 42,
        seian_aq_3 = 43,
        seian_aq_2 = 44,
        seian_aq_1 = 45,
        seian_cq_5 = 46,
        seian_cq_4 = 47,

        // Byte 6 (offset +6)
        seian_cq_3 = 48,
        seian_cq_2 = 49,
        seian_cq_1 = 50,
        ryoshima_5 = 51,
        ryoshima_4 = 52,
        ryoshima_3 = 53,
        ryoshima_2 = 54,
        ryoshima_1 = 55,

        // Byte 7 (offset +7)
        checkpoint_5 = 56,
        checkpoint_4 = 57,
        checkpoint_3 = 58,
        checkpoint_2 = 59,
        checkpoint_1 = 60,
        gale_shrine_3 = 61,
        gale_shrine_2 = 62,
        gale_shrine_1 = 63,

        // Byte 8 (offset +8)
        inner_yoshpet_3 = 64,
        inner_yoshpet_2 = 65,
        inner_yoshpet_1 = 66,
        ponctan_3 = 67,
        ponctan_2 = 68,
        ponctan_1 = 69,
        yoshpet_3 = 70,
        yoshpet_2 = 71,

        // Byte 9 (offset +9)
        yoshpet_1 = 72,
        kamui_ezofuji_4 = 73,
        kamui_ezofuji_3 = 74,
        kamui_ezofuji_2 = 75,
        kamui_ezofuji_1 = 76,
        wepkeer_4 = 77,
        wepkeer_3 = 78,
        wepkeer_2 = 79,

        // Byte 10 (offset +10)
        wepkeer_1 = 80,
        kamui_6 = 81,
        kamui_5 = 82,
        kamui_4 = 83,
        kamui_3 = 84,
        kamui_2 = 85,
        kamui_1 = 86,
        dragon_palace_4 = 87,

        // Byte 11 (offset +11)
        dragon_palace_3 = 88,
        dragon_palace_2 = 89,
        dragon_palace_1 = 90,
        catcall_3 = 91,
        catcall_2 = 92,
        catcall_1 = 93,
        nryoshima_7 = 94,
        nryoshima_6 = 95,

        // Bytes 12-15 Unused

        // Byte 15
        ark_yamato = 124,
        wawku_3 = 125,
        wawku_2 = 126,
        wawku_1 = 127
    };

} // namespace okami
