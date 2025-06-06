#include "gamehandler.h"
#include "okami.h"

#pragma comment(lib, "vcpkg_installed/x64-windows/lib/minhook.x64.lib")

typedef void(__cdecl *FunctionType)();

// Startup Hook
FunctionType Main_FlowerStartupOrigin = nullptr;
void __cdecl GameHandler::onGameStart()
{
    std::cout << "Startup!" << std::endl;
    Main_FlowerStartupOrigin();
}

// Shutdown Hook
FunctionType Main_FlowerStopOrigin = nullptr;
void __cdecl GameHandler::onGameStop()
{
    std::cout << "Cleaning Up...";
    GameHandler::cleanup();
    std::cout << "Done!" << std::endl;
    Main_FlowerStopOrigin();
}

// MainTick Hook
FunctionType Main_FlowerTickOrigin = nullptr;
void __cdecl GameHandler::onGameTick()
{
    okami::printMonitors();
    Main_FlowerTickOrigin();
}

void GameHandler::setup()
{
    std::cout << "Initializing MinHook...";
    if (MH_Initialize() != MH_OK)
    {
        std::cout << "Failed!" << std::endl;
    }
    else
    {
        std::cout << "Done!" << std::endl;
    }

    MH_CreateHook(okami::MainFlowerStartupFunctionPtr, &onGameStart, (LPVOID *)(&Main_FlowerStartupOrigin));
    MH_CreateHook(okami::MainFlowerStopFunctionPtr, &onGameStop, (LPVOID *)(&Main_FlowerStopOrigin));
    MH_CreateHook(okami::MainFlowerTickFunctionPtr, &onGameTick, (LPVOID *)(&Main_FlowerTickOrigin));

    std::cout << "Hooks Initialized!" << std::endl;
    MH_EnableHook(MH_ALL_HOOKS);
}

void GameHandler::cleanup()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
