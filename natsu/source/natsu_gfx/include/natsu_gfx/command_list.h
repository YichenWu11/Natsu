#pragma once

#include "natsu_gfx/pch.h"

namespace Natsu::GFX {
    class GraphicsCommandList {
    public:
        GraphicsCommandList(D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT);
        ~GraphicsCommandList();

        ID3D12GraphicsCommandList*       operator->() { return m_CommandList; }
        const ID3D12GraphicsCommandList* operator->() const { return m_CommandList; }

        void Create(ID3D12Device* pDevice, ID3D12CommandAllocator* allocator);
        void Shutdown();

        inline bool IsReady() {
            return m_CommandList != nullptr;
        }

        ID3D12GraphicsCommandList* GetCommandList() { return m_CommandList; }

        void Reset(ID3D12CommandAllocator* pAllocator);

        void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const FLOAT color[4]);
        void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView);

        void RSSetViewport(D3D12_VIEWPORT viewport);
        void RSSetScissorRect(D3D12_RECT rect);

        // simple version
        // pro: OMSetRenderTargets
        void OMSetRenderTarget(
            D3D12_CPU_DESCRIPTOR_HANDLE rendertarget,
            D3D12_CPU_DESCRIPTOR_HANDLE depthstencil);

        // one instance
        void DrawIndexed(
            UINT IndexCount,
            UINT StartIndexLocation,
            INT  BaseVertexLocation);

    private:
        ID3D12GraphicsCommandList* m_CommandList;

        const D3D12_COMMAND_LIST_TYPE m_Type;
    };
} // namespace Natsu::GFX