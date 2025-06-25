#include "treasures.hpp"

namespace okami
{
namespace Treasures
{
static const char *name[NUM_TREASURES] = {
    "Glass Bead",          "Dragonfly Bead",    "Wooden Bear",    "Coral Fragment",      "Vase",
    "Incense Burner",      "Lacquerware Set",   "Bull Horn",      "Rat Statue",          "Bull Statue",
    "Tiger Statue",        "Rabbit Statue",     "Dragon Statue",  "Snake Statue",        "Horse Statue",
    "Sheep Statue",        "Monkey Statue",     "Rooster Statue", "Dog Statue",          "Boar Statue",
    "Cat Statue",          "Crystal",           "Pearl",          "Amethyst Tassels",    "Amber Tassels",
    "Jade Tassels",        "Cat's Eye Tassels", "Agate Tassels",  "Turquoise Tassels",   "Ruby Tassels",
    "Sapphire Tassels",    "Emerald Tassels",   "Kutani Pottery", "White Porcelain Pot", "Etched Glass",
    "Silver Pocket Watch",
};

const char *GetName(unsigned index)
{
    if (index < NUM_TREASURES)
    {
        return name[index];
    }
    return "invalid";
}
} // namespace Treasures
} // namespace okami