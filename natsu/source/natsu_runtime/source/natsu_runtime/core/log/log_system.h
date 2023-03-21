#pragma once

#include <filesystem>
#include <memory>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

// TODO: remove to Engine

namespace Natsu::Runtime {
    class LogSystem {
    public:
        LogSystem();

        std::shared_ptr<spdlog::logger>& getLogger() { return s_logger; }

    private:
        std::shared_ptr<spdlog::logger> s_logger;
    };
} // namespace Natsu::Runtime
