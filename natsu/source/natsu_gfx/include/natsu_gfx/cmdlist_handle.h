#pragma once

#include "natsu_gfx/pch.h"

namespace Natsu::GFX {
    class CmdListHandle {
        ID3D12GraphicsCommandList* cmdList;

    public:
        CmdListHandle(CmdListHandle&&) noexcept;

        CmdListHandle(CmdListHandle const&) = delete;

        ID3D12GraphicsCommandList* CmdList() const { return cmdList; }

        CmdListHandle(
            ID3D12CommandAllocator*    allocator,
            ID3D12GraphicsCommandList* cmdList);

        ~CmdListHandle();
    };

} // namespace Natsu::GFX