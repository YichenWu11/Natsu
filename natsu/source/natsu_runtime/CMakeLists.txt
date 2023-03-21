find_package(spdlog CONFIG REQUIRED)

Module(
    NAME Runtime
    TYPE StaticLibrary
    SRC_PATH source
    DEPS
    DEPS_PUBLIC Natsu::GFX spdlog::spdlog
    INCLUDES_PUBLIC source
    INCLUDES source
    LINKS
    LINKS_PUBLIC
)

target_compile_definitions(Runtime PUBLIC ROOT_DIR=${ROOT_DIR})