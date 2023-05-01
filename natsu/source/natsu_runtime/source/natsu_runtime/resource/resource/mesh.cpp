#include "natsu_runtime/resource/resource/mesh.h"

#include "natsu_runtime/function/render/render_system/render_system.h"

namespace Natsu::Runtime {
    static GFX::VertexBufferLayout               layout;
    static std::vector<GFX::rtti::Struct const*> structs;

    Resource<ResourceType::Mesh>::Resource(const std::string& name) :
        IResource(name) {
    }

    Resource<ResourceType::Mesh>::Resource(const std::string& name,
                                           GFX::VertexData*   vertices,
                                           uint32_t           vertices_count,
                                           uint32_t*          indices,
                                           uint32_t           indices_count) :
        IResource(name) {
        structs.clear();
        structs.emplace_back(&layout);

        ComPtr<ID3D12CommandAllocator>    m_cmdAllocator;
        ComPtr<ID3D12GraphicsCommandList> m_commandList;
        ID3D12Device*                     device =
            g_runtime_global_context.m_render_system->GetCommandContext()->m_device.DxDevice();

        ThrowIfFailed(
            device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_cmdAllocator.GetAddressOf())));
        ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
        ThrowIfFailed(m_commandList->Close());
        ThrowIfFailed(m_cmdAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(m_cmdAllocator.Get(), nullptr));

        m_mesh = std::make_unique<GFX::Mesh>(
            device,
            structs,
            vertices_count,
            indices_count);

        auto vert_buffer_square = GFX::VertexBuffer::Create(
            device,
            reinterpret_cast<float*>(vertices),
            vertices_count);

        auto indi_buffer_square = GFX::IndexBuffer::Create(
            device,
            indices,
            indices_count);

        vert_buffer_square->Bind(m_commandList.Get(), m_mesh.get());
        indi_buffer_square->Bind(m_commandList.Get(), m_mesh.get());

        ThrowIfFailed(m_commandList->Close());

        ID3D12CommandList* ppM_commandLists[] = {m_commandList.Get()};
        g_runtime_global_context.m_render_system->GetCommandContext()->m_GraphicsQueue->ExecuteCommandLists(array_count(ppM_commandLists), ppM_commandLists);
        g_runtime_global_context.m_render_system->GetCommandContext()->FlushCommandQueue();

        LOG_INFO("create mesh named {0} success!", name);
    }
} // namespace Natsu::Runtime