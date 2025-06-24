#include "straybeads.hpp"

namespace okami
{
namespace StrayBeads
{
static const char *names[NUM_STRAY_BEADS] = {
    "Cave Of Nagi 1",    "Kamiki Village 1",  "Kamiki Village 2",  "Kamiki Village 3",  "Shinshu Field 1",   "Shinshu Field 2",   "Shinshu Field 3",
    "Shinshu Field 4",   "Shinshu Field 5",   "Shinshu Field 6",   "Hana Valley 1",     "Agata Forest 1",    "Agata Forest 2",    "Agata Forest 3",
    "Agata Forest 4",    "Agata Forest 5",    "Agata Forest 6",    "Tsuta Ruins 1",     "Tsuta Ruins 2",     "Tsuta Ruins 3",     "Taka Pass 1",
    "Taka Pass 2",       "Taka Pass 3",       "Taka Pass 4",       "Taka Pass 5",       "Taka Pass 6",       "Kusa Village 1",    "Kusa Village 2",
    "Kusa Village 3",    "Sasa Sanctuary 1",  "Sasa Sanctuary 2",  "Sasa Sanctuary 3",  "Gale Shrine 1",     "Gale Shrine 2",     "Gale Shrine 3",
    "City Checkpoint 1", "City Checkpoint 2", "City Checkpoint 3", "City Checkpoint 4", "City Checkpoint 5", "Ryoshima Coast 1",  "Ryoshima Coast 2",
    "Ryoshima Coast 3",  "Ryoshima Coast 4",  "Ryoshima Coast 5",  "Seian C. Qtr. 1",   "Seian C. Qtr. 2",   "Seian C. Qtr. 3",   "Seian C. Qtr. 4",
    "Seian C. Qtr. 5",   "Seian A. Qtr. 1",   "Seian A. Qtr. 2",   "Seian A. Qtr. 3",   "Seian A. Qtr. 4",   "Seian A. Qtr. 5",   "Seian A. Qtr. 6",
    "Sunken Ship 1",     "Sunken Ship 2",     "Imperial Palace 1", "NRyoshima Coast 1", "NRyoshima Coast 2", "NRyoshima Coast 3", "NRyoshima Coast 4",
    "NRyoshima Coast 5", "NRyoshima Coast 6", "NRyoshima Coast 7", "Catcall Tower 1",   "Catcall Tower 2",   "Catcall Tower 3",   "Dragon Palace 1",
    "Dragon Palace 2",   "Dragon Palace 3",   "Dragon Palace 4",   "Kamui 1",           "Kamui 2",           "Kamui 3",           "Kamui 4",
    "Kamui 5",           "Kamui 6",           "Wepkeer 1",         "Wepkeer 2",         "Wepkeer 3",         "Wepkeer 4",         "Kamua Ezofuji 1",
    "Kamua Ezofuji 2",   "Kamua Ezofuji 3",   "Kamua Ezofuji 4",   "Yoshpet 1",         "Yoshpet 2",         "Yoshpet 3",         "Ponctan 1",
    "Ponctan 2",         "Ponctan 3",         "Inner Yoshpet 1",   "Inner Yoshpet 2",   "Inner Yoshpet 3",   "Wawku Shrine 1",    "Wawku Shrine 2",
    "Wawku Shrine 3",    "Ark Of Yamato 1",
};

const char *GetName(unsigned value)
{
    if (value < NUM_STRAY_BEADS)
    {
        return names[value];
    }
    return "invalid";
}
} // namespace StrayBeads
} // namespace okami
