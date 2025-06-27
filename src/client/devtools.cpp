#include "devtools.h"

#include "gamehooks.h"
#include "gui.h"
#include "imgui.h"
#include "okami/animals.hpp"
#include "okami/bestiarytome.hpp"
#include "okami/data/brushtype.hpp"
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

template <unsigned int N> void checkboxBitField(const char *label, unsigned idx, okami::BitField<N> &bits)
{
    ImGui::CheckboxFlags(label, bits.GetIdxPtr(idx), bits.GetIdxMask(idx));
}

template <unsigned int N, class Fn> void checklistCols(unsigned numCols, const Fn &pNameFn, okami::BitField<N> &bits)
{
    if (ImGui::Button("All##Btn"))
    {
        bits.SetAll();
    }
    ImGui::SameLine();
    if (ImGui::Button("None##Btn"))
    {
        bits.ClearAll();
    }

    std::string tableName = std::string("TblId") + pNameFn(0) + pNameFn(1); // should be unique enough?
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

template <unsigned int N> void checklistColsUnnamed(unsigned numCols, const char *basename, okami::BitField<N> &bits)
{
    std::string mem;
    auto NameFn = [&](unsigned id) -> const char *
    {
        mem = basename + std::to_string(id);
        return mem.c_str();
    };
    checklistCols(numCols, NameFn, bits);
}

template <unsigned int N, class Fn> void checklistColsTome(const Fn &pNameFn, okami::BitField<N> &collected, okami::BitField<N> &viewed)
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

    std::string tableName = std::string("TblId") + pNameFn(0) + pNameFn(1); // should be unique enough?
    ImGui::BeginTable(tableName.c_str(), 3);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
    ImGui::TableSetupColumn("Col.");
    ImGui::TableSetupColumn("Read");
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

static std::array<const char *, 4> notebookNames = {"Mika's Monster Notebook", "Haruka's Revenge Contract", "Masu's Monster Manifest",
                                                    "Wali's Record of Penance"};

static std::array<std::array<const char *, 5>, 4> bountyNames = {
    std::array<const char *, 5>{"Onimaru the Incorrigible", "Bitwamaru the Vandal", "Akuzo the Interloper", "Izo the String Cutter",
                                "Toya of the Short Temper"},
    {"Bulging Eyes the Despised", "Weirdo the ABhorrent", "Death Fin the Repugnant", "Red Devil the Detested", "Curse Gill the Repulsive"},
    {"Dishonorable Tempest", "Foul Thunder", "Petulant Lightning", "Storm of Degradation", "Shame Flasher"},
    {"Creeping Igloo", "Snowy Stigma", "Stalking Blizzard", "Cold Remorse", "Frozen Penitence"},
};

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
    ImGui::Text("Frame Time: %.2f ms (%.2f FPS)", Framer.getFrameTimeMs(), Framer.getFPS());

    if (ImGui::CollapsingHeader("Ammy Stats", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Pos: (%.2f,%.2f,%.2f)", okami::AmmyPosX.get(), okami::AmmyPosY.get(), okami::AmmyPosZ.get());

        drawStatPair("Health", ImGuiDataType_U16, &okami::AmmyStats->currentHealth, &okami::AmmyStats->maxHealth);
        drawStatPair("Money", ImGuiDataType_U32, &okami::AmmyCollections->currentMoney, &okami::AmmyCollections->world.totalMoney);
        drawStatPair("Praise", ImGuiDataType_U16, &okami::AmmyStats->currentPraise, &okami::AmmyStats->totalPraise);
        drawStatPair("Ink", ImGuiDataType_U32, &okami::AmmyCollections->currentInk, &okami::AmmyCollections->maxInk);
        drawStatPair("Food", ImGuiDataType_U16, &okami::AmmyStats->currentFood, &okami::AmmyStats->maxFood);
        drawStat("Godhood", ImGuiDataType_U16, &okami::AmmyStats->godhood);
        drawStat("Demon Fangs", ImGuiDataType_U32, &okami::AmmyCollections->world.totalDemonFangs);
    }

    if (ImGui::CollapsingHeader("CharacterStats"))
    {
        ImGui::Text("unk1: %d", okami::AmmyStats->unk1);
        ImGui::Text("padding1: %d", okami::AmmyStats->__padding1);
        if (ImGui::CollapsingHeader("Dojo Techniques"))
        {
            checklistCols(2, okami::DojoTechs::GetName, okami::AmmyStats->dojoTechniquesUnlocked);
        }
        ImGui::Text("unk1b: %d", okami::AmmyStats->unk1b);
        ImGui::Text("Main Weapon: %d", okami::AmmyStats->mainWeapon);
        ImGui::Text("Sub Weapon: %d", okami::AmmyStats->subWeapon);
        ImGui::Text("unk4: %d", okami::AmmyStats->unk4);
        ImGui::Text("padding4: %d", okami::AmmyStats->__padding4);
        ImGui::Text("padding5: %d", okami::AmmyStats->__padding5);
        if (ImGui::CollapsingHeader("Weapons upgraded"))
        {
            checklistColsUnnamed(4, "Weapon", okami::AmmyStats->weaponsUpgraded);
        }

        ImGui::Text("vengeanceSlipTimer: %d", okami::AmmyStats->vengeanceSlipTimer);
        ImGui::Text("attackIncreaseTimer: %d", okami::AmmyStats->attackIncreaseTimer);
        ImGui::Text("defenseIncreaseTimer: %d", okami::AmmyStats->defenseIncreaseTimer);
        ImGui::Text("padding7: %d", okami::AmmyStats->__padding7);
    }

    if (ImGui::CollapsingHeader("Collections"))
    {
        ImGui::Text("numSaves: %d", okami::AmmyCollections->numSaves);
        ImGui::Text("unk1: %d", okami::AmmyCollections->unk1);
        ImGui::Text("unk2: %d", okami::AmmyCollections->unk2);
        ImGui::Text("walletUpgrades: %d", okami::AmmyCollections->walletUpgrades);
        ImGui::Text("healthUpgrades: %d", okami::AmmyCollections->healthUpgrades);
        ImGui::Text("unk3: %d", okami::AmmyCollections->unk3);
        ImGui::Text("unk4: %d", okami::AmmyCollections->unk4);
        if (ImGui::CollapsingHeader("Stray Beads"))
        {
            checklistCols(5, okami::StrayBeads::GetName, okami::AmmyCollections->strayBeadsCollected);
        }
        if (ImGui::CollapsingHeader("Travel Guides"))
        {
            checklistColsTome(okami::TravelGuides::GetName, okami::AmmyCollections->travelGuidesCollected, okami::AmmyCollections->travelGuidesViewed);
        }

        if (ImGui::CollapsingHeader("Move List"))
        {
            checklistColsTome(okami::MoveListTome::GetName, okami::AmmyCollections->dojoMovesCollected, okami::AmmyCollections->dojoMovesViewed);
        }

        if (ImGui::CollapsingHeader("Fish Tome"))
        {
            checklistColsTome(okami::FishTome::GetName, okami::AmmyCollections->fishTomesCollected, okami::AmmyCollections->fishTomesViewed);
        }

        if (ImGui::CollapsingHeader("Animal Tome"))
        {
            checklistColsTome(okami::Animals::GetName, okami::AmmyCollections->animalTomesCollected, okami::AmmyCollections->animalTomesViewed);
        }

        if (ImGui::CollapsingHeader("Treasure Tome"))
        {
            checklistColsTome(okami::Treasures::GetName, okami::AmmyCollections->treasureTomesCollected, okami::AmmyCollections->treasureTomesViewed);
        }

        if (ImGui::CollapsingHeader("Inventory"))
        {
            ImGui::BeginTable("InventoryTbl", 2);
            for (unsigned i = 0; i < okami::ItemTypes::NUM_ITEM_TYPES; i++)
            {
                ImGui::TableNextColumn();
                drawStat(okami::ItemTypes::GetName(i), ImGuiDataType_U16, &okami::AmmyCollections->inventory[i]);
            }
            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("World State"))
    {
        drawStat("timeOfDay", ImGuiDataType_U32, &okami::AmmyCollections->world.timeOfDay);
        drawStat("day", ImGuiDataType_U16, &okami::AmmyCollections->world.day);
        ImGui::Text("unk1: %u", okami::AmmyCollections->world.unk1);
        ImGui::Text("unk2: %u", okami::AmmyCollections->world.unk2);
        ImGui::Text("unk3: %u", okami::AmmyCollections->world.unk3);
        ImGui::Text("unk4: %u", okami::AmmyCollections->world.unk4);
        ImGui::Text("unk5: %u", okami::AmmyCollections->world.unk5);

        if (ImGui::CollapsingHeader("Gold Dust Obtained"))
        {
            checklistColsUnnamed(4, "GoldDust", okami::AmmyCollections->world.goldDustsAcquired);
        }

        for (int i = 0; i < 3; i++)
        {
            ImGui::Text("Holy Artifact %d: %s", i + 1, okami::ItemTypes::GetName(okami::AmmyCollections->world.holyArtifactsEquipped[i]));
        }
        ImGui::Text("unk10: %u", okami::AmmyCollections->world.unk10);
        if (ImGui::CollapsingHeader("unk11"))
        {
            for (unsigned i = 0; i < 56; i++)
            {
                ImGui::Text("unk11[%u]: %u", i, okami::AmmyCollections->world.unk11[i]);
            }
        }
        if (ImGui::CollapsingHeader("Map state bits"))
        {
            for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES + 1; i++)
            {
                if (ImGui::CollapsingHeader(okami::MapTypes::GetName(i)))
                {
                    std::string name = std::string("Map") + std::to_string(i) + "State";
                    checklistColsUnnamed(4, name.c_str(), okami::AmmyCollections->world.mapStateBits[i]);
                }
            }
        }
        if (ImGui::CollapsingHeader("Animal fed bits"))
        {
            checklistColsUnnamed(4, "AnimalFed", okami::AmmyCollections->world.animalsFedBits);
        }
        if (ImGui::CollapsingHeader("Num animals fed"))
        {
            for (unsigned i = 0; i < okami::Animals::NUM_ANIMALS; i++)
            {
                drawStat(okami::Animals::GetName(i), ImGuiDataType_U16, &okami::AmmyCollections->world.numAnimalsFed[i]);
            }
        }
        if (ImGui::CollapsingHeader("Wanted Lists"))
        {
            checklistCols(2, [&](unsigned id) { return notebookNames[id]; }, okami::AmmyCollections->world.wantedListsUnlocked);
            for (int i = 0; i < 4; i++)
            {
                if (ImGui::CollapsingHeader(notebookNames[i]))
                {
                    checklistCols(1, [&](unsigned id) { return bountyNames[i][id]; }, okami::AmmyCollections->world.bountiesSlain[i]);
                }
            }
        }
        for (unsigned i = 0; i < 5; i++)
        {
            ImGui::Text("unk15[%u]: %08X", i, okami::AmmyCollections->world.unk15[i]);
        }
        ImGui::Text("unk16: %08X", okami::AmmyCollections->world.unk16);
        ImGui::Text("unk17[0]: %08X", okami::AmmyCollections->world.unk17[0]);
        ImGui::Text("unk17[1]: %08X", okami::AmmyCollections->world.unk17[1]);
        ImGui::Text("unk17[2]: %08X", okami::AmmyCollections->world.unk17[2]);
        ImGui::Text("unk17[3]: %08X", okami::AmmyCollections->world.unk17[3]);
        ImGui::Text("unk18: %08X", okami::AmmyCollections->world.unk18);
        ImGui::Text("unk19: %08X", okami::AmmyCollections->world.unk19);
        ImGui::Text("unk20: %08X", okami::AmmyCollections->world.unk20);

        if (ImGui::CollapsingHeader("Logbook Viewed"))
        {
            checklistColsUnnamed(2, "LogbookViewed", okami::AmmyCollections->world.logbookViewed);
        }
        if (ImGui::CollapsingHeader("unk22"))
        {
            for (unsigned i = 0; i < 195; i++)
            {
                ImGui::Text("unk22[%u]: %08X", i, okami::AmmyCollections->world.unk22[i]);
            }
        }
        ImGui::Text("Enemies killed: %u", okami::AmmyCollections->world.totalEnemiesKilled);
        if (ImGui::CollapsingHeader("unk24"))
        {
            for (unsigned i = 0; i < 7; i++)
            {
                ImGui::Text("unk24[%u]: %08X", i, okami::AmmyCollections->world.unk24[i]);
            }
        }
    }

    if (ImGui::CollapsingHeader("Tracker"))
    {
        if (ImGui::CollapsingHeader("first time item"))
        {
            checklistCols(2, okami::ItemTypes::GetName, okami::AmmyTracker->firstTimeItem);
        }
        if (ImGui::CollapsingHeader("logbook available"))
        {
            checklistColsUnnamed(2, "LogBookBit", okami::AmmyTracker->logbookAvailable);
        }
        if (ImGui::CollapsingHeader("animalsFedFirstTime"))
        {
            checklistColsUnnamed(2, "animalsFedFirstTime", okami::AmmyTracker->animalsFedFirstTime);
        }
        ImGui::Text("unk1[0]: %08X", okami::AmmyTracker->unk1[0]);
        ImGui::Text("unk1[1]: %08X", okami::AmmyTracker->unk1[1]);
        ImGui::Text("unk1[2]: %08X", okami::AmmyTracker->unk1[2]);
        ImGui::Text("field_40: %08X", okami::AmmyTracker->field_40);
        ImGui::Text("field_44: %08X", okami::AmmyTracker->field_44);
        ImGui::Text("field_48: %08X", okami::AmmyTracker->field_48);
        ImGui::Text("field_4C: %04X", okami::AmmyTracker->field_4C);
        ImGui::Text("field_4E: %04X", okami::AmmyTracker->field_4E);
        ImGui::Text("field_50: %04X", okami::AmmyTracker->field_50);
        ImGui::Text("field_52: %04X", okami::AmmyTracker->field_52);

        if (ImGui::CollapsingHeader("Bestiary Tome"))
        {
            checklistColsTome(okami::BestiaryTome::GetName, okami::AmmyTracker->bestiaryTomeUnlocked, okami::AmmyTracker->bestiaryTomeRead);
        }
        ImGui::Text("unk2: %02X", okami::AmmyTracker->unk2);
        ImGui::Text("field_6D: %02X", okami::AmmyTracker->field_6D);
        ImGui::Text("field_6E: %02X", okami::AmmyTracker->field_6E);
        ImGui::Text("field_6F: %02X", okami::AmmyTracker->field_6F);
        ImGui::Text("field_70[0]: %08X", okami::AmmyTracker->field_70[0]);
        ImGui::Text("field_70[1]: %08X", okami::AmmyTracker->field_70[1]);
        if (ImGui::CollapsingHeader("Mirrors Unlocked"))
        {
            checklistColsUnnamed(2, "mirror", okami::AmmyTracker->mirrorsUnlocked);
        }
    }

    // TODO MapData

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
            checklistCols(2, okami::BrushTypes::GetName, *okami::AmmyUsableBrushes.get_ptr());
        }

        if (ImGui::CollapsingHeader("Obtained Brushes"))
        {
            checklistCols(2, okami::BrushTypes::GetName, *okami::AmmyObtainedBrushes.get_ptr());
        }
    }

    if (ImGui::CollapsingHeader("Maps"))
    {
        static int MapID = 0;
        ImGui::Text("External Map: %d (%s)", okami::ExteriorMapID.get(), okami::decodeMapName(okami::ExteriorMapID.get()).c_str());
        ImGui::Text("Current Map: %d (%s)", okami::CurrentMapID.get(), okami::decodeMapName(okami::CurrentMapID.get()).c_str());
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
            checklistColsUnnamed(4, "MapWorldBit", okami::AmmyCollections->world.mapStateBits[mapIndex]);
        }
        if (ImGui::CollapsingHeader("Current Map Progress Bits"))
        {
            // checklistColsUnnamed(4, "MapProgressBit", okami::MapData->at(mapIndex));
        }
    }

    ImGui::End();
}
