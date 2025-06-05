#pragma once
#include "pch.h"

namespace core
{
    extern uintptr_t MainBase;
    extern uintptr_t FlowerBase;
    bool initialize(HMODULE MainDllModuleHandle, HMODULE FlowerDllModuleHandle);
}