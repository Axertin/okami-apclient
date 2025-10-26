#pragma once

namespace okami
{
namespace Animals
{
/**
 * @brief Enum representing all animals.
 */
enum Enum
{
    Sparrow,
    Chicken,
    Dog,
    Hare,
    Boar,
    BoarPiglet,
    Monkey,
    Pig,
    Deer,
    Nightingale,
    Fox,
    Raccoon,
    Horse,
    Tiger,
    Mouse,
    Cat,
    Cow,
    Bear,
    Sheep,
    Crane,
    NUM_ANIMALS
};
static const char *name[NUM_ANIMALS] = {
    "Sparrow", "Chicken", "Dog",   "Hare",  "Boar",  "Boar Piglet", "Monkey", "Pig",  "Deer",  "Nightingale",
    "Fox",     "Raccoon", "Horse", "Tiger", "Mouse", "Cat",         "Cow",    "Bear", "Sheep", "Crane",
};

inline const char *GetName(unsigned index)
{
    if (index < NUM_ANIMALS)
    {
        return name[index];
    }
    return "invalid";
}

} // namespace Animals
} // namespace okami
