#include "natsu_runtime/core/log/log_system.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "natsu_runtime/resource/config_manager/config_manager.h"

namespace Natsu::Runtime {
    LogSystem::LogSystem() {
        auto log_path = g_runtime_global_context.m_config_manager->GetRootFolder() / "log/natsu.log";

        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path.string(), true));

        logSinks[0]->set_pattern("[%^%l%$] %v");
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        s_logger = std::make_shared<spdlog::logger>("Natsu", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_logger);
        s_logger->set_level(spdlog::level::trace);
        s_logger->flush_on(spdlog::level::trace);
    }
} // namespace Natsu::Runtime