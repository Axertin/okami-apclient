#include "devtools.h"

#include "gamehooks.h"
#include "gui.h"
#include "imgui.h"
#include "okami/animals.hpp"
#include "okami/bestiarytome.hpp"
#include "okami/data/itemtype.hpp"
#include "okami/data/maptype.hpp"
#include "okami/dojotech.hpp"
#include "okami/fish.hpp"
#include "okami/items.hpp"
#include "okami/memorymap.hpp"
#include "okami/movelisttome.hpp"
#include "okami/straybeads.hpp"
#include "okami/travelguides.hpp"
#include "okami/treasures.hpp"

void DevTools::toggleVisibility()
{
    IsVisible = !IsVisible;
}

void drawStatPair(const char *name, int type, void *pCurrent, void *pTotal)
{
    ImGui::Text("%s:", name);
    ImGui::SameLine();
    std::string currentName = std::string("##") + name + "_current";
    ImGui::InputScalar(currentName.c_str(), type, pCurrent);

    ImGui::SameLine();
    ImGui::Text("/");

    ImGui::SameLine();
    std::string totalName = std::string("##") + name + "_total";
    ImGui::InputScalar(totalName.c_str(), type, pTotal);
}

void drawStat(const char *name, int type, void *pCurrent)
{
    ImGui::Text("%s:", name);
    ImGui::SameLine();
    std::string currentName = std::string("##") + name + "_current";
    ImGui::InputScalar(currentName.c_str(), type, pCurrent);
}

template <unsigned int N>
void checkboxBitField(const char *label, unsigned idx, okami::BitField<N> &bits)
{
    ImGui::CheckboxFlags(label, bits.GetIdxPtr(idx), bits.GetIdxMask(idx));
}

template <unsigned int N, class Fn>
void checklistCols(unsigned numCols, const Fn &pNameFn,
                   okami::BitField<N> &bits)
{
    if (ImGui::Button("All"))
    {
        bits.SetAll();
    }
    ImGui::SameLine();
    if (ImGui::Button("None"))
    {
        bits.ClearAll();
    }

    std::string tableName = std::string("TblId") + pNameFn(0) +
                            pNameFn(1); // should be unique enough?
    ImGui::BeginTable(tableName.c_str(), numCols);

    unsigned rows = (bits.count + numCols - 1) / numCols;
    for (unsigned i = 0; i < rows; i++)
    {
        for (unsigned j = 0; j < numCols; j++)
        {
            ImGui::TableNextColumn();
            unsigned index = i + j * rows;
            if (index < bits.count)
            {
                checkboxBitField(pNameFn(index), index, bits);
            }
            else
            {
                ImGui::Text("");
            }
        }
    }
    ImGui::EndTable();
}

template <unsigned int N>
void checklistColsUnnamed(unsigned numCols, const char *basename,
                          okami::BitField<N> &bits)
{
    std::string mem;
    auto NameFn = [&](unsigned id) -> const char *
    {
        mem = basename + std::to_string(id);
        return mem.c_str();
    };
    checklistCols(numCols, NameFn, bits);
}

template <unsigned int N, class Fn>
void checklistColsTome(const Fn &pNameFn, okami::BitField<N> &collected,
                       okami::BitField<N> &viewed)
{
    if (ImGui::Button("All##Btn"))
    {
        collected.SetAll();
        viewed.SetAll();
    }
    ImGui::SameLine();
    if (ImGui::Button("None##Btn"))
    {
        collected.ClearAll();
        viewed.ClearAll();
    }

    std::string tableName = std::string("TblId") + pNameFn(0) +
                            pNameFn(1); // should be unique enough?
    ImGui::BeginTable(tableName.c_str(), 3);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
    ImGui::TableSetupColumn("Col.");
    ImGui::TableSetupColumn("Seen");
    ImGui::TableHeadersRow();

    for (unsigned i = 0; i < N; i++)
    {
        ImGui::TableNextRow();
        const char *name = pNameFn(i);

        ImGui::TableNextColumn();
        ImGui::Text("%s", name);

        ImGui::TableNextColumn();
        std::string collectedName = std::string("##Collected") + name;
        checkboxBitField(collectedName.c_str(), i, collected);

        ImGui::TableNextColumn();
        std::string viewedName = std::string("##Viewed") + name;
        checkboxBitField(viewedName.c_str(), i, viewed);
    }

    ImGui::EndTable();
}

