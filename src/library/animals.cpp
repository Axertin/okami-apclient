#include "okami/animals.hpp"

namespace okami
{
namespace Animals
{
static const char *name[NUM_ANIMALS] = {
    "Sparrow", "Chicken", "Dog",   "Hare",  "Boar",  "Boar Piglet", "Monkey", "Pig",  "Deer",  "Nightingale",
    "Fox",     "Raccoon", "Horse", "Tiger", "Mouse", "Cat",         "Cow",    "Bear", "Sheep", "Crane",
};

const char *GetName(unsigned index)
{
    if (index < NUM_ANIMALS)
    {
        return name[index];
    }
    return "invalid";
}
} // namespace Animals
} // namespace okami
