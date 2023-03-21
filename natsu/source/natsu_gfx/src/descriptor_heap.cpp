#include "natsu_gfx/descriptor_heap.h"

namespace Natsu::GFX {
    DescriptorHeapAllocation::DescriptorHeapAllocation(
        IDescriptorAllocator*       pAllocator,
        ID3D12DescriptorHeap*       pHeap,
        D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle,
        D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle,
        uint32_t                    NHandles,
        uint16_t                    AllocationManagerId) :
        m_FirstCpuHandle{CpuHandle},
        m_FirstGpuHandle{GpuHandle},
        m_pAllocator{pAllocator},
        m_pDescriptorHeap{pHeap},
        m_NumHandles{NHandles},
        m_AllocationManagerId{AllocationManagerId} {
        assert(m_pAllocator != nullptr && m_pDescriptorHeap != nullptr);
        auto DescriptorSize = m_pAllocator->GetDescriptorSize();
        m_DescriptorSize    = static_cast<uint16_t>(DescriptorSize);
    }

    DescriptorHeapAllocation::DescriptorHeapAllocation(DescriptorHeapAllocation&& Allocation) noexcept :
        m_FirstCpuHandle{Allocation.m_FirstCpuHandle},
        m_FirstGpuHandle{Allocation.m_FirstGpuHandle},
        m_pAllocator{Allocation.m_pAllocator},
        m_pDescriptorHeap{Allocation.m_pDescriptorHeap},
        m_NumHandles{Allocation.m_NumHandles},
        m_AllocationManagerId{Allocation.m_AllocationManagerId},
        m_DescriptorSize{Allocation.m_DescriptorSize} {
        Allocation.Reset();
    }

    DescriptorHeapAllocation& DescriptorHeapAllocation::operator=(DescriptorHeapAllocation&& Allocation) noexcept {
        m_FirstCpuHandle      = Allocation.m_FirstCpuHandle;
        m_FirstGpuHandle      = Allocation.m_FirstGpuHandle;
        m_pAllocator          = Allocation.m_pAllocator;
        m_pDescriptorHeap     = Allocation.m_pDescriptorHeap;
        m_NumHandles          = Allocation.m_NumHandles;
        m_AllocationManagerId = Allocation.m_AllocationManagerId;
        m_DescriptorSize      = Allocation.m_DescriptorSize;

        Allocation.Reset();

        return *this;
    }

    DescriptorHeapAllocation::~DescriptorHeapAllocation() {
        if (!IsNull() && m_pAllocator)
            m_pAllocator->Free(std::move(*this));
        // Allocation must have been disposed by the allocator
        assert("Non-null descriptor is being destroyed" && IsNull());
    }

    void DescriptorHeapAllocation::Reset() noexcept {
        m_FirstCpuHandle.ptr  = 0;
        m_FirstGpuHandle.ptr  = 0;
        m_pAllocator          = nullptr;
        m_pDescriptorHeap     = nullptr;
        m_NumHandles          = 0;
        m_AllocationManagerId = static_cast<uint16_t>(-1);
        m_DescriptorSize      = 0;
    }

    DescriptorHeapAllocationMngr::DescriptorHeapAllocationMngr(DescriptorHeapAllocationMngr&& rhs) noexcept :
        m_pDevice{rhs.m_pDevice},
        m_ParentAllocator{rhs.m_ParentAllocator},
        m_ThisManagerId{rhs.m_ThisManagerId},
        m_HeapDesc{rhs.m_HeapDesc},
        m_DescriptorSize{rhs.m_DescriptorSize},
        m_NumDescriptorsInAllocation{rhs.m_NumDescriptorsInAllocation},
        m_FirstCPUHandle{rhs.m_FirstCPUHandle},
        m_FirstGPUHandle{rhs.m_FirstGPUHandle},
        // Mutex is not movable
        // m_FreeBlockManagerMutex   { std::move(rhs.m_FreeBlockManagerMutex) },
        m_FreeBlockManager{std::move(rhs.m_FreeBlockManager)},
        m_pd3d12DescriptorHeap{std::move(rhs.m_pd3d12DescriptorHeap)} {
        rhs.m_NumDescriptorsInAllocation = 0; // Must be set to zero so that debug check in dtor passes
        rhs.m_ThisManagerId              = static_cast<size_t>(-1);
        rhs.m_FirstCPUHandle.ptr         = 0;
        rhs.m_FirstGPUHandle.ptr         = 0;
    }

