#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/shader/shader.h"

namespace Natsu::GFX {
    class BasicShader : public Shader {
    public:
        ComPtr<ID3DBlob>                      vsShader = nullptr;
        ComPtr<ID3DBlob>                      psShader = nullptr;
        ComPtr<ID3DBlob>                      hsShader = nullptr;
        ComPtr<ID3DBlob>                      dsShader = nullptr;
        D3D12_RASTERIZER_DESC                 rasterizerState{CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT)};
        D3D12_DEPTH_STENCIL_DESC              depthStencilState{CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)};
        D3D12_BLEND_DESC                      blendState{CD3DX12_BLEND_DESC(D3D12_DEFAULT)};
        std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

        BasicShader(
            std::span<std::pair<std::string, Property> const> properties,
            ID3D12Device*                                     device,
            std::span<D3D12_STATIC_SAMPLER_DESC>              samplers = GlobalSamplers::GetSamplers());
        BasicShader(
            std::span<std::pair<std::string, Property> const> properties,
            ComPtr<ID3D12RootSignature>&&                     rootSig);

        void SetVsShader(
            const wchar_t*          path,
            const D3D_SHADER_MACRO* shaderDefines = nullptr,
            const std::string&      entryPoint    = "VSMain") {
            if (path != nullptr)
                vsShader = DXUtil::CompileShader(path, shaderDefines, entryPoint, "vs_5_1");
        }

        void SetPsShader(
            const wchar_t*          path,
            const D3D_SHADER_MACRO* shaderDefines = nullptr,
            const std::string&      entryPoint    = "PSMain") {
            if (path != nullptr)
                psShader = DXUtil::CompileShader(path, shaderDefines, entryPoint, "ps_5_1");
        }

        void SetHsShader(
            const wchar_t*          path,
            const D3D_SHADER_MACRO* shaderDefines = nullptr,
            const std::string&      entryPoint    = "HSMain") {
            if (path != nullptr)
                hsShader = DXUtil::CompileShader(path, shaderDefines, entryPoint, "hs_5_1");
        }

        void SetDsShader(
            const wchar_t*          path,
            const D3D_SHADER_MACRO* shaderDefines = nullptr,
            const std::string&      entryPoint    = "DSMain") {
            if (path != nullptr)
                dsShader = DXUtil::CompileShader(path, shaderDefines, entryPoint, "ds_5_1");
        }
    };
} // namespace Natsu::GFX