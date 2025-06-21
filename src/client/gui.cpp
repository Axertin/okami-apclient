
#include "gui.h"

#include "loginwindow.h"
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include "okami/memorymap.hpp"
#include "devtools.h"
#include "console.h"

#ifdef _WIN32
#include <MinHook.h>
#include <d3d11.h>
#include <dxgi.h>
#include <imgui_impl_win32.h>
#endif

#ifdef _WIN32

static ID3D11Device *device = nullptr;
static ID3D11DeviceContext *context = nullptr;
static HWND hwnd = nullptr;
static ID3D11RenderTargetView *rtv = nullptr;
static bool GuiIsVisible = true;

static std::vector<std::unique_ptr<Window>> Windows;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @brief Hook for WndProc
 */
static WNDPROC oWndProc = nullptr;
LRESULT WINAPI onWndProc(HWND Handle, UINT Msg, WPARAM WParam, LPARAM LParam)
{
    if (ImGui_ImplWin32_WndProcHandler(Handle, Msg, WParam, LParam))
    {
        return 0;
    }

    if (GetAsyncKeyState(VK_DELETE))
    {
        GuiIsVisible = !GuiIsVisible;
        return 0;
    }

    ImGuiIO &io = ImGui::GetIO();
    if (GuiIsVisible && io.WantCaptureMouse)
    {
        switch (Msg)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_MOUSEWHEEL:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
            return 0;
        }
    }

    return oWndProc ? oWndProc(Handle, Msg, WParam, LParam)
                    : DefWindowProc(Handle, Msg, WParam, LParam);
}

/**
 * @brief Try to initialize ImGUI
 *
 * @param pSwapChain Pointer to DX11 swapchain
 * @return true if successful
 * @return false if unsuccessful
 */
bool guiTryInit(IDXGISwapChain *pSwapChain)
{
    logInfo("[gui] Initializing ImGui");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    DXGI_SWAP_CHAIN_DESC desc;
    pSwapChain->GetDesc(&desc);
    hwnd = desc.OutputWindow;

    if (!hwnd || !IsWindow(hwnd))
    {
        logError("[gui] Invalid HWND from swapchain!");
        return false;
    }

    if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void **>(&device))))
    {
        logError("[gui] Failed to get render device!");
        return false;
    }

    oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&onWndProc)));

    device->GetImmediateContext(&context); // https://i.kym-cdn.com/photos/images/original/003/033/089/164.jpeg

    ImGui::StyleColorsDark();

    io.FontGlobalScale = 1.0f;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    Windows.push_back(std::make_unique<LoginWindow>());
    Windows.push_back(std::make_unique<DevTools>());
    Windows.push_back(std::make_unique<Console>());

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    return true;
}

/**
 * @brief Render ImGUI frame
 * @param pSwapChain Pointer to DX11 swapchain
 */
void guiRenderFrame(IDXGISwapChain *pSwapChain)
{
    static int WindowWidth;
    static int WindowHeight;

    if (!ImGui::GetCurrentContext())
        logError("[gui] ImGui context missing!");
    else
    {
        RECT rect;
        if (!GetClientRect(hwnd, &rect))
        {
            logError("[gui] Failed to get window size!");
            return;
        }
        const int BaseWidth = 1920;
        const int BaseHeight = 1080;
        float WidthScale = static_cast<float>(WindowWidth) / BaseWidth;
        float HeightScale = static_cast<float>(WindowHeight) / BaseHeight;
        float UIScale = (((WidthScale) < (HeightScale)) ? (WidthScale) : (HeightScale));

        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();
        ImGui::NewFrame();

        for (auto &Window : Windows)
        {
            Window.get()->draw(WindowWidth, WindowHeight, UIScale);
        }

        ImGui::Render();

        if (!rtv)
        {
            ID3D11Texture2D *backBuffer = nullptr;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
            device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
            backBuffer->Release();
        }
        context->OMSetRenderTargets(1, &rtv, nullptr);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}

/**
 * @brief Hook for DX11 Present() Function
 */
typedef HRESULT(__stdcall *PresentFn)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);
static PresentFn oPresent = nullptr;
HRESULT __stdcall onRenderPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool initialized = false;

    if (!initialized && guiTryInit(pSwapChain))
        initialized = true;

    if (GuiIsVisible)
    {
        static bool CursorOn = false;
        ClipCursor(nullptr);
        ReleaseCapture();
        if (!CursorOn)
        {
            for (int i = 0; i < 10; i++)
                if (ShowCursor(true) >= 0)
                    break;
            CursorOn = true;
        }
        ImGui::GetIO().MouseDrawCursor = false;
    }
    else
    {
        static bool CursorOn = true;
        if (CursorOn)
        {
            for (int i = 0; i < 10; ++i)
                if (ShowCursor(false) < 0)
                    break;
            CursorOn = false;
        }
        ImGui::GetIO().MouseDrawCursor = false;
    }

    if (initialized && GuiIsVisible)
    {
        guiRenderFrame(pSwapChain);
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void guiCleanup()
{
    if (!ImGui::GetCurrentContext())
        return;

    if (rtv)
    {
        rtv->Release();
        rtv = nullptr;
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void getPresentFunctionPtr()
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
        logError("[gui] Failed to initialize dummy D3D11 device!");
    }

    void **vtable = *reinterpret_cast<void ***>(pSwapChain);
    okami::D3D11PresentFnPtr = vtable[8];

    pSwapChain->Release();
    pDevice->Release();
    pContext->Release();
}

void guiInitHooks()
{
    getPresentFunctionPtr();

    std::thread([]
                {                                 
                std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for game swapchain and window to initialize
                MH_CreateHook(okami::D3D11PresentFnPtr, reinterpret_cast<LPVOID>(&onRenderPresent), reinterpret_cast<LPVOID *>(&oPresent));
                MH_EnableHook(okami::D3D11PresentFnPtr); })
        .detach();
}

#else // If not _WIN32

void guiInitHooks()
{
    logWarning("[gui] Not built for WIN32, no GUI is available.");
}

#endif
