#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/frame_resource.h"
#include "natsu_gfx/resource/resource.h"
#include "natsu_gfx/resource_manager.h"
#include "natsu_gfx/shader/basic_shader.h"
#include "natsu_gfx/shader/compute_shader.h"

namespace Natsu::GFX {
    struct ResourceImpl {
        RsrcViewType type;
        uint32_t     handle_offset;
        Resource*    resource;
    };

    using PassResource = std::map<std::string, ResourceImpl>;

    class RenderPass {
    public:
        RenderPass();
        virtual ~RenderPass();

        virtual void PreLoad() = 0;
        virtual void OnResize() {}
        virtual void Execute()                                     = 0;
        virtual void DelayDisposeResource(FrameResource& frameRes) = 0;

    private:
        PassResource m_resourceMap;

        // using Shader to construct PSO Directly
        std::map<std::string, BasicShader*>   m_basicShaderMap;
        std::map<std::string, ComputeShader*> m_computeShaderMap;
    };
} // namespace Natsu::GFX