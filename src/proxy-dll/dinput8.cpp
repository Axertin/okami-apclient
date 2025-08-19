// Mostly generated and then modified to work with non-VS, see README
#include <Windows.h>

#include <dinput.h>

#include "modloader.h"

static struct dinput8_dll
{
    HMODULE dll;
    decltype(DirectInput8Create) *OriginalDirectInput8Create;
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
HRESULT WINAPI FakeDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
    LoadMods();
    return dinput8.OriginalDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
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

BOOL APIENTRY DllMain([[maybe_unused]] HMODULE hModule, DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
{
    char path[MAX_PATH * 2];
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
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
        break;
    case DLL_PROCESS_DETACH:
        UnloadMods();
        FreeLibrary(dinput8.dll);
        break;
    }
    return TRUE;
}
