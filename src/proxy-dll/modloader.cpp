#include <Windows.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

static std::vector<HMODULE> loadedMods;
static fs::path moddedTemp = fs::temp_directory_path() / "okami_modded_signal.txt";

bool IsModded()
{
    LPCSTR cmdLine = GetCommandLineA();
    if (cmdLine != nullptr && strstr(cmdLine, "-MODDED") != nullptr)
    {
        return true;
    }

    if (fs::exists(moddedTemp))
    {
        auto lastWrite = fs::last_write_time(moddedTemp);
        auto diff = decltype(lastWrite)::clock::now() - lastWrite;

        // If older than 30 seconds, assume not modded.
        if (diff < 30s)
        {
            return true;
        }
    }
    return false;
}

void LoadMods()
{
    static bool alreadyLoaded = false;
    if (alreadyLoaded || !IsModded())
        return;

    // Remove the temp file so that subsequent launch attempt in a short time span doesn't open modded
    fs::remove(moddedTemp);

    alreadyLoaded = true;

    for (const auto &dll : fs::recursive_directory_iterator("mods"))
    {
        const auto &path = dll.path();
        if (path.extension().string() == ".dll")
        {
            HMODULE hMod = LoadLibraryW(path.c_str());
            if (hMod == nullptr)
            {
                std::string msg = "Failed to load mod: " + path.string();
                MessageBoxA(nullptr, msg.c_str(), "Error", MB_ICONERROR);
                continue;
            }

            loadedMods.emplace_back(hMod);
        }
    }
}

void UnloadMods()
{
    for (HMODULE hMod : loadedMods)
    {
        FreeLibrary(hMod);
    }
    loadedMods.clear();
}
