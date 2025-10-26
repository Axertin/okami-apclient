#pragma once

#include <string>
#include <unordered_map>

namespace apitems
{

static std::unordered_map<int, std::string> progressives{{300, "Progressive Mirror"}, {301, "Progressive Rosary"}, {302, "Progressive Sword"}};

static std::unordered_map<int, std::string> events{{0x303, "Save Rei"},   {0x304, "Save Shin"}, {0x305, "Save Chi"},
                                                   {0x306, "Save Ko"},    {0x307, "Save Tei"},  {0x4E, "Loyalty Orb"},
                                                   {0x4F, "Justice Orb"}, {0x50, "Duty Orb"},   {0x308, "Serpent Crystal"}};

} // namespace apitems
