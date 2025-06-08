#pragma once
#include <cstdint>
#include "bitfieldflags.hpp"

namespace okami
{
    enum class BeadByte1 : uint8_t
    {
        sasa_3 = 1 << 0,
        sasa_2 = 1 << 1,
        sasa_1 = 1 << 2,
        kusa_3 = 1 << 3,
        kusa_2 = 1 << 4,
        kusa_1 = 1 << 5,
        taka_6 = 1 << 6,
        taka_5 = 1 << 7
    };

    enum class BeadByte2 : uint8_t
    {
        taka_4 = 1 << 0,
        taka_3 = 1 << 1,
        taka_2 = 1 << 2,
        taka_1 = 1 << 3,
        tsuta_3 = 1 << 4,
        tsuta_2 = 1 << 5,
        tsuta_1 = 1 << 6,
        agata_6 = 1 << 7
    };

    enum class BeadByte3 : uint8_t
    {
        agata_5 = 1 << 0,
        agata_4 = 1 << 1,
        agata_3 = 1 << 2,
        agata_2 = 1 << 3,
        agata_1 = 1 << 4,
        hana_valley = 1 << 5,
        shinshu_6 = 1 << 6,
        shinshu_5 = 1 << 7
    };

    enum class BeadByte4 : uint8_t
    {
        shinshu_4 = 1 << 0,
        shinshu_3 = 1 << 1,
        shinshu_2 = 1 << 2,
        shinshu_1 = 1 << 3,
        kamiki_3 = 1 << 4,
        kamiki_2 = 1 << 5,
        kamiki_1 = 1 << 6,
        cave_of_nagi = 1 << 7
    };

    enum class BeadByte5 : uint8_t
    {
        nryoshima_5 = 1 << 0,
        nryoshima_4 = 1 << 1,
        nryoshima_3 = 1 << 2,
        nryoshima_2 = 1 << 3,
        nryoshima_1 = 1 << 4,
        imperial_palace = 1 << 5,
        sunken_ship_2 = 1 << 6,
        sunken_ship_1 = 1 << 7
    };

    enum class BeadByte6 : uint8_t
    {
        seian_aq_6 = 1 << 0,
        seian_aq_5 = 1 << 1,
        seian_aq_4 = 1 << 2,
        seian_aq_3 = 1 << 3,
        seian_aq_2 = 1 << 4,
        seian_aq_1 = 1 << 5,
        seian_cq_5 = 1 << 6,
        seian_cq_4 = 1 << 7
    };

    enum class BeadByte7 : uint8_t
    {
        seian_cq_3 = 1 << 0,
        seian_cq_2 = 1 << 1,
        seian_cq_1 = 1 << 2,
        ryoshima_5 = 1 << 3,
        ryoshima_4 = 1 << 4,
        ryoshima_3 = 1 << 5,
        ryoshima_2 = 1 << 6,
        ryoshima_1 = 1 << 7
    };

    enum class BeadByte8 : uint8_t
    {
        checkpoint_5 = 1 << 0,
        checkpoint_4 = 1 << 1,
        checkpoint_3 = 1 << 2,
        checkpoint_2 = 1 << 3,
        checkpoint_1 = 1 << 4,
        gale_shrine_3 = 1 << 5,
        gale_shrine_2 = 1 << 6,
        gale_shrine_1 = 1 << 7
    };

    enum class BeadByte9 : uint8_t
    {
        inner_yoshpet_3 = 1 << 0,
        inner_yoshpet_2 = 1 << 1,
        inner_yoshpet_1 = 1 << 2,
        ponctan_3 = 1 << 3,
        ponctan_2 = 1 << 4,
        ponctan_1 = 1 << 5,
        yoshpet_3 = 1 << 6,
        yoshpet_2 = 1 << 7
    };

    enum class BeadByte10 : uint8_t
    {
        yoshpet_1 = 1 << 0,
        kamui_ezofuji_4 = 1 << 1,
        kamui_ezofuji_3 = 1 << 2,
        kamui_ezofuji_2 = 1 << 3,
        kamui_ezofuji_1 = 1 << 4,
        wepkeer_4 = 1 << 5,
        wepkeer_3 = 1 << 6,
        wepkeer_2 = 1 << 7
    };

    enum class BeadByte11 : uint8_t
    {
        wepkeer_1 = 1 << 0,
        kamui_6 = 1 << 1,
        kamui_5 = 1 << 2,
        kamui_4 = 1 << 3,
        kamui_3 = 1 << 4,
        kamui_2 = 1 << 5,
        kamui_1 = 1 << 6,
        dragon_palace_4 = 1 << 7
    };

    enum class BeadByte12 : uint8_t
    {
        dragon_palace_3 = 1 << 0,
        dragon_palace_2 = 1 << 1,
        dragon_palace_1 = 1 << 2,
        catcall_3 = 1 << 3,
        catcall_2 = 1 << 4,
        catcall_1 = 1 << 5,
        nryoshima_7 = 1 << 6,
        nryoshima_6 = 1 << 7
    };

    enum class BeadByte13 : uint8_t
    {
        // All unused (None)
    };

    enum class BeadByte14 : uint8_t
    {
        // All unused (None)
    };

    enum class BeadByte15 : uint8_t
    {
        // All unused (None)
    };

    enum class BeadByte16 : uint8_t
    {
        // Bits 4–7 only used
        ark_yamato = 1 << 5,
        wawku_3 = 1 << 6,
        wawku_2 = 1 << 7,
        wawku_1 = 1 << 8
    };

#pragma pack(push, 1)
    struct StrayBeadOverlay
    {
        BitfieldFlags<BeadByte1> byte1;
        BitfieldFlags<BeadByte2> byte2;
        BitfieldFlags<BeadByte3> byte3;
        BitfieldFlags<BeadByte4> byte4;
        BitfieldFlags<BeadByte5> byte5;
        BitfieldFlags<BeadByte6> byte6;
        BitfieldFlags<BeadByte7> byte7;
        BitfieldFlags<BeadByte8> byte8;
        BitfieldFlags<BeadByte9> byte9;
        BitfieldFlags<BeadByte10> byte10;
        BitfieldFlags<BeadByte11> byte11;
        BitfieldFlags<BeadByte12> byte12;
        BitfieldFlags<BeadByte13> byte13;
        BitfieldFlags<BeadByte14> byte14;
        BitfieldFlags<BeadByte15> byte15;
        BitfieldFlags<BeadByte16> byte16;
    };
#pragma pack(pop)

    inline StrayBeadOverlay *CollectionStrayBeads;
} // namespace okami
