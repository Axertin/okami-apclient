// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "gamehooks.h"
#include "archipelagosocket.h"
#include "gui.h"
#include "checks.h"
#include "logger.h"
#include <okami_apclient-GitVersion.h>
#include "okami/memorymap.hpp"

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

/** @brief Find Base Addresses of both vanilla DLLs
 *  @param MainDllModuleHandle Handle of main.dll
 *  @param FlowerDllModuleHandle Handle of flower_kernel.dll
 *  @return success boolean
 */
inline bool initialize(void *MainDllModuleHandle, void *FlowerDllModuleHandle)
{
    logInfo("[apclient] Initializing Modules...");
    okami::MainBase = reinterpret_cast<uintptr_t>(MainDllModuleHandle);
    if (okami::MainBase == 0)
    {
        logError("[apclient] Main.dll BaseAddress not found!");
        return false;
    }
    okami::FlowerBase = reinterpret_cast<uintptr_t>(FlowerDllModuleHandle);
    if (okami::FlowerBase == 0)
    {
        logError("[apclient] flower_kernel.dll BaseAddress not found!");
        return false;
    }

    okami::initVariables();
    okami::initFunctions();

    logInfo("[apclient] Module Addresses: main.dll->0x%p flower_kernel.dll->0x%p", okami::MainBase, okami::FlowerBase);

    return true;
}

extern "C" __declspec(dllexport) void okamiAPClientInit()
{
    initializeLogger();
    logInfo("[apclient] Initializing okami_apclient v%s (%s)", okami_apclient::version_string(), okami_apclient::version_shorthash());

    initialize(GetModuleHandleW(L"main.dll"), GetModuleHandleW(L"flower_kernel.dll"));
    GameHooks::setup();
    guiInitHooks();
    checkInit();
}
