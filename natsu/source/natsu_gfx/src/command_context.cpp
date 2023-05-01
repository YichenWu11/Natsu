#include "natsu_gfx/command_context.h"

namespace Natsu::GFX {
    CommandContext::CommandContext(HWND window_handle, int width, int height) :
        m_window_handle(window_handle),
        m_GraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT),
        m_ComputeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE),
        m_CopyQueue(D3D12_COMMAND_LIST_TYPE_COPY) {
        m_device.Create(D3D_FEATURE_LEVEL_11_0);

        DescriptorHeapMngr::GetInstance().Init(
            m_device.DxDevice(), 16384, 16384, 16384, 16384, 16384);

        m_GraphicsQueue.Create(m_device.DxDevice());
        m_ComputeQueue.Create(m_device.DxDevice());
        m_CopyQueue.Create(m_device.DxDevice());

        m_psoMngr.Create(m_device.DxDevice());
        m_resourceMngr.Create(m_device.DxDevice());
        m_frameResourceMngr.Create(3, m_device.DxDevice()); // triple buffering

        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount           = 3;
        swapChainDesc.Width                 = width;
        swapChainDesc.Height                = height;
        swapChainDesc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count      = 1;

        ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(m_device.DxgiFactory()->CreateSwapChainForHwnd(
            m_GraphicsQueue.GetCommandQueue(),
            window_handle,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain));

        ThrowIfFailed(m_device.DxgiFactory()->MakeWindowAssociation(m_window_handle, DXGI_MWA_NO_ALT_ENTER));
        ThrowIfFailed(swapChain.As(&m_SwapChain));

        {
            ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
            m_fenceValue = 0;
        }
    }

    CommandContext::~CommandContext() {
        FlushCommandQueue();

        m_psoMngr.Shutdown();
        m_frameResourceMngr.Shutdown();
        m_resourceMngr.Shutdown();

        m_CopyQueue.Shutdown();
        m_ComputeQueue.Shutdown();
        m_GraphicsQueue.Shutdown();
    }

    void CommandContext::FlushCommandQueue() {
        m_fenceValue++;

        ThrowIfFailed(m_GraphicsQueue->Signal(m_fence.Get(), m_fenceValue));

        if (m_fence->GetCompletedValue() < m_fenceValue) {
            HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

            ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, eventHandle));

            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    void CommandContext::PreLoad(const std::vector<RenderPass*>& render_path) {
        for (auto render_pass : render_path)
            render_pass->PreLoad();
    }

    void CommandContext::Execute(const std::vector<RenderPass*>& render_path) {
        m_frameResourceMngr.BeginFrame();

        {
            auto cmdListHandle = m_frameResourceMngr.GetCurrentFrameResource()->Command();
            for (auto render_pass : render_path)
                render_pass->Execute(*(m_frameResourceMngr.GetCurrentFrameResource()), m_backBufferIndex);
        }

        m_frameResourceMngr.Execute(m_GraphicsQueue);

        ThrowIfFailed(m_SwapChain->Present(0, 0));
        m_frameResourceMngr.EndFrame(m_GraphicsQueue);

        m_backBufferIndex = (m_backBufferIndex + 1) % 3;
    }

    void CommandContext::OnResize(const std::vector<RenderPass*>& render_path) {
        FlushCommandQueue();

        for (auto render_pass : render_path)
            render_pass->OnResize();
    }

} // namespace Natsu::GFX