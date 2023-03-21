#pragma once

#include <variant>

#include "natsu_gfx/pch.h"

#include "natsu_gfx/resource/buffer_view.h"
#include "natsu_gfx/resource/descriptor_heap_alloc_view.h"

namespace Natsu::GFX {
    struct BindProperty {
        std::string name;
        std::variant<
            BufferView,
            DescriptorHeapAllocView>
            prop;

        BindProperty() {}

        template <typename A, typename B>
        requires(
            std::is_constructible_v<decltype(name), A&&> || std::is_constructible_v<decltype(prop), B&&>)
            BindProperty(
                A&& a,
                B&& b) :
            name(std::forward<A>(a)),
            prop(std::forward<B>(b)) {}
    };
} // namespace Natsu::GFX