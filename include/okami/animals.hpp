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
const char *GetName(unsigned index);

} // namespace Animals
} // namespace okami
