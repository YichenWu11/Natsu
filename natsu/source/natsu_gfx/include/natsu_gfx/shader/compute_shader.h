#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/shader/shader.h"

namespace Natsu::GFX {
    // TODO:
    class ComputeShader : public Shader {
    public:
        ComPtr<ID3DBlob> csShader = nullptr;
    };
} // namespace Natsu::GFX
