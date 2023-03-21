#pragma once

#include <filesystem>
#include <string>

namespace Natsu::Runtime {
    class Engine {
    public:
        Engine()  = default;
        ~Engine() = default;

        Engine(const Engine&)            = delete;
        Engine& operator=(const Engine&) = delete;

        void startEngine(const std::string& config_file_path);
        void shutdownEngine();

        void initialize();
        void clear();

    private:
    };
} // namespace Natsu::Runtime