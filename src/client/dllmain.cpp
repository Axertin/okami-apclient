// dllmain.cpp : Defines the entry point for the DLL application.
#include "aplocationmonitor.h"
#include "archipelagosocket.h" // include the socket first, before any windows headers / minhook!
#include "checks.h"
#include "framework.h"
#include "gamehooks.h"
#include "gui.h"
#include "logger.h"
#include "okami/memorymap.hpp"
#include "version.h"

#include <MinHook.h>

BOOL APIENTRY DllMain([[maybe_unused]] HMODULE hModule, DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
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

extern "C" __declspec(dllexport) LRESULT CALLBACK entry(int nCode, WPARAM wParam, LPARAM lParam)
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;

        initializeLogger();
        logInfo("[apclient] Initializing okami_apclient v%s (%s)", version::string(), version::hash());

        if (!initialize(GetModuleHandleW(L"main.dll"), GetModuleHandleW(L"flower_kernel.dll")))
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        GameHooks::setup();
        guiInitHooks();
        checkInit();

        APLocationMonitor::instance().initialize();
        APLocationMonitor::instance().setSocket(&ArchipelagoSocket::instance());
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
