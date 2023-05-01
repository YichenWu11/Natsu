#pragma once

#include "natsu_runtime/pch.h"

namespace Natsu::Runtime {
    class ConfigManager;
    class LogSystem;
    class WindowSystem;
    class RenderSystem;
    class ResourceManager;

    class RuntimeGlobalContext {
    public:
        // create all global systems and initialize these systems
        void StartSystems(const std::string& config_file_path);
        // destroy all global systems
        void ShutdownSystems();

    public:
        std::unique_ptr<ConfigManager>   m_config_manager;
        std::unique_ptr<LogSystem>       m_logger_system;
        std::unique_ptr<WindowSystem>    m_window_system;
        std::unique_ptr<RenderSystem>    m_render_system;
        std::unique_ptr<ResourceManager> m_resource_manager;
    };

    extern RuntimeGlobalContext g_runtime_global_context;
} // namespace Natsu::Runtime