#pragma once

#include <atlbase.h>

#include "natsu_gfx/pch.h"

#include "natsu_gfx/resource/resource.h"
#include "natsu_gfx/util/var_size_alloc_mngr.h"

namespace Natsu::GFX {
    class DescriptorHeapAllocation;
    class DynamicSuballocationsManager;

    class IDescriptorAllocator {
    public:
        // Allocate Count descriptors
        virtual DescriptorHeapAllocation Allocate(uint32_t Count)                    = 0;
        virtual void                     Free(DescriptorHeapAllocation&& Allocation) = 0;
        virtual uint32_t                 GetDescriptorSize() const                   = 0;
    };

    class DescriptorHeapAllocation : public Resource {
    public:
        // Creates null allocation
        DescriptorHeapAllocation() = default;

        // Initializes non-null allocation
        DescriptorHeapAllocation(
            IDescriptorAllocator*       pAllocator,
            ID3D12DescriptorHeap*       pHeap,
            D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle,
            D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle,
            uint32_t                    NHandles,
            uint16_t                    AllocationManagerId);

        // Move constructor
        DescriptorHeapAllocation(DescriptorHeapAllocation&& Allocation) noexcept;

        // Move assignment
        DescriptorHeapAllocation& operator=(DescriptorHeapAllocation&& Allocation) noexcept;

        // copy/copy assignment is not allowed.
        DescriptorHeapAllocation(const DescriptorHeapAllocation&)            = delete;
        DescriptorHeapAllocation& operator=(const DescriptorHeapAllocation&) = delete;

        ~DescriptorHeapAllocation();

        void Reset() noexcept;

        // Returns CPU descriptor handle at the specified offset
        D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t Offset = 0) const {
            D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = m_FirstCpuHandle;
            if (Offset != 0)
                CPUHandle.ptr += m_DescriptorSize * Offset;
            return CPUHandle;
        }

