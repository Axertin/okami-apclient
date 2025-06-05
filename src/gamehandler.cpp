#include "pch.h"
#include "gamehandler.h"
#include "core.h"
#include "memtable.h"

#pragma comment(lib, "vcpkg_installed/x64-windows/lib/minhook.x64.lib")

typedef void(__cdecl *FunctionType)();

FunctionType Main_FlowerTickOrigin = nullptr;
uint8_t *Main_FlowerTick;
void __cdecl GameHandler::onGameTick()
{
    printMemTable(getMemTable());
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
    Main_FlowerTick = (uint8_t *)(core::MainBase + 0x4B63B0);
    MH_CreateHook((LPVOID)Main_FlowerTick, &onGameTick, (LPVOID *)(&Main_FlowerTickOrigin));

    std::cout << "Hooks Initialized!" << std::endl;
    MH_EnableHook(MH_ALL_HOOKS);
}

void GameHandler::cleanup()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
