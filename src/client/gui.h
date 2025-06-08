#pragma once

#include "pch.h"
#include <d3d11.h>
#include <dxgi.h>

void guiInitHooks();
bool guiSetup(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);
void guiCleanup(void);
