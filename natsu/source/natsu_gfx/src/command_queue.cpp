#include "natsu_gfx/command_queue.h"

namespace Natsu::GFX {
    CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE Type) :
        m_Type(Type),
        m_CommandQueue(nullptr) {
    }

    void CommandQueue::Shutdown() {
        if (m_CommandQueue == nullptr)
            return;

        m_CommandQueue->Release();
        m_CommandQueue = nullptr;
    }

    void CommandQueue::Create(ID3D12Device* pDevice) {
        assert(pDevice != nullptr);
        assert(!IsReady());

        D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
        QueueDesc.Type                     = m_Type;
        QueueDesc.NodeMask                 = 1;
        pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&m_CommandQueue));
        m_CommandQueue->SetName(L"m_CommandQueue");

        assert(IsReady());
    }

    void CommandQueue::Execute(ID3D12GraphicsCommandList* list) {
        const std::array<ID3D12CommandList*, 1> listArr = {list};
        m_CommandQueue->ExecuteCommandLists(1, listArr.data());
    }
} // namespace Natsu::GFX