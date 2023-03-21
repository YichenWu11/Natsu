#include "natsu_runtime/engine.h"

#include "natsu_runtime/core/base/macro.h"
#include "natsu_runtime/function/global/global_context.h"

namespace Natsu::Runtime {
    void Engine::startEngine(const std::string& config_file_path) {
        g_runtime_global_context.startSystems(config_file_path);

        LOG_INFO("engine start");
    }

    void Engine::initialize() {}
    void Engine::clear() {}

    void Engine::shutdownEngine() {
        g_runtime_global_context.shutdownSystems();
    }

} // namespace Natsu::Runtime