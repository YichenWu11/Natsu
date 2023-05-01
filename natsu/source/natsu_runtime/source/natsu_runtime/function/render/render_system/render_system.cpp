#include "natsu_runtime/function/render/render_system/render_system.h"

#include "natsu_runtime/function/render/render_system/pass/test_pass.h"

namespace Natsu::Runtime {
    RenderSystem::~RenderSystem() {
        Shutdown();
    }

    void RenderSystem::Init(const RenderSystemInitInfo& init_info) {
        m_context = std::make_unique<GFX::CommandContext>(
            init_info.window_system->GetNativeWindowHandle(),
            init_info.window_system->GetWidth(),
            init_info.window_system->GetHeight());

        m_render_path.push_back(new TestPass);
        m_name2pass["TestPass"] = 0;

        m_context->PreLoad(m_render_path);
    }

    void RenderSystem::Shutdown() {
        for (GFX::RenderPass* pass : m_render_path) {
            if (pass) {
                delete pass;
                pass = nullptr;
            }
        }
    }

    void RenderSystem::OnUpdate() {
        m_context->Execute(m_render_path);
        m_render_data.clear();
    }

    void RenderSystem::OnResize() {
        m_context->OnResize(m_render_path);
    }
} // namespace Natsu::Runtime