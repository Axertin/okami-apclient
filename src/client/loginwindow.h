#pragma once
#include "gui.h"
#include "archipelagosocket.h"
#include "window.h"

class LoginWindow : public Window
{
public:
    ~LoginWindow() override
    {
        CheckingVisibility = false;
        if (titleChecker.joinable())
        {
            titleChecker.join(); // wait for thread to exit cleanly
        }
    }
    LoginWindow() : Window("Login"), CheckingVisibility(true), OnTitleScreen(true)
    {
        IsVisible = true;
        std::string SavedServer, SavedSlot, SavedPassword;
        if (loadLoginData(JsonSavePath, SavedServer, SavedSlot, SavedPassword))
        {
            // If data was loaded successfully, pre-fill the fields
            strncpy_s(Server, SavedServer.c_str(), sizeof(Server));
            strncpy_s(Slot, SavedSlot.c_str(), sizeof(Slot));
            strncpy_s(Password, SavedPassword.c_str(), sizeof(Password));
        }
        titleChecker = std::thread(&LoginWindow::checkIfShouldBeVisible, this);
    }
    LoginWindow(bool Check) : Window("Login"), OnTitleScreen(true)
    {
        CheckingVisibility = Check;
        IsVisible = true;
    }
    void toggleVisibility() override;
    void draw(int OuterWidth, int OuterHeight, float UIScale) override;
    void setMessage(std::string);
    bool loadLoginData(const std::string &path, std::string &oServer, std::string &oSlot, std::string &oPassword);
    void saveLoginData(const std::string &path, const std::string &oServer, const std::string &oSlot, const std::string &oPassword);
    char Server[128] = "archipelago.gg:";
    char Password[128] = "";
    char Slot[128] = "";
    char ID[128] = "";
    char mPtr[128] = "";
    char mState[128] = "";

private:
    const std::string JsonSavePath = "./connection.json";
    std::thread titleChecker;
    std::string message;
    std::string mPointer;
    bool CheckingVisibility;
    std::atomic<bool> OnTitleScreen;
    void checkIfShouldBeVisible();
};
