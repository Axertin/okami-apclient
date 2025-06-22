#include "injector.h"
#include <iostream>
#include <vector>
#include <optional>
#include <thread>

std::optional<DWORD> OkamiInjector::findOkamiProcess()
{
    // Okami HD window class and title
    HWND hwnd = FindWindowW(nullptr, L"ŌKAMI HD");
    if (!hwnd)
    {
        // Try alternative window title
        hwnd = FindWindowW(L"MT Framework", nullptr);
    }

    if (!hwnd)
    {
        return std::nullopt;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    return processId ? std::optional<DWORD>(processId) : std::nullopt;
}

bool OkamiInjector::inject(DWORD processId, const std::wstring &dllPath)
{
    // Open target process
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
            PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, processId);

    if (!hProcess)
    {
        std::cerr << "Failed to open process: " << formatWindowsError(GetLastError()) << std::endl;
        return false;
    }

    // Allocate memory for DLL path
    size_t pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID remotePath = VirtualAllocEx(hProcess, nullptr, pathSize,
                                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!remotePath)
    {
        std::cerr << "Failed to allocate memory: " << formatWindowsError(GetLastError()) << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    // Write DLL path
    if (!WriteProcessMemory(hProcess, remotePath, dllPath.c_str(), pathSize, nullptr))
    {
        std::cerr << "Failed to write memory: " << formatWindowsError(GetLastError()) << std::endl;
        VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get LoadLibraryW address
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    LPVOID loadLibraryAddr = reinterpret_cast<LPVOID>(GetProcAddress(hKernel32, "LoadLibraryW"));

    // Load the DLL
    HANDLE hLoadThread = CreateRemoteThread(hProcess, nullptr, 0,
                                            reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryAddr), remotePath, 0, nullptr);

    if (!hLoadThread)
    {
        std::cerr << "Failed to create LoadLibrary thread: " << formatWindowsError(GetLastError()) << std::endl;
        VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Wait for DLL to load
    WaitForSingleObject(hLoadThread, INFINITE);

    // Get the module handle from LoadLibrary
    DWORD hModule = 0;
    GetExitCodeThread(hLoadThread, &hModule);
    CloseHandle(hLoadThread);

    // Cleanup path memory
    VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);

    if (!hModule)
    {
        std::cerr << "LoadLibrary failed in target process" << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    // Now call the entry point
    if (!callEntryPoint(hProcess, dllPath))
    {
        CloseHandle(hProcess);
        return false;
    }

    CloseHandle(hProcess);
    return true;
}

bool OkamiInjector::callEntryPoint(HANDLE hProcess, const std::wstring &dllPath)
{
    // Get the base name of the DLL
    std::wstring dllName = dllPath.substr(dllPath.find_last_of(L"\\") + 1);

    // Find the module in the target process
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (!EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        std::cerr << "Failed to enumerate modules" << std::endl;
        return false;
    }

    HMODULE targetModule = nullptr;
    for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
    {
        wchar_t modName[MAX_PATH];
        if (GetModuleFileNameExW(hProcess, hMods[i], modName, MAX_PATH))
        {
            std::wstring modPath(modName);
            if (modPath.find(dllName) != std::wstring::npos)
            {
                targetModule = hMods[i];
                break;
            }
        }
    }

    if (!targetModule)
    {
        std::cerr << "Could not find injected module in target process" << std::endl;
        return false;
    }

    // Get offset of 'entry' export in our process
    HMODULE localModule = LoadLibraryW(dllPath.c_str());
    if (!localModule)
    {
        std::cerr << "Failed to load DLL locally for export resolution" << std::endl;
        return false;
    }

    FARPROC entryProc = GetProcAddress(localModule, "entry");
    if (!entryProc)
    {
        std::cerr << "Could not find 'entry' export in DLL" << std::endl;
        FreeLibrary(localModule);
        return false;
    }

    // Calculate offset
    DWORD_PTR offset = reinterpret_cast<DWORD_PTR>(entryProc) - reinterpret_cast<DWORD_PTR>(localModule);
    FreeLibrary(localModule);

    // Calculate remote address
    LPVOID remoteEntry = reinterpret_cast<LPVOID>(reinterpret_cast<DWORD_PTR>(targetModule) + offset);

    // Create thread to run entry point
    HANDLE hEntryThread = CreateRemoteThread(hProcess, nullptr, 0,
                                             reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteEntry), nullptr, 0, nullptr);

    if (!hEntryThread)
    {
        std::cerr << "Failed to create entry point thread: " << formatWindowsError(GetLastError()) << std::endl;
        return false;
    }

    // Wait for entry to complete
    WaitForSingleObject(hEntryThread, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeThread(hEntryThread, &exitCode);
    CloseHandle(hEntryThread);

    return exitCode == 0;
}

std::string OkamiInjector::formatWindowsError(DWORD error)
{
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);

    return message;
}

bool OkamiInjector::launchOkami(const std::wstring &exePath)
{
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};

    // Create process suspended to ensure we can inject before it fully initializes
    if (!CreateProcessW(
            exePath.c_str(),
            nullptr,
            nullptr,
            nullptr,
            FALSE,
            CREATE_SUSPENDED,
            nullptr,
            nullptr,
            &si,
            &pi))
    {
        std::cerr << "Failed to launch okami.exe: " << formatWindowsError(GetLastError()) << std::endl;
        return false;
    }

    // Resume the main thread
    ResumeThread(pi.hThread);

    // Close thread handle (we don't need it)
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // Give the game time to initialize its window
    std::cout << "Waiting for game to initialize..." << std::endl;
    for (int i = 0; i < 30; i++)
    {
        if (findOkamiProcess())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Extra time for full init
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return false;
}
