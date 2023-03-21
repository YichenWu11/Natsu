#pragma once

#include "natsu_gfx/pch.h"

namespace Natsu::GFX {
    class FrameResource;

    class Resource {
    protected:
        ID3D12Device* device{nullptr};

    public:
        ID3D12Device* GetDevice() const { return device; }

        Resource() = default;

        Resource(ID3D12Device* device) :
            device(device) {}

        Resource(Resource&&)      = default;
        Resource(Resource const&) = delete;

        virtual ~Resource() = default;
        virtual ID3D12Resource**      ReleaseAndGetAddress() { return nullptr; }
        virtual ID3D12Resource*       GetResource() const { return nullptr; }
        virtual D3D12_RESOURCE_STATES GetInitState() const { return D3D12_RESOURCE_STATE_COMMON; }
        virtual void                  DelayDispose(FrameResource* frameRes) const {}
    };
} // namespace Natsu::GFX