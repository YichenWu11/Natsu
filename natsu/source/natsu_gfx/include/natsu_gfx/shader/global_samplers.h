#pragma once

#include "natsu_gfx/pch.h"

namespace Natsu::GFX {
    class GlobalSamplers {
    public:
        static std::span<D3D12_STATIC_SAMPLER_DESC> GetSamplers();
        static std::span<D3D12_STATIC_SAMPLER_DESC> GetSSAOSamplers();
    };
} // namespace Natsu::GFX