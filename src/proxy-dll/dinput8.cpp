// Mostly generated and then modified to work with non-VS, see README
#include <Windows.h>

#include <iostream>
#include <string>

#include "modloader.h"

#include <MinHook.h>

static struct dinput8_dll
{
    HMODULE dll;
    FARPROC OriginalDirectInput8Create;
    FARPROC OriginalDllCanUnloadNow;
    FARPROC OriginalDllGetClassObject;
    FARPROC OriginalDllRegisterServer;
    FARPROC OriginalDllUnregisterServer;
    FARPROC OriginalGetdfDIJoystick;
} dinput8;

/*
IMPORTANT:
  Originally used __asm tags and jmp, which is not cross-compiler compatible.
  Currently relies on whatever the compiler does, if it doesn't only generate a jmp instruction it can break.
*/
void __stdcall FakeDirectInput8Create()
{
    dinput8.OriginalDirectInput8Create();
}
void __stdcall FakeDllCanUnloadNow()
{
    dinput8.OriginalDllCanUnloadNow();
}
void __stdcall FakeDllGetClassObject()
{
    dinput8.OriginalDllGetClassObject();
}
void __stdcall FakeDllRegisterServer()
{
    dinput8.OriginalDllRegisterServer();
}
void __stdcall FakeDllUnregisterServer()
{
    dinput8.OriginalDllUnregisterServer();
}
void __stdcall FakeGetdfDIJoystick()
{
    dinput8.OriginalGetdfDIJoystick();
}

void error(const std::string &msg)
{
    MessageBox(nullptr, msg.c_str(), "Error", MB_ICONERROR);
    std::cerr << msg << std::endl;
}

static bool(__fastcall *pOriginalFlowerStartup)(bool started);
bool __fastcall OverrideFlowerStartup(bool started)
{
    if (!started)
    {
        LoadMods();
    }
    return pOriginalFlowerStartup(started);
}

static decltype(CreateWindowExW) *pOriginalCreateWindowExW;
HWND WINAPI OverrideCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        if (MH_OK != MH_CreateHookApi(L"main.dll", "?flower_startup@@YA_N_N@Z", reinterpret_cast<LPVOID>(&OverrideFlowerStartup),
                                      reinterpret_cast<LPVOID *>(&pOriginalFlowerStartup)))
        {
            error("Failed to hook flower_startup");
        }
        MH_EnableHook(MH_ALL_HOOKS);
    }

    return pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

void InitiateMainHook()
{
    if (!IsModded())
        return;

    MH_Initialize();

    // dinput8 gets loaded before main.dll so we hook something else and wait for that to be called
    if (MH_OK != MH_CreateHookApi(L"User32.dll", "CreateWindowExW", reinterpret_cast<LPVOID>(&OverrideCreateWindowExW),
                                  reinterpret_cast<LPVOID *>(&pOriginalCreateWindowExW)))
    {
        error("Failed to hook CreateWindowExW");
    }
    MH_EnableHook(MH_ALL_HOOKS);
}

void DestroyHooks()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

void LoadOriginalLibrary()
{
    char path[MAX_PATH * 2];
    GetSystemDirectoryA(path, MAX_PATH);
    strcat(path, "\\dinput8.dll");

    dinput8.dll = LoadLibraryA(path);
    if (dinput8.dll == false)
    {
        MessageBoxA(0, "Cannot load original dinput8.dll library", "Proxy", MB_ICONERROR);
        ExitProcess(0);
    }

    reinterpret_cast<FARPROC &>(dinput8.OriginalDirectInput8Create) = GetProcAddress(dinput8.dll, "DirectInput8Create");
    dinput8.OriginalDllCanUnloadNow = GetProcAddress(dinput8.dll, "DllCanUnloadNow");
    dinput8.OriginalDllGetClassObject = GetProcAddress(dinput8.dll, "DllGetClassObject");
    dinput8.OriginalDllRegisterServer = GetProcAddress(dinput8.dll, "DllRegisterServer");
    dinput8.OriginalDllUnregisterServer = GetProcAddress(dinput8.dll, "DllUnregisterServer");
    dinput8.OriginalGetdfDIJoystick = GetProcAddress(dinput8.dll, "GetdfDIJoystick");
}

BOOL APIENTRY DllMain([[maybe_unused]] HMODULE hModule, DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        LoadOriginalLibrary();
        InitiateMainHook();
        break;
    case DLL_PROCESS_DETACH:
        UnloadMods();
        DestroyHooks();
        FreeLibrary(dinput8.dll);
        break;
    }
    return TRUE;
}
