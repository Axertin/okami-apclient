#include "loginwindow.h"

#include "gui.h"
#include "logger.h"
#include "okami/memorymap.hpp"
#ifdef _WIN32
#include "imgui.h"
#endif

void LoginWindow::checkIfShouldBeVisible()
{
    while (CheckingVisibility)
    {
        OnTitleScreen = (okami::ExteriorMapID.get() == 0xC00); // Title Screen

        for (int i = 0; i < 25 && CheckingVisibility; ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void LoginWindow::toggleVisibility()
{
    IsVisible = !IsVisible;
    if (IsVisible)
    {
        manuallyShown = true; // User explicitly opened it
    }
}

void LoginWindow::draw(int OuterWidth, int OuterHeight, float UIScale)
{
#ifdef _WIN32
    (void)OuterWidth;
    (void)OuterHeight;
    (void)UIScale;

    if (!IsVisible)
    {
        return;
    }

    ImGui::Begin(name.c_str(), &IsVisible, ImGuiWindowFlags_AlwaysAutoResize);

    // Connection status with color
    if (Socket.isConnected())
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Disconnected");
    }

    ImGui::Separator();

    ImGui::InputText("Server", Server, IM_ARRAYSIZE(Server));
    ImGui::InputText("Slot Name", Slot, IM_ARRAYSIZE(Slot));
    ImGui::InputText("Password", Password, IM_ARRAYSIZE(Password), ImGuiInputTextFlags_Password);

    if (!Socket.isConnected())
    {
        bool canConnect = strlen(Server) > 0 && strlen(Slot) > 0;

        if (!canConnect)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Connect"))
        {
            saveLoginData(JsonSavePath, Server, Slot, Password);
            // NEW: Use the updated interface
            Socket.connect(Server, Slot, Password);
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
            Socket.disconnect();
        }
    }

    // Display status message with colors
    std::string status = Socket.getStatus();
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

        ImGui::TextColored(color, "%s", status.c_str());
    }

    ImGui::End();
#endif
}

void LoginWindow::saveLoginData(const std::string &path, const std::string &oServer, const std::string &oSlot, const std::string &oPassword)
{
    nlohmann::json jsonData;
    jsonData["Server"] = oServer;
    jsonData["Slot"] = oSlot;
    jsonData["Password"] = oPassword;

    std::ofstream file(path);
    if (file.is_open())
    {
        file << jsonData.dump(4);
        file.close();
    }
}

bool LoginWindow::loadLoginData(const std::string &path, std::string &oServer, std::string &oSlot, std::string &oPassword)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        logWarning("[login] Failed to open file: %s", path.c_str());
        return false;
    }

    nlohmann::json jsonData;
    try
    {
        file >> jsonData;
    }
    catch (const std::exception &e)
    {
        logError("[login] JSON parse failed: %s", e.what());
        return false;
    }
    file.close();

    oServer = jsonData.value("Server", "");
    oSlot = jsonData.value("Slot", "");
    oPassword = jsonData.value("Password", "");

    return true;
}
