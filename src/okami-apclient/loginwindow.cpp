#include "loginwindow.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>
#include <wolf_framework.hpp>

#include "isocket.h"
#include "version.h"

#ifdef _WIN32
#include "imgui.h"
#endif

// Static state
static std::string WindowName = "Archipelago Client " + std::string(version::string());
static ISocket *g_socket = nullptr;
static bool g_visible = true;

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
}

void shutdown()
{
    g_socket = nullptr;
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

    ImGui::Begin(WindowName.c_str(), &g_visible, ImGuiWindowFlags_AlwaysAutoResize);

    renderConnectionStatus();
    ImGui::Separator();
    renderConnectionForm();

    ImGui::End();
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
