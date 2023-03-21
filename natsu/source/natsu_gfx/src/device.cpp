#include "natsu_gfx/device.h"

namespace Natsu::GFX {
    void Device::Create(D3D_FEATURE_LEVEL level) {
        uint32_t dxgiFactoryFlags = 0;

#if defined(_DEBUG)
        // Enable the debug layer (requires the Graphics Tools "optional feature").
        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();

                // Enable additional debug layers.
                dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }
#endif
        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));
        uint adapterIndex = 0;     // we'll start looking for directx 12  compatible graphics devices starting at index 0
        bool adapterFound = false; // set this to true when a good one was found
        while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0) {
                HRESULT hr = D3D12CreateDevice(adapter.Get(), level,
                                               IID_PPV_ARGS(&dxDevice));
                if (SUCCEEDED(hr)) {
                    adapterFound = true;
                    break;
                }
            }
            adapter = nullptr;
            adapterIndex++;
        }
    }

    void Device::CreateDescriptorHeap(UINT size, D3D12_DESCRIPTOR_HEAP_TYPE type,
                                      ID3D12DescriptorHeap** pHeap) {
        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
        cbvHeapDesc.NumDescriptors = size;
        cbvHeapDesc.Type           = type;
        cbvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.NodeMask       = 0;
        ThrowIfFailed(dxDevice->CreateDescriptorHeap(&cbvHeapDesc,
                                                     IID_PPV_ARGS(pHeap)));
    }

    ComPtr<ID3D12RootSignature> Device::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc) {
        Microsoft::WRL::ComPtr<ID3DBlob> pSigBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;
        HRESULT                          hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pSigBlob, &pErrorBlob);
        if (FAILED(hr)) {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            return nullptr;
        }
        Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSig;
        ThrowIfFailed(dxDevice->CreateRootSignature(0, pSigBlob->GetBufferPointer(), pSigBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSig)));
        return pRootSig;
    }

} // namespace Natsu::GFX