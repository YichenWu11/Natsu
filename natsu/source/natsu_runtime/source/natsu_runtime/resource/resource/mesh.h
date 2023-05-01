#pragma once

#include "natsu_gfx/resource/mesh.h"
#include "natsu_gfx/resource/vertex_index_buffer.h"
#include "natsu_runtime/resource/resource.h"

namespace Natsu::Runtime {
    template <>
    class Resource<ResourceType::Mesh> final : public IResource {
    public:
        struct ResLoader final {
            using res_type = std::unique_ptr<Resource<ResourceType::Mesh>>;

            res_type operator()(const std::string& name) const {
                return std::make_unique<Resource<ResourceType::Mesh>>(name);
            }

            res_type operator()(const std::string& name,
                                GFX::VertexData*   vertices,
                                uint32_t           vertices_count,
                                uint32_t*          indices,
                                uint32_t           indices_count) const {
                return std::make_unique<Resource<ResourceType::Mesh>>(name, vertices, vertices_count, indices, indices_count);
            }
        };

    public:
        Resource(const std::string& name);
        Resource(const std::string& name,
                 GFX::VertexData*   vertices,
                 uint32_t           vertices_count,
                 uint32_t*          indices,
                 uint32_t           indices_count);

        bool Validate() const { return m_is_valid; }

        GFX::Mesh* GetMesh() { return m_mesh.get(); }

    private:
        std::unique_ptr<GFX::Mesh> m_mesh;
    };
} // namespace Natsu::Runtime