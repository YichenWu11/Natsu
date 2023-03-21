#include "natsu_runtime/engine.h"

int main() {
    auto engine = new Natsu::Runtime::Engine;

    // Natsu::GFX::CommandContext context(GetActiveWindow(), 100, 100);

    engine->startEngine(std::string{});
    engine->initialize();

    // LOG_INFO("Hello NatsuEditor");

    engine->clear();
    engine->shutdownEngine();

    delete engine;
}
