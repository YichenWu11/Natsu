#include "natsu_gfx/command_list.h"

namespace Natsu::GFX {
    GraphicsCommandList::GraphicsCommandList(D3D12_COMMAND_LIST_TYPE Type) :
        m_Type(Type),
        m_CommandList(nullptr) {
    }

    GraphicsCommandList::~GraphicsCommandList() {
        Shutdown();
    }

    void GraphicsCommandList::Create(ID3D12Device* pDevice, ID3D12CommandAllocator* allocator) {
        assert(pDevice != nullptr);
        assert(allocator != nullptr);
        assert(!IsReady());

        ThrowIfFailed(pDevice->CreateCommandList(
            0,
            m_Type,
            allocator,
            nullptr,
            IID_PPV_ARGS(&m_CommandList)));
        m_CommandList->SetName(L"m_CommandList");

        assert(IsReady());
    }

    void GraphicsCommandList::Shutdown() {
        if (m_CommandList == nullptr)
            return;

        m_CommandList->Release();
        m_CommandList = nullptr;
    }

    void GraphicsCommandList::Reset(ID3D12CommandAllocator* pAllocator) {
        ThrowIfFailed(m_CommandList->Reset(pAllocator, nullptr));
    }

    void GraphicsCommandList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const FLOAT color[4]) {
        m_CommandList->ClearRenderTargetView(RenderTargetView, color, 0, nullptr);
    }

    void GraphicsCommandList::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView) {
        m_CommandList->ClearDepthStencilView(DepthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
    }

    void GraphicsCommandList::RSSetViewport(D3D12_VIEWPORT viewport) {
        m_CommandList->RSSetViewports(1, &viewport);
    }

    void GraphicsCommandList::RSSetScissorRect(D3D12_RECT rect) {
        m_CommandList->RSSetScissorRects(1, &rect);
    }

    // simple version
    // pro: OMSetRenderTargets
    void GraphicsCommandList::OMSetRenderTarget(
        D3D12_CPU_DESCRIPTOR_HANDLE rendertarget,
        D3D12_CPU_DESCRIPTOR_HANDLE depthstencil) {
        m_CommandList->OMSetRenderTargets(1, &rendertarget, true, &depthstencil);
    }

    // one instance
    void GraphicsCommandList::DrawIndexed(
        UINT IndexCount,
        UINT StartIndexLocation,
        INT  BaseVertexLocation) {
        m_CommandList->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
    }

} // namespace Natsu::GFX