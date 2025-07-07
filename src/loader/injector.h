#pragma once
#include <optional>
#include <string>

#include "framework.h"

class OkamiInjector
{
  public:
    static std::optional<DWORD> findOkamiProcess(int tries = 30);
    static bool inject(DWORD processId, const std::wstring &dllPath);
    static std::optional<DWORD> launchOkami(const std::wstring &exePath);

  private:
    static DWORD getMainThreadId(DWORD processId);
    static std::string formatWindowsError(DWORD error);
};
