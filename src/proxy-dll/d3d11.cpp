// Mostly generated and then modified to work with non-VS, see README
#include <Windows.h>

#include <d3d11.h>

#include "modloader.h"

static struct d3d11_dll
{
    HMODULE dll;
    FARPROC OrignalCreateDirect3D11DeviceFromDXGIDevice;
    FARPROC OrignalCreateDirect3D11SurfaceFromDXGISurface;
    FARPROC OrignalD3D11CoreCreateDevice;
    FARPROC OrignalD3D11CoreCreateLayeredDevice;
    FARPROC OrignalD3D11CoreGetLayeredDeviceSize;
    FARPROC OrignalD3D11CoreRegisterLayers;
    decltype(D3D11CreateDevice) *OrignalD3D11CreateDevice;
    FARPROC OrignalD3D11CreateDeviceAndSwapChain;
    FARPROC OrignalD3D11CreateDeviceForD3D12;
    FARPROC OrignalD3D11On12CreateDevice;
    FARPROC OrignalD3DKMTCloseAdapter;
    FARPROC OrignalD3DKMTCreateAllocation;
    FARPROC OrignalD3DKMTCreateContext;
    FARPROC OrignalD3DKMTCreateDevice;
    FARPROC OrignalD3DKMTCreateSynchronizationObject;
    FARPROC OrignalD3DKMTDestroyAllocation;
    FARPROC OrignalD3DKMTDestroyContext;
    FARPROC OrignalD3DKMTDestroyDevice;
    FARPROC OrignalD3DKMTDestroySynchronizationObject;
    FARPROC OrignalD3DKMTEscape;
    FARPROC OrignalD3DKMTGetContextSchedulingPriority;
    FARPROC OrignalD3DKMTGetDeviceState;
    FARPROC OrignalD3DKMTGetDisplayModeList;
    FARPROC OrignalD3DKMTGetMultisampleMethodList;
    FARPROC OrignalD3DKMTGetRuntimeData;
    FARPROC OrignalD3DKMTGetSharedPrimaryHandle;
    FARPROC OrignalD3DKMTLock;
    FARPROC OrignalD3DKMTOpenAdapterFromHdc;
    FARPROC OrignalD3DKMTOpenResource;
    FARPROC OrignalD3DKMTPresent;
    FARPROC OrignalD3DKMTQueryAdapterInfo;
    FARPROC OrignalD3DKMTQueryAllocationResidency;
    FARPROC OrignalD3DKMTQueryResourceInfo;
    FARPROC OrignalD3DKMTRender;
    FARPROC OrignalD3DKMTSetAllocationPriority;
    FARPROC OrignalD3DKMTSetContextSchedulingPriority;
    FARPROC OrignalD3DKMTSetDisplayMode;
    FARPROC OrignalD3DKMTSetDisplayPrivateDriverFormat;
    FARPROC OrignalD3DKMTSetGammaRamp;
    FARPROC OrignalD3DKMTSetVidPnSourceOwner;
    FARPROC OrignalD3DKMTSignalSynchronizationObject;
    FARPROC OrignalD3DKMTUnlock;
    FARPROC OrignalD3DKMTWaitForSynchronizationObject;
    FARPROC OrignalD3DKMTWaitForVerticalBlankEvent;
    FARPROC OrignalD3DPerformance_BeginEvent;
    FARPROC OrignalD3DPerformance_EndEvent;
    FARPROC OrignalD3DPerformance_GetStatus;
    FARPROC OrignalD3DPerformance_SetMarker;
    FARPROC OrignalEnableFeatureLevelUpgrade;
    FARPROC OrignalOpenAdapter10;
    FARPROC OrignalOpenAdapter10_2;
} d3d11;

/*
IMPORTANT:
  Originally used __asm tags and jmp, which is not cross-compiler compatible.
  Currently relies on whatever the compiler does, if it doesn't only generate a jmp instruction it can break.
*/

