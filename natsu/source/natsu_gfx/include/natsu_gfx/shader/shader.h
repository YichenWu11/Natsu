#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/descriptor_heap.h"
#include "natsu_gfx/resource/buffer_view.h"
#include "natsu_gfx/resource/descriptor_heap_alloc_view.h"
#include "natsu_gfx/shader/global_samplers.h"
#include "natsu_gfx/shader/shader_variable_type.h"

namespace Natsu::GFX {
    class Shader {
    public:
        struct Property {
            ShaderVariableType type;
            uint32_t           spaceIndex;
            uint32_t           registerIndex;
            uint32_t           arrSize;
            Property(
                ShaderVariableType type,
                uint32_t           spaceIndex,
                uint32_t           registerIndex,
                uint32_t           arrSize) :
                type(type),
                spaceIndex(spaceIndex),
                registerIndex(registerIndex),
                arrSize(arrSize) {}
        };

    protected:
        struct InsideProperty : public Property {
            uint32_t rootSigPos;
            InsideProperty(Property const& p) :
                Property(p) {}
        };
        ComPtr<ID3D12RootSignature>                     rootSig;
        std::optional<InsideProperty>                   GetProperty(std::string_view str) const;
        std::unordered_map<std::string, InsideProperty> properties;

    public:
        Shader(
            std::span<std::pair<std::string, Property> const> properties,
            ID3D12Device*                                     device,
            std::span<D3D12_STATIC_SAMPLER_DESC>              samplers = GlobalSamplers::GetSamplers());

        Shader(
            std::span<std::pair<std::string, Property> const> properties,
            ComPtr<ID3D12RootSignature>&&                     rootSig);

        Shader(Shader&& v) = default;

        int32_t GetPropRootSigPos(std::string_view propertyName) const;

        ID3D12RootSignature* RootSig() const { return rootSig.Get(); }

        bool SetResource(
            std::string_view           propertyName,
            ID3D12GraphicsCommandList* cmdList,
            BufferView                 buffer) const;

        bool SetResource(
            std::string_view           propertyName,
            ID3D12GraphicsCommandList* cmdList,
            DescriptorHeapAllocView    view) const;

        virtual ~Shader() = default;
    };
} // namespace Natsu::GFX