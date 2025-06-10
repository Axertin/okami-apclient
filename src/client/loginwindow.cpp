#include "pch.h"
#include "gui.h"
#include "loginwindow.h"
#include "okami/okami.hpp"

/**
 * @brief Background thread that periodically checks if the login window should be visible.
 */
void LoginWindow::checkIfShouldBeVisible()
{
    while (CheckingVisibility)
    {
        OnTitleScreen = (okami::ExeriorMapID.get() == 0xC00); // Title Screen

        for (int i = 0; i < 25 && CheckingVisibility; ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return;
}

void LoginWindow::toggleVisibility()
{
    IsVisible = !IsVisible;
}

/**
 * @brief Renders the login window UI using ImGui.
 *
 * @param OuterWidth Width of the outer container (currently unused).
 * @param OuterHeight Height of the outer container (currently unused).
 * @param UIScale UI scaling factor (currently unused).
 */
void LoginWindow::draw(int OuterWidth, int OuterHeight, float UIScale)
{
    (void)OuterWidth;
    (void)OuterHeight;
    (void)UIScale;

    if (!IsVisible || !OnTitleScreen)
        return;

    // Draw login window
    ImGui::Begin(name.c_str(), &IsVisible, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::InputText("Server", Server, IM_ARRAYSIZE(Server));
    ImGui::InputText("Password", Password, IM_ARRAYSIZE(Password), ImGuiInputTextFlags_Password);
    ImGui::InputText("Slot Name", Slot, IM_ARRAYSIZE(Slot));

    if (!ArchipelagoSocket::Connected)
    {
        if (ImGui::Button("Connect"))
        {
            if (strlen(Server) > 0 && strlen(Slot) > 0)
            {
                saveLoginData(Server, Slot, Password);
                ArchipelagoSocket::clientConnect(this);
                setMessage("Connecting to " + std::string(Server) + "...");
            }
            else
            {
                setMessage("Please enter Server and Slot name");
            }
        }
    }
    // else
    // {
    //     if (ImGui::Button("Disconnect"))
    //     {
    //         ArchipelagoSocket::disconnect();
    //         SetMessage("");
    //     }
    // }

    ImGui::TextWrapped("%s", message.c_str());
    ImGui::End();
}

/**
 * @brief Sets the login window message
 *
 * @param newMessage The message string to display.
 */
void LoginWindow::setMessage(std::string newMessage)
{
    message = newMessage;
}

/**
 * @brief Saves login data (server, slot name, password) to a JSON file.
 *
 * @param oServer The server address.
 * @param oSlot The slot (player name).
 * @param oPassword The password used for authentication.
 */
void LoginWindow::saveLoginData(const std::string &oServer, const std::string &oSlot, const std::string &oPassword)
{
    nlohmann::json jsonData;
    jsonData["Server"] = oServer;
    jsonData["Slot"] = oSlot;
    jsonData["Password"] = oPassword;

    // Save the JSON data to a file
    std::ofstream file("./connection.json");
    file << jsonData.dump(4); // Pretty print with an indentation of 4
    file.close();
}

/**
 * @brief Loads login data (server, slot name, password) from a JSON file.
 *
 * @param oServer Reference to store the loaded server address.
 * @param oSlot Reference to store the loaded slot name.
 * @param oPassword Reference to store the loaded password.
 * @return true If data was loaded successfully.
 * @return false If the file was not found or loading failed.
 */
bool LoginWindow::loadLoginData(std::string &oServer, std::string &oSlot, std::string &oPassword)
{
    std::ifstream file("./connection.json");

    if (!file.is_open())
    {
        return false; // Return false if the file doesn't exist
    }

    nlohmann::json jsonData;
    file >> jsonData;
    file.close();

    // Retrieve data from the JSON object
    oServer = jsonData.value("Server", "");
    oSlot = jsonData.value("Slot", "");
    oPassword = jsonData.value("Password", "");

    return true; // Return true if loading was successful
}
