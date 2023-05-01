#pragma once

#include "natsu_runtime/pch.h"

#include "natsu_runtime/function/render/render_system/render_data.h"
#include "natsu_runtime/function/render/window_system/window_system.h"

#include "natsu_gfx/command_context.h"

namespace Natsu::Runtime {
    struct RenderSystemInitInfo {
        WindowSystem* window_system;
    };

    class RenderSystem {
    public:
        RenderSystem() = default;
        ~RenderSystem();

        void Init(const RenderSystemInitInfo& init_info);
        void Shutdown();
        void OnUpdate();
        void OnResize();

        GFX::RenderPass* GetPass(const std::string& pass_name) {
            if (m_name2pass.count(pass_name))
                return nullptr;
            return m_render_path[m_name2pass[pass_name]];
        }

        GFX::CommandContext* GetCommandContext() { return m_context.get(); }

    public:
        std::vector<RenderData> m_render_data;

    private:
        std::vector<GFX::RenderPass*>           m_render_path;
        std::unordered_map<std::string, size_t> m_name2pass;

        std::unique_ptr<GFX::CommandContext> m_context;
    };
} // namespace Natsu::Runtime