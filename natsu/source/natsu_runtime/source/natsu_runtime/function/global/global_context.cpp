#include "natsu_runtime/function/global/global_context.h"

#include "natsu_runtime/core/log/log_system.h"

namespace Natsu::Runtime {
    RuntimeGlobalContext g_runtime_global_context;

    void RuntimeGlobalContext::startSystems(const std::string& config_file_path) {
        m_logger_system = std::make_unique<LogSystem>();
    }

    void RuntimeGlobalContext::shutdownSystems() {
        m_logger_system.reset();
    }

} // namespace Natsu::Runtime