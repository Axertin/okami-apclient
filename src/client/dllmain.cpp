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
 *  @return success boolean
 */
inline bool initializeMain()
{
    logInfo("[apclient] Initializing main.dll module...");
    HMODULE hMainDll = GetModuleHandleW(L"main.dll");
    okami::MainBase = reinterpret_cast<uintptr_t>(hMainDll);
    if (okami::MainBase == 0)
    {
        logError("[apclient] Main.dll BaseAddress not found!");
        return false;
    }
    okami::initVariables();

    logInfo("[apclient] Module Addresses: main.dll->0x%p flower_kernel.dll->0x%p", okami::MainBase, okami::FlowerBase);

    return true;
}

void initializePrerequisites()
{
    initializeLogger();
    logInfo("[apclient] Initializing okami_apclient v%s (%s)", version::string(), version::hash());

    okami::RecompileItemGraphics();
    GameHooks::initialize();
}

void hook()
{
    if (!initializeMain())
        return;

    GameHooks::setup();
    guiInitHooks();

    APLocationMonitor::instance().initialize();
    APLocationMonitor::instance().setSocket(&ArchipelagoSocket::instance());
}

static void *(__fastcall *oGetSteamSyncObjectAddress)(void *arg);
static void *__fastcall onGetSteamSyncObjectAddress(void *arg)
{
    static bool calledOnce = false;
    if (!calledOnce)
    {
        calledOnce = true;
        hook();
    }
    return oGetSteamSyncObjectAddress(arg);
}

bool hookFlowerKernel()
{
    logInfo("[apclient] Initializing flower_kernel.dll module...");

    HMODULE hFlowerDll = GetModuleHandleW(L"flower_kernel.dll");
    okami::FlowerBase = reinterpret_cast<uintptr_t>(hFlowerDll);
    if (okami::FlowerBase == 0)
    {
        logError("[apclient] flower_kernel.dll BaseAddress not found!");
        return false;
    }

    // This can be just ANY function that gets called early
    LPVOID pTarget = reinterpret_cast<LPVOID>(GetProcAddress(hFlowerDll, "?GetSteamSyncObjectAddress@m2@@YAPEAVSyncObject@sync@1@XZ"));
    MH_CreateHook(pTarget, reinterpret_cast<LPVOID>(&onGetSteamSyncObjectAddress), reinterpret_cast<LPVOID *>(&oGetSteamSyncObjectAddress));
    MH_STATUS result = MH_EnableHook(pTarget);

    if (result != MH_OK)
    {
        logInfo("[apclient] Failed to hook early in flower_kernel...");
        return false;
    }
    return true;
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
        initializePrerequisites();
        hookFlowerKernel();
        break;
    case DLL_PROCESS_DETACH:
        unhook();
        break;
    }

    return TRUE;
}
