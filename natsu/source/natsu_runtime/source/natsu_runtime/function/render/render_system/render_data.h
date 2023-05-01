#pragma once

#include "natsu_runtime/pch.h"

namespace Natsu::Runtime {
    // required data for rendering backend
    struct RenderData {
        std::string_view mesh_name;
    };

    // enable/disable some render_pass/post_processing
    struct RenderConfig {
    };
} // namespace Natsu::Runtime