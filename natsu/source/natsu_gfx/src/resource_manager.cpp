#include "natsu_gfx/resource_manager.h"

namespace Natsu::GFX {
    void ResourceManager::Create(ID3D12Device* _device) {
        device       = _device;
        csuDynamicDH = new DynamicSuballocMngr{
            DescriptorHeapMngr::GetInstance().GetCSUGpuDH(),
            256,
            "ResourceManager::csuDynamicDH"};
        DHReserve();
    }

    void ResourceManager::Shutdown() {
        if (!csuDH.IsNull())
            DescriptorHeapMngr::GetInstance().GetCSUGpuDH()->Free(std::move(csuDH));
        if (!rtvDH.IsNull())
            DescriptorHeapMngr::GetInstance().GetRTVCpuDH()->Free(std::move(rtvDH));
        if (!dsvDH.IsNull())
            DescriptorHeapMngr::GetInstance().GetDSVCpuDH()->Free(std::move(dsvDH));
        csuDynamicDH->ReleaseAllocations();
        delete csuDynamicDH;
    }

    void ResourceManager::CSUDHReserve(uint32_t num) {
        assert(csuDHused.empty());

        uint32_t origSize = csuDH.GetNumHandles();
        if (origSize >= num)
            return;

        if (!csuDH.IsNull())
            DescriptorHeapMngr::GetInstance().GetCSUGpuDH()->Free(std::move(csuDH));
        csuDH = DescriptorHeapMngr::GetInstance().GetCSUGpuDH()->Allocate(num);

        for (uint32_t i = origSize; i < num; i++)
            csuDHfree.push_back(i);
    }

    void ResourceManager::DsvDHReserve(uint32_t num) {
        assert(dsvDHused.empty());

        uint32_t origSize = dsvDH.GetNumHandles();
        if (dsvDH.GetNumHandles() >= num)
            return;

        if (!dsvDH.IsNull())
            DescriptorHeapMngr::GetInstance().GetDSVCpuDH()->Free(std::move(dsvDH));
        dsvDH = DescriptorHeapMngr::GetInstance().GetDSVCpuDH()->Allocate(num);

        for (uint32_t i = origSize; i < num; i++)
            dsvDHfree.push_back(i);
    }

    void ResourceManager::RtvDHReserve(uint32_t num) {
        assert(rtvDHused.empty());

        uint32_t origSize = rtvDH.GetNumHandles();
        if (origSize >= num)
            return;

        if (!rtvDH.IsNull())
            DescriptorHeapMngr::GetInstance().GetRTVCpuDH()->Free(std::move(rtvDH));
        rtvDH = DescriptorHeapMngr::GetInstance().GetRTVCpuDH()->Allocate(num);

        for (uint32_t i = origSize; i < num; i++)
            rtvDHfree.push_back(i);
    }

    void ResourceManager::DHReserve() {
        uint32_t numDSV = 32;
        uint32_t numRTV = 32;
        uint32_t numCSU = 128;

        CSUDHReserve(numCSU);
        RtvDHReserve(numRTV);
        DsvDHReserve(numDSV);
    }

    uint32_t ResourceManager::AllocateHandle(RsrcViewType type, const std::string& rsrcName) {
        switch (type) {
            case RTV: {
                auto idx = rtvDHfree.back();
                rtvDHfree.pop_back();
                rtvDHused.insert(idx);
                if (!rsrcName.empty())
                    m_passPubRsrcHanleMap[type][rsrcName] = idx;
                return idx;
            } break;
            case DSV: {
                auto idx = dsvDHfree.back();
                dsvDHfree.pop_back();
                dsvDHused.insert(idx);
                if (!rsrcName.empty())
                    m_passPubRsrcHanleMap[type][rsrcName] = idx;
                return idx;
            } break;
            case CSU: {
                auto idx = csuDHfree.back();
                csuDHfree.pop_back();
                csuDHused.insert(idx);
                if (!rsrcName.empty())
                    m_passPubRsrcHanleMap[type][rsrcName] = idx;
                return idx;
            } break;
        }
        return -1;
    }

    void ResourceManager::FreeHandle(RsrcViewType type, uint32_t offset) {
        switch (type) {
            case RTV: {
                rtvDHfree.push_back(offset);
                rtvDHused.erase(rtvDHused.find(offset));
                std::string to_erase;
                for (auto& ele : m_passPubRsrcHanleMap[type]) {
                    if (ele.second == offset) {
                        to_erase = ele.first;
                        break;
                    }
                }
                if (!to_erase.empty()) {
                    m_passPubRsrcHanleMap[type].erase(
                        m_passPubRsrcHanleMap[type].find(to_erase));
                }
            } break;
            case DSV: {
                dsvDHfree.push_back(offset);
                dsvDHused.erase(rtvDHused.find(offset));
                std::string to_erase;
                for (auto& ele : m_passPubRsrcHanleMap[type]) {
                    if (ele.second == offset) {
                        to_erase = ele.first;
                        break;
                    }
                }
                if (!to_erase.empty()) {
                    m_passPubRsrcHanleMap[type].erase(
                        m_passPubRsrcHanleMap[type].find(to_erase));
                }
            } break;
            case CSU: {
                csuDHfree.push_back(offset);
                csuDHused.erase(rtvDHused.find(offset));
                std::string to_erase;
                for (auto& ele : m_passPubRsrcHanleMap[type]) {
                    if (ele.second == offset) {
                        to_erase = ele.first;
                        break;
                    }
                }
                if (!to_erase.empty()) {
                    m_passPubRsrcHanleMap[type].erase(
                        m_passPubRsrcHanleMap[type].find(to_erase));
                }
            } break;
        }
    }

} // namespace Natsu::GFX