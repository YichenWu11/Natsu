#pragma once

#include "natsu_gfx/pch.h"

#include "natsu_gfx/descriptor_heap.h"

namespace Natsu::GFX {
    class DescriptorHeapMngr {
    public:
        static DescriptorHeapMngr& GetInstance() noexcept {
            static DescriptorHeapMngr instance;
            return instance;
        }

        void Init(
            ID3D12Device* device,
            uint32_t      numCpuCSU,
            uint32_t      numCpuRTV,
            uint32_t      numCpuDSV,
            uint32_t      numGpuCSU_static,
            uint32_t      numGpuCSU_dynamic);

        CPUDescriptorHeap* GetCSUCpuDH() const noexcept {
            assert(isInit);
            return CSU_CpuDH;
        }
        CPUDescriptorHeap* GetRTVCpuDH() const noexcept {
            assert(isInit);
            return RTV_CpuDH;
        }
        CPUDescriptorHeap* GetDSVCpuDH() const noexcept {
            assert(isInit);
            return DSV_CpuDH;
        }
        GPUDescriptorHeap* GetCSUGpuDH() const noexcept {
            assert(isInit);
            return CSU_GpuDH;
        }

        void Clear();

    private:
        DescriptorHeapMngr() = default;
        ~DescriptorHeapMngr();

        bool isInit{false};

        // CSU ---> CBV_SRV_UAV
        CPUDescriptorHeap* CSU_CpuDH{nullptr};
        CPUDescriptorHeap* RTV_CpuDH{nullptr};
        CPUDescriptorHeap* DSV_CpuDH{nullptr};
        GPUDescriptorHeap* CSU_GpuDH{nullptr};
    };
} // namespace Natsu::GFX