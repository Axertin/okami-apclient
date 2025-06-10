
#include "gui.h"
#include "okami/okami.hpp"
#include "loginwindow.h"

static ID3D11Device *device = nullptr;
static ID3D11DeviceContext *context = nullptr;
static HWND hwnd = nullptr;
static ID3D11RenderTargetView *rtv = nullptr;
static bool GuiIsVisible = true;

static std::vector<std::unique_ptr<Window>> Windows;

bool guiTryInit(IDXGISwapChain *pSwapChain)
{
    std::cout << "[gui] Initializing ImGui...";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    DXGI_SWAP_CHAIN_DESC desc;
    pSwapChain->GetDesc(&desc);
    hwnd = desc.OutputWindow;

    if (!hwnd || !IsWindow(hwnd))
    {
        std::cout << std::endl
                  << "[gui] Invalid HWND from swapchain!" << std::endl;
        return false;
    }

    if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void **>(&device))))
    {
        std::cout << std::endl
                  << "[gui] Failed to get render device!" << std::endl;
        return false;
    }

    device->GetImmediateContext(&context);

    ImGui::StyleColorsDark();

    io.FontGlobalScale = 1.0f;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    Windows.push_back(std::make_unique<LoginWindow>());

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);
    std::cout << "Done!" << std::endl;

    return true;
}

void guiRenderFrame(IDXGISwapChain *pSwapChain)
{
    static int WindowWidth;
    static int WindowHeight;

    if (!ImGui::GetCurrentContext())
        std::cout << "[gui] ImGui context missing!" << std::endl;
    else
    {
        RECT rect;
        if (!GetClientRect(hwnd, &rect))
        {
            std::cout << "[gui] Failed to get window size!" << std::endl;
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

typedef HRESULT(__stdcall *PresentFn)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);
static PresentFn oPresent = nullptr;
HRESULT __stdcall onRenderPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags)
{

    static bool initialized = false;

    if (!initialized && guiTryInit(pSwapChain))
        initialized = true;

    if (GetAsyncKeyState(VK_TAB))
        GuiIsVisible = !GuiIsVisible;

    if (initialized && GuiIsVisible)
        guiRenderFrame(pSwapChain);

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

void guiInitHooks()
{
    std::thread([]
                {
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for game swapchain and window to initialize
            MH_CreateHook(okami::D3D11PresentFnPtr, reinterpret_cast<LPVOID>(&onRenderPresent), reinterpret_cast<LPVOID *>(&oPresent));
            MH_EnableHook(okami::D3D11PresentFnPtr); })
        .detach();
}
