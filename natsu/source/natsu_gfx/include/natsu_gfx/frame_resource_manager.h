#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/frame_resource.h"

namespace Natsu::GFX {
    class FrameResourceMngr {
    public:
        FrameResourceMngr()  = default;
        ~FrameResourceMngr() = default;

        void Create(size_t numFrame, ID3D12Device*);
        void Shutdown();

        size_t         GetNumFrame() const noexcept { return frameResources.size(); }
        FrameResource* GetCurrentFrameResource() noexcept;
        size_t         GetCurrentCpuFence() const noexcept { return cpuFence; }
        size_t         GetCurrentIndex() const noexcept { return (cpuFence % frameResources.size()); }

        void BeginFrame();
        void EndFrame(CommandQueue& commandQueue);
        void Execute(CommandQueue& commandQueue);

        void CleanUp();

    private:
        HANDLE                                      eventHandle;
        std::vector<std::unique_ptr<FrameResource>> frameResources;
        size_t                                      cpuFence{0};
        Microsoft::WRL::ComPtr<ID3D12Fence>         gpuFence;
    };
} // namespace Natsu::GFX