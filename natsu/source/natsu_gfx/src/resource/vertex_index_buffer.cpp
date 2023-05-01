#include "natsu_gfx/resource/vertex_index_buffer.h"

#include "natsu_gfx/util/meta_lib.h"

using namespace DirectX;

namespace Natsu::GFX {
    /*
        VertexBuffer
    */

    VertexBuffer::VertexBuffer(ID3D12Device* device, float* vertices, uint32_t vertex_count) {
        VertexBufferLayout buffer_layout;
        uint32_t           byte_size   = vertex_count * buffer_layout.structSize;
        size_t             ARRAY_COUNT = byte_size / sizeof(float);
        std::vector<vbyte> vertex_data(byte_size);
        vbyte*             vertex_dataPtr = vertex_data.data();
        for (size_t i = 0; i < ARRAY_COUNT; i += buffer_layout.structSize / sizeof(float)) {
            // position
            XMFLOAT3 position{vertices[i], vertices[i + 1], vertices[i + 2]};
            buffer_layout.position.Get(vertex_dataPtr) = position;
            // tex_coord
            XMFLOAT2 tex_coord{vertices[i + 3], vertices[i + 4]};
            buffer_layout.tex_coord.Get(vertex_dataPtr) = tex_coord;
            // normal
            XMFLOAT3 normal{vertices[i + 5], vertices[i + 6], vertices[i + 7]};
            buffer_layout.normal.Get(vertex_dataPtr) = normal;
            // tangent
            XMFLOAT3 tangent{vertices[i + 8], vertices[i + 9], vertices[i + 10]};
            buffer_layout.tangent.Get(vertex_dataPtr) = tangent;

            vertex_dataPtr += buffer_layout.structSize;
        }

        m_vertex_upload = new UploadBuffer(device, vertex_data.size());
        m_vertex_upload->CopyData(0, vertex_data);
    }

    VertexBuffer::~VertexBuffer() {
        delete m_vertex_upload;
    }

    void VertexBuffer::Bind(ID3D12GraphicsCommandList* cmdlist, Mesh* mesh) const {
        cmdlist->CopyBufferRegion(
            mesh->VertexBuffers()[0].GetResource(),
            0,
            m_vertex_upload->GetResource(),
            0,
            m_vertex_upload->GetByteSize());
    }

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(ID3D12Device* device, float* vertices, uint32_t vertex_count) {
        return std::make_shared<VertexBuffer>(device, vertices, vertex_count);
    }

    /*
        IndexBuffer
    */

    IndexBuffer::IndexBuffer(ID3D12Device* device, uint32_t* indices, uint32_t index_count) :
        m_index_count(index_count) {
        uint32_t byte_size = index_count * sizeof(uint32_t);
        m_index_upload     = new UploadBuffer(device, byte_size);
        m_index_upload->CopyData(0, {reinterpret_cast<vbyte const*>(indices), byte_size});
    }

    IndexBuffer::~IndexBuffer() {
        delete m_index_upload;
    }

    void IndexBuffer::Bind(ID3D12GraphicsCommandList* cmdlist, Mesh* mesh) const {
        cmdlist->CopyBufferRegion(
            mesh->IndexBuffer().GetResource(),
            0,
            m_index_upload->GetResource(),
            0,
            m_index_upload->GetByteSize());
    }

    std::shared_ptr<IndexBuffer> IndexBuffer::Create(ID3D12Device* device, uint32_t* indices, uint32_t index_count) {
        return std::make_shared<IndexBuffer>(device, indices, index_count);
    }
} // namespace Natsu::GFX