        // Returns GPU descriptor handle at the specified offset
        D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t Offset = 0) const {
            D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = m_FirstGpuHandle;
            if (Offset != 0)
                GPUHandle.ptr += m_DescriptorSize * Offset;
            return GPUHandle;
        }

        // Returns pointer to the descriptor heap that contains this allocation
        ID3D12DescriptorHeap* GetDescriptorHeap() { return m_pDescriptorHeap; }

        size_t GetNumHandles() const { return m_NumHandles; }

        bool IsNull() const { return m_FirstCpuHandle.ptr == 0; }
        bool IsShaderVisible() const { return m_FirstGpuHandle.ptr != 0; }

        size_t GetAllocationManagerId() { return m_AllocationManagerId; }
        UINT   GetDescriptorSize() const { return m_DescriptorSize; }

    private:
        // First CPU descriptor handle in this allocation
        D3D12_CPU_DESCRIPTOR_HANDLE m_FirstCpuHandle{0};

        // First GPU descriptor handle in this allocation
        D3D12_GPU_DESCRIPTOR_HANDLE m_FirstGpuHandle{0};

        // Pointer to the descriptor heap allocator that created this allocation
        IDescriptorAllocator* m_pAllocator{nullptr};

        // Pointer to the D3D12 descriptor heap that contains descriptors in this allocation
        ID3D12DescriptorHeap* m_pDescriptorHeap{nullptr};

        // Number of descriptors in the allocation
        uint32_t m_NumHandles = 0;

        // Allocation manager ID
        // Indicates which AllocMngr this allocation is included in.
        uint16_t m_AllocationManagerId = static_cast<uint16_t>(-1);

        // Descriptor size
        uint16_t m_DescriptorSize = 0;
    };

    struct DescriptorHeapAllocationPack {
        DescriptorHeapAllocation     heap;
        std::vector<uint32_t>        free;
        std::unordered_set<uint32_t> used;
    };

    /*
    Every allocation that the class creates is represented by an instance of DescriptorHeapAllocation class.
    The list of free descriptors is managed by m_FreeBlocksManager member
*/
    class DescriptorHeapAllocationMngr {
    public:
        // Creates a new D3D12 descriptor heap
        DescriptorHeapAllocationMngr(
            ID3D12Device*                     pDevice,
            IDescriptorAllocator&             pParentAllocator,
            size_t                            ThisManagerId,
            const D3D12_DESCRIPTOR_HEAP_DESC& HeapDesc);

        // Uses subrange of descriptors in the existing D3D12 descriptor heap
        // that starts at offset FirstDescriptor and uses NumDescriptors descriptors
        DescriptorHeapAllocationMngr(
            ID3D12Device*         pDevice,
            IDescriptorAllocator& ParentAllocator,
            size_t                ThisManagerId,
            ID3D12DescriptorHeap* pd3d12Descriptor,
            uint32_t              FirstDescriptor,
            uint32_t              NumDescriptors);

        // Move constructor
        DescriptorHeapAllocationMngr(DescriptorHeapAllocationMngr&& rhs) noexcept;

        DescriptorHeapAllocationMngr& operator=(DescriptorHeapAllocationMngr&& rhs)  = delete;
        DescriptorHeapAllocationMngr(const DescriptorHeapAllocationMngr&)            = delete;
        DescriptorHeapAllocationMngr& operator=(const DescriptorHeapAllocationMngr&) = delete;

        ~DescriptorHeapAllocationMngr() = default;

        // Allocates Count descriptors
        DescriptorHeapAllocation Allocate(uint32_t Count);

        // Releases descriptor heap allocation. Note
        // that the allocation is not released immediately, but
        // added to the release queue in the allocations manager
        void Free(DescriptorHeapAllocation&& Allocation);

        void FreeAllocation(DescriptorHeapAllocation&& Allocation);

        size_t GetNumAvailableDescriptors() const { return m_FreeBlockManager.GetFreeSize(); }

        uint32_t GetMaxDescriptors() const { return m_NumDescriptorsInAllocation; }

    private:
        std::mutex            m_AllocationMutex;
        ID3D12Device*         m_pDevice = nullptr;
        IDescriptorAllocator& m_ParentAllocator;

        // External ID assigned to this descriptor allocations manager
        size_t m_ThisManagerId = static_cast<size_t>(-1);

        // Heap description
        D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;

        UINT m_DescriptorSize = 0;

        // Number of descriptors in the all allocation.
        // If this manager was initialized as a subrange in the existing heap,
        // this value may be different from m_HeapDesc.NumDescriptors
        uint32_t m_NumDescriptorsInAllocation = 0;

        // First CPU descriptor handle in the available descriptor range
        D3D12_CPU_DESCRIPTOR_HANDLE m_FirstCPUHandle = {0};

        // First GPU descriptor handle in the available descriptor range
        D3D12_GPU_DESCRIPTOR_HANDLE m_FirstGPUHandle = {0};

        // Allocations manager used to handle descriptor allocations within the heap
        Util::VarSizeAllocMngr m_FreeBlockManager;

        // Strong reference to D3D12 descriptor heap object
        CComPtr<ID3D12DescriptorHeap> m_pd3d12DescriptorHeap;
    };

    // CPU descriptor heap is intended to provide storage for resource view descriptor handles.
    // It contains a pool of DescriptorHeapAllocationMngr object instances, where every instance manages
    // its own CPU-only D3D12 descriptor heap:
    //
    //           m_HeapPool[0]                m_HeapPool[1]                 m_HeapPool[2]
    //   |  X  X  X  X  X  X  X  X |, |  X  X  X  O  O  X  X  O  |, |  X  O  O  O  O  O  O  O  |
    //
    //    X - used descriptor                m_AvailableHeaps = {1,2}
    //    O - available descriptor
    //
    // Allocation routine goes through the list of managers that have available descriptors and tries to process
    // the request using every manager. If there are no available managers or no manager was able to handle the request,
    // the function creates a new descriptor heap manager and lets it handle the request
    //
    // Render device contains four CPUDescriptorHeap object instances (one for each D3D12 heap type). The heaps are accessed
    // when a texture or a buffer view is created.

    class CPUDescriptorHeap final : public IDescriptorAllocator {
    public:
        // Initializes the heap
        CPUDescriptorHeap(ID3D12Device*               pDevice,
                          uint32_t                    NumDescriptorsInHeap,
                          D3D12_DESCRIPTOR_HEAP_TYPE  Type,
                          D3D12_DESCRIPTOR_HEAP_FLAGS Flags);

        CPUDescriptorHeap(const CPUDescriptorHeap&)            = delete;
        CPUDescriptorHeap(CPUDescriptorHeap&&)                 = delete;
        CPUDescriptorHeap& operator=(const CPUDescriptorHeap&) = delete;
        CPUDescriptorHeap& operator=(CPUDescriptorHeap&&)      = delete;

        ~CPUDescriptorHeap();

        virtual DescriptorHeapAllocation Allocate(uint32_t Count) override final;
        virtual void                     Free(DescriptorHeapAllocation&& Allocation) override final;

        virtual uint32_t GetDescriptorSize() const noexcept override final { return m_DescriptorSize; }

    private:
        ID3D12Device* m_pDevice;

        // Pool of descriptor heap managers
        std::mutex                                m_HeapPoolMutex;
        std::vector<DescriptorHeapAllocationMngr> m_HeapPool;
        // Indices of available descriptor heap managers
        std::unordered_set<size_t> m_AvailableHeaps;

        D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
        const UINT                 m_DescriptorSize = 0;

        // Maximum heap size during the application lifetime - for statistic purposes
        uint32_t m_MaxSize     = 0;
        uint32_t m_CurrentSize = 0;
    };

    // GPU descriptor heap provides storage for shader-visible descriptors
    // The heap contains single D3D12 descriptor heap that is split into two parts.
    // The first part stores static and mutable resource descriptor handles.
    // The second part is intended to provide temporary storage for dynamic resources.
    // Space for dynamic resources is allocated in chunks, and then descriptors are suballocated within every
    // chunk. DynamicSuballocMngr facilitates this process.
    //
    //
    //     static and mutable handles      ||                 dynamic space
    //                                     ||    chunk 0     chunk 1     chunk 2     unused
    //  | X O O X X O X O O O O X X X X O  ||  | X X X O | | X X O O | | O O O O |  O O O O  ||
    //                                               |         |
    //                                     suballocation       suballocation
    //                                    within chunk 0       within chunk 1
    //
    // Render device contains two GPUDescriptorHeap instances (CBV_SRV_UAV and SAMPLER). The heaps
    // are used to allocate GPU-visible descriptors for shader resource binding objects. The heaps
    // are also used by the command contexts (through DynamicSuballocMngr to allocated dynamic descriptors)
    //
    //  _______________________________________________________________________________________________________________________________
    // | Render Device                                                                                                                 |
    // |                                                                                                                               |
    // | m_CPUDescriptorHeaps[CBV_SRV_UAV] |  X  X  X  X  X  X  X  X  |, |  X  X  X  X  X  X  X  X  |, |  X  O  O  X  O  O  O  O  |    |
    // | m_CPUDescriptorHeaps[SAMPLER]     |  X  X  X  X  O  O  O  X  |, |  X  O  O  X  O  O  O  O  |                                  |
    // | m_CPUDescriptorHeaps[RTV]         |  X  X  X  O  O  O  O  O  |, |  O  O  O  O  O  O  O  O  |                                  |
    // | m_CPUDescriptorHeaps[DSV]         |  X  X  X  O  X  O  X  O  |                                                                |
    // |                                                                               ctx1        ctx2                                |
    // | m_GPUDescriptorHeaps[CBV_SRV_UAV]  | X O O X X O X O O O O X X X X O  ||  | X X X O | | X X O O | | O O O O |  O O O O  ||    |
    // | m_GPUDescriptorHeaps[SAMPLER]      | X X O O X O X X X O O X O O O O  ||  | X X O O | | X O O O | | O O O O |  O O O O  ||    |
    // |                                                                                                                               |
    // |_______________________________________________________________________________________________________________________________|
    //
    //  ________________________________________________               ________________________________________________
    // |Device Context 1                                |             |Device Context 2                                |
    // |                                                |             |                                                |
    // | m_DynamicGPUDescriptorAllocator[CBV_SRV_UAV]   |             | m_DynamicGPUDescriptorAllocator[CBV_SRV_UAV]   |
    // | m_DynamicGPUDescriptorAllocator[SAMPLER]       |             | m_DynamicGPUDescriptorAllocator[SAMPLER]       |
    // |________________________________________________|             |________________________________________________|
    //
    class GPUDescriptorHeap final : public IDescriptorAllocator {
    public:
        GPUDescriptorHeap(ID3D12Device*               pDevice,
                          uint32_t                    NumDescriptorsInHeap,
                          uint32_t                    NumDynamicDescriptors,
                          D3D12_DESCRIPTOR_HEAP_TYPE  Type,
                          D3D12_DESCRIPTOR_HEAP_FLAGS Flags);

        GPUDescriptorHeap(const GPUDescriptorHeap&)            = delete;
        GPUDescriptorHeap(GPUDescriptorHeap&&)                 = delete;
        GPUDescriptorHeap& operator=(const GPUDescriptorHeap&) = delete;
        GPUDescriptorHeap& operator=(GPUDescriptorHeap&&)      = delete;

        virtual DescriptorHeapAllocation Allocate(uint32_t count) override final { return m_HeapAllocationManager.Allocate(count); }

        DescriptorHeapAllocation AllocateDynamic(uint32_t count) { return m_DynamicAllocationsManager.Allocate(count); }

        virtual void     Free(DescriptorHeapAllocation&&) override final;
        virtual uint32_t GetDescriptorSize() const override final { return m_DescriptorSize; }

        const D3D12_DESCRIPTOR_HEAP_DESC& GetHeapDesc() const noexcept { return m_HeapDesc; }
        uint32_t                          GetMaxStaticDescriptors() const noexcept { return m_HeapAllocationManager.GetMaxDescriptors(); }
        uint32_t                          GetMaxDynamicDescriptors() const noexcept { return m_DynamicAllocationsManager.GetMaxDescriptors(); }
        ID3D12DescriptorHeap*             GetDescriptorHeap() const noexcept { return m_pd3d12DescriptorHeap.p; }

    protected:
        ID3D12Device* m_Device;

        const D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
        CComPtr<ID3D12DescriptorHeap>    m_pd3d12DescriptorHeap;

        const UINT m_DescriptorSize;

        static constexpr size_t StaticHeapAllocatonManagerID  = 0;
        static constexpr size_t DynamicHeapAllocatonManagerID = 1;

        // Allocation manager for static/mutable part
        DescriptorHeapAllocationMngr m_HeapAllocationManager;

        // Allocation manager for dynamic part
        DescriptorHeapAllocationMngr m_DynamicAllocationsManager;
    };

    // The class facilitates allocation of dynamic descriptor handles. It requests a chunk of heap
    // from the master GPU descriptor heap and then performs linear suballocation within the chunk
    // At the end of the frame all allocations are disposed.

    //     static and mutable handles     ||                 dynamic space
    //                                    ||    chunk 0                 chunk 2
    //  |                                 ||  | X X X O |             | O O O O |           || GPU Descriptor Heap
    //                                        |                       |
    //                                        m_Suballocations[0]     m_Suballocations[1]
    //
    class DynamicSuballocMngr final : public IDescriptorAllocator {
    public:
        DynamicSuballocMngr(
            GPUDescriptorHeap* ParentGPUHeap,
            uint32_t           DynamicChunkSize,
            std::string        ManagerName) noexcept;

        DynamicSuballocMngr(const DynamicSuballocMngr&)            = delete;
        DynamicSuballocMngr(DynamicSuballocMngr&&)                 = delete;
        DynamicSuballocMngr& operator=(const DynamicSuballocMngr&) = delete;
        DynamicSuballocMngr& operator=(DynamicSuballocMngr&&)      = delete;

        ~DynamicSuballocMngr();

        virtual DescriptorHeapAllocation Allocate(uint32_t count) override final;
        virtual void                     Free(DescriptorHeapAllocation&& allocation) noexcept override final {
            // Do nothing. Dynamic allocations are not disposed individually, but as whole chunks
            // at the end of the frame by ReleaseAllocations()
            allocation.Reset();
        }

        void ReleaseAllocations();

        virtual uint32_t GetDescriptorSize() const noexcept override final { return m_ParentGPUHeap->GetDescriptorSize(); }

        size_t GetSuballocationCount() const noexcept { return m_Suballocations.size(); }

    private:
        // Parent GPU descriptor heap that is used to allocate chunks
        GPUDescriptorHeap* m_ParentGPUHeap;
        std::string        m_ManagerName;

        // List of chunks allocated from the master GPU descriptor heap. All chunks are disposed at the end
        // of the frame
        std::vector<DescriptorHeapAllocation> m_Suballocations;

        uint32_t m_CurrentSuballocationOffset = 0;
        uint32_t m_DynamicChunkSize           = 0;

        uint32_t m_CurrDescriptorCount         = 0;
        uint32_t m_PeakDescriptorCount         = 0;
        uint32_t m_CurrSuballocationsTotalSize = 0;
        uint32_t m_PeakSuballocationsTotalSize = 0;
    };
} // namespace Natsu::GFX