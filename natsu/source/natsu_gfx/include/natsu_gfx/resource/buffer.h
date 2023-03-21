#pragma once

/*
    A Buffer is the simplest GPU resource,
    which is essentially a piece of linear memory on the GPU.

    Buffer:
        - DefaultBuffer
        - UploadBuffer
        - ReadbackBuffer
*/

#include "natsu_gfx/resource/buffer_view.h"
#include "natsu_gfx/resource/resource.h"

namespace Natsu::GFX {
    class Buffer : public Resource {
    public:
        Buffer(ID3D12Device* device);
        Buffer(Buffer&&) = default;

        virtual uint64_t                  GetByteSize() const = 0;
        virtual D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const  = 0;
        virtual ~Buffer();
    };
} // namespace Natsu::GFX