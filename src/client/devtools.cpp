#include "devtools.h"

#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <utility>

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

/**
 * TODO:
 * - Make map bits slimmer
 * - Implement other map info
 * - Integrate with known info table
 * - Figure out brush upgrades
 */

void DevTools::toggleVisibility()
{
    IsVisible = !IsVisible;
}

void drawStatPair(const char *name, int type, void *pCurrent, void *pTotal)
{
    ImGui::Text("%s:", name);
    ImGui::SameLine();
    ImGui::SetCursorPosX(100);
    std::string currentName = std::string("##") + name + "_current";
    ImGui::SetNextItemWidth(80);
    ImGui::InputScalar(currentName.c_str(), type, pCurrent);

    ImGui::SameLine();
    ImGui::Text("/");

    ImGui::SameLine();
    std::string totalName = std::string("##") + name + "_total";
    ImGui::SetNextItemWidth(80);
    ImGui::InputScalar(totalName.c_str(), type, pTotal);
}

void drawStat(const char *name, int type, void *pCurrent)
{
    ImGui::Text("%s:", name);
    ImGui::SameLine();
    ImGui::SetCursorPosX(100);
    std::string currentName = std::string("##") + name + "_current";
    ImGui::SetNextItemWidth(80);
    ImGui::InputScalar(currentName.c_str(), type, pCurrent);
}

template <unsigned int N> void checkboxBitField(const char *label, unsigned idx, okami::BitField<N> &bits)
{
    ImGui::CheckboxFlags(label, bits.GetIdxPtr(idx), bits.GetIdxMask(idx));
}

template <unsigned int N, class Fn> void checklistCols(const char *groupName, unsigned numCols, const Fn &pNameFn, okami::BitField<N> &bits)
{
    std::string name;
    std::string uniqueExt = std::string("##") + (groupName ? groupName : "") + pNameFn(0) + pNameFn(1);

    if (groupName ? ImGui::CollapsingHeader(groupName) : true)
    {
        ImGui::Indent(6);
        name = std::string("All##Btn") + uniqueExt;
        if (ImGui::Button(name.c_str()))
        {
            bits.SetAll();
        }
        ImGui::SameLine();
        name = std::string("None##Btn") + uniqueExt;
        if (ImGui::Button(name.c_str()))
        {
            bits.ClearAll();
        }

        name = std::string("TblId") + uniqueExt;
        ImGui::BeginTable(name.c_str(), numCols);

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
        ImGui::Indent(-6);
        if (groupName)
        {
            ImGui::Separator();
        }
    }
}

template <unsigned int N> void checklistColsUnnamed(const char *groupName, unsigned numCols, const char *basename, okami::BitField<N> &bits)
{
    std::string mem;
    auto NameFn = [&](unsigned id) -> const char *
    {
        mem = basename + std::to_string(id);
        return mem.c_str();
    };
    checklistCols(groupName, numCols, NameFn, bits);
}

template <unsigned int N, class Fn> void checklistColsTome(const char *groupName, const Fn &pNameFn, okami::BitField<N> &collected, okami::BitField<N> &viewed)
{
    std::string name;
    std::string uniqueExt = std::string("##") + groupName + pNameFn(0) + pNameFn(1);

    if (ImGui::CollapsingHeader(groupName))
    {
        ImGui::Indent(6);
        name = std::string("All##Btn") + uniqueExt;
        if (ImGui::Button(name.c_str()))
        {
            collected.SetAll();
            viewed.SetAll();
        }
        ImGui::SameLine();
        name = std::string("None##Btn") + uniqueExt;
        if (ImGui::Button(name.c_str()))
        {
            collected.ClearAll();
            viewed.ClearAll();
        }

        std::string tableName = std::string("TblId") + uniqueExt;
        ImGui::BeginTable(tableName.c_str(), 3);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
        ImGui::TableSetupColumn("Col.");
        ImGui::TableSetupColumn("Read");
        ImGui::TableHeadersRow();

        for (unsigned i = 0; i < N; i++)
        {
            ImGui::TableNextRow();
            const char *itemName = pNameFn(i);

            ImGui::TableNextColumn();
            ImGui::Text("%s", itemName);

            ImGui::TableNextColumn();
            std::string collectedName = std::string("##Collected") + itemName;
            checkboxBitField(collectedName.c_str(), i, collected);

            ImGui::TableNextColumn();
            std::string viewedName = std::string("##Viewed") + itemName;
            checkboxBitField(viewedName.c_str(), i, viewed);
        }

        ImGui::EndTable();
        ImGui::Indent(-6);
        ImGui::Separator();
    }
}

