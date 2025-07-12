#include "devtools.h"

#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "gamehooks.h"
#include "gui.h"
#include "imgui.h"
#include "logger.h"
#include "okami/animals.hpp"
#include "okami/bestiarytome.hpp"
#include "okami/data/brushtype.hpp"
#include "okami/data/itemtype.hpp"
#include "okami/data/logbook.hpp"
#include "okami/data/maptype.hpp"
#include "okami/dojotech.hpp"
#include "okami/fish.hpp"
#include "okami/gamestateregistry.h"
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
#define CONCAT2(a, b) a##b
#define CONCAT(a, b) CONCAT2(a, b)
#define GROUP(...) if (auto CONCAT(__indent__, __LINE__) = IndentedGroup(__VA_ARGS__))

class IndentedGroup
{
  private:
    int amount;
    bool result = false;

  public:
    IndentedGroup(const char *name, ImGuiTreeNodeFlags flags = 0, int amt = 6) : amount(amt)
    {
        result = ImGui::CollapsingHeader(name, flags);
        ImGui::PushID(name);
        ImGui::Indent(amount);
    }
    ~IndentedGroup()
    {
        ImGui::PopID();
        ImGui::Indent(-amount);
    }
    operator bool()
    {
        return result;
    }
};

void DevTools::toggleVisibility()
{
    IsVisible = !IsVisible;
}

void drawStatPair(const char *name, int type, void *pCurrent, void *pTotal)
{
    ImGui::PushID(name);

    ImGui::Text("%s:", name);
    ImGui::SameLine();
    ImGui::SetCursorPosX(100);
    ImGui::SetNextItemWidth(80);
    ImGui::InputScalar("##current", type, pCurrent);

    ImGui::SameLine();
    ImGui::Text("/");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputScalar("##total", type, pTotal);

    ImGui::PopID();
}

void drawStat(const char *name, int type, void *pCurrent)
{
    ImGui::PushID(name);

    ImGui::Text("%s:", name);
    ImGui::SameLine();
    ImGui::SetCursorPosX(100);
    ImGui::SetNextItemWidth(80);
    ImGui::InputScalar("##current", type, pCurrent);

    ImGui::PopID();
}

template <unsigned int N> void checkboxBitField(const char *label, unsigned idx, okami::BitField<N> &bits)
{
    ImGui::CheckboxFlags(label, bits.GetIdxPtr(idx), bits.GetIdxMask(idx));
    ImGui::SetItemTooltip("%d (0x%X)", idx, idx);
}

template <unsigned int N> void checklistManyMapped(const char *groupName, const char *category, okami::MapTypes::Enum map, okami::BitField<N> &bits)
{
    GROUP(groupName)
    {
        auto &registry = okami::GameStateRegistry::instance();

        for (unsigned i = 0; i < N; i++)
        {
            ImGui::PushID(i);

            auto description = registry.getMapDescription(map, category, i);
            if (!description.empty())
            {
                checkboxBitField(description.data(), i, bits);
            }
            else
            {
                checkboxBitField("", i, bits);
                if ((i + 1) % 8 != 0)
                {
                    auto next_desc = registry.getMapDescription(map, category, i + 1);
                    if (next_desc.empty())
                    {
                        ImGui::SameLine();
                    }
                }
            }

            ImGui::PopID();
        }
    }
}

template <unsigned int N> void checklistManyMappedGlobal(const char *groupName, const char *category, okami::BitField<N> &bits)
{
    GROUP(groupName)
    {
        auto &registry = okami::GameStateRegistry::instance();

        for (unsigned i = 0; i < N; i++)
        {
            ImGui::PushID(i);

            auto description = registry.getGlobalDescription(category, i);
            if (!description.empty())
            {
                checkboxBitField(description.data(), i, bits);
            }
            else
            {
                checkboxBitField("", i, bits);
                if ((i + 1) % 8 != 0)
                {
                    auto next_desc = registry.getGlobalDescription(category, i + 1);
                    if (next_desc.empty())
                    {
                        ImGui::SameLine();
                    }
                }
            }

            ImGui::PopID();
        }
    }
}