void __stdcall FakeCreateDirect3D11DeviceFromDXGIDevice()
{
    d3d11.OrignalCreateDirect3D11DeviceFromDXGIDevice();
}
void __stdcall FakeCreateDirect3D11SurfaceFromDXGISurface()
{
    d3d11.OrignalCreateDirect3D11SurfaceFromDXGISurface();
}
void __stdcall FakeD3D11CoreCreateDevice()
{
    d3d11.OrignalD3D11CoreCreateDevice();
}
void __stdcall FakeD3D11CoreCreateLayeredDevice()
{
    d3d11.OrignalD3D11CoreCreateLayeredDevice();
}
void __stdcall FakeD3D11CoreGetLayeredDeviceSize()
{
    d3d11.OrignalD3D11CoreGetLayeredDeviceSize();
}
void __stdcall FakeD3D11CoreRegisterLayers()
{
    d3d11.OrignalD3D11CoreRegisterLayers();
}

HRESULT WINAPI FakeD3D11CreateDevice(IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, CONST D3D_FEATURE_LEVEL *pFeatureLevels,
                                     UINT FeatureLevels, UINT SDKVersion, ID3D11Device **ppDevice, D3D_FEATURE_LEVEL *pFeatureLevel,
                                     ID3D11DeviceContext **ppImmediateContext)
{
    LoadMods();

    return d3d11.OrignalD3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel,
                                          ppImmediateContext);
}
void __stdcall FakeD3D11CreateDeviceAndSwapChain()
{
    d3d11.OrignalD3D11CreateDeviceAndSwapChain();
}
void __stdcall FakeD3D11CreateDeviceForD3D12()
{
    d3d11.OrignalD3D11CreateDeviceForD3D12();
}
void __stdcall FakeD3D11On12CreateDevice()
{
    d3d11.OrignalD3D11On12CreateDevice();
}
void __stdcall FakeD3DKMTCloseAdapter()
{
    d3d11.OrignalD3DKMTCloseAdapter();
}
void __stdcall FakeD3DKMTCreateAllocation()
{
    d3d11.OrignalD3DKMTCreateAllocation();
}
void __stdcall FakeD3DKMTCreateContext()
{
    d3d11.OrignalD3DKMTCreateContext();
}
void __stdcall FakeD3DKMTCreateDevice()
{
    d3d11.OrignalD3DKMTCreateDevice();
}
void __stdcall FakeD3DKMTCreateSynchronizationObject()
{
    d3d11.OrignalD3DKMTCreateSynchronizationObject();
}
void __stdcall FakeD3DKMTDestroyAllocation()
{
    d3d11.OrignalD3DKMTDestroyAllocation();
}
void __stdcall FakeD3DKMTDestroyContext()
{
    d3d11.OrignalD3DKMTDestroyContext();
}
void __stdcall FakeD3DKMTDestroyDevice()
{
    d3d11.OrignalD3DKMTDestroyDevice();
}
void __stdcall FakeD3DKMTDestroySynchronizationObject()
{
    d3d11.OrignalD3DKMTDestroySynchronizationObject();
}
void __stdcall FakeD3DKMTEscape()
{
    d3d11.OrignalD3DKMTEscape();
}
void __stdcall FakeD3DKMTGetContextSchedulingPriority()
{
    d3d11.OrignalD3DKMTGetContextSchedulingPriority();
}
void __stdcall FakeD3DKMTGetDeviceState()
{
    d3d11.OrignalD3DKMTGetDeviceState();
}
void __stdcall FakeD3DKMTGetDisplayModeList()
{
    d3d11.OrignalD3DKMTGetDisplayModeList();
}
void __stdcall FakeD3DKMTGetMultisampleMethodList()
{
    d3d11.OrignalD3DKMTGetMultisampleMethodList();
}
void __stdcall FakeD3DKMTGetRuntimeData()
{
    d3d11.OrignalD3DKMTGetRuntimeData();
}
void __stdcall FakeD3DKMTGetSharedPrimaryHandle()
{
    d3d11.OrignalD3DKMTGetSharedPrimaryHandle();
}
void __stdcall FakeD3DKMTLock()
{
    d3d11.OrignalD3DKMTLock();
}
void __stdcall FakeD3DKMTOpenAdapterFromHdc()
{
    d3d11.OrignalD3DKMTOpenAdapterFromHdc();
}
void __stdcall FakeD3DKMTOpenResource()
{
    d3d11.OrignalD3DKMTOpenResource();
}
void __stdcall FakeD3DKMTPresent()
{
    d3d11.OrignalD3DKMTPresent();
}
void __stdcall FakeD3DKMTQueryAdapterInfo()
{
    d3d11.OrignalD3DKMTQueryAdapterInfo();
}
void __stdcall FakeD3DKMTQueryAllocationResidency()
{
    d3d11.OrignalD3DKMTQueryAllocationResidency();
}
void __stdcall FakeD3DKMTQueryResourceInfo()
{
    d3d11.OrignalD3DKMTQueryResourceInfo();
}
void __stdcall FakeD3DKMTRender()
{
    d3d11.OrignalD3DKMTRender();
}
void __stdcall FakeD3DKMTSetAllocationPriority()
{
    d3d11.OrignalD3DKMTSetAllocationPriority();
}
void __stdcall FakeD3DKMTSetContextSchedulingPriority()
{
    d3d11.OrignalD3DKMTSetContextSchedulingPriority();
}
void __stdcall FakeD3DKMTSetDisplayMode()
{
    d3d11.OrignalD3DKMTSetDisplayMode();
}
void __stdcall FakeD3DKMTSetDisplayPrivateDriverFormat()
{
    d3d11.OrignalD3DKMTSetDisplayPrivateDriverFormat();
}
void __stdcall FakeD3DKMTSetGammaRamp()
{
    d3d11.OrignalD3DKMTSetGammaRamp();
}
void __stdcall FakeD3DKMTSetVidPnSourceOwner()
{
    d3d11.OrignalD3DKMTSetVidPnSourceOwner();
}
void __stdcall FakeD3DKMTSignalSynchronizationObject()
{
    d3d11.OrignalD3DKMTSignalSynchronizationObject();
}
void __stdcall FakeD3DKMTUnlock()
{
    d3d11.OrignalD3DKMTUnlock();
}
void __stdcall FakeD3DKMTWaitForSynchronizationObject()
{
    d3d11.OrignalD3DKMTWaitForSynchronizationObject();
}
void __stdcall FakeD3DKMTWaitForVerticalBlankEvent()
{
    d3d11.OrignalD3DKMTWaitForVerticalBlankEvent();
}
void __stdcall FakeD3DPerformance_BeginEvent()
{
    d3d11.OrignalD3DPerformance_BeginEvent();
}
void __stdcall FakeD3DPerformance_EndEvent()
{
    d3d11.OrignalD3DPerformance_EndEvent();
}
void __stdcall FakeD3DPerformance_GetStatus()
{
    d3d11.OrignalD3DPerformance_GetStatus();
}
void __stdcall FakeD3DPerformance_SetMarker()
{
    d3d11.OrignalD3DPerformance_SetMarker();
}
void __stdcall FakeEnableFeatureLevelUpgrade()
{
    d3d11.OrignalEnableFeatureLevelUpgrade();
}
void __stdcall FakeOpenAdapter10()
{
    d3d11.OrignalOpenAdapter10();
}
void __stdcall FakeOpenAdapter10_2()
{
    d3d11.OrignalOpenAdapter10_2();
}

