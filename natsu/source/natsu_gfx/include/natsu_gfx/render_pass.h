#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/frame_resource.h"
#include "natsu_gfx/resource/resource.h"
#include "natsu_gfx/resource/texture.h"
#include "natsu_gfx/resource_manager.h"
#include "natsu_gfx/shader/basic_shader.h"
#include "natsu_gfx/shader/compute_shader.h"

namespace Natsu::GFX {
    // struct ResourceImpl {
    //     RsrcViewType type;
    //     uint32_t     handle_offset;
    //     Resource*    resource;
    // };

    // using PassResource = std::map<std::string, ResourceImpl>;

    class RenderPass {
    public:
        RenderPass() = default;
        virtual ~RenderPass() {
            for (auto [name, bs] : m_basicShaderMap) {
                if (bs) {
                    delete bs;
                    bs = nullptr;
                }
            }
            for (auto [name, cs] : m_computeShaderMap) {
                if (cs) {
                    delete cs;
                    cs = nullptr;
                }
            }
        }

        virtual void PreLoad() = 0;
        virtual void OnResize() {}
        virtual void Execute(FrameResource& frameRes, uint32_t frameIndex) = 0;
        virtual void DelayDisposeResource(FrameResource& frameRes)         = 0;

    protected:
        // PassResource m_resourceMap;

        // using Shader to construct PSO Directly
        std::map<std::string, BasicShader*>   m_basicShaderMap;
        std::map<std::string, ComputeShader*> m_computeShaderMap;
    };
} // namespace Natsu::GFX