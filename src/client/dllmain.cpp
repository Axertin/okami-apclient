// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "gamehooks.h"
#include "archipelago.h"
#include "gui.h"
#include <okami_apclient-GitVersion.h>
#include "okami/okami.hpp"

BOOL APIENTRY DllMain([[maybe_unused]] HMODULE hModule,
                      DWORD ul_reason_for_call,
                      [[maybe_unused]] LPVOID lpReserved)
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        guiCleanup();
        break;
    }

    return TRUE;
}

extern "C" __declspec(dllexport) void okamiAPClientInit()
{
    std::cout << "[apclient] Initializing okami_apclient v" << okami_apclient::version_string() << "( " << okami_apclient::version_shorthash() << ")" << std::endl;

    okami::initialize(GetModuleHandleW(L"main.dll"), GetModuleHandleW(L"flower_kernel.dll"));
    GameHooks::setup();
    guiInitHooks();
}
