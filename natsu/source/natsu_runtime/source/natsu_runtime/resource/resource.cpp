#include "natsu_runtime/resource/resource.h"

#include "natsu_runtime/core/base/hash.h"

namespace Natsu::Runtime {
    IResource::IResource(std::string_view name) :
        m_name(name) {
    }

    std::string_view IResource::getName() const { return m_name; }
    size_t           IResource::getUUID() const { return Hash(m_name); }

    // ****************************************************************************

    template class Resource<ResourceType::Mesh>;
    template class Resource<ResourceType::Texture>;
} // namespace Natsu::Runtime