bool itemComboBox(const char *comboId, int *itemID)
{
    static bool itemListInitialized = false;
    static const char *itemList[okami::ItemTypes::NUM_ITEM_TYPES];
    if (!itemListInitialized)
    {
        itemListInitialized = true;
        for (unsigned i = 0; i < okami::ItemTypes::NUM_ITEM_TYPES; i++)
        {
            itemList[i] = okami::ItemTypes::GetName(i);
        }
    }

    return ImGui::Combo(comboId, itemID, itemList, okami::ItemTypes::NUM_ITEM_TYPES);
}

bool mapComboBox(const char *comboId, int *mapID)
{
    static bool itemListInitialized = false;

    static std::unordered_map<const char *, int> comboSelectionMap;
    static std::vector<std::pair<uint16_t, std::string>> mapIDIndex;
    static std::vector<const char *> itemList;
    if (!itemListInitialized)
    {
        itemListInitialized = true;
        std::copy(okami::MapNames.begin(), okami::MapNames.end(), std::back_inserter(mapIDIndex));
        for (auto &pair : mapIDIndex)
        {
            itemList.emplace_back(pair.second.c_str());
        }
    }

    if (ImGui::Combo(comboId, &comboSelectionMap[comboId], itemList.data(), itemList.size()))
    {
        *mapID = mapIDIndex[comboSelectionMap[comboId]].first;
        return true;
    }
    return false;
}

