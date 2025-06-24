#include "devtools.h"

#include "gamehooks.h"
#include "gui.h"
#include "imgui.h"
#include "okami/data/itemtype.hpp"
#include "okami/data/maptype.hpp"
#include "okami/items.hpp"
#include "okami/memorymap.hpp"

void DevTools::toggleVisibility()
{
    IsVisible = !IsVisible;
}

/**
 * @brief Renders the Developer Tools window UI using ImGui.
 *
 * @param OuterWidth Width of the outer container (currently unused).
 * @param OuterHeight Height of the outer container (currently unused).
 * @param UIScale UI scaling factor (currently unused).
 */
void DevTools::draw(int OuterWidth, int OuterHeight, float UIScale)
{
    (void)OuterWidth;
    (void)OuterHeight;
    (void)UIScale;

    if (!IsVisible)
        return;

    Framer.update();

    ImGui::Begin(name.c_str(), &IsVisible, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("IGT: %d", okami::AmmyTracker->timePlayed);
    ImGui::Text("Frame Time: %.2f ms (%.2f FPS)", Framer.getFrameTimeMs(),
                Framer.getFPS());

    if (ImGui::CollapsingHeader("Ammy Stats", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Pos: (%.2f,%.2f,%.2f)", okami::AmmyPosX.get(),
                    okami::AmmyPosY.get(), okami::AmmyPosZ.get());
        ImGui::Text("Health: %d / %d", okami::AmmyStats->currentHealth,
                    okami::AmmyStats->maxHealth);
        ImGui::Text("Money: %d / %d", okami::AmmyCollections->currentMoney,
                    okami::AmmyCollections->world.totalMoney);
        ImGui::Text("Praise: %d / %d", okami::AmmyStats->currentPraise,
                    okami::AmmyStats->totalPraise);
        ImGui::Text("Ink: %d / %d", okami::AmmyCollections->currentInk,
                    okami::AmmyCollections->maxInk);
        ImGui::Text("Food: %d / %d", okami::AmmyStats->currentFood,
                    okami::AmmyStats->maxFood);
        ImGui::Text("Godhood: %d", okami::AmmyStats->godhood);
        ImGui::Text("Demon Fangs: %d",
                    okami::AmmyCollections->world.demonFangs);
    }

    if (ImGui::CollapsingHeader("CharacterStats"))
    {
        ImGui::Text("unk1: %d", okami::AmmyStats->unk1);
        ImGui::Text("padding1: %d", okami::AmmyStats->__padding1);
        ImGui::Text("padding2: %d", okami::AmmyStats->__padding2);
        ImGui::Text("padding3: %d", okami::AmmyStats->__padding3);
        if (ImGui::CollapsingHeader("Dojo Techniques"))
        {
            for (unsigned i = 0; i < 64; i++)
            {
                ImGui::Text("Move %u: %s", i,
                            okami::AmmyStats->dojoTechniquesUnlocked.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }
        ImGui::Text("unk2: %d", okami::AmmyStats->unk2);
        ImGui::Text("unk3: %d", okami::AmmyStats->unk3);
        ImGui::Text("unk4: %d", okami::AmmyStats->unk4);
        ImGui::Text("padding4: %d", okami::AmmyStats->__padding4);
        ImGui::Text("padding5: %d", okami::AmmyStats->__padding5);
        if (ImGui::CollapsingHeader("Weapons"))
        {
            for (unsigned i = 0; i < 64; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyStats->weaponsUnlocked.IsSet(i) ? "yes"
                                                                       : "no");
            }
        }

        ImGui::Text("unk5: %d", okami::AmmyStats->unk5);
        ImGui::Text("unk6: %d", okami::AmmyStats->unk6);
        ImGui::Text("unk7: %d", okami::AmmyStats->unk7);
        ImGui::Text("padding7: %d", okami::AmmyStats->__padding7);
    }

    if (ImGui::CollapsingHeader("Collections"))
    {
        ImGui::Text("numSaves: %d", okami::AmmyCollections->numSaves);
        ImGui::Text("unk1: %d", okami::AmmyCollections->unk1);
        ImGui::Text("unk2: %d", okami::AmmyCollections->unk2);
        ImGui::Text("walletUpgrades: %d",
                    okami::AmmyCollections->walletUpgrades);
        ImGui::Text("healthUpgrades: %d",
                    okami::AmmyCollections->healthUpgrades);
        ImGui::Text("unk3: %d", okami::AmmyCollections->unk3);
        ImGui::Text("unk4: %d", okami::AmmyCollections->unk4);
        if (ImGui::CollapsingHeader("Stray Beads"))
        {
            for (unsigned i = 0; i < 128; i++)
            {
                ImGui::Text("Bead %u: %s", i,
                            okami::AmmyCollections->strayBeadsCollected.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }
        if (ImGui::CollapsingHeader("Travel Guides Collected"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text(
                    "%u: %s", i,
                    okami::AmmyCollections->travelGuidesCollected.IsSet(i)
                        ? "yes"
                        : "no");
            }
        }
        if (ImGui::CollapsingHeader("Travel Guides Viewed"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyCollections->travelGuidesViewed.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }

        if (ImGui::CollapsingHeader("Dojo Moves Collected"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyCollections->dojoMovesCollected.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }

        if (ImGui::CollapsingHeader("Dojo Moves Viewed"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyCollections->dojoMovesViewed.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }

        if (ImGui::CollapsingHeader("Fish Tomes Collected"))
        {
            for (unsigned i = 0; i < 64; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyCollections->fishTomesCollected.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }

        if (ImGui::CollapsingHeader("Fish Tomes Viewed"))
        {
            for (unsigned i = 0; i < 64; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyCollections->fishTomesViewed.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }

        if (ImGui::CollapsingHeader("Animal Tomes Collected"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text(
                    "%u: %s", i,
                    okami::AmmyCollections->animalTomesCollected.IsSet(i)
                        ? "yes"
                        : "no");
            }
        }

        if (ImGui::CollapsingHeader("Animal Tomes Viewed"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyCollections->animalTomesViewed.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }

        if (ImGui::CollapsingHeader("Treasure Tomes Collected"))
        {
            for (unsigned i = 0; i < 64; i++)
            {
                ImGui::Text(
                    "%u: %s", i,
                    okami::AmmyCollections->treasureTomesCollected.IsSet(i)
                        ? "yes"
                        : "no");
            }
        }

        if (ImGui::CollapsingHeader("Treasure Tomes Viewed"))
        {
            for (unsigned i = 0; i < 64; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyCollections->treasureTomesViewed.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }

        if (ImGui::CollapsingHeader("Inventory"))
        {
            for (unsigned i = 0; i < okami::ItemTypes::NUM_ITEM_TYPES; i++)
            {
                ImGui::Text("%s: %u",
                            okami::ItemTable.contains(i)
                                ? okami::ItemTable.at(i).Name.c_str()
                                : "",
                            okami::AmmyCollections->inventory[i]);
            }
        }
    }

    if (ImGui::CollapsingHeader("World State"))
    {
        ImGui::Text("timeOfDay: %u", okami::AmmyCollections->world.timeOfDay);
        ImGui::Text("day: %u", okami::AmmyCollections->world.day);
        ImGui::Text("unk1: %u", okami::AmmyCollections->world.unk1);
        ImGui::Text("unk2: %u", okami::AmmyCollections->world.unk2);
        ImGui::Text("unk3: %u", okami::AmmyCollections->world.unk3);
        ImGui::Text("unk4: %u", okami::AmmyCollections->world.unk4);
        ImGui::Text("unk5: %u", okami::AmmyCollections->world.unk5[0]);
        ImGui::Text("unk6: %u", okami::AmmyCollections->world.unk6[0]);
        ImGui::Text("unk7: %u", okami::AmmyCollections->world.unk7);
        ImGui::Text("unk8: %u", okami::AmmyCollections->world.unk8);
        ImGui::Text("unk9: %u", okami::AmmyCollections->world.unk9);
        ImGui::Text("unk10: %u", okami::AmmyCollections->world.unk10);
        for (unsigned i = 0; i < 56; i++)
        {
            ImGui::Text("unk11[%u]: %u", i,
                        okami::AmmyCollections->world.unk11[i]);
        }
        if (ImGui::CollapsingHeader("Map state bits"))
        {
            for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES + 1; i++)
            {
                if (ImGui::CollapsingHeader(okami::MapTypes::GetName(i)))
                {
                    for (unsigned j = 0; j < 256; j++)
                    {
                        ImGui::Text(
                            "%u: %s", j,
                            okami::AmmyCollections->world.mapStateBits[i].IsSet(
                                j)
                                ? "yes"
                                : "no");
                    }
                }
            }
        }
        if (ImGui::CollapsingHeader("Animal fed bits"))
        {
            for (unsigned i = 0; i < 256; i++)
            {
                ImGui::Text(
                    "%u: %s", i,
                    okami::AmmyCollections->world.animalsFedBits.IsSet(i)
                        ? "yes"
                        : "no");
            }
        }
        if (ImGui::CollapsingHeader("Num animals fed"))
        {
            for (unsigned i = 0; i < 20; i++)
            {
                ImGui::Text("%u: %u", i,
                            okami::AmmyCollections->world.numAnimalsFed[i]);
            }
        }
        for (unsigned i = 0; i < 10; i++)
        {
            ImGui::Text("unk15[%u]: %u", i,
                        okami::AmmyCollections->world.unk15[i]);
        }
        ImGui::Text("unk16: %u", okami::AmmyCollections->world.unk16[0]);
        ImGui::Text("unk17[0]: %u", okami::AmmyCollections->world.unk17[0]);
        ImGui::Text("unk17[1]: %u", okami::AmmyCollections->world.unk17[1]);
        ImGui::Text("unk17[2]: %u", okami::AmmyCollections->world.unk17[2]);
        ImGui::Text("unk17[3]: %u", okami::AmmyCollections->world.unk17[3]);
        ImGui::Text("unk18: %u", okami::AmmyCollections->world.unk18);
        ImGui::Text("unk19: %u", okami::AmmyCollections->world.unk19);
        ImGui::Text("unk20: %u", okami::AmmyCollections->world.unk20);
        ImGui::Text("unk21[0]: %u", okami::AmmyCollections->world.unk21[0]);
        ImGui::Text("unk21[1]: %u", okami::AmmyCollections->world.unk21[1]);
        ImGui::Text("unk21[2]: %u", okami::AmmyCollections->world.unk21[2]);
        ImGui::Text("unk21[3]: %u", okami::AmmyCollections->world.unk21[3]);
        if (ImGui::CollapsingHeader("unk22"))
        {
            for (unsigned i = 0; i < 780; i++)
            {
                ImGui::Text("unk22[%u]: %u", i,
                            okami::AmmyCollections->world.unk22[i]);
            }
        }
        ImGui::Text("enemiesKilled: %u",
                    okami::AmmyCollections->world.enemiesKilled);
        if (ImGui::CollapsingHeader("unk24"))
        {
            for (unsigned i = 0; i < 28; i++)
            {
                ImGui::Text("unk24[%u]: %u", i,
                            okami::AmmyCollections->world.unk24[i]);
            }
        }
    }

    if (ImGui::CollapsingHeader("Tracker"))
    {
        if (ImGui::CollapsingHeader("first time item"))
        {
            for (unsigned i = 0; i < okami::ItemTypes::NUM_ITEM_TYPES; i++)
            {
                ImGui::Text("%s: %s",
                            okami::ItemTable.contains(i)
                                ? okami::ItemTable.at(i).Name.c_str()
                                : "",
                            okami::AmmyTracker->firstTimeItem.IsSet(i) ? "yes"
                                                                       : "no");
            }
        }
        if (ImGui::CollapsingHeader("logbook available"))
        {
            for (unsigned i = 0; i < 96; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyTracker->logbookAvailable.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }
        if (ImGui::CollapsingHeader("animal tome unlocked"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyTracker->animalTomeUnlocked.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }
        ImGui::Text("unk1[0]: %u", okami::AmmyTracker->unk1[0]);
        ImGui::Text("unk1[1]: %u", okami::AmmyTracker->unk1[1]);
        ImGui::Text("unk1[2]: %u", okami::AmmyTracker->unk1[2]);
        ImGui::Text("unk1[3]: %u", okami::AmmyTracker->unk1[3]);
        ImGui::Text("field_40: %u", okami::AmmyTracker->field_40);
        ImGui::Text("field_44: %u", okami::AmmyTracker->field_44);
        ImGui::Text("field_48: %u", okami::AmmyTracker->field_48);
        ImGui::Text("field_4C: %u", okami::AmmyTracker->field_4C);
        ImGui::Text("field_4E: %u", okami::AmmyTracker->field_4E);
        ImGui::Text("field_50: %u", okami::AmmyTracker->field_50);
        ImGui::Text("field_52: %u", okami::AmmyTracker->field_52);

        if (ImGui::CollapsingHeader("bestiary tome unlocked"))
        {
            for (unsigned i = 0; i < 96; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyTracker->bestiaryTomeUnlocked.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }
        if (ImGui::CollapsingHeader("bestiary tome read"))
        {
            for (unsigned i = 0; i < 96; i++)
            {
                ImGui::Text("%u: %s", i,
                            okami::AmmyTracker->bestiaryTomeRead.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }
        ImGui::Text("unk2: %u", okami::AmmyTracker->unk2);
        ImGui::Text("field_6D: %u", okami::AmmyTracker->field_6D);
        ImGui::Text("field_6E: %u", okami::AmmyTracker->field_6E);
        ImGui::Text("field_6F: %u", okami::AmmyTracker->field_6F);
        ImGui::Text("field_70[0]: %u", okami::AmmyTracker->field_70[0]);
        ImGui::Text("field_70[1]: %u", okami::AmmyTracker->field_70[1]);
        ImGui::Text("field_70[2]: %u", okami::AmmyTracker->field_70[2]);
    }

    // TODO MapBits
    // TODO IssunDialogBits

    if (ImGui::CollapsingHeader("Items"))
    {
        static int ItemID = 0;
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::InputInt("Item ID", &ItemID);
        ImGui::SameLine();
        if (ImGui::Button("Give"))
        {
            GameHooks::giveItem(ItemID, 1);
        }
    }

    if (ImGui::CollapsingHeader("Flags"))
    {
        ImGui::Text(
            "Brushes: 0x%X",
            okami::AmmyCollections->world.usableBrushTechniques.word(1));
        if (ImGui::CollapsingHeader("Dojo"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text("Move %u: %s", i,
                            okami::AmmyCollections->dojoMovesCollected.IsSet(i)
                                ? "yes"
                                : "no");
            }
        }
    }

    if (ImGui::CollapsingHeader("Maps"))
    {
        static int MapID = 0;
        ImGui::Text("External Map: %d (%s)", okami::ExteriorMapID.get(),
                    okami::decodeMapName(okami::ExteriorMapID.get()).c_str());
        ImGui::Text("Current Map: %d (%s)", okami::CurrentMapID.get(),
                    okami::decodeMapName(okami::CurrentMapID.get()).c_str());
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::InputInt("Map ID", &MapID);
        ImGui::SameLine();
        if (ImGui::Button("Teleport"))
        {
            okami::ExteriorMapID.set(MapID);
            okami::CurrentMapID.set(MapID);
            okami::LoadingZoneTrigger.set(0x2);
        }
    }

    ImGui::End();
}
