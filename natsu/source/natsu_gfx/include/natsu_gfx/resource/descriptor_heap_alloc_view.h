#pragma once

#include "natsu_gfx/descriptor_heap.h"

namespace Natsu::GFX {

    struct DescriptorHeapAllocView {
        DescriptorHeapAllocation const* heap;
        uint64                          index;

        DescriptorHeapAllocView(
            DescriptorHeapAllocation const* heap,
            uint64                          index) :
            heap(heap),
            index(index) {}

        DescriptorHeapAllocView(
            DescriptorHeapAllocation const* heap) :
            heap(heap),
            index(0) {}
    };
} // namespace Natsu::GFX