void drawInventory(const char *categoryName, const std::vector<uint16_t> &items)
{
    ImGui::SeparatorText(categoryName);

    std::string tblName = std::string("InventoryTbl") + categoryName;
    ImGui::BeginTable(tblName.c_str(), 4);
    for (auto &i : items)
    {
        ImGui::TableNextColumn();

        const char *itemName = okami::ItemTypes::GetName(i);
        ImGui::Text("%s:", itemName);

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(40);
        std::string currentName = std::string("##") + itemName + "_current";
        ImGui::InputScalar(currentName.c_str(), ImGuiDataType_U16, &okami::AmmyCollections->inventory[i]);
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

    if (ImGui::Button("CHEAT ME"))
    {
        // allow celestial brush use at start of game
        okami::AmmyCollections->world.mapStateBits[0].Clear(10);
        // skip a part of cutscene which causes some overlapping issun dialog hiccups
        okami::AmmyCollections->world.mapStateBits[okami::MapTypes::KamikiVillage].Set(10);
        okami::AmmyStats->dojoTechniquesUnlocked.SetAll();
        okami::AmmyUsableBrushes->SetAll();
        okami::AmmyObtainedBrushes->SetAll();
        okami::AmmyCollections->inventory[okami::ItemTypes::FeedbagFish] = 100;
        okami::AmmyCollections->inventory[okami::ItemTypes::FeedbagHerbs] = 100;
        okami::AmmyCollections->inventory[okami::ItemTypes::FeedbagMeat] = 100;
        okami::AmmyCollections->inventory[okami::ItemTypes::FeedbagSeeds] = 100;
        okami::AmmyCollections->inventory[okami::ItemTypes::MermaidCoin] = 100;
        okami::AmmyCollections->inventory[okami::ItemTypes::DemonFang] = 100;
        okami::AmmyCollections->inventory[okami::ItemTypes::GoldDust] = 100;
        okami::AmmyCollections->currentMoney = 999999999;
    }
    ImGui::SameLine();
    ImGui::Text("IGT: %d", okami::AmmyTracker->timePlayed);
    ImGui::Text("Frame Time: %.2f ms (%.2f FPS)", Framer.getFrameTimeMs(), Framer.getFPS());

    if (ImGui::CollapsingHeader("Ammy Stats", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent(6);
        ImGui::Text("Pos: (%.2f, %.2f, %.2f)", okami::AmmyPosX.get(), okami::AmmyPosY.get(), okami::AmmyPosZ.get());

        drawStatPair("Health", ImGuiDataType_U16, &okami::AmmyStats->currentHealth, &okami::AmmyStats->maxHealth);
        drawStatPair("Money", ImGuiDataType_U32, &okami::AmmyCollections->currentMoney, &okami::AmmyCollections->world.totalMoney);
        drawStatPair("Praise", ImGuiDataType_U16, &okami::AmmyStats->currentPraise, &okami::AmmyStats->totalPraise);
        drawStatPair("Ink", ImGuiDataType_U32, &okami::AmmyCollections->currentInk, &okami::AmmyCollections->maxInk);
        drawStatPair("Food", ImGuiDataType_U16, &okami::AmmyStats->currentFood, &okami::AmmyStats->maxFood);
        drawStat("Godhood", ImGuiDataType_U16, &okami::AmmyStats->godhood);
        drawStat("Demon Fangs", ImGuiDataType_U32, &okami::AmmyCollections->world.totalDemonFangs);
        ImGui::Indent(-6);
    }

    if (ImGui::CollapsingHeader("CharacterStats"))
    {
        ImGui::Indent(6);
        ImGui::Text("unk1: %d", okami::AmmyStats->unk1);
        ImGui::Text("padding1: %d", okami::AmmyStats->__padding1);

        checklistCols("Dojo Techniques", 2, okami::DojoTechs::GetName, okami::AmmyStats->dojoTechniquesUnlocked);

        ImGui::Text("unk1b: %d", okami::AmmyStats->unk1b);
        ImGui::Text("Main Weapon: %d", okami::AmmyStats->mainWeapon);
        ImGui::Text("Sub Weapon: %d", okami::AmmyStats->subWeapon);
        ImGui::Text("unk4: %d", okami::AmmyStats->unk4);
        ImGui::Text("padding4: %d", okami::AmmyStats->__padding4);
        ImGui::Text("padding5: %d", okami::AmmyStats->__padding5);

        checklistColsUnnamed("Weapons upgraded", 4, "Weapon", okami::AmmyStats->weaponsUpgraded);

        ImGui::Text("vengeanceSlipTimer: %d", okami::AmmyStats->vengeanceSlipTimer);
        ImGui::Text("attackIncreaseTimer: %d", okami::AmmyStats->attackIncreaseTimer);
        ImGui::Text("defenseIncreaseTimer: %d", okami::AmmyStats->defenseIncreaseTimer);
        ImGui::Text("padding7: %d", okami::AmmyStats->__padding7);
        ImGui::Indent(-6);
    }

    if (ImGui::CollapsingHeader("Collections"))
    {
        ImGui::Indent(6);
        ImGui::Text("numSaves: %d", okami::AmmyCollections->numSaves);
        ImGui::Text("unk1: %d", okami::AmmyCollections->unk1);
        ImGui::Text("unk2: %d", okami::AmmyCollections->unk2);
        ImGui::Text("walletUpgrades: %d", okami::AmmyCollections->walletUpgrades);
        ImGui::Text("healthUpgrades: %d", okami::AmmyCollections->healthUpgrades);
        ImGui::Text("unk3: %d", okami::AmmyCollections->unk3);
        ImGui::Text("unk4: %d", okami::AmmyCollections->unk4);

        checklistCols("Stray Beads", 5, okami::StrayBeads::GetName, okami::AmmyCollections->strayBeadsCollected);
        checklistColsTome("Travel Guides", okami::TravelGuides::GetName, okami::AmmyCollections->travelGuidesCollected,
                          okami::AmmyCollections->travelGuidesViewed);
        checklistColsTome("Move List", okami::MoveListTome::GetName, okami::AmmyCollections->dojoMovesCollected, okami::AmmyCollections->dojoMovesViewed);
        checklistColsTome("Fish Tome", okami::FishTome::GetName, okami::AmmyCollections->fishTomesCollected, okami::AmmyCollections->fishTomesViewed);
        checklistColsTome("Animal Tome", okami::Animals::GetName, okami::AmmyCollections->animalTomesCollected, okami::AmmyCollections->animalTomesViewed);
        checklistColsTome("Treasure Tome", okami::Treasures::GetName, okami::AmmyCollections->treasureTomesCollected,
                          okami::AmmyCollections->treasureTomesViewed);

        ImGui::Indent(-6);
    }

    if (ImGui::CollapsingHeader("World State"))
    {
        ImGui::Indent(6);
        drawStat("timeOfDay", ImGuiDataType_U32, &okami::AmmyCollections->world.timeOfDay);
        drawStat("day", ImGuiDataType_U16, &okami::AmmyCollections->world.day);
        ImGui::Text("unk1: %u", okami::AmmyCollections->world.unk1);
        ImGui::Text("unk2: %u", okami::AmmyCollections->world.unk2);
        ImGui::Text("unk3: %u", okami::AmmyCollections->world.unk3);
        ImGui::Text("unk4: %u", okami::AmmyCollections->world.unk4);

        checklistColsUnnamed("Key Items Obtained", 4, "KeyItem", okami::AmmyCollections->world.keyItemsAcquired);
        checklistColsUnnamed("Gold Dust Obtained", 4, "GoldDust", okami::AmmyCollections->world.goldDustsAcquired);

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
            ImGui::Indent(6);
            for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES + 1; i++)
            {
                std::string name = std::string("Map") + std::to_string(i) + "State";
                checklistColsUnnamed(okami::MapTypes::GetName(i), 4, name.c_str(), okami::AmmyCollections->world.mapStateBits[i]);
            }
            ImGui::Indent(-6);
        }
        checklistColsUnnamed("Animal fed bits", 4, "AnimalFed", okami::AmmyCollections->world.animalsFedBits);
        if (ImGui::CollapsingHeader("Num animals fed"))
        {
            for (unsigned i = 0; i < okami::Animals::NUM_ANIMALS; i++)
            {
                drawStat(okami::Animals::GetName(i), ImGuiDataType_U16, &okami::AmmyCollections->world.numAnimalsFed[i]);
            }
        }
        if (ImGui::CollapsingHeader("Wanted Lists"))
        {
            // checklistCols(nullptr, 2, [&](unsigned id) { return notebookNames[id]; }, okami::AmmyCollections->world.wantedListsUnlocked);
            for (int i = 0; i < 4; i++)
            {
                std::string name = std::string("##") + notebookNames[i];
                checkboxBitField(name.c_str(), i, okami::AmmyCollections->world.wantedListsUnlocked);
                ImGui::SameLine();
                ImGui::Indent(20);
                ImGui::SeparatorText(notebookNames[i]);
                ImGui::Indent(-20);

                checklistCols(nullptr, 2, [&](unsigned id) { return bountyNames[i][id]; }, okami::AmmyCollections->world.bountiesSlain[i]);
            }
            ImGui::Separator();
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

        checklistColsUnnamed("Logbook Viewed", 2, "LogbookViewed", okami::AmmyCollections->world.logbookViewed);
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
        ImGui::Indent(-6);
    }

    if (ImGui::CollapsingHeader("Tracker"))
    {
        ImGui::Indent(6);
        checklistCols("first time item", 2, okami::ItemTypes::GetName, okami::AmmyTracker->firstTimeItem);
        checklistColsUnnamed("logbook available", 3, "LogBookBit", okami::AmmyTracker->logbookAvailable);
        checklistColsUnnamed("animalsFedFirstTime", 2, "animalsFedFirstTime", okami::AmmyTracker->animalsFedFirstTime);
        checklistColsUnnamed("field_34", 2, "f34_", okami::AmmyTracker->field_34);
        checklistColsUnnamed("field_38", 2, "f38_", okami::AmmyTracker->field_38);
        checklistColsUnnamed("brushUpgrades", 2, "brushUpg", okami::AmmyTracker->brushUpgrades);
        ImGui::Text("field_40: %08X", okami::AmmyTracker->field_40);
        ImGui::Text("field_44: %08X", okami::AmmyTracker->field_44);

        checklistColsUnnamed("areasRestored", 2, "AreaRest", okami::AmmyTracker->areasRestored);
        ImGui::Text("field_4C: %04X", okami::AmmyTracker->field_4C);
        ImGui::Text("field_4E: %04X", okami::AmmyTracker->field_4E);
        ImGui::Text("field_50: %04X", okami::AmmyTracker->field_50);
        ImGui::Text("field_52: %04X", okami::AmmyTracker->field_52);

        checklistColsTome("Bestiary Tome", okami::BestiaryTome::GetName, okami::AmmyTracker->bestiaryTomeUnlocked, okami::AmmyTracker->bestiaryTomeRead);

        ImGui::Text("unk2: %02X", okami::AmmyTracker->unk2);
        ImGui::Text("field_6D: %02X", okami::AmmyTracker->field_6D);
        ImGui::Text("field_6E: %02X", okami::AmmyTracker->field_6E);
        ImGui::Text("field_6F: %02X", okami::AmmyTracker->field_6F);
        checklistCols("Maps Visited", 2, okami::MapTypes::GetName, okami::AmmyTracker->mapLocationsRevealed);
        ImGui::Indent(-6);
    }

    // TODO MapData

    if (ImGui::CollapsingHeader("Items"))
    {
        ImGui::Indent(6);

        static int ItemID = 0;

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        itemComboBox("Item ID", &ItemID);
        ImGui::SameLine();
        if (ImGui::Button("Give"))
        {
            GameHooks::giveItem(ItemID, 1);
        }

        if (ImGui::CollapsingHeader("Inventory"))
        {
            static std::unordered_map<okami::ItemCategory, std::vector<uint16_t>> itemsByCategory;
            if (itemsByCategory.empty())
            {
                for (auto &item : okami::ItemTable)
                {
                    itemsByCategory[item.second.Category].emplace_back(item.first);
                }
            }

            drawInventory("Consumables", itemsByCategory[okami::ItemCategory::Consumable]);
            drawInventory("Collectibles", itemsByCategory[okami::ItemCategory::Collectible]);
            drawInventory("Key Items", itemsByCategory[okami::ItemCategory::KeyItem]);
            drawInventory("Weapons", itemsByCategory[okami::ItemCategory::Weapon]);
            drawInventory("Karmic Transformer", itemsByCategory[okami::ItemCategory::KT]);
            drawInventory("Artifacts", itemsByCategory[okami::ItemCategory::Artifact]);
            drawInventory("Treasures", itemsByCategory[okami::ItemCategory::Treasure]);
            drawInventory("Fish", itemsByCategory[okami::ItemCategory::Fish]);
            drawInventory("Maps", itemsByCategory[okami::ItemCategory::Map]);
            drawInventory("Held Items", itemsByCategory[okami::ItemCategory::HeldItem]);
        }
        ImGui::Indent(-6);
    }

    if (ImGui::CollapsingHeader("Brushes"))
    {
        ImGui::Indent(6);
        checklistCols("Usable Brushes", 2, okami::BrushTypes::GetName, *okami::AmmyUsableBrushes.get_ptr());
        checklistCols("Obtained Brushes", 2, okami::BrushTypes::GetName, *okami::AmmyObtainedBrushes.get_ptr());
        if (ImGui::CollapsingHeader("Brush Unknown"))
        {
            for (int i = 0; i < 64; i++)
            {
                std::string name = std::string("BrushUnk") + std::to_string(i);
                drawStat(name.c_str(), ImGuiDataType_U8, &okami::AmmyCollections->world.brushUnknown[i]);
            }
        }
        ImGui::Indent(-6);
    }

    if (ImGui::CollapsingHeader("Maps"))
    {
        ImGui::Indent(6);
        static int MapID = okami::CurrentMapID.get();

        ImGui::Text("External Map: %d (%s)", okami::ExteriorMapID.get(), okami::decodeMapName(okami::ExteriorMapID.get()).c_str());
        ImGui::Text("Current Map: %d (%s)", okami::CurrentMapID.get(), okami::decodeMapName(okami::CurrentMapID.get()).c_str());

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        mapComboBox("Map ID", &MapID);
        ImGui::SameLine();
        int mapIndex = okami::MapTypes::FromMapId(MapID);
        ImGui::Text("%04X (%u)", MapID, mapIndex);
        ImGui::SameLine();
        if (ImGui::Button("Teleport"))
        {
            okami::ExteriorMapID.set(MapID);
            okami::CurrentMapID.set(MapID);
            okami::LoadingZoneTrigger.set(0x2);
        }

        int currentMapIndex = okami::MapTypes::FromMapId(okami::CurrentMapID.get());
        checklistColsUnnamed("Current Map World Bits", 4, "MapBit", okami::AmmyCollections->world.mapStateBits[currentMapIndex]);
        ImGui::Indent(-6);
    }

    ImGui::End();
}
