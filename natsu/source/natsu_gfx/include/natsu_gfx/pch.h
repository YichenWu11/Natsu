#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows
#include <WindowsX.h>
#include <windows.h>
#include <wrl.h>

// Direct3d12
#include <D3Dcompiler.h>
#include <D3d12SDKLayers.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3d12.h>
#include <dxgi1_4.h>

using Microsoft::WRL::ComPtr;

#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <set>
#include <shellapi.h>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "natsu_gfx/dx_common.h"
#include "natsu_gfx/util/d3dx12.h"
#include "natsu_gfx/util/meta_lib.h"

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

// Size of Dynamic Resource's Page (1M Bytes)
#define DYNAMIC_RESOURCE_PAGE_SIZE 1048576