const char *getItemName(unsigned idx)
{
    static std::string mem;
    if (okami::ItemTable.contains(idx))
    {
        mem = okami::ItemTable.at(idx).Name;
    }
    {
        mem = std::string("Item") + std::to_string(idx);
    }
    return mem.c_str();
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

        drawStatPair("Health", ImGuiDataType_U16,
                     &okami::AmmyStats->currentHealth,
                     &okami::AmmyStats->maxHealth);
        drawStatPair("Money", ImGuiDataType_U32,
                     &okami::AmmyCollections->currentMoney,
                     &okami::AmmyCollections->world.totalMoney);
        drawStatPair("Praise", ImGuiDataType_U16,
                     &okami::AmmyStats->currentPraise,
                     &okami::AmmyStats->totalPraise);
        drawStatPair("Ink", ImGuiDataType_U32,
                     &okami::AmmyCollections->currentInk,
                     &okami::AmmyCollections->maxInk);
        drawStatPair("Food", ImGuiDataType_U16, &okami::AmmyStats->currentFood,
                     &okami::AmmyStats->maxFood);
        drawStat("Godhood", ImGuiDataType_U16, &okami::AmmyStats->godhood);
        drawStat("Demon Fangs", ImGuiDataType_U32,
                 &okami::AmmyCollections->world.demonFangs);
    }

    if (ImGui::CollapsingHeader("CharacterStats"))
    {
        ImGui::Text("unk1: %d", okami::AmmyStats->unk1);
        ImGui::Text("padding1: %d", okami::AmmyStats->__padding1);
        ImGui::Text("padding2: %d", okami::AmmyStats->__padding2);
        ImGui::Text("padding3: %d", okami::AmmyStats->__padding3);
        if (ImGui::CollapsingHeader("Dojo Techniques"))
        {
            checklistCols(2, okami::DojoTechs::GetName,
                          okami::AmmyStats->dojoTechniquesUnlocked);
        }
        ImGui::Text("unk1b: %d", okami::AmmyStats->unk1b);
        ImGui::Text("unk2: %d", okami::AmmyStats->unk2);
        ImGui::Text("unk3: %d", okami::AmmyStats->unk3);
        ImGui::Text("unk4: %d", okami::AmmyStats->unk4);
        ImGui::Text("padding4: %d", okami::AmmyStats->__padding4);
        ImGui::Text("padding5: %d", okami::AmmyStats->__padding5);
        if (ImGui::CollapsingHeader("Weapons??"))
        {
            checklistColsUnnamed(4, "Weapon",
                                 okami::AmmyStats->weaponsUnlocked);
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
            checklistCols(5, okami::StrayBeads::GetName,
                          okami::AmmyCollections->strayBeadsCollected);
        }
        if (ImGui::CollapsingHeader("Travel Guides"))
        {
            checklistColsTome(okami::TravelGuides::GetName,
                              okami::AmmyCollections->travelGuidesCollected,
                              okami::AmmyCollections->travelGuidesViewed);
        }

        if (ImGui::CollapsingHeader("Move List"))
        {
            checklistColsTome(okami::MoveListTome::GetName,
                              okami::AmmyCollections->dojoMovesCollected,
                              okami::AmmyCollections->dojoMovesViewed);
        }

        if (ImGui::CollapsingHeader("Fish Tome"))
        {
            checklistColsTome(okami::FishTome::GetName,
                              okami::AmmyCollections->fishTomesCollected,
                              okami::AmmyCollections->fishTomesViewed);
        }

        if (ImGui::CollapsingHeader("Animal Tome"))
        {
            checklistColsTome(okami::Animals::GetName,
                              okami::AmmyCollections->animalTomesCollected,
                              okami::AmmyCollections->animalTomesViewed);
        }

        if (ImGui::CollapsingHeader("Treasure Tome"))
        {
            checklistColsTome(okami::Treasures::GetName,
                              okami::AmmyCollections->treasureTomesCollected,
                              okami::AmmyCollections->treasureTomesViewed);
        }

        if (ImGui::CollapsingHeader("Inventory"))
        {
            ImGui::BeginTable("InventoryTbl", 2);
            for (unsigned i = 0; i < okami::ItemTypes::NUM_ITEM_TYPES; i++)
            {
                ImGui::TableNextColumn();
                drawStat(getItemName(i), ImGuiDataType_U16,
                         &okami::AmmyCollections->inventory[i]);
            }
            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("World State"))
    {
        drawStat("timeOfDay", ImGuiDataType_U32,
                 &okami::AmmyCollections->world.timeOfDay);
        drawStat("day", ImGuiDataType_U16, &okami::AmmyCollections->world.day);
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
        if (ImGui::CollapsingHeader("unk11"))
        {
            for (unsigned i = 0; i < 56; i++)
            {
                ImGui::Text("unk11[%u]: %u", i,
                            okami::AmmyCollections->world.unk11[i]);
            }
        }
        if (ImGui::CollapsingHeader("Map state bits"))
        {
            for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES + 1; i++)
            {
                if (ImGui::CollapsingHeader(okami::MapTypes::GetName(i)))
                {
                    std::string name =
                        std::string("Map") + std::to_string(i) + "State";
                    checklistColsUnnamed(
                        4, name.c_str(),
                        okami::AmmyCollections->world.mapStateBits[i]);
                }
            }
        }
        if (ImGui::CollapsingHeader("Animal fed bits"))
        {
            checklistColsUnnamed(4, "AnimalFed",
                                 okami::AmmyCollections->world.animalsFedBits);
        }
        if (ImGui::CollapsingHeader("Num animals fed"))
        {
            for (unsigned i = 0; i < okami::Animals::NUM_ANIMALS; i++)
            {
                drawStat(okami::Animals::GetName(i), ImGuiDataType_U16,
                         &okami::AmmyCollections->world.numAnimalsFed[i]);
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
            checklistCols(2, getItemName, okami::AmmyTracker->firstTimeItem);
        }
        if (ImGui::CollapsingHeader("logbook available"))
        {
            checklistColsUnnamed(2, "LogBookBit",
                                 okami::AmmyTracker->logbookAvailable);
        }
        if (ImGui::CollapsingHeader("unknown"))
        {
            checklistColsUnnamed(2, "unknown", okami::AmmyTracker->unknown);
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
            checklistCols(2, okami::BestiaryTome::GetName,
                          okami::AmmyTracker->bestiaryTomeUnlocked);
        }
        if (ImGui::CollapsingHeader("bestiary tome read"))
        {
            checklistCols(2, okami::BestiaryTome::GetName,
                          okami::AmmyTracker->bestiaryTomeRead);
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
        if (ImGui::CollapsingHeader("Usable Brushes"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text("Brush %u: %s", i,
                            okami::AmmyUsableBrushes->IsSet(i) ? "yes" : "no");
            }
        }

        if (ImGui::CollapsingHeader("Obtained Brushes"))
        {
            for (unsigned i = 0; i < 32; i++)
            {
                ImGui::Text("Brush %u: %s", i,
                            okami::AmmyObtainedBrushes->IsSet(i) ? "yes"
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

        int mapIndex = okami::MapTypes::FromMapId(okami::CurrentMapID.get());
        if (ImGui::CollapsingHeader("Current Map World Bits"))
        {
            checklistColsUnnamed(
                4, "MapWorldBit",
                okami::AmmyCollections->world.mapStateBits[mapIndex]);
        }
        if (ImGui::CollapsingHeader("Current Map Progress Bits"))
        {
            checklistColsUnnamed(4, "MapProgressBit",
                                 okami::MapBits->at(mapIndex));
        }
        if (ImGui::CollapsingHeader("Current Map Issun Bits"))
        {
            checklistColsUnnamed(4, "IssunBit",
                                 okami::IssunDialogBits->at(mapIndex));
        }
    }

    ImGui::End();
}
