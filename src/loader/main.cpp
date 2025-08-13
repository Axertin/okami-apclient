#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <thread>

#include "injector.h"

namespace fs = std::filesystem;

int error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 1;
}

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
        return error(std::format("Cannot find okami-apclient.dll at: {}", dllPath.string()));
    }

    // Check if game exe exists
    if (!fs::exists(okamiExe))
    {
        return error(std::format("Cannot find okami.exe at: {}", okamiExe.string()));
    }

    // Check if game is already running
    std::cout << "Checking for Okami HD process..." << std::endl;
    std::optional<DWORD> processId = OkamiInjector::findOkamiProcess(1);

    if (!processId)
    {
        std::cout << "Okami HD not running. Launching game..." << std::endl;
        processId = OkamiInjector::launchOkami(okamiExe.wstring());
        if (!processId)
        {
            return error("Failed to launch Okami HD!");
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
        return error("Injection failed!");
    }
}
