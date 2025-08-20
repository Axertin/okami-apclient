#include <Windows.h>

#include <filesystem>
#include <string>
#include <vector>

static std::vector<HMODULE> loadedMods;

bool IsModded()
{
    LPCSTR cmdLine = GetCommandLineA();
    if (cmdLine == nullptr)
        return false;

    return strstr(cmdLine, "-MODDED") != nullptr;
}

void LoadMods()
{
    static bool alreadyLoaded = false;
    if (alreadyLoaded || !IsModded())
        return;

    alreadyLoaded = true;

    for (const auto &dll : std::filesystem::recursive_directory_iterator("mods"))
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
