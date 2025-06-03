// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "memtable.h"
#include <okami_apclient-GitVersion.h>

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{

    // This gets called by the game very inconsistently, and often many times in quick succession
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        std::cout << "Loaded okami-apclient v" << okami_apclient::version_string() << " (" << okami_apclient::version_shorthash() << ")" << std::endl;
        std::cout << "BaseAddress: 0x" << std::hex << std::uppercase << getBaseAddress() << std::dec << std::endl;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}