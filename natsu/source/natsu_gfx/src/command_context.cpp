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

        for (auto render_pass : m_RenderPath)
            render_pass->PreLoad();
    }

    CommandContext::~CommandContext() {
        m_psoMngr.Shutdown();
        m_frameResourceMngr.Shutdown();
        m_resourceMngr.Shutdown();

        m_CopyQueue.Shutdown();
        m_ComputeQueue.Shutdown();
        m_GraphicsQueue.Shutdown();
    }

    void CommandContext::BeginFrame() {
    }

    void CommandContext::Execute() {
        for (auto render_pass : m_RenderPath)
            render_pass->Execute();
    }

    void CommandContext::EndFrame() {
    }

    void CommandContext::OnResize() {
        for (auto render_pass : m_RenderPath)
            render_pass->OnResize();
    }

} // namespace Natsu::GFX