#include "warpwindow.h"

#include <algorithm>
#include <cstring>
#include <string>

#include <okami/maps.hpp>
#include <okami/warp.hpp>
#include <wolf_framework.hpp>

#include "gamestate_accessors.hpp"

#ifdef _WIN32
#include "imgui.h"
#endif

namespace
{
// Game Memory Constants (cross-platform)
constexpr uint8_t WARP_TRIGGER_FLAG = 0x02;

#ifdef _WIN32
// UI Layout Constants (Windows-only, used in ImGui code)
constexpr float COORD_STEP = 1.0f;
constexpr float COORD_FAST_STEP = 10.0f;
constexpr float FACING_STEP = 0.1f;
constexpr float FACING_FAST_STEP = 1.0f;
constexpr int INT_STEP = 1;
constexpr int INT_FAST_STEP = 16;
constexpr size_t LABEL_BUFFER_SIZE = 128;
constexpr int MAX_MAP_ID = 0xFFFF;
constexpr int MAX_JUMP_ID = 0xFF;
constexpr int MAX_FLIP_CAMERA = 0xFF;
#endif
} // anonymous namespace

// Static state
static bool g_visible = false;
static size_t g_selectedCategory = 0;
static size_t g_selectedPreset = 0;

// Manual coordinate entry (for custom warps)
static float g_manualX = 0.0f;
static float g_manualY = 0.0f;
static float g_manualZ = 0.0f;
static float g_manualFacing = 0.0f;
static int g_manualMapID = 0x0102;
static int g_manualJumpID = 0xFF;
static int g_manualFlipCamera = 0x00;

