#pragma once

#include "natsu_gfx/pch.h"

namespace Natsu::GFX {
    class CommandQueue {
        friend class CommandContext;

    public:
        CommandQueue(D3D12_COMMAND_LIST_TYPE Type);
        ~CommandQueue() = default;

        ID3D12CommandQueue*       operator->() { return m_CommandQueue; }
        const ID3D12CommandQueue* operator->() const { return m_CommandQueue; }

        void Create(ID3D12Device* pDevice);
        void Shutdown();

        inline bool IsReady() {
            return m_CommandQueue != nullptr;
        }

        ID3D12CommandQueue* GetCommandQueue() { return m_CommandQueue; }

        void Execute(ID3D12GraphicsCommandList* list);

    private:
        ID3D12CommandQueue* m_CommandQueue;

        const D3D12_COMMAND_LIST_TYPE m_Type;
    };

} // namespace Natsu::GFX