template <unsigned int N, class Fn> void checklistCols(const char *groupName, unsigned numCols, const Fn &pNameFn, okami::BitField<N> &bits)
{
    ImGui::PushID(&bits);
    if (groupName ? ImGui::CollapsingHeader(groupName) : true)
    {
        ImGui::Indent(6);
        if (ImGui::Button("All##Btn"))
        {
            bits.SetAll();
        }
        ImGui::SameLine();
        if (ImGui::Button("None##Btn"))
        {
            bits.ClearAll();
        }

        ImGui::BeginTable("TblId", numCols);

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
    ImGui::PopID();
}

template <unsigned int N>
void checklistColsMapped(const char *groupName, unsigned numCols, const char *basename, okami::BitField<N> &bits,
                         const std::unordered_map<unsigned, std::string> &mapping)
{
    std::string mem;
    auto NameFn = [&](unsigned id) -> const char *
    {
        if (mapping.count(id))
        {
            return mapping.at(id).c_str();
        }
        mem = basename + std::to_string(id);
        return mem.c_str();
    };
    checklistCols(groupName, numCols, NameFn, bits);
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

    GROUP(groupName)
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

        ImGui::BeginTable("TblId", 3);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
        ImGui::TableSetupColumn("Col.");
        ImGui::TableSetupColumn("Read");
        ImGui::TableHeadersRow();

        for (unsigned i = 0; i < N; i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            const char *itemName = pNameFn(i);

            ImGui::TableNextColumn();
            ImGui::Text("%s", itemName);

            ImGui::TableNextColumn();
            checkboxBitField("##Collected", i, collected);

            ImGui::TableNextColumn();
            checkboxBitField("##Viewed", i, viewed);
            ImGui::PopID();
        }

        ImGui::EndTable();
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
    ImGui::PushID(categoryName);
    ImGui::SeparatorText(categoryName);

    ImGui::BeginTable("InventoryTbl", 2);
    for (auto &i : items)
    {
        ImGui::PushID(i);
        ImGui::TableNextColumn();

        int step = 1;
        ImGui::SetNextItemWidth(80);
        ImGui::InputScalar(okami::ItemTypes::GetName(i), ImGuiDataType_U16, &okami::AmmyCollections->inventory[i], &step);
        ImGui::PopID();
    }
    ImGui::EndTable();
    ImGui::PopID();
}

static const std::array<const char *, 4> notebookNames = {"Mika's Monster Notebook", "Haruka's Revenge Contract", "Masu's Monster Manifest",
                                                          "Wali's Record of Penance"};

static const std::array<const std::array<const char *, 5>, 4> bountyNames = {
    std::array<const char *, 5>{"Onimaru the Incorrigible", "Bitwamaru the Vandal", "Akuzo the Interloper", "Izo the String Cutter",
                                "Toya of the Short Temper"},
    {"Bulging Eyes the Despised", "Weirdo the ABhorrent", "Death Fin the Repugnant", "Red Devil the Detested", "Curse Gill the Repulsive"},
    {"Dishonorable Tempest", "Foul Thunder", "Petulant Lightning", "Storm of Degradation", "Shame Flasher"},
    {"Creeping Igloo", "Snowy Stigma", "Stalking Blizzard", "Cold Remorse", "Frozen Penitence"},
};

static const std::unordered_map<unsigned, std::string> weaponSlotNames = {
    {0x00, "Divine Retribution"}, {0x01, "Snarling Beast"},     {0x02, "Infinity Judge"}, {0x03, "Trinity Mirror"},
    {0x04, "Solar Flare"},        {0x10, "Tsumugari"},          {0x11, "Seven Strike"},   {0x12, "Blade of Kusanagi"},
    {0x13, "Eighth Wonder"},      {0x14, "Thunder Edge"},       {0x20, "Devout Beads"},   {0x21, "Life Beads"},
    {0x22, "Exorcism Beads"},     {0x23, "Resurrection Beads"}, {0x24, "Tundra Beads"},   {0xFF, "None"},
};

static const std::vector<std::pair<uint16_t, uint8_t>> weaponsList = {
    {okami::ItemTypes::DivineRetribution, 0x00}, {okami::ItemTypes::SnarlingBeast, 0x01},     {okami::ItemTypes::InfinityJudge, 0x02},
    {okami::ItemTypes::TrinityMirror, 0x03},     {okami::ItemTypes::SolarFlare, 0x04},        {okami::ItemTypes::Tsumugari, 0x10},
    {okami::ItemTypes::SevenStrike, 0x11},       {okami::ItemTypes::BladeOfKusanagi, 0x12},   {okami::ItemTypes::EighthWonder, 0x13},
    {okami::ItemTypes::ThunderEdge, 0x14},       {okami::ItemTypes::DevoutBeads, 0x20},       {okami::ItemTypes::LifeBeads, 0x21},
    {okami::ItemTypes::ExorcismBeads, 0x22},     {okami::ItemTypes::ResurrectionBeads, 0x23}, {okami::ItemTypes::TundraBeads, 0x24},
};

bool weaponComboBox(const char *comboId, int *weapon)
{
    static bool itemListInitialized = false;

    static std::vector<std::pair<unsigned, std::string>> mapIDIndex;
    static std::unordered_map<int, int> idToCombo;
    static std::vector<const char *> itemList;
    if (!itemListInitialized)
    {
        itemListInitialized = true;
        std::copy(weaponSlotNames.begin(), weaponSlotNames.end(), std::back_inserter(mapIDIndex));
        int i = 0;
        for (auto &pair : mapIDIndex)
        {
            itemList.emplace_back(pair.second.c_str());
            idToCombo[pair.first] = i++;
        }
    }

    int comboSelection = idToCombo[*weapon];
    if (ImGui::Combo(comboId, &comboSelection, itemList.data(), itemList.size()))
    {
        *weapon = mapIDIndex[comboSelection].first;
        return true;
    }
    return false;
}

void mapGroup(unsigned mapIdx)
{
    auto &registry = okami::GameStateRegistry::instance();
    okami::MapState &mapState = okami::MapData->at(mapIdx);
    auto mapEnum = static_cast<okami::MapTypes::Enum>(mapIdx);

    checklistManyMapped("Event Bits", "worldStateBits", mapEnum, okami::AmmyCollections->world.mapStateBits[mapIdx]);
    checklistManyMapped("Collected Objects", "collectedObjects", mapEnum, mapState.collectedObjects);
    checklistManyMapped("Areas Restored", "areasRestored", mapEnum, mapState.areasRestored);
    checklistManyMapped("Trees Bloomed", "treesBloomed", mapEnum, mapState.treesBloomed);
    checklistManyMapped("Cursed Trees Bloomed", "cursedTreesBloomed", mapEnum, mapState.cursedTreesBloomed);
    checklistManyMapped("Fights Cleared", "fightsCleared", mapEnum, mapState.fightsCleared);
    checklistManyMapped("Maps Explored", "mapsExplored", mapEnum, mapState.mapsExplored);
    checklistManyMapped("NPC Has More to Say", "npcs", mapEnum, mapState.npcHasMoreToSay);
    checklistManyMapped("NPC Unknown", "npcs", mapEnum, mapState.npcUnknown);

    GROUP("Custom Data")
    {
        for (unsigned i = 0; i < 32; i++)
        {
            ImGui::PushID(i);
            auto description = registry.getMapDescription(mapEnum, "userIndices", i);
            std::string name = !description.empty() ? std::string(description) : std::to_string(i);
            ImGui::Text("%08X", mapState.user[i]);
            ImGui::SameLine();
            ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &mapState.user[i]);
            ImGui::SetItemTooltip("%d (0x%X)", i, i);
            ImGui::PopID();
        }
    }

    checklistManyMapped("Unknown DC", "field_DC", mapEnum, mapState.field_DC);
    checklistManyMapped("Unknown E0", "field_E0", mapEnum, mapState.field_E0);
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

    static char markBuffer[256];
    static int markNum = 0;

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    ImGui::InputText("##Log Markpoint", markBuffer, sizeof(markBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Mark"))
    {
        if (strlen(markBuffer) > 0)
        {
            logInfo("%s", markBuffer);
            markBuffer[0] = '\0';
        }
        else
        {
            logInfo("User Markpoint %d", ++markNum);
        }
    }

    GROUP("Ammy Stats", ImGuiTreeNodeFlags_DefaultOpen)
    {
        ImGui::Text("Pos: (%.2f, %.2f, %.2f)", okami::AmmyPosX.get(), okami::AmmyPosY.get(), okami::AmmyPosZ.get());

        drawStatPair("Health", ImGuiDataType_U16, &okami::AmmyStats->currentHealth, &okami::AmmyStats->maxHealth);
        drawStatPair("Money", ImGuiDataType_U32, &okami::AmmyCollections->currentMoney, &okami::AmmyCollections->world.totalMoney);
        drawStatPair("Praise", ImGuiDataType_U16, &okami::AmmyStats->currentPraise, &okami::AmmyStats->totalPraise);
        drawStatPair("Ink", ImGuiDataType_U32, &okami::AmmyCollections->currentInk, &okami::AmmyCollections->maxInk);
        drawStatPair("Food", ImGuiDataType_U16, &okami::AmmyStats->currentFood, &okami::AmmyStats->maxFood);
        drawStat("Godhood", ImGuiDataType_U16, &okami::AmmyStats->godhood);
        drawStat("Demon Fangs", ImGuiDataType_U32, &okami::AmmyCollections->world.totalDemonFangs);
    }

    GROUP("CharacterStats")
    {
        ImGui::Text("unk1: %d", okami::AmmyStats->unk1);
        ImGui::Text("padding1: %d", okami::AmmyStats->__padding1);

        checklistCols("Dojo Techniques", 2, okami::DojoTechs::GetName, okami::AmmyStats->dojoTechniquesUnlocked);

        ImGui::Text("unk1b: %d", okami::AmmyStats->unk1b);

        drawStat("Transformation", ImGuiDataType_U8, &okami::AmmyStats->currentTransformation);
        ImGui::Text("padding4: %d", okami::AmmyStats->__padding4);
        ImGui::Text("padding5: %d", okami::AmmyStats->__padding5);

        ImGui::Text("vengeanceSlipTimer: %d", okami::AmmyStats->vengeanceSlipTimer);
        ImGui::Text("attackIncreaseTimer: %d", okami::AmmyStats->attackIncreaseTimer);
        ImGui::Text("defenseIncreaseTimer: %d", okami::AmmyStats->defenseIncreaseTimer);
        ImGui::Text("padding7: %d", okami::AmmyStats->__padding7);
    }

    GROUP("Collections")
    {
        ImGui::Text("numSaves: %d", okami::AmmyCollections->numSaves);
        ImGui::Text("currentMapId: %d", okami::AmmyCollections->currentMapId);
        ImGui::Text("lastMapId: %d", okami::AmmyCollections->lastMapId);
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
    }

    GROUP("World State")
    {
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
        GROUP("unk11")
        {
            for (unsigned i = 0; i < 56; i++)
            {
                ImGui::Text("unk11[%u]: %u", i, okami::AmmyCollections->world.unk11[i]);
            }
        }
        GROUP("Map state bits")
        {
            for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES + 1; i++)
            {
                ImGui::PushID(i);
                checklistColsUnnamed(okami::MapTypes::GetName(i), 4, "Bit", okami::AmmyCollections->world.mapStateBits[i]);
                ImGui::PopID();
            }
        }
        checklistColsUnnamed("Animal fed bits", 4, "AnimalFed", okami::AmmyCollections->world.animalsFedBits);
        GROUP("Num animals fed")
        {
            for (unsigned i = 0; i < okami::Animals::NUM_ANIMALS; i++)
            {
                drawStat(okami::Animals::GetName(i), ImGuiDataType_U16, &okami::AmmyCollections->world.numAnimalsFed[i]);
            }
        }
        GROUP("Wanted Lists")
        {
            // checklistCols(nullptr, 2, [&](unsigned id) { return notebookNames[id]; }, okami::AmmyCollections->world.wantedListsUnlocked);
            for (int i = 0; i < 4; i++)
            {
                ImGui::PushID(i);
                checkboxBitField("##Notebook", i, okami::AmmyCollections->world.wantedListsUnlocked);
                ImGui::SameLine();
                ImGui::Indent(20);
                ImGui::SeparatorText(notebookNames[i]);
                ImGui::Indent(-20);

                checklistCols(nullptr, 2, [&](unsigned id) { return bountyNames[i][id]; }, okami::AmmyCollections->world.bountiesSlain[i]);
                ImGui::PopID();
            }
            ImGui::Separator();
        }
        for (unsigned i = 0; i < 3; i++)
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

        checklistCols("Logbook Viewed", 2, okami::LogBook::GetName, okami::AmmyCollections->world.logbookViewed);
        GROUP("unk22")
        {
            for (unsigned i = 0; i < 194; i++)
            {
                ImGui::Text("unk22[%u]: %08X", i, okami::AmmyCollections->world.unk22[i]);
            }
        }
        ImGui::Text("Enemies killed: %u", okami::AmmyCollections->world.totalEnemiesKilled);
        GROUP("unk24")
        {
            for (unsigned i = 0; i < 7; i++)
            {
                ImGui::Text("unk24[%u]: %08X", i, okami::AmmyCollections->world.unk24[i]);
            }
        }
    }

    GROUP("Tracker")
    {
        checklistCols("first time item", 2, okami::ItemTypes::GetName, okami::AmmyTracker->firstTimeItem);
        checklistColsUnnamed("game progression", 4, "Prog", okami::AmmyTracker->gameProgressionBits);
        checklistColsUnnamed("animalsFedFirstTime", 2, "animalsFedFirstTime", okami::AmmyTracker->animalsFedFirstTime);
        checklistColsUnnamed("field_34", 2, "f34_", okami::AmmyTracker->field_34);
        checklistColsUnnamed("field_38", 2, "f38_", okami::AmmyTracker->field_38);
        checklistColsUnnamed("brushUpgrades", 2, "brushUpg", okami::AmmyTracker->brushUpgrades);
        ImGui::Text("gameOverCount: %d", okami::AmmyTracker->gameOverCount);
        checklistColsUnnamed("optionFlags", 2, "opt", okami::AmmyTracker->optionFlags);

        checklistColsUnnamed("areasRestored", 2, "AreaRest", okami::AmmyTracker->areasRestored);
        ImGui::Text("volumeBGM: %d", okami::AmmyTracker->volumeBGM);
        ImGui::Text("volumeSE: %d", okami::AmmyTracker->volumeSE);
        ImGui::Text("volumeVoice: %d", okami::AmmyTracker->volumeVoice);
        ImGui::Text("field_52: %04X", okami::AmmyTracker->field_52);

        checklistColsTome("Bestiary Tome", okami::BestiaryTome::GetName, okami::AmmyTracker->bestiaryTomeUnlocked, okami::AmmyTracker->bestiaryTomeRead);

        ImGui::Text("unk2: %02X", okami::AmmyTracker->unk2);
        ImGui::Text("field_6D: %02X", okami::AmmyTracker->field_6D);
        ImGui::Text("field_6E: %02X", okami::AmmyTracker->field_6E);
        ImGui::Text("field_6F: %02X", okami::AmmyTracker->field_6F);
        checklistCols("Maps Visited", 2, okami::MapTypes::GetName, okami::AmmyTracker->areaVisitedFlags);
    }

    // TODO MapData

    GROUP("Items")
    {
        static int ItemID = 0;

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        itemComboBox("Item ID", &ItemID);
        ImGui::SameLine();
        if (ImGui::Button("Give"))
        {
            GameHooks::giveItem(ItemID, 1);
        }

        static std::unordered_map<okami::ItemCategory, std::vector<uint16_t>> itemsByCategory;
        if (itemsByCategory.empty())
        {
            for (auto &item : okami::ItemTable)
            {
                itemsByCategory[item.second.Category].emplace_back(item.first);
            }
        }

        GROUP("Weapons")
        {
            int mainWpn = okami::AmmyStats->mainWeapon;
            if (weaponComboBox("Main Weapon", &mainWpn))
            {
                okami::AmmyStats->mainWeapon = mainWpn;
            }

            int subWpn = okami::AmmyStats->subWeapon;
            if (weaponComboBox("Sub Weapon", &subWpn))
            {
                okami::AmmyStats->subWeapon = subWpn;
            }

            // ----

            ImGui::BeginTable("TblIdWeapons", 3);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoHeaderLabel);
            ImGui::TableSetupColumn("Have");
            ImGui::TableSetupColumn("Gold");
            ImGui::TableHeadersRow();

            for (auto &wpn : weaponsList)
            {
                ImGui::PushID(wpn.first);
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text("%s", okami::ItemTypes::GetName(wpn.first));

                ImGui::TableNextColumn();
                bool hasWpn = okami::AmmyCollections->inventory[wpn.first] != 0;
                if (ImGui::Checkbox("##HasWpn", &hasWpn))
                {
                    okami::AmmyCollections->inventory[wpn.first] = hasWpn ? 1 : 0;
                }

                ImGui::TableNextColumn();
                bool upgWpn = (okami::AmmyStats->weaponsUpgraded & (1ull << wpn.second));
                if (ImGui::Checkbox("##UpgWpn", &upgWpn))
                {
                    if (upgWpn)
                    {
                        okami::AmmyStats->weaponsUpgraded |= (1ull << wpn.second);
                    }
                    else
                    {
                        okami::AmmyStats->weaponsUpgraded &= ~(1ull << wpn.second);
                    }
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        GROUP("Inventory")
        {
            drawInventory("Consumables", itemsByCategory[okami::ItemCategory::Consumable]);
            drawInventory("Collectibles", itemsByCategory[okami::ItemCategory::Collectible]);
            drawInventory("Key Items", itemsByCategory[okami::ItemCategory::KeyItem]);
            drawInventory("Karmic Transformer", itemsByCategory[okami::ItemCategory::KT]);
            drawInventory("Artifacts", itemsByCategory[okami::ItemCategory::Artifact]);
            drawInventory("Treasures", itemsByCategory[okami::ItemCategory::Treasure]);
            drawInventory("Fish", itemsByCategory[okami::ItemCategory::Fish]);
            drawInventory("Maps", itemsByCategory[okami::ItemCategory::Map]);
        }
    }

    GROUP("Brushes")
    {
        checklistCols("Usable Brushes", 2, okami::BrushTypes::GetName, *okami::AmmyUsableBrushes.get_ptr());
        checklistCols("Obtained Brushes", 2, okami::BrushTypes::GetName, *okami::AmmyObtainedBrushes.get_ptr());
        GROUP("Brush Unknown")
        {
            for (int i = 0; i < 64; i++)
            {
                std::string name = std::string("BrushUnk") + std::to_string(i);
                drawStat(name.c_str(), ImGuiDataType_U8, &okami::AmmyBrushUpgrades->data()[i]);
            }
        }
    }

    GROUP("Maps")
    {
        static int MapID = okami::CurrentMapID.get();

        ImGui::Text("External Map: %d (%s)", okami::ExteriorMapID.get(), okami::decodeMapName(okami::ExteriorMapID.get()).c_str());
        ImGui::Text("Current Map: %d (%s)", okami::CurrentMapID.get(), okami::decodeMapName(okami::CurrentMapID.get()).c_str());

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        mapComboBox("Map ID", &MapID);
        ImGui::SameLine();

        int mapIndex = okami::MapTypes::FromMapId(MapID);
        ImGui::Text("%04X (%u)", MapID, mapIndex);
        ImGui::SameLine();
        // TODO teleport to specific map entrance (maybe we can do entrance rando)
        if (ImGui::Button("Teleport"))
        {
            okami::ExteriorMapID.set(MapID);
            okami::CurrentMapID.set(MapID);
            okami::LoadingZoneTrigger.set(0x2);
        }

        checklistManyMappedGlobal("Global Event Bits", "commonStates", okami::AmmyCollections->world.mapStateBits[0]);

        GROUP("Current Map")
        {
            int currentMapIndex = okami::MapTypes::FromMapId(okami::CurrentMapID.get());
            mapGroup(currentMapIndex);
        }

        GROUP("All Maps")
        {
            for (unsigned i = 0; i < okami::MapTypes::NUM_MAP_TYPES; i++)
            {
                GROUP(okami::MapTypes::GetName(i))
                {
                    mapGroup(i);
                }
            }
        }
    }

    ImGui::End();
}