namespace warpwindow
{

// Forward declarations
static void renderCurrentMap();
static void renderPresetSelection();
static void renderManualCoordinates();
static void renderWarpButton();
static void executeWarp(const okami::WarpData &data);

void initialize()
{
    g_visible = false;
    g_selectedCategory = 0;
    g_selectedPreset = 0;

    wolf::addCommand("warps", []([[maybe_unused]] const std::vector<std::string> &args) { g_visible = true; }, "Enable the warps menu");
}

void shutdown()
{
}

bool isVisible()
{
    return g_visible;
}

void setVisible(bool visible)
{
    g_visible = visible;
}

void toggle()
{
    g_visible = !g_visible;
}

void draw()
{
#ifdef _WIN32
    if (!g_visible)
    {
        return;
    }

    ImGui::Begin("Map Warp", &g_visible, ImGuiWindowFlags_AlwaysAutoResize);

    renderCurrentMap();
    ImGui::Separator();
    renderPresetSelection();
    ImGui::Separator();
    renderWarpButton();
    ImGui::Separator();
    renderManualCoordinates();

    ImGui::End();
#endif
}

static void renderCurrentMap()
{
#ifdef _WIN32
    uint16_t currentMapId = apgame::collectionData->currentMapId;
    std::string mapName = okami::decodeMapName(currentMapId);

    ImGui::Text("Current: %s (0x%04X)", mapName.c_str(), currentMapId);
#endif
}

static void renderPresetSelection()
{
#ifdef _WIN32
    const auto &categories = okami::WarpPresets::AllCategories;

    // Category dropdown
    if (ImGui::BeginCombo("Category", categories[g_selectedCategory].name))
    {
        for (size_t i = 0; i < categories.size(); ++i)
        {
            bool isSelected = (i == g_selectedCategory);
            if (ImGui::Selectable(categories[i].name, isSelected))
            {
                g_selectedCategory = i;
                g_selectedPreset = 0; // Reset preset when category changes
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // Preset dropdown for selected category
    const auto *presets = categories[g_selectedCategory].presets;
    if (presets && !presets->empty())
    {
        if (g_selectedPreset >= presets->size())
        {
            g_selectedPreset = 0; // Reset to valid index
        }
        const char *currentPresetName = (*presets)[g_selectedPreset].name;

        if (ImGui::BeginCombo("Destination", currentPresetName))
        {
            for (size_t i = 0; i < presets->size(); ++i)
            {
                bool isSelected = (i == g_selectedPreset);
                const auto &preset = (*presets)[i];

                // Show preset name with map ID for reference
                char label[LABEL_BUFFER_SIZE];
                snprintf(label, sizeof(label), "%s (0x%04X)", preset.name, preset.data.mapID);

                if (ImGui::Selectable(label, isSelected))
                {
                    g_selectedPreset = i;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
#endif
}

static void renderManualCoordinates()
{
#ifdef _WIN32
    if (ImGui::CollapsingHeader("Manual Coordinates"))
    {
        ImGui::InputFloat("X", &g_manualX, COORD_STEP, COORD_FAST_STEP, "%.1f");
        ImGui::InputFloat("Y", &g_manualY, COORD_STEP, COORD_FAST_STEP, "%.1f");
        ImGui::InputFloat("Z", &g_manualZ, COORD_STEP, COORD_FAST_STEP, "%.1f");
        ImGui::InputFloat("Facing", &g_manualFacing, FACING_STEP, FACING_FAST_STEP, "%.3f");

        ImGui::InputInt("Map ID", &g_manualMapID, INT_STEP, INT_FAST_STEP, ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::InputInt("Jump ID", &g_manualJumpID, INT_STEP, INT_FAST_STEP);
        ImGui::InputInt("Flip Camera", &g_manualFlipCamera, INT_STEP, INT_FAST_STEP);

        // Clamp values to valid ranges
        g_manualMapID = std::clamp(g_manualMapID, 0, MAX_MAP_ID);
        g_manualJumpID = std::clamp(g_manualJumpID, 0, MAX_JUMP_ID);
        g_manualFlipCamera = std::clamp(g_manualFlipCamera, 0, MAX_FLIP_CAMERA);

        ImGui::Spacing();

        if (ImGui::Button("Warp (Manual)"))
        {
            okami::WarpData data = {g_manualX,
                                    g_manualY,
                                    g_manualZ,
                                    g_manualFacing,
                                    static_cast<uint16_t>(g_manualMapID),
                                    static_cast<uint8_t>(g_manualJumpID),
                                    static_cast<uint8_t>(g_manualFlipCamera),
                                    0};
            executeWarp(data);
        }

        // Button to copy selected preset to manual fields
        ImGui::SameLine();
        if (ImGui::Button("Copy From Preset"))
        {
            const auto &categories = okami::WarpPresets::AllCategories;
            const auto *presets = categories[g_selectedCategory].presets;
            if (presets && !presets->empty() && g_selectedPreset < presets->size())
            {
                const auto &preset = (*presets)[g_selectedPreset];
                g_manualX = preset.data.x;
                g_manualY = preset.data.y;
                g_manualZ = preset.data.z;
                g_manualFacing = preset.data.facingDirection;
                g_manualMapID = preset.data.mapID;
                g_manualJumpID = preset.data.jumpID;
                g_manualFlipCamera = preset.data.flipCamera;
            }
        }
    }
#endif
}

static void renderWarpButton()
{
#ifdef _WIN32
    const auto &categories = okami::WarpPresets::AllCategories;
    const auto *presets = categories[g_selectedCategory].presets;

    bool canWarp = presets && !presets->empty() && g_selectedPreset < presets->size();

    if (!canWarp)
    {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("Warp", ImVec2(200, 30)))
    {
        if (canWarp)
        {
            const auto &preset = (*presets)[g_selectedPreset];
            executeWarp(preset.data);
        }
    }

    if (!canWarp)
    {
        ImGui::EndDisabled();
    }
#endif
}

static void executeWarp(const okami::WarpData &data)
{
    // Write warp data to game memory using direct pointer access
    okami::WarpData *warpPtr = apgame::warpData.get_ptr();
    if (!warpPtr)
    {
        wolf::logError("[Warp] Failed to get warp data pointer");
        return;
    }

    uint8_t *flagsPtr = apgame::mapLoadFlags.get_ptr();
    if (!flagsPtr)
    {
        wolf::logError("[Warp] Failed to get map load flags pointer");
        return;
    }

    *warpPtr = data;

    // Set the trigger bit to initiate warp
    // Bit 1 (value 0x02) triggers the warp
    *flagsPtr |= WARP_TRIGGER_FLAG;

    wolf::logInfo("[Warp] Initiated warp to map 0x%04X at (%.1f, %.1f, %.1f)", data.mapID, data.x, data.y, data.z);
}

} // namespace warpwindow
