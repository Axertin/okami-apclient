#include <Windows.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    return 1;
}

std::string formatWindowsError(DWORD error)
{
    if (error == 0)
        return "Success";

    LPSTR messageBuffer = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);

    std::string message(messageBuffer);
    LocalFree(messageBuffer);

    // Remove trailing newlines
    while (!message.empty() && (message.back() == '\n' || message.back() == '\r'))
        message.pop_back();

    return message;
}

void createModdedSignal()
{
    fs::path temp = fs::temp_directory_path() / "okami_modded_signal.txt";
    fs::remove(temp);
    std::ofstream{temp};
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    std::cout << "Okami APClient Loader" << std::endl;

    // Get loader directory
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    fs::path loaderDir = fs::path(exePath).parent_path();

    // Build paths
    fs::path okamiExe = loaderDir / "okami.exe";

    // Check if game exe exists
    if (!fs::exists(okamiExe))
    {
        return error(std::format("Cannot find okami.exe at: {}", okamiExe.string()));
    }

    createModdedSignal();

    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};
    std::wstring wideOkamiExe = okamiExe;
    if (!CreateProcessW(wideOkamiExe.c_str(), nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        return error(std::format("Failed to launch okami.exe: {}", formatWindowsError(GetLastError())));
    }

    return 0;
}
