#include "loginwindow.h"

#include <algorithm>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>
#include <wolf_framework.hpp>

#include "isocket.h"
#include "version.h"

#ifdef _WIN32
#include <d3d11.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#endif

// Static state
static std::string WindowName = "Archipelago Client " + std::string(version::string());
static ISocket *g_socket = nullptr;

// Connection form data (kept as char arrays for ImGui)
static char g_server[128] = "archipelago.gg:38281";
static char g_slot[128] = "";
static char g_password[128] = "";

// Settings
static const std::string g_saveFilePath = "./connection.json";

template <size_t N> static void copyToBuffer(char (&buffer)[N], const std::string &source)
{
    size_t copyLen = std::min(source.length(), N - 1);
    std::copy(source.begin(), source.begin() + copyLen, buffer);
    buffer[copyLen] = '\0';
}

namespace loginwindow
{
// Forward declarations
static void renderConnectionForm();
static void renderConnectionStatus();
static void saveConnectionData();
static void loadConnectionData();

void initialize(ISocket &socket)
{
    g_socket = &socket;
    loadConnectionData();

    if (!wolf::setupSharedImGuiAllocators())
    {
        wolf::logError("Failed to setup mod ImGui context!");
        return;
    }

    WOLF_IMGUI_INIT_BACKEND();

    // Register with Wolf GUI system
    if (!wolf::registerGuiWindow(WindowName.c_str(), render, true))
    {
        wolf::logError("Failed to register ");
        return;
    }

    // Register cleanup handler
    wolf::registerCleanupHandler(
        []()
        {
            wolf::unregisterGuiWindow(WindowName.c_str());
            wolf::logInfo(": GUI cleanup complete");
        });
}

void shutdown()
{
    wolf::unregisterGuiWindow(WindowName.c_str());
    g_socket = nullptr;
}

void show()
{
    wolf::logDebug("Showing Login Window");
    wolf::setGuiWindowVisible(WindowName.c_str(), true);
}

void hide()
{
    wolf::logDebug("Hiding Login Window");
    wolf::setGuiWindowVisible(WindowName.c_str(), false);
}

void toggle()
{
    wolf::logDebug("Toggling Login Window Visibility");
    wolf::toggleGuiWindow(WindowName.c_str());
}

void render(int outerWidth, int outerHeight, float uiScale)
{
#ifdef _WIN32
    WOLF_IMGUI_BEGIN(outerWidth, outerHeight, uiScale);

    ImGui::Begin(WindowName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    renderConnectionStatus();
    ImGui::Separator();
    renderConnectionForm();

    ImGui::End();
    WOLF_IMGUI_END();
#endif
}

static void renderConnectionStatus()
{
#ifdef _WIN32
    if (!g_socket)
        return;

    // Connection status
    if (g_socket->isConnected())
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Disconnected");
    }

    // Display status message with colors
    std::string status = g_socket->getStatus();
    if (!status.empty())
    {
        ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default white

        if (status.find("Connected") != std::string::npos)
        {
            color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        }
        else if (status.find("refused") != std::string::npos || status.find("failed") != std::string::npos || status.find("missing") != std::string::npos ||
                 status.find("empty") != std::string::npos)
        {
            color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); // Red
        }
        else if (status.find("Connecting") != std::string::npos)
        {
            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        }
        ImGui::Text("%s", status.c_str());
    }

#endif
}

static void renderConnectionForm()
{
#ifdef _WIN32
    if (!g_socket)
        return;

    ImGui::InputText("Server", g_server, IM_ARRAYSIZE(g_server));
    ImGui::InputText("Slot Name", g_slot, IM_ARRAYSIZE(g_slot));
    ImGui::InputText("Password", g_password, IM_ARRAYSIZE(g_password), ImGuiInputTextFlags_Password);

    if (!g_socket->isConnected())
    {
        bool canConnect = strlen(g_server) > 0 && strlen(g_slot) > 0;

        if (!canConnect)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Connect"))
        {
            saveConnectionData();
            g_socket->connect(g_server, g_slot, g_password);
        }

        if (!canConnect)
        {
            ImGui::EndDisabled();
        }
    }
    else
    {
        if (ImGui::Button("Disconnect"))
        {
            g_socket->disconnect();
        }
    }
#endif
}

static void saveConnectionData()
{
    nlohmann::json jsonData;
    jsonData["Server"] = g_server;
    jsonData["Slot"] = g_slot;
    jsonData["Password"] = g_password;

    std::ofstream file(g_saveFilePath);
    if (file.is_open())
    {
        file << jsonData.dump(4);
        file.close();
    }
}

static void loadConnectionData()
{
    std::ifstream file(g_saveFilePath);
    if (!file.is_open())
    {
        wolf::logWarning("[Login] Failed to open file: %s", g_saveFilePath.c_str());
        // Set default values when file doesn't exist
        copyToBuffer(g_server, "archipelago.gg:38281");
        copyToBuffer(g_slot, "");
        copyToBuffer(g_password, "");
        return;
    }

    nlohmann::json jsonData;
    try
    {
        file >> jsonData;
    }
    catch (const std::exception &e)
    {
        wolf::logError("[Login] JSON parse failed: %s", e.what());
        // Set default values
        copyToBuffer(g_server, "archipelago.gg:38281");
        copyToBuffer(g_slot, "");
        copyToBuffer(g_password, "");
        return;
    }
    file.close();

    std::string serverStr = jsonData.value("Server", "archipelago.gg:");
    std::string slotStr = jsonData.value("Slot", "");
    std::string passwordStr = jsonData.value("Password", "");

    copyToBuffer(g_server, serverStr);
    copyToBuffer(g_slot, slotStr);
    copyToBuffer(g_password, passwordStr);
}

// Accessor functions
const char *getLoginServer()
{
    return g_server;
}

const char *getLoginSlot()
{
    return g_slot;
}

const char *getLoginPassword()
{
    return g_password;
}

} // namespace loginwindow
