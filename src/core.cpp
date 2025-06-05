
#include "core.h"

namespace core
{
    uintptr_t MainBase;
    uintptr_t FlowerBase;
}

bool core::initialize(HMODULE MainDllModuleHandle, HMODULE FlowerDllModuleHandle)
{
    std::cout << "Initializing Modules...";
    core::MainBase = (uintptr_t)(MainDllModuleHandle);
    if (core::MainBase == 0)
    {
        std::cout << std::endl
                  << "Main.dll BaseAddress not found!" << std::endl;
        return false;
    }
    core::FlowerBase = (uintptr_t)(FlowerDllModuleHandle);
    if (core::FlowerBase == 0)
    {
        std::cout << std::endl
                  << "flower_kernel.dll BaseAddress not found!" << std::endl;
        return false;
    }

    std::cout << "Done!" << std::endl;
    return true;
}