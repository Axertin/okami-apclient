#include "devtools.h"
#include "gui.h"
#include "okami/okami.hpp"
#include "gamehooks.h"

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
    ImGui::Text("IGT: %d", okami::IngameTimeFrames.get());
    ImGui::Text("Frame Time: %.2f ms (%.2f FPS)", Framer.getFrameTimeMs(), Framer.getFPS());
    if (ImGui::CollapsingHeader("Ammy Stats", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Pos: (%.2f,%.2f,%.2f)", okami::AmmyPosX.get(), okami::AmmyPosY.get(), okami::AmmyPosZ.get());
        ImGui::Text("Health: %d", okami::AmmyCurrentHealth.get());
        ImGui::Text("Money: %d", okami::AmmyCurrentMoney.get());
        ImGui::Text("Praise: %d", okami::AmmyCurrentPraise.get());
        ImGui::Text("Ink: %d", okami::AmmyCurrentInk.get());
        ImGui::Text("Food: %d", okami::AmmyCurrentFood.get());
        ImGui::Text("Demon Fangs: %d", okami::AmmyCurrentDemonFangs.get());
    }

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
        ImGui::Text("Brushes: %X %X %X %X", okami::AmmyUsableBrushTechniques.get().rawByte(0), okami::AmmyUsableBrushTechniques.get().rawByte(1), okami::AmmyUsableBrushTechniques.get().rawByte(2), okami::AmmyUsableBrushTechniques.get().rawByte(3));
        ImGui::Text("Dojo: %X %X %X %X", okami::AmmyUsableDojoTechniques.get().rawByte(0), okami::AmmyUsableDojoTechniques.get().rawByte(1), okami::AmmyUsableDojoTechniques.get().rawByte(2), okami::AmmyUsableDojoTechniques.get().rawByte(3));
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
    }

    ImGui::End();
}
