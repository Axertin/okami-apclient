// dllmain.cpp : Defines the entry point for the DLL application.
// Bootstrap loader for other DLLs

#include "pch.h"

typedef void (*StartOkamiClientFn)();

DWORD WINAPI bootstrapThread(void)
{
    std::cout << "[Loader] Bootstrap thread started\n";

    WCHAR oldSearchPath[MAX_PATH];
    GetDllDirectoryW(MAX_PATH, oldSearchPath);

    WCHAR path[MAX_PATH];
    GetModuleFileNameW(GetModuleHandleW(L"okami-loader.dll"), path, MAX_PATH);
    PathRemoveFileSpecW(path);
    wcscat_s(path, L"\\mods\\apclient\\okami-apclient.dll");

    std::wcout << "[Loader] Attempting to load DLL from: " << path << std::endl;

    SetDllDirectoryW(path);
    HMODULE okamiDLL = LoadLibraryExW(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    DWORD errorCode = GetLastError();
    if (!okamiDLL)
    {
        std::cout << "[Loader] Failed to load okami-apclient.dll. Error code: " << errorCode << std::endl;
        return 1;
    }

    auto startFn = reinterpret_cast<StartOkamiClientFn>(
        GetProcAddress(okamiDLL, "okamiAPClientInit"));

    if (!startFn)
    {
        std::cout << "[Loader] Failed to find okamiAPClientInit export" << std::endl;
        return 1;
    }

    std::cout << "[Loader] Calling okamiAPClientInit()" << std::endl;
    startFn();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        // Spin off a completely detached thread to avoid loader lock issues
        HANDLE thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)bootstrapThread, nullptr, 0, nullptr);
        if (thread)
            CloseHandle(thread); // Close handle immediately — we don’t join
    }

    return TRUE;
}