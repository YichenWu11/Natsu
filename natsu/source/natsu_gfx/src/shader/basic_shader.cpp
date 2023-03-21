#include "natsu_gfx/shader/basic_shader.h"

namespace Natsu::GFX {
    BasicShader::BasicShader(
        std::span<std::pair<std::string, Property> const> properties,
        ID3D12Device*                                     device,
        std::span<D3D12_STATIC_SAMPLER_DESC>              samplers) :
        Shader(properties, device, samplers) {}

    BasicShader::BasicShader(
        std::span<std::pair<std::string, Property> const> properties,
        ComPtr<ID3D12RootSignature>&&                     rootSig) :
        Shader(properties, std::move(rootSig)) {}
} // namespace Natsu::GFX