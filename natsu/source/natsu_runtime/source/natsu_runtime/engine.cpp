#include "natsu_runtime/engine.h"

#include "natsu_gfx/resource/vertex_index_buffer.h"
#include "natsu_runtime/function/global/global_context.h"
#include "natsu_runtime/function/render/render_system/render_system.h"
#include "natsu_runtime/function/render/window_system/window_system.h"
#include "natsu_runtime/resource/config_manager/config_manager.h"
#include "natsu_runtime/resource/resource_manager.h"

namespace Natsu::Runtime {
    void Engine::StartEngine(const std::string& config_file_path) {
        g_runtime_global_context.StartSystems(config_file_path);

        LOG_INFO("engine start");
    }

    void Engine::Init() {
        GFX::VertexData vertices_square[] = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        };

        uint32_t indices_square[] = {0, 3, 1, 3, 2, 1};

        GFX::VertexBufferLayout layout;

        g_runtime_global_context.m_resource_manager->add<ResourceType::Mesh>(
            "square",
            vertices_square,
            array_count(vertices_square) * sizeof(GFX::VertexData) / layout.structSize,
            indices_square,
            array_count(indices_square));
    }

    void Engine::Clear() {}

    void Engine::ShutdownEngine() {
        LOG_INFO("engine shutdown");

        g_runtime_global_context.ShutdownSystems();
    }

    void Engine::Run() {
        while (!g_runtime_global_context.m_window_system->ShouldClose()) {
            g_runtime_global_context.m_window_system->OnUpdate();
            g_runtime_global_context.m_render_system->OnUpdate();
        }
    }
} // namespace Natsu::Runtime