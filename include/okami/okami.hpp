
#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <imgui.h>
#include "memory.hpp"
#include "ammy.hpp"
#include "items.hpp"
#include "maps.hpp"
#include "brushes.hpp"

namespace okami
{
    inline void *D3D11PresentFnPtr;

    inline void getPresentFunctionPtr()
    {
        DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
        SwapChainDesc.BufferCount = 1;
        SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        SwapChainDesc.OutputWindow = GetForegroundWindow();
        SwapChainDesc.SampleDesc.Count = 1;
        SwapChainDesc.Windowed = TRUE;
        SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SwapChainDesc.BufferDesc.Width = 2;
        SwapChainDesc.BufferDesc.Height = 2;
        SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        ID3D11Device *pDevice = nullptr;
        ID3D11DeviceContext *pContext = nullptr;
        IDXGISwapChain *pSwapChain = nullptr;

        if (FAILED(D3D11CreateDeviceAndSwapChain(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
                D3D11_SDK_VERSION, &SwapChainDesc, &pSwapChain, &pDevice, nullptr, &pContext)))
        {
            std::cout << "[okamigame] Failed to initialize dummy D3D11 device!";
        }

        void **vtable = *reinterpret_cast<void ***>(pSwapChain);
        okami::D3D11PresentFnPtr = vtable[8];

        pSwapChain->Release();
        pDevice->Release();
        pContext->Release();
    }

    /// @brief Find Base Addresses of both vanilla DLLs
    /// @param MainDllModuleHandle Handle of main.dll
    /// @param FlowerDllModuleHandle Handle of flower_kernel.dll
    /// @return success boolean
    inline bool initialize(void *MainDllModuleHandle, void *FlowerDllModuleHandle)
    {
        std::cout << "[okamigame] Initializing Modules...";
        okami::MainBase = (uintptr_t)(MainDllModuleHandle);
        if (okami::MainBase == 0)
        {
            std::cout << std::endl
                      << "[okamigame] Main.dll BaseAddress not found!" << std::endl;
            return false;
        }
        okami::FlowerBase = (uintptr_t)(FlowerDllModuleHandle);
        if (okami::FlowerBase == 0)
        {
            std::cout << std::endl
                      << "[okamigame] flower_kernel.dll BaseAddress not found!" << std::endl;
            return false;
        }

        okami::initVariables();
        okami::initFunctions();
        getPresentFunctionPtr();

        std::cout << "Done!" << std::endl;

        std::cout << std::hex << "[okamigame] Module Addresses: main.dll->0x" << okami::MainBase << " flower_kernel.dll->0x" << okami::FlowerBase << std::dec << std::endl;

        return true;
    }
}
