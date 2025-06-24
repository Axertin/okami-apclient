#pragma once
#include <optional>
#include <string>

#include "framework.h"

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