BOOL APIENTRY DllMain([[maybe_unused]] HMODULE hModule, DWORD ul_reason_for_call, [[maybe_unused]] LPVOID lpReserved)
{
    char path[MAX_PATH * 2];
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        GetSystemDirectoryA(path, MAX_PATH);
        strcat(path, "\\d3d11.dll");

        d3d11.dll = LoadLibraryA(path);
        if (d3d11.dll == false)
        {
            MessageBoxA(0, "Cannot load original d3d11.dll library", "Proxy", MB_ICONERROR);
            ExitProcess(0);
        }
        d3d11.OrignalCreateDirect3D11DeviceFromDXGIDevice = GetProcAddress(d3d11.dll, "CreateDirect3D11DeviceFromDXGIDevice");
        d3d11.OrignalCreateDirect3D11SurfaceFromDXGISurface = GetProcAddress(d3d11.dll, "CreateDirect3D11SurfaceFromDXGISurface");
        d3d11.OrignalD3D11CoreCreateDevice = GetProcAddress(d3d11.dll, "D3D11CoreCreateDevice");
        d3d11.OrignalD3D11CoreCreateLayeredDevice = GetProcAddress(d3d11.dll, "D3D11CoreCreateLayeredDevice");
        d3d11.OrignalD3D11CoreGetLayeredDeviceSize = GetProcAddress(d3d11.dll, "D3D11CoreGetLayeredDeviceSize");
        d3d11.OrignalD3D11CoreRegisterLayers = GetProcAddress(d3d11.dll, "D3D11CoreRegisterLayers");
        d3d11.OrignalD3D11CreateDevice = reinterpret_cast<decltype(d3d11.OrignalD3D11CreateDevice)>(GetProcAddress(d3d11.dll, "D3D11CreateDevice"));
        d3d11.OrignalD3D11CreateDeviceAndSwapChain = GetProcAddress(d3d11.dll, "D3D11CreateDeviceAndSwapChain");
        d3d11.OrignalD3D11CreateDeviceForD3D12 = GetProcAddress(d3d11.dll, "D3D11CreateDeviceForD3D12");
        d3d11.OrignalD3D11On12CreateDevice = GetProcAddress(d3d11.dll, "D3D11On12CreateDevice");
        d3d11.OrignalD3DKMTCloseAdapter = GetProcAddress(d3d11.dll, "D3DKMTCloseAdapter");
        d3d11.OrignalD3DKMTCreateAllocation = GetProcAddress(d3d11.dll, "D3DKMTCreateAllocation");
        d3d11.OrignalD3DKMTCreateContext = GetProcAddress(d3d11.dll, "D3DKMTCreateContext");
        d3d11.OrignalD3DKMTCreateDevice = GetProcAddress(d3d11.dll, "D3DKMTCreateDevice");
        d3d11.OrignalD3DKMTCreateSynchronizationObject = GetProcAddress(d3d11.dll, "D3DKMTCreateSynchronizationObject");
        d3d11.OrignalD3DKMTDestroyAllocation = GetProcAddress(d3d11.dll, "D3DKMTDestroyAllocation");
        d3d11.OrignalD3DKMTDestroyContext = GetProcAddress(d3d11.dll, "D3DKMTDestroyContext");
        d3d11.OrignalD3DKMTDestroyDevice = GetProcAddress(d3d11.dll, "D3DKMTDestroyDevice");
        d3d11.OrignalD3DKMTDestroySynchronizationObject = GetProcAddress(d3d11.dll, "D3DKMTDestroySynchronizationObject");
        d3d11.OrignalD3DKMTEscape = GetProcAddress(d3d11.dll, "D3DKMTEscape");
        d3d11.OrignalD3DKMTGetContextSchedulingPriority = GetProcAddress(d3d11.dll, "D3DKMTGetContextSchedulingPriority");
        d3d11.OrignalD3DKMTGetDeviceState = GetProcAddress(d3d11.dll, "D3DKMTGetDeviceState");
        d3d11.OrignalD3DKMTGetDisplayModeList = GetProcAddress(d3d11.dll, "D3DKMTGetDisplayModeList");
        d3d11.OrignalD3DKMTGetMultisampleMethodList = GetProcAddress(d3d11.dll, "D3DKMTGetMultisampleMethodList");
        d3d11.OrignalD3DKMTGetRuntimeData = GetProcAddress(d3d11.dll, "D3DKMTGetRuntimeData");
        d3d11.OrignalD3DKMTGetSharedPrimaryHandle = GetProcAddress(d3d11.dll, "D3DKMTGetSharedPrimaryHandle");
        d3d11.OrignalD3DKMTLock = GetProcAddress(d3d11.dll, "D3DKMTLock");
        d3d11.OrignalD3DKMTOpenAdapterFromHdc = GetProcAddress(d3d11.dll, "D3DKMTOpenAdapterFromHdc");
        d3d11.OrignalD3DKMTOpenResource = GetProcAddress(d3d11.dll, "D3DKMTOpenResource");
        d3d11.OrignalD3DKMTPresent = GetProcAddress(d3d11.dll, "D3DKMTPresent");
        d3d11.OrignalD3DKMTQueryAdapterInfo = GetProcAddress(d3d11.dll, "D3DKMTQueryAdapterInfo");
        d3d11.OrignalD3DKMTQueryAllocationResidency = GetProcAddress(d3d11.dll, "D3DKMTQueryAllocationResidency");
        d3d11.OrignalD3DKMTQueryResourceInfo = GetProcAddress(d3d11.dll, "D3DKMTQueryResourceInfo");
        d3d11.OrignalD3DKMTRender = GetProcAddress(d3d11.dll, "D3DKMTRender");
        d3d11.OrignalD3DKMTSetAllocationPriority = GetProcAddress(d3d11.dll, "D3DKMTSetAllocationPriority");
        d3d11.OrignalD3DKMTSetContextSchedulingPriority = GetProcAddress(d3d11.dll, "D3DKMTSetContextSchedulingPriority");
        d3d11.OrignalD3DKMTSetDisplayMode = GetProcAddress(d3d11.dll, "D3DKMTSetDisplayMode");
        d3d11.OrignalD3DKMTSetDisplayPrivateDriverFormat = GetProcAddress(d3d11.dll, "D3DKMTSetDisplayPrivateDriverFormat");
        d3d11.OrignalD3DKMTSetGammaRamp = GetProcAddress(d3d11.dll, "D3DKMTSetGammaRamp");
        d3d11.OrignalD3DKMTSetVidPnSourceOwner = GetProcAddress(d3d11.dll, "D3DKMTSetVidPnSourceOwner");
        d3d11.OrignalD3DKMTSignalSynchronizationObject = GetProcAddress(d3d11.dll, "D3DKMTSignalSynchronizationObject");
        d3d11.OrignalD3DKMTUnlock = GetProcAddress(d3d11.dll, "D3DKMTUnlock");
        d3d11.OrignalD3DKMTWaitForSynchronizationObject = GetProcAddress(d3d11.dll, "D3DKMTWaitForSynchronizationObject");
        d3d11.OrignalD3DKMTWaitForVerticalBlankEvent = GetProcAddress(d3d11.dll, "D3DKMTWaitForVerticalBlankEvent");
        d3d11.OrignalD3DPerformance_BeginEvent = GetProcAddress(d3d11.dll, "D3DPerformance_BeginEvent");
        d3d11.OrignalD3DPerformance_EndEvent = GetProcAddress(d3d11.dll, "D3DPerformance_EndEvent");
        d3d11.OrignalD3DPerformance_GetStatus = GetProcAddress(d3d11.dll, "D3DPerformance_GetStatus");
        d3d11.OrignalD3DPerformance_SetMarker = GetProcAddress(d3d11.dll, "D3DPerformance_SetMarker");
        d3d11.OrignalEnableFeatureLevelUpgrade = GetProcAddress(d3d11.dll, "EnableFeatureLevelUpgrade");
        d3d11.OrignalOpenAdapter10 = GetProcAddress(d3d11.dll, "OpenAdapter10");
        d3d11.OrignalOpenAdapter10_2 = GetProcAddress(d3d11.dll, "OpenAdapter10_2");
        break;
    }
    case DLL_PROCESS_DETACH:
    {
        UnloadMods();
        FreeLibrary(d3d11.dll);
    }
    break;
    }
    return TRUE;
}
