#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

#include "injector.h"

namespace fs = std::filesystem;

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    std::cout << "Okami APClient Loader" << std::endl;

    // Get loader directory
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    fs::path loaderDir = fs::path(exePath).parent_path();

    // Build paths
    fs::path dllPath = loaderDir / "mods" / "apclient" / "okami-apclient.dll";
    fs::path okamiExe = loaderDir / "okami.exe";

    // Check if DLL exists
    if (!fs::exists(dllPath))
    {
        std::cerr << "Error: Cannot find okami-apclient.dll at: " << dllPath.string() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    // Check if game exe exists
    if (!fs::exists(okamiExe))
    {
        std::cerr << "Error: Cannot find okami.exe at: " << okamiExe.string() << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    // Check if game is already running
    std::cout << "Checking for Okami HD process..." << std::endl;
    std::optional<DWORD> processId = OkamiInjector::findOkamiProcess();

    if (!processId)
    {
        std::cout << "Okami HD not running. Launching game..." << std::endl;

        if (!OkamiInjector::launchOkami(okamiExe.wstring()))
        {
            std::cerr << "Failed to launch Okami HD!" << std::endl;
            std::cout << "Press Enter to exit..." << std::endl;
            std::cin.get();
            return 1;
        }

        // Try to find the process again
        processId = OkamiInjector::findOkamiProcess();
        if (!processId)
        {
            std::cerr << "Error: Game launched but process not found." << std::endl;
            std::cout << "Press Enter to exit..." << std::endl;
            std::cin.get();
            return 1;
        }
    }

    std::cout << "Found Okami HD process (PID: " << *processId << ")" << std::endl;
    std::cout << "Injecting: " << dllPath.filename().string() << std::endl;

    // Perform injection and call entry point
    if (OkamiInjector::inject(*processId, dllPath.wstring()))
    {
        std::cout << "Injection successful!" << std::endl;
        std::cout << "Closing in 3 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 0;
    }
    else
    {
        std::cerr << "Injection failed!" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }
}
