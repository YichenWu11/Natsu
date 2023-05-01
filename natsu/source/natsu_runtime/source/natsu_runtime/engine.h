#pragma once

#include "natsu_runtime/pch.h"

#include "natsu_runtime/core/util/timer.h"

namespace Natsu::Runtime {
    class Engine {
    public:
        Engine()  = default;
        ~Engine() = default;

        Engine(const Engine&)            = delete;
        Engine& operator=(const Engine&) = delete;

        void StartEngine(const std::string& config_file_path);
        void ShutdownEngine();

        void Init();
        void Clear();

        void Run();

    private:
        Util::Timer m_timer;
    };
} // namespace Natsu::Runtime