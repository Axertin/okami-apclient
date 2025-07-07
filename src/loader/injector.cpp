#include "injector.h"

#include <iostream>
#include <optional>
#include <thread>

std::optional<DWORD> OkamiInjector::findOkamiProcess(int tries)
{
    std::cout << "Trying window-based detection..." << std::endl;

    for (int i = 0; i < tries; ++i)
    {
        HWND hwnd = FindWindowW(nullptr, L"ŌKAMI HD");
        if (!hwnd)
            hwnd = FindWindowW(nullptr, L"steam_app_587620");
        if (!hwnd)
            hwnd = FindWindowW(L"MT Framework", nullptr);

        if (hwnd)
        {
            DWORD processId = 0;
            GetWindowThreadProcessId(hwnd, &processId);
            if (processId)
            {
                std::cout << "Found Okami HD process: " << processId << std::endl;
                return processId;
            }
        }

        std::cout << ".";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "\nWindow detection failed." << std::endl;
    return std::nullopt;
}

bool OkamiInjector::inject(DWORD processId, const std::wstring &dllPath)
{
    std::cout << "Using SetWindowsHookEx injection for process " << processId << std::endl;

    // Find the main thread of the target process
    DWORD mainThreadId = getMainThreadId(processId);
    if (!mainThreadId)
    {
        std::cerr << "Could not find main thread of target process" << std::endl;
        return false;
    }

    std::cout << "Target main thread ID: " << mainThreadId << std::endl;

    // Load the DLL in our process first to get the entry point
    HMODULE hLocalModule = LoadLibraryW(dllPath.c_str());
    if (!hLocalModule)
    {
        std::cerr << "Failed to load DLL locally: " << formatWindowsError(GetLastError()) << std::endl;
        return false;
    }

    // Ignore the warning - We know the signature's right, but the compiler doesn't.
#pragma warning(push)
#pragma warning(disable : 4191) // MSVC/Clang-cl: 'reinterpret_cast': unsafe conversion from 'FARPROC' to 'HOOKPROC'
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type" // GCC/MinGW: function pointer cast warnings
    // Get the entry point (which now has HOOKPROC signature)
    HOOKPROC entryPoint = reinterpret_cast<HOOKPROC>(GetProcAddress(hLocalModule, "entry"));
#pragma warning(pop)
#pragma GCC diagnostic pop

    if (!entryPoint)
    {
        std::cerr << "Could not find 'entry' export in DLL" << std::endl;
        FreeLibrary(hLocalModule);
        return false;
    }

    std::cout << "Entry point found at: " << reinterpret_cast<void *>(entryPoint) << std::endl;

    // Install the hook - this will load the DLL into the target process
    HHOOK hHook = SetWindowsHookExW(WH_GETMESSAGE, // Hook type that gets called on message processing
                                    entryPoint,    // Our entry point as the hook procedure
                                    hLocalModule,  // Module containing the hook procedure
                                    mainThreadId   // Target thread
    );

    if (!hHook)
    {
        std::cerr << "Failed to install hook: " << formatWindowsError(GetLastError()) << std::endl;
        FreeLibrary(hLocalModule);
        return false;
    }

    std::cout << "Hook installed successfully" << std::endl;

    // Trigger the hook by sending a message to the target thread
    std::cout << "Triggering hook execution..." << std::endl;

    // Send a harmless message that will trigger our hook
    if (!PostThreadMessageW(mainThreadId, WM_NULL, 0, 0))
    {
        std::cout << "PostThreadMessage failed, trying alternative trigger..." << std::endl;

        // Alternative: Find window and send a message to it
        HWND hwnd = nullptr;
        EnumWindows(
            [](HWND hWnd, LPARAM lParam) -> BOOL
            {
                DWORD windowProcessId;
                GetWindowThreadProcessId(hWnd, &windowProcessId);
                if (windowProcessId == static_cast<DWORD>(lParam))
                {
                    *reinterpret_cast<HWND *>(&lParam) = hWnd; // Hack to return the HWND
                    return FALSE;                              // Stop enumeration
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&hwnd));

        if (hwnd)
        {
            std::cout << "Sending message to window..." << std::endl;
            SendMessageTimeoutW(hwnd, WM_NULL, 0, 0, SMTO_NORMAL, 5000, nullptr);
        }
    }

    // Give the hook some time to execute
    std::cout << "Waiting for hook execution..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Clean up the hook
    UnhookWindowsHookEx(hHook);
    FreeLibrary(hLocalModule);

    std::cout << "Hook injection completed" << std::endl;
    return true;
}

DWORD OkamiInjector::getMainThreadId(DWORD processId)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    DWORD mainThreadId = 0;
    FILETIME earliestTime = {};
    bool foundFirst = false;

    if (Thread32First(snapshot, &te32))
    {
        do
        {
            if (te32.th32OwnerProcessID == processId)
            {
                // Open thread to get creation time
                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                if (hThread)
                {
                    FILETIME creationTime, exitTime, kernelTime, userTime;
                    if (GetThreadTimes(hThread, &creationTime, &exitTime, &kernelTime, &userTime))
                    {
                        // Find the earliest created thread (likely the main thread)
                        if (!foundFirst || CompareFileTime(&creationTime, &earliestTime) < 0)
                        {
                            earliestTime = creationTime;
                            mainThreadId = te32.th32ThreadID;
                            foundFirst = true;
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(snapshot, &te32));
    }

    CloseHandle(snapshot);
    return mainThreadId;
}

std::string OkamiInjector::formatWindowsError(DWORD error)
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

std::optional<DWORD> OkamiInjector::launchOkami(const std::wstring &exePath)
{
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};

    if (!CreateProcessW(exePath.c_str(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi))
    {
        std::cerr << "Failed to launch okami.exe: " << formatWindowsError(GetLastError()) << std::endl;
        return std::nullopt;
    }

    std::cout << "Launched game with PID: " << pi.dwProcessId << std::endl;
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    std::cout << "Waiting for game to initialize..." << std::endl;
    return findOkamiProcess(60);
}
