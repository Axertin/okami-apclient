// Static assertions to validate data struct members are exactly where they are
// expected. This verifies that things are defined correctly and that align
// directives act as expected.

#include <cstddef>

#include "okami/data/structs.hpp"

using namespace okami;

#define OFFCHECK(type, member, offset) static_assert(offsetof(type, member) == (offset), #type "." #member " at wrong offset, expected @ " #offset)

#define SIZECHECK(type, size) static_assert(sizeof(type) == (size), #type " is the wrong size, expected " #size)

#define CHARA_CHECK(member, offset) OFFCHECK(CharacterStats, member, offset)

SIZECHECK(CharacterStats, 0x48);
CHARA_CHECK(currentHealth, 0x00);
CHARA_CHECK(maxHealth, 0x02);
CHARA_CHECK(currentFood, 0x04);
CHARA_CHECK(maxFood, 0x06);
CHARA_CHECK(currentPraise, 0x0A);
CHARA_CHECK(totalPraise, 0x0C);
CHARA_CHECK(dojoTechniquesUnlocked, 0x10);
CHARA_CHECK(mainWeapon, 0x18);
CHARA_CHECK(subWeapon, 0x19);
CHARA_CHECK(godhood, 0x1C);
CHARA_CHECK(weaponsUpgraded, 0x20);
CHARA_CHECK(vengeanceSlipTimer, 0x28);
CHARA_CHECK(attackIncreaseTimer, 0x2A);
CHARA_CHECK(defenseIncreaseTimer, 0x2C);
CHARA_CHECK(x, 0x30);
CHARA_CHECK(y, 0x34);
CHARA_CHECK(z, 0x38);
CHARA_CHECK(u, 0x3C);
CHARA_CHECK(v, 0x40);
CHARA_CHECK(w, 0x44);

#define TRACK_CHECK(member, offset) OFFCHECK(TrackerData, member, offset)

SIZECHECK(TrackerData, 0x80);
TRACK_CHECK(firstTimeItem, 0x00);
TRACK_CHECK(gameProgressionBits, 0x20);
TRACK_CHECK(animalsFedFirstTime, 0x2C);
TRACK_CHECK(brushUpgrades, 0x3C);
TRACK_CHECK(gameOverCount, 0x40);
TRACK_CHECK(optionFlags, 0x44);
TRACK_CHECK(areasRestored, 0x48);
TRACK_CHECK(volumeBGM, 0x4C);
TRACK_CHECK(volumeSE, 0x4E);
TRACK_CHECK(volumeVoice, 0x50);
TRACK_CHECK(bestiaryTomeUnlocked, 0x54);
TRACK_CHECK(bestiaryTomeRead, 0x60);
TRACK_CHECK(areaVisitedFlags, 0x70);
TRACK_CHECK(timePlayed, 0x7C);

#define COLLECT_CHECK(member, offset) OFFCHECK(CollectionData, member, offset)

SIZECHECK(CollectionData, 0x11B0);
SIZECHECK(WorldStateData, 0xF50);
COLLECT_CHECK(numSaves, 0x00);
COLLECT_CHECK(currentMapId, 0x02);
COLLECT_CHECK(lastMapId, 0x04);
COLLECT_CHECK(currentInk, 0x08);
COLLECT_CHECK(maxInk, 0x0C);
COLLECT_CHECK(currentMoney, 0x10);
COLLECT_CHECK(walletUpgrades, 0x14);
COLLECT_CHECK(healthUpgrades, 0x15);
COLLECT_CHECK(strayBeadsCollected, 0x18);
COLLECT_CHECK(travelGuidesCollected, 0x28);
COLLECT_CHECK(travelGuidesViewed, 0x2C);
COLLECT_CHECK(dojoMovesCollected, 0x30);
COLLECT_CHECK(dojoMovesViewed, 0x34);
COLLECT_CHECK(fishTomesCollected, 0x38);
COLLECT_CHECK(fishTomesViewed, 0x40);
COLLECT_CHECK(animalTomesCollected, 0x48);
COLLECT_CHECK(animalTomesViewed, 0x4C);
COLLECT_CHECK(treasureTomesCollected, 0x50);
COLLECT_CHECK(treasureTomesViewed, 0x58);

COLLECT_CHECK(inventory, 0x60);

COLLECT_CHECK(world, 0x260);
COLLECT_CHECK(world.timeOfDay, 0x260 + 0x00);
COLLECT_CHECK(world.day, 0x260 + 0x04);
COLLECT_CHECK(world.usableBrushTechniques, 0x260 + 0x10);
COLLECT_CHECK(world.obtainedBrushTechniques, 0x260 + 0x18);
COLLECT_CHECK(world.brushUnknown, 0x260 + 0x20);
COLLECT_CHECK(world.riverOfHeavensRejuvenationBits, 0x260 + 0x60);
COLLECT_CHECK(world.goldDustsAcquired, 0x260 + 0x84);
COLLECT_CHECK(world.holyArtifactsEquipped, 0x260 + 0x88);
COLLECT_CHECK(world.mapStateBits, 0x260 + 0xFC);
COLLECT_CHECK(world.animalsFedBits, 0x260 + 0xB7C);
COLLECT_CHECK(world.numAnimalsFed, 0x260 + 0xB9C);
COLLECT_CHECK(world.wantedListsUnlocked, 0x260 + 0xBC4);
COLLECT_CHECK(world.bountiesSlain, 0x260 + 0xBC8);
COLLECT_CHECK(world.logbookViewed, 0x260 + 0xC0C);
COLLECT_CHECK(world.totalMoney, 0x260 + 0xF28);
COLLECT_CHECK(world.totalDemonFangs, 0x260 + 0xF2C);
COLLECT_CHECK(world.totalEnemiesKilled, 0x260 + 0xF30);

SIZECHECK(hx::Texture, 0x18);
SIZECHECK(CustomTextures, 0x10160);

#define MAP_CHECK(member, offset) OFFCHECK(MapState, member, offset)

SIZECHECK(MapState, 0xE4);
MAP_CHECK(user, 0x00);
MAP_CHECK(unburiedObjects, 0x80);
MAP_CHECK(collectedObjects, 0x8C);
MAP_CHECK(timeOfDay, 0x9C);
MAP_CHECK(areasRestored, 0xA0);
MAP_CHECK(npcHasMoreToSay, 0xC4);
