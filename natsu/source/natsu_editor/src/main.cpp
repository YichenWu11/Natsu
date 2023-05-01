#include "natsu_runtime/engine.h"

int main() {
    auto engine = new Natsu::Runtime::Engine;

    engine->StartEngine(std::string{});
    engine->Init();

    engine->Run();

    engine->Clear();
    engine->ShutdownEngine();

    delete engine;
}
