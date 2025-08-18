// dllmain.cpp : Defines the entry point for the DLL application.
#include "aplocationmonitor.h"
#include "archipelagosocket.h" // include the socket first, before any windows headers / minhook!
#include "framework.h"
#include "gamehooks.h"
#include "gui.h"
#include "logger.h"
#include "okami/memorymap.hpp"
#include "okami/resource.h"
#include "version.h"

#include <MinHook.h>

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
    okami::RecompileItemGraphics();

    logInfo("[apclient] Module Addresses: main.dll->0x%p flower_kernel.dll->0x%p", okami::MainBase, okami::FlowerBase);

    return true;
}

void hook()
{
    static bool initialized = false;
    if (initialized)
        return;

    initialized = true;

    initializeLogger();
    logInfo("[apclient] Initializing okami_apclient v%s (%s)", version::string(), version::hash());

    if (!initialize(GetModuleHandleW(L"main.dll"), GetModuleHandleW(L"flower_kernel.dll")))
        return;

    GameHooks::setup();
    guiInitHooks();

    APLocationMonitor::instance().initialize();
    APLocationMonitor::instance().setSocket(&ArchipelagoSocket::instance());
}

void unhook()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    guiCleanup();
}

BOOL APIENTRY DllMain([[maybe_unused]] HMODULE hModule, DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        hook();
        break;
    case DLL_PROCESS_DETACH:
        unhook();
        break;
    }

    return TRUE;
}
