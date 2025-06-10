
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <iomanip>
#include "memoryaccessor.hpp"

namespace okami
{
    inline MemoryAccessor<uint16_t> ExeriorMapID; // Loading zone sequence 3
    inline MemoryAccessor<uint16_t> CurrentMapID; // Loading zone sequence 1
    inline MemoryAccessor<uint16_t> VestigialMapID1;
    inline MemoryAccessor<uint16_t> VestigialMapID2; // Loading zone sequence 2

    inline static const std::unordered_map<uint16_t, std::string> MapNames = {
        {0x006, "Endless loading screen"},
        {0x007, "2005 Prototype Intro Cutscene"},
        {0x010, "Item Test Map #1"},
        {0x011, "Item Test Map #2"},
        {0x020, "Brush Interaction Test Map #1"},
        {0x021, "Brush Interaction Test Map #2"},
        {0x030, "Brush Interaction Test Map #3"},
        {0x031, "Brush Interaction Test Map #4"},
        {0x032, "Brush Interaction Test Map #5"},
        {0x033, "Brush Interaction Test Map #6"},
        {0x034, "Brush Interaction Test Map #7"},
        {0x0FF, "Collision and Movement Test Map"},
        {0x100, "Kamiki Village - Cursed"},
        {0x101, "Cave of Nagi"},
        {0x102, "Kamiki Village"},
        {0x103, "Hana Valley"},
        {0x104, "Tsuta Ruins"},
        {0x105, "City Checkpoint"},
        {0x106, "Tsuta Ruins - Spider Queen Arena"},
        {0x107, "Gale Shrine"},
        {0x108, "Kusa Village"},
        {0x109, "Sasa Sanctuary"},
        {0x10A, "Agata Forest - Madame Fawn's House"},
        {0x10B, "Digging Minigame"},
        {0x10C, "Onigiri Dojo Lesson Room"},
        {0x10D, "Gale Shrine - Crimson Helm Arena"},
        {0x10E, "Calcified Cavern"},
        {0x110, "Moon Cave - Interior"},
        {0x111, "Moon Cave - Staircase and Orochi Arena"},
        {0x112, "Kamiki Village - Post-Tei"},
        {0x113, "Ryoshima Coast - Bandit Spider Arena"},
        {0x114, "Taka Pass - Stray Bead #26 Cave"},
        {0x115, "Kamui - Cherry Bomb 3 Cave"},
        {0x116, "North Ryoshima Coast - Power Slash 2 Cave"},
        {0x117, "North Ryoshima Coast - Cherry Bomb 2 Cave"},
        {0x118, "Kamui - Power Slash 3 Cave"},
        {0x119, "Kamui - Blockhead Grande Cave"},
        {0x11A, "North Ryoshima Coast - Bandit Spider Arena"},
        {0x11B, "Kamui - Bandit Spider Arena"},
        {0x11C, "Dragon Palace - Stray Bead Cave"},
        {0x11D, "North Ryoshima Coast - Stray Bead Cave"},
        {0x120, "New Game Intro Cutscene"},
        {0x121, "Beta Kamiki"},
        {0x122, "River of the Heavens"},
        {0x200, "Sei-an City (Aristocratic Quarter)"},
        {0x201, "Sei-an City (Commoners' Quarter)"},
        {0x202, "Sei-an City (Aristocratic Quarter) - Himiko's Palace"},
        {0x203, "Dragon Palace"},
        {0x204, "Inside the Dragon"},
        {0x205, "Sunken Ship"},
        {0x206, "Imperial Palace - Ammy Size"},
        {0x207, "Imperial Palace - Issun Size"},
        {0x208, "Oni Island - Lower Interior"},
        {0x209, "Oni Island - Ninetails Arena"},
        {0x20A, "Catcall Tower"},
        {0x20B, "Imperial Palace - Blight Arena"},
        {0x20C, "Sei-an City (Commoners' Quarter) - Kimono Shop"},
        {0x20D, "Oni Island - Exterior"},
        {0x20E, "Oni Island - Upper Interior"},
        {0x20F, "Oni Island - Sidescroller"},
        {0x301, "Wep'keer"},
        {0x302, "Kamiki Village - Past"},
        {0x303, "Wawku Shrine"},
        {0x304, "Wawku Shrine - Nechku Arena"},
        {0x305, "Ponc'tan"},
        {0x306, "Moon Cave - Past"},
        {0x307, "Ark of Yamato"},
        {0x308, "Ark of Yamato - Orochi Arena"},
        {0x309, "Ark of Yamato - Blight Arena"},
        {0x30A, "Ark of Yamato - Ninetails Arena"},
        {0x30B, "Ark of Yamato - Crimson Helm Arena"},
        {0x30C, "Ponc'tan - Mrs. Seal's House"},
        {0x310, "Yoshpet"},
        {0x311, "Inner Yoshpet"},
        {0x312, "Ark of Yamato - Yami Arena"},
        {0x313, "Wep'keer Square"},
        {0x314, "Wawku Shrine - Lechku & Nechku Arena"},
        {0xC00, "Title Screen"},
        {0xC01, "Unknown"},
        {0xC02, "Presents from Issun"},
        {0xD00, "Beta Shinshu"},
        {0xD01, "Beta Hana"},
        {0xD02, "Beta Tsuta"},
        {0xD03, "Beta Agata"},
        {0xD04, "Beta Ryoshima"},
        {0xD05, "Beta Kamui"},
        {0xD06, "Beta Taka"},
        {0xD07, "Title Screen Demo Cutscene"},
        {0xE00, "Fishing (Sei-an Bridge)"},
        {0xE01, "Fishing (Himiko's Palace)"},
        {0xE02, "Fishing (Agata)"},
        {0xE03, "Fishing (Kamui)"},
        {0xE04, "Fishing (North Ryoshima)"},
        {0xF01, "Shinshu Field - Cursed"},
        {0xF02, "Shinshu Field - Healed"},
        {0xF03, "Agata Forest - Cursed"},
        {0xF04, "Agata Forest - Healed"},
        {0xF06, "Moon Cave Entrance"},
        {0xF07, "Taka Pass - Cursed"},
        {0xF08, "Taka Pass - Healed"},
        {0xF09, "Ryoshima Coast - Cursed"},
        {0xF0a, "Ryoshima Coast - Healed"},
        {0xF0c, "N. Ryoshima Coast"},
        {0xF11, "Kamui - Cursed"},
        {0xF12, "Kamui - Healed"},
        {0xF13, "Kamui (Ezofuji)"},
        {0xF20, "Shinshu Field - Past"},
        {0xF21, "Moon Cave Entrance - Past"}};

    /// @brief Decode Map name from ID
    /// @param ID Map ID
    /// @return String Map Name
    inline std::string decodeMapName(uint16_t ID)
    {
        if (auto it = okami::MapNames.find(ID); it != MapNames.end())
        {
            return it->second;
        }

        std::stringstream stream;
        stream << "Unknown Map 0x" << std::hex << ID << std::dec;
        return stream.str();
    }
}
