#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/util/hash.h"

// hash
namespace std {
    template <>
    struct hash<D3D12_GRAPHICS_PIPELINE_STATE_DESC> {
        using argument_type = D3D12_GRAPHICS_PIPELINE_STATE_DESC;
        using result_type   = size_t;
        size_t operator()(argument_type const& v) const {
            return GetHash<argument_type>(v);
        }
    };

    template <>
    struct equal_to<D3D12_GRAPHICS_PIPELINE_STATE_DESC> {
        using argument_type = D3D12_GRAPHICS_PIPELINE_STATE_DESC;
        using result_type   = size_t;
        bool operator()(argument_type const& a, argument_type const& b) const {
            return memcmp(&a, &b, sizeof(argument_type)) == 0;
        }
    };

    template <>
    struct hash<D3D12_COMPUTE_PIPELINE_STATE_DESC> {
        using argument_type = D3D12_COMPUTE_PIPELINE_STATE_DESC;
        using result_type   = size_t;
        size_t operator()(argument_type const& v) const {
            return GetHash<argument_type>(v);
        }
    };

    template <>
    struct equal_to<D3D12_COMPUTE_PIPELINE_STATE_DESC> {
        using argument_type = D3D12_COMPUTE_PIPELINE_STATE_DESC;
        using result_type   = size_t;
        bool operator()(argument_type const& a, argument_type const& b) const {
            return memcmp(&a, &b, sizeof(argument_type)) == 0;
        }
    };
} // namespace std

namespace Natsu::GFX {
    class BasicShader;
    class ComputeShader;

    class PSOManager {
        std::unordered_map<D3D12_GRAPHICS_PIPELINE_STATE_DESC, ComPtr<ID3D12PipelineState>> graphics_pipelineStates;
        std::unordered_map<D3D12_COMPUTE_PIPELINE_STATE_DESC, ComPtr<ID3D12PipelineState>>  compute_pipelineStates;
        ID3D12Device*                                                                       device;
        ID3D12PipelineState*                                                                GetPipelineState(D3D12_GRAPHICS_PIPELINE_STATE_DESC const& stateDesc);
        ID3D12PipelineState*                                                                GetPipelineState(D3D12_COMPUTE_PIPELINE_STATE_DESC const& stateDesc);

    public:
        PSOManager()  = default;
        ~PSOManager() = default;

        void Create(ID3D12Device* _device) { device = _device; }
        void Shutdown(){};

        ID3D12PipelineState* GetPipelineState(
            std::span<D3D12_INPUT_ELEMENT_DESC const> meshLayout,
            BasicShader const*                        shader,
            std::span<DXGI_FORMAT>                    rtvFormats,
            DXGI_FORMAT                               depthFormat,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE             topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

        ID3D12PipelineState* GetPipelineState(
            std::span<D3D12_INPUT_ELEMENT_DESC const> meshLayout,
            ComputeShader const*                      shader,
            std::span<DXGI_FORMAT>                    rtvFormats,
            DXGI_FORMAT                               depthFormat,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE             topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    };
} // namespace Natsu::GFX