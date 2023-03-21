#include "natsu_runtime/core/log/log_system.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Natsu::Runtime {
    LogSystem::LogSystem() {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        logSinks[0]->set_pattern("[%^%l%$] %v");

        s_logger = std::make_shared<spdlog::logger>("Natsu", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_logger);
        s_logger->set_level(spdlog::level::trace);
        s_logger->flush_on(spdlog::level::trace);
    }
} // namespace Natsu::Runtime