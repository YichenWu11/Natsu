#include "natsu_gfx/dx_common.h"

#include <comdef.h>
#include <fstream>

namespace Natsu::GFX {
    DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
        ErrorCode(hr),
        FunctionName(functionName),
        Filename(filename),
        LineNumber(lineNumber) {
    }

    bool DXUtil::IsKeyDown(int vkeyCode) {
        return (GetAsyncKeyState(vkeyCode) & 0x8000) != 0;
    }

    ComPtr<ID3DBlob> DXUtil::LoadBinary(const std::wstring& filename) {
        std::ifstream fin(filename, std::ios::binary);

        fin.seekg(0, std::ios_base::end);
        std::ifstream::pos_type size = (int)fin.tellg();
        fin.seekg(0, std::ios_base::beg);

        ComPtr<ID3DBlob> blob;
        ThrowIfFailed(D3DCreateBlob(size, blob.GetAddressOf()));

        fin.read((char*)blob->GetBufferPointer(), size);
        fin.close();

        return blob;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> DXUtil::CreateDefaultBuffer(
        ID3D12Device*                           device,
        ID3D12GraphicsCommandList*              cmdList,
        const void*                             initData,
        UINT64                                  byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer) {
        ComPtr<ID3D12Resource> defaultBuffer;

        // Create the actual default buffer resource.
        ThrowIfFailed(device->CreateCommittedResource(
            get_rvalue_ptr(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)),
            D3D12_HEAP_FLAG_NONE,
            get_rvalue_ptr(CD3DX12_RESOURCE_DESC::Buffer(byteSize)),
            D3D12_RESOURCE_STATE_COMMON, // 初始默认状态
            nullptr,
            IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

        // In order to copy CPU memory data into our default buffer, we need to create
        // an intermediate upload heap.
        ThrowIfFailed(device->CreateCommittedResource(
            get_rvalue_ptr(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)), // 上传堆
            D3D12_HEAP_FLAG_NONE,
            get_rvalue_ptr(CD3DX12_RESOURCE_DESC::Buffer(byteSize)), // 大小与刚才创建的默认堆相同
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA subResourceData = {};
        subResourceData.pData                  = initData;
        subResourceData.RowPitch               = byteSize;
        subResourceData.SlicePitch             = subResourceData.RowPitch;

        cmdList->ResourceBarrier(1, get_rvalue_ptr(CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
                                                                                        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST)));
        UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
        cmdList->ResourceBarrier(1, get_rvalue_ptr(CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
                                                                                        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ)));

        // Note: uploadBuffer has to be kept alive after the above function calls because
        // the command list has not been executed yet that performs the actual copy.
        // The caller can Release the uploadBuffer after it knows the copy has been executed.

        return defaultBuffer;
    }

    ComPtr<ID3DBlob> DXUtil::CompileShader(
        const std::wstring&     filename,
        const D3D_SHADER_MACRO* defines,
        const std::string&      entrypoint,
        const std::string&      target) {
        // enable bindless
        UINT compileFlags = D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;

#if defined(DEBUG) || defined(_DEBUG)
        compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

        ComPtr<ID3DBlob> byteCode = nullptr;
        ComPtr<ID3DBlob> errors;
        hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

        if (errors != nullptr)
            OutputDebugStringA((char*)errors->GetBufferPointer());

        ThrowIfFailed(hr);

        return byteCode;
    }

    // output error msg when catch the exception
    std::wstring DxException::ToString() const {
        // Get the string description of the error code.
        _com_error   err(ErrorCode);
        std::wstring msg = err.ErrorMessage();

        return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
    }
} // namespace Natsu::GFX