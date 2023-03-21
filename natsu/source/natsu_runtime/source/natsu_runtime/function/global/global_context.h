#pragma once

#include <memory>
#include <string>

namespace Natsu::Runtime {
    class LogSystem;

    class RuntimeGlobalContext {
    public:
        // create all global systems and initialize these systems
        void startSystems(const std::string& config_file_path);
        // destroy all global systems
        void shutdownSystems();

    public:
        std::unique_ptr<LogSystem> m_logger_system;
    };

    extern RuntimeGlobalContext g_runtime_global_context;
} // namespace Natsu::Runtime