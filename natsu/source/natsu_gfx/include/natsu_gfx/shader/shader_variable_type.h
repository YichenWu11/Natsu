#pragma once

#include "natsu_gfx/pch.h"

namespace Natsu::GFX {
    enum class ShaderVariableType : uint8_t {
        ConstantBuffer,
        CBVDescriptorHeap,
        SRVDescriptorHeap,
        UAVDescriptorHeap,
        StructuredBuffer,
        RWStructuredBuffer
    };

    struct ShaderVariable {
        std::string        name;
        ShaderVariableType type;
        uint32_t           tableSize;
        uint32_t           registerPos;
        uint32_t           space;
        ShaderVariable() {}
        ShaderVariable(
            const std::string& name,
            ShaderVariableType type,
            uint32_t           tableSize,
            uint32_t           registerPos,
            uint32_t           space) :
            name(name),
            type(type),
            tableSize(tableSize),
            registerPos(registerPos),
            space(space) {}
    };
} // namespace Natsu::GFX