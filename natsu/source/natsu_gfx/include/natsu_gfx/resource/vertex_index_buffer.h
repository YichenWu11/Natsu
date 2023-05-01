#pragma once

#include "natsu_gfx/resource/buffer.h"
#include "natsu_gfx/resource/mesh.h"
#include "natsu_gfx/resource/upload_buffer.h"
#include "natsu_gfx/util/reflactable_struct.h"

namespace Natsu::GFX {
    struct VertexBufferLayout : public rtti::Struct {
        rtti::Var<DirectX::XMFLOAT3> position  = "POSITION";
        rtti::Var<DirectX::XMFLOAT2> tex_coord = "TEXCOORD";
        rtti::Var<DirectX::XMFLOAT3> normal    = "NORMAL";
        rtti::Var<DirectX::XMFLOAT3> tangent   = "TANGENT";
    };

    struct VertexData {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 tex_coord;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT3 tangent;
    };

    class VertexBuffer {
    public:
        VertexBuffer(ID3D12Device* device, float* vertices, uint32_t vertex_count);
        ~VertexBuffer();

        void Bind(ID3D12GraphicsCommandList* cmdlist, Mesh* mesh) const; // bind to mesh (copy data to mesh)

        void DelayDispose();

        static std::shared_ptr<VertexBuffer> Create(ID3D12Device* device, float* vertices, uint32_t vertex_count);

    private:
        UploadBuffer* m_vertex_upload;
    };

    // only support `uint32_t` IndexBuffer
    class IndexBuffer {
    public:
        IndexBuffer(ID3D12Device* device, uint32_t* indices, uint32_t index_count);
        ~IndexBuffer();

        void     Bind(ID3D12GraphicsCommandList* cmdlist, Mesh* mesh) const; // bind to mesh (copy data to mesh)
        uint32_t GetCount() const { return m_index_count; }

        static std::shared_ptr<IndexBuffer> Create(ID3D12Device* device, uint32_t* indices, uint32_t index_count);

    private:
        uint32_t      m_index_count;
        UploadBuffer* m_index_upload;
    };
} // namespace Natsu::GFX