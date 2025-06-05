// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "okami.h"
#include "gamehandler.h"
#include <okami_apclient-GitVersion.h>

const uint32_t RETRIES = 5;
static HMODULE MinHook;
static std::atomic<bool> SetupCalled = false;

DWORD WINAPI deferredInit(LPVOID)
{
    std::cout << "Loading MinHook...";

    // Make sure MinHook is loaded before we try and call things from it
    // We don't know when this will happen, so give it a few tries over a couple seconds
    for (uint32_t i = 0; i < RETRIES; i++)
    {
        MinHook = GetModuleHandle("minhook.x64.dll");
        if (!MinHook)
        {
            std::cout << ".";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        else
        {
            std::cout << "Done!" << std::endl;
            GameHandler::setup();
            SetupCalled = true;
            return 0;
        }
    }

    std::cout << std::endl
              << "MinHook not loaded after " << RETRIES << "retries!" << std::endl;
    return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::cout << "Loaded okami-apclient v" << okami_apclient::version_string() << " (" << okami_apclient::version_shorthash() << ")" << std::endl;
        if (!okami::initialize(GetModuleHandleW(L"main.dll"), GetModuleHandleW(L"flower_kernel.dll")))
        {
            return FALSE;
        }

        // Defer initialization until after DllMain() returns, to somewhat mitigate risk of deadlock
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&deferredInit, NULL, NULL, NULL);

        break;

    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        // Only perform heavy cleanup if early in process teardown
        if ((lpReserved == nullptr) && SetupCalled)
        {
            if (MinHook)
            {
                GameHandler::cleanup();
            }
        }
        // Otherwise, it is in the OS's hands to actually release our resources.
        break;
    }

    return TRUE;
}