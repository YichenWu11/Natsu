#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/descriptor_heap_manager.h"
#include "natsu_gfx/device.h"
#include "natsu_gfx/frame_resource_manager.h"
#include "natsu_gfx/render_pass.h"

#include "natsu_gfx/resource/resource_stateTracker.h"
#include "natsu_gfx/resource_manager.h"

#include "natsu_gfx/shader/pso_manager.h"

namespace Natsu::GFX {
    struct NonCopyable {
        NonCopyable()                              = default;
        NonCopyable(const NonCopyable&)            = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
        ~NonCopyable()                             = default;
    };

    class CommandContext : NonCopyable {
    public:
        CommandContext(HWND window_handle, int width, int height);
        ~CommandContext();

        void BeginFrame();
        void Execute();
        void EndFrame();

        void OnResize();

    private:
        HWND m_window_handle;

        std::vector<RenderPass*> m_RenderPath;

        CommandQueue m_GraphicsQueue;
        CommandQueue m_ComputeQueue;
        CommandQueue m_CopyQueue;

        PSOManager        m_psoMngr;
        ResourceManager   m_resourceMngr;
        FrameResourceMngr m_frameResourceMngr;

        // Synchronization objects
        uint32_t            m_backBufferIndex;
        ComPtr<ID3D12Fence> m_fence;
        uint64_t            m_fenceValue;

        ComPtr<IDXGISwapChain3> m_SwapChain;

        ResourceStateTracker m_StateTracker;

        Device m_device;
    };
} // namespace Natsu::GFX