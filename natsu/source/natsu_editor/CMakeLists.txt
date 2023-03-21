Module(
    NAME Editor
    TYPE Executable
    SRC_PATH
    DEPS
    DEPS_PUBLIC Natsu::Runtime
    INCLUDES_PUBLIC
    INCLUDES
    LINKS
    LINKS_PUBLIC
)

target_compile_definitions(Editor PUBLIC ROOT_DIR=${ROOT_DIR})