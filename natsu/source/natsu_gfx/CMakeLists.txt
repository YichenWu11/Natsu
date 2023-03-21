find_package(directxtk12 CONFIG REQUIRED)

Module(
    NAME GFX
    TYPE StaticLibrary
    SRC_PATH
    DEPS
    DEPS_PUBLIC Microsoft::DirectXTK12
    INCLUDES_PUBLIC
    INCLUDES
    LINKS
    LINKS_PUBLIC d3d12 dxgi dxguid d3dcompiler
)

target_precompile_headers(GFX PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/natsu_gfx/pch.h)