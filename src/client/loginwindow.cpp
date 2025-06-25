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

    // Auto-hide when connected and in-game, unless manually shown
    if (!IsVisible && !manuallyShown && Socket.isConnected() && !OnTitleScreen)
    {
        return;
    }

    if (!IsVisible)
    {
        return;
    }

    ImGui::Begin(name.c_str(), &IsVisible, ImGuiWindowFlags_AlwaysAutoResize);

    // Simple connection status
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
            Socket.clientConnect(this);
            setMessage("Connecting...");
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
            // TODO: Implement disconnect
            setMessage("Disconnect not implemented");
        }
    }

    // Simple message display with colors
    if (!message.empty())
    {
        ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default white

        if (message.find("Connected") != std::string::npos)
        {
            color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        }
        else if (message.find("refused") != std::string::npos || message.find("failed") != std::string::npos)
        {
            color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); // Red
        }
        else if (message.find("Connecting") != std::string::npos)
        {
            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        }

        ImGui::TextColored(color, "%s", message.c_str());
    }

    // Reset manual flag when window is closed and we should auto-hide
    if (!IsVisible && Socket.isConnected() && !OnTitleScreen)
    {
        manuallyShown = false;
    }

    ImGui::End();
#endif
}

void LoginWindow::setMessage(const std::string &newMessage)
{
    message = newMessage;
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
