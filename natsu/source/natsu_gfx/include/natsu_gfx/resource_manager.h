#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/descriptor_heap.h"
#include "natsu_gfx/descriptor_heap_manager.h"
#include "natsu_gfx/device.h"

#include "natsu_gfx/resource/resource.h"
#include "natsu_gfx/resource/texture.h"

namespace Natsu::GFX {
    enum RsrcViewType {
        RTV,
        DSV,
        CSU
    };

    class ResourceManager {
    public:
        using PassRsrcMap =
            std::unordered_map<RsrcViewType, std::unordered_map<std::string, uint32_t>>;

    public:
        ResourceManager()  = default;
        ~ResourceManager() = default;

        void Create(ID3D12Device* _device);
        void Shutdown();
        void DHReserve();

        uint32_t AllocateHandle(RsrcViewType type, const std::string& rsrcName = {});
        void     FreeHandle(RsrcViewType type, uint32_t offset);

    private:
        void CSUDHReserve(uint32_t num);
        void DsvDHReserve(uint32_t num);
        void RtvDHReserve(uint32_t num);

    private:
        ID3D12Device* device{nullptr};

        DynamicSuballocMngr* csuDynamicDH{nullptr};

        DescriptorHeapAllocation     rtvDH;
        std::vector<uint32_t>        rtvDHfree;
        std::unordered_set<uint32_t> rtvDHused;

        DescriptorHeapAllocation     dsvDH;
        std::vector<uint32_t>        dsvDHfree;
        std::unordered_set<uint32_t> dsvDHused;

        DescriptorHeapAllocation     csuDH;
        std::vector<uint32_t>        csuDHfree;
        std::unordered_set<uint32_t> csuDHused;

        PassRsrcMap m_passPubRsrcHanleMap;
    };
} // namespace Natsu::GFX
