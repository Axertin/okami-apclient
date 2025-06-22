#pragma once
#include "framework.h"
#include <string>
#include <optional>

class OkamiInjector
{
public:
    static std::optional<DWORD> findOkamiProcess();
    static bool inject(DWORD processId, const std::wstring &dllPath);
    static bool launchOkami(const std::wstring &exePath);

private:
    static bool callEntryPoint(HANDLE hProcess, const std::wstring &dllPath);
    static std::string formatWindowsError(DWORD error);
};
