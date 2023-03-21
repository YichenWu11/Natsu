#pragma once

#include "natsu_gfx/pch.h"

namespace Natsu::GFX {
    class Device {
    public:
        Device()  = default;
        ~Device() = default;

        ID3D12Device*       operator->() { return dxDevice.Get(); }
        const ID3D12Device* operator->() const { return dxDevice.Get(); }

        void Create(D3D_FEATURE_LEVEL level);

        ID3D12Device*  DxDevice() const { return dxDevice.Get(); }
        IDXGIAdapter1* Adapter() const { return adapter.Get(); }
        IDXGIFactory4* DxgiFactory() const { return dxgiFactory.Get(); }

        void CreateDescriptorHeap(UINT size, D3D12_DESCRIPTOR_HEAP_TYPE type,
                                  ID3D12DescriptorHeap** pHeap);

        ComPtr<ID3D12RootSignature> CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc);

    private:
        ComPtr<ID3D12Device>  dxDevice;
        ComPtr<IDXGIAdapter1> adapter;
        ComPtr<IDXGIFactory4> dxgiFactory;
    };
} // namespace Natsu::GFX