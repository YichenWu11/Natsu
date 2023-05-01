#include "natsu_runtime/function/global/global_context.h"

#include "natsu_runtime/core/log/log_system.h"
#include "natsu_runtime/function/render/render_system/render_system.h"
#include "natsu_runtime/function/render/window_system/window_system.h"
#include "natsu_runtime/resource/config_manager/config_manager.h"
#include "natsu_runtime/resource/resource_manager.h"

namespace Natsu::Runtime {
    RuntimeGlobalContext g_runtime_global_context;

    void RuntimeGlobalContext::StartSystems(const std::string& config_file_path) {
        m_config_manager = std::make_unique<ConfigManager>();

        m_logger_system = std::make_unique<LogSystem>();

        m_window_system = std::make_unique<WindowSystem>();
        m_window_system->Init();

        m_render_system = std::make_unique<RenderSystem>();
        m_render_system->Init(RenderSystemInitInfo{m_window_system.get()});

        m_resource_manager = std::make_unique<ResourceManager>();
    }

    void RuntimeGlobalContext::ShutdownSystems() {
        m_render_system.reset();
        m_resource_manager.reset();
        m_window_system.reset();
        m_logger_system.reset();
    }

} // namespace Natsu::Runtime