    // Creates a new descriptor heap and reference the entire heap
    DescriptorHeapAllocationMngr::DescriptorHeapAllocationMngr(
        ID3D12Device*                     pDevice,
        IDescriptorAllocator&             ParentAllocator,
        size_t                            ThisManagerId,
        const D3D12_DESCRIPTOR_HEAP_DESC& HeapDesc) :
        m_pDevice{pDevice},
        m_ParentAllocator{ParentAllocator},
        m_ThisManagerId{ThisManagerId},
        m_HeapDesc{HeapDesc},
        m_DescriptorSize{pDevice->GetDescriptorHandleIncrementSize(m_HeapDesc.Type)},
        m_NumDescriptorsInAllocation{HeapDesc.NumDescriptors},
        m_FreeBlockManager{HeapDesc.NumDescriptors} {
        m_FirstCPUHandle.ptr = 0;
        m_FirstGPUHandle.ptr = 0;

        pDevice->CreateDescriptorHeap(
            &m_HeapDesc,
            __uuidof(m_pd3d12DescriptorHeap),
            reinterpret_cast<void**>(static_cast<ID3D12DescriptorHeap**>(&m_pd3d12DescriptorHeap)));
        m_FirstCPUHandle = m_pd3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        // if shader visible
        if (m_HeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
            m_FirstGPUHandle = m_pd3d12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    // Uses subrange of descriptors in the existing D3D12 descriptor heap
    // that starts at offset FirstDescriptor and uses NumDescriptors descriptors
    DescriptorHeapAllocationMngr::DescriptorHeapAllocationMngr(
        ID3D12Device*         pDevice,
        IDescriptorAllocator& ParentAllocator,
        size_t                ThisManagerId,
        ID3D12DescriptorHeap* pd3d12DescriptorHeap,
        uint32_t              FirstDescriptor,
        uint32_t              NumDescriptors) :
        m_pDevice{pDevice},
        m_ParentAllocator{ParentAllocator},
        m_ThisManagerId{ThisManagerId},
        m_HeapDesc{pd3d12DescriptorHeap->GetDesc()},
        m_DescriptorSize{pDevice->GetDescriptorHandleIncrementSize(m_HeapDesc.Type)},
        m_NumDescriptorsInAllocation{NumDescriptors},
        m_FreeBlockManager{NumDescriptors},
        m_pd3d12DescriptorHeap{pd3d12DescriptorHeap} {
        m_FirstCPUHandle = pd3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        m_FirstCPUHandle.ptr += m_DescriptorSize * FirstDescriptor;
        // if shader visible
        if (m_HeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
            m_FirstGPUHandle = pd3d12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
            m_FirstGPUHandle.ptr += m_DescriptorSize * FirstDescriptor;
        }
    }

    DescriptorHeapAllocation DescriptorHeapAllocationMngr::Allocate(uint32_t Count) {
        assert(Count > 0);

        std::lock_guard<std::mutex> LockGuard(m_AllocationMutex);

        // Use variable-size GPU allocations manager to allocate the requested number of descriptors
        auto DescriptorHandleOffset = m_FreeBlockManager.Allocate(Count);
        // allocation failed.
        if (DescriptorHandleOffset == Util::VarSizeGPUAllocMngr::InvalidOffset)
            return DescriptorHeapAllocation();

        // Compute the first CPU and GPU descriptor handles in the allocation by
        // offseting the first CPU and GPU descriptor handle in the range
        auto CPUHandle = m_FirstCPUHandle;
        CPUHandle.ptr += DescriptorHandleOffset * m_DescriptorSize;

        auto GPUHandle = m_FirstGPUHandle;
        // if shader visible
        if (m_HeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
            GPUHandle.ptr += DescriptorHandleOffset * m_DescriptorSize;

        return DescriptorHeapAllocation(
            &m_ParentAllocator,
            m_pd3d12DescriptorHeap,
            CPUHandle,
            GPUHandle,
            Count,
            static_cast<uint16_t>(m_ThisManagerId));
    }

    void DescriptorHeapAllocationMngr::FreeAllocation(DescriptorHeapAllocation&& allocation) {
        assert(allocation.GetAllocationManagerId() == m_ThisManagerId && "Invalid descriptor heap manager Id");

        if (allocation.IsNull())
            return;

        std::lock_guard<std::mutex> LockGuard(m_AllocationMutex);
        auto                        DescriptorOffset = (allocation.GetCpuHandle().ptr - m_FirstCPUHandle.ptr) / m_DescriptorSize;
        // Methods of VariableSizeAllocationsManager class are not thread safe!
        m_FreeBlockManager.Free(DescriptorOffset, allocation.GetNumHandles());

        // Clear the allocation
        allocation.Reset();
        allocation = DescriptorHeapAllocation();
    }

    void DescriptorHeapAllocationMngr::Free(DescriptorHeapAllocation&& Allocation) {
        std::lock_guard<std::mutex> LockGuard(m_AllocationMutex);
        auto                        DescriptorOffset = (Allocation.GetCpuHandle().ptr - m_FirstCPUHandle.ptr) / m_DescriptorSize;
        // Note that the allocation is not released immediately, but added to the release queue in the allocations manager
        m_FreeBlockManager.Free(DescriptorOffset, Allocation.GetNumHandles());
        // m_FreeBlockManager.Free(DescriptorOffset, Allocation.GetNumHandles(), m_pDeviceD3D12Impl->GetCurrentFrame());

        // Clear the allocation
        Allocation.Reset();
        Allocation = DescriptorHeapAllocation();
    }

    CPUDescriptorHeap::CPUDescriptorHeap(
        ID3D12Device*               pDevice,
        uint32_t                    NumDescriptorsInHeap,
        D3D12_DESCRIPTOR_HEAP_TYPE  Type,
        D3D12_DESCRIPTOR_HEAP_FLAGS Flags) :
        m_pDevice{pDevice},
        m_HeapDesc{
            Type,
            NumDescriptorsInHeap,
            Flags,
            1 // NodeMask
        },
        m_DescriptorSize{pDevice->GetDescriptorHandleIncrementSize(Type)} {
        // Create one pool
        m_HeapPool.emplace_back(m_pDevice, *this, 0, m_HeapDesc);
        m_AvailableHeaps.insert(0);
    }

    CPUDescriptorHeap::~CPUDescriptorHeap() {
        assert(m_CurrentSize == 0 && "Not all allocations released");
        assert(m_AvailableHeaps.size() == m_HeapPool.size() && "Not all descriptor heap pools are released");

#ifndef NDEBUG
        for (const auto& Heap : m_HeapPool)
            assert(Heap.GetNumAvailableDescriptors() == Heap.GetMaxDescriptors() && "Not all descriptors in the descriptor pool are released");
#endif // !NDEBUG
    }

    DescriptorHeapAllocation CPUDescriptorHeap::Allocate(uint32_t Count) {
        std::lock_guard<std::mutex> LockGuard(m_HeapPoolMutex);
        DescriptorHeapAllocation    Allocation;
        // Go through all descriptor heap managers that have free descriptors
        for (auto AvailableHeapIt = m_AvailableHeaps.begin(); AvailableHeapIt != m_AvailableHeaps.end(); ++AvailableHeapIt) {
            // Try to allocate descriptors using the current descriptor heap manager
            Allocation = m_HeapPool[*AvailableHeapIt].Allocate(Count);
            // Remove the manager from the pool if it has no more available descriptors
            if (m_HeapPool[*AvailableHeapIt].GetNumAvailableDescriptors() == 0)
                m_AvailableHeaps.erase(*AvailableHeapIt);

            // Terminate the loop if descriptor was successfully allocated, otherwise
            // go to the next manager
            if (Allocation.GetCpuHandle().ptr != 0)
                break;
        }

        // If there were no available descriptor heap managers or no manager was able
        // to suffice the allocation request, create a new manager
        if (Allocation.GetCpuHandle().ptr == 0) {
            // Make sure the heap is large enough to accomodate the requested number of descriptors
            if (static_cast<UINT>(Count) > m_HeapDesc.NumDescriptors) m_HeapDesc.NumDescriptors = static_cast<UINT>(Count);
            // Create a new descriptor heap manager. Note that this constructor creates a new D3D12 descriptor
            // heap and references the entire heap. Pool index is used as manager ID
            m_HeapPool.emplace_back(m_pDevice, *this, m_HeapPool.size(), m_HeapDesc);
            auto NewHeapIt = m_AvailableHeaps.insert(m_HeapPool.size() - 1);

            // Use the new manager to allocate descriptor handles
            Allocation = m_HeapPool[*NewHeapIt.first].Allocate(Count);
        }

        m_CurrentSize += (Allocation.GetCpuHandle().ptr != 0) ? Count : 0;
        m_MaxSize = (m_CurrentSize > m_MaxSize) ? m_CurrentSize : m_MaxSize;

        return Allocation;
    }

    void CPUDescriptorHeap::Free(DescriptorHeapAllocation&& Allocation) {
        std::lock_guard<std::mutex> LockGuard(m_HeapPoolMutex);
        auto                        ManagerId = Allocation.GetAllocationManagerId();
        m_CurrentSize -= static_cast<uint32_t>(Allocation.GetNumHandles());
        m_HeapPool[ManagerId].Free(std::move(Allocation));
    }

    GPUDescriptorHeap::GPUDescriptorHeap(
        ID3D12Device*               device,
        uint32_t                    NumDescriptorsInHeap,
        uint32_t                    NumDynamicDescriptors,
        D3D12_DESCRIPTOR_HEAP_TYPE  Type,
        D3D12_DESCRIPTOR_HEAP_FLAGS Flags) :
        m_Device{device},
        m_HeapDesc{
            Type,
            NumDescriptorsInHeap + NumDynamicDescriptors,
            Flags,
            1 // UINT NodeMask;
        },
        m_pd3d12DescriptorHeap{
            [&] {
        CComPtr<ID3D12DescriptorHeap> pHeap;
        device->CreateDescriptorHeap(&m_HeapDesc, __uuidof(pHeap), reinterpret_cast<void**>(&pHeap));
        return pHeap;
        }()},
    m_DescriptorSize{device->GetDescriptorHandleIncrementSize(Type)},
    m_HeapAllocationManager{device, *this, StaticHeapAllocatonManagerID, m_pd3d12DescriptorHeap, 0, NumDescriptorsInHeap},
    m_DynamicAllocationsManager{device, *this, DynamicHeapAllocatonManagerID, m_pd3d12DescriptorHeap, NumDescriptorsInHeap, NumDynamicDescriptors} {
    }

    void GPUDescriptorHeap::Free(DescriptorHeapAllocation&& Allocation) {
        auto MgrId = Allocation.GetAllocationManagerId();
        assert((MgrId == StaticHeapAllocatonManagerID || MgrId == DynamicHeapAllocatonManagerID)
               && "Unexpected allocation manager ID");

        if (MgrId == StaticHeapAllocatonManagerID)
            m_HeapAllocationManager.FreeAllocation(std::move(Allocation));
        else // MgrId == DynamicHeapAllocatonManagerID
            m_DynamicAllocationsManager.FreeAllocation(std::move(Allocation));
    }

    DynamicSuballocMngr::DynamicSuballocMngr(
        GPUDescriptorHeap* ParentGPUHeap,
        uint32_t           DynamicChunkSize,
        std::string        ManagerName) noexcept :
        m_ParentGPUHeap{ParentGPUHeap},
        m_ManagerName{std::move(ManagerName)},
        m_DynamicChunkSize{DynamicChunkSize} {
        assert(ParentGPUHeap != nullptr);
    }

    DynamicSuballocMngr::~DynamicSuballocMngr() {
        assert(m_Suballocations.empty() && m_CurrDescriptorCount == 0 && m_CurrSuballocationsTotalSize == 0 && "All dynamic suballocations must be released!");
    }

    void DynamicSuballocMngr::ReleaseAllocations() {
        // Clear the list and dispose all allocated chunks of GPU descriptor heap.
        // The chunks will be added to release queues and eventually returned to the
        // parent GPU heap.
        for (auto& Allocation : m_Suballocations)
            m_ParentGPUHeap->Free(std::move(Allocation));
        m_Suballocations.clear();
        m_CurrDescriptorCount         = 0;
        m_CurrSuballocationsTotalSize = 0;
    }

    DescriptorHeapAllocation DynamicSuballocMngr::Allocate(uint32_t Count) {
        // This method is intentionally lock-free as it is expected to
        // be called through device context from single thread only

        // Check if there are no chunks or the last chunk does not have enough space
        if (m_Suballocations.empty() || m_CurrentSuballocationOffset + Count > m_Suballocations.back().GetNumHandles()) {
            // Request a new chunk from the parent GPU descriptor heap
            auto suballocationSize       = (m_DynamicChunkSize > Count) ? m_DynamicChunkSize : Count;
            auto NewDynamicSubAllocation = m_ParentGPUHeap->AllocateDynamic(suballocationSize);
            if (NewDynamicSubAllocation.IsNull())
                return {};
            m_Suballocations.emplace_back(std::move(NewDynamicSubAllocation));
            m_CurrentSuballocationOffset = 0;

            m_CurrSuballocationsTotalSize += suballocationSize;
            m_PeakSuballocationsTotalSize = (m_PeakSuballocationsTotalSize > m_CurrSuballocationsTotalSize) ? m_PeakSuballocationsTotalSize : m_CurrSuballocationsTotalSize;
        }

        // Perform suballocation from the last chunk
        auto& currentSuballocation = m_Suballocations.back();

        auto managerId = currentSuballocation.GetAllocationManagerId();
        // assert(managerId < std::numeric_limits<uint16_t>::max() && "ManagerID exceed allowed limit");
        DescriptorHeapAllocation allocation(
            this,
            currentSuballocation.GetDescriptorHeap(),
            currentSuballocation.GetCpuHandle(m_CurrentSuballocationOffset),
            currentSuballocation.GetGpuHandle(m_CurrentSuballocationOffset),
            Count,
            static_cast<uint16_t>(managerId));
        m_CurrentSuballocationOffset += Count;
        m_CurrDescriptorCount += Count;
        m_PeakDescriptorCount = (m_PeakDescriptorCount > m_CurrDescriptorCount) ? m_PeakDescriptorCount : m_CurrDescriptorCount;
        return allocation;
    }
} // namespace Natsu::GFX