#include "bestiarytome.hpp"

namespace okami
{
namespace BestiaryTome
{
static const char *name[NUM_BESTIARY_ENTRIES] = {
    "Green Imp",
    "Red Imp",
    "Yellow Imp",
    "Blue Imp",
    "Black Imp",
    "Headless Guardian",
    "Bell Guardian",
    "Halo Guardian",
    "Executioner Guardian",
    "Namahage",
    "Blade Namahage",
    "Bucket Namahage",
    "Umbrella Namahage",
    "Cannon Namahage",
    "Clay Soldier",
    "Clay Samurai",
    "Clay Drummer",
    "Clay Flyer",
    "Clay Shogun",
    "Bud Ogre",
    "Chimera",
    "Igloo Turtle",
    "Dead Fish",
    "Crow Tengu",
    "Ubume",
    "Fire Eye",
    "Ice Mouth",
    "Thunder Ear",
    "Earth Nose",
    "Fire Doom Mirror",
    "Ice Doom Mirror",
    "Thunder Doom Mirror",
    "Wind Doom Mirror",
    "Poltergeist",
    "Ichiro",
    "Jiro",
    "Saburo",
    "Blue Cyclops",
    "Great Tengu",
    "Crimson Helm",
    "Red Ogre",
    "Blue Ogre",
    "Bull Charger",
    "Spider Queen",
    "Bandit Spider",
    "Orochi",
    "True Orochi",
    "Waka (w/ flute)",
    "Waka (w/ swords)",
    "Blight",
    "Evil Rao",
    "Oki",
    "Nagi",
    "Dogu",
    "Ninetails",
    "Canine Warrior Chu",
    "Canine Warrior Jin",
    "Canine Warrior Gi",
    "Canine Warrior Tei",
    "Canine Warrior Chi",
    "Canine Warrior Ko",
    "Canine Warrior Rei",
    "Canine Warrior Shin",
    "Tube Fox",
    "Golden Demon Lechku",
    "Silver Demon Nechku",
    "Yami (red)",
    "Yami (green)",
    "Yami (blue)",
    "Yami (yellow)",
    "Yami (hand)",
};

const char *GetName(unsigned index)
{
    if (index < NUM_BESTIARY_ENTRIES)
    {
        return name[index];
    }
    return "invalid";
}
} // namespace BestiaryTome
} // namespace okami