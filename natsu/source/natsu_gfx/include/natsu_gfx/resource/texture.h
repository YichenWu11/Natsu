#pragma once

#include "natsu_gfx/resource/resource.h"

namespace Natsu::GFX {
    enum class TextureDimension : uint8_t {
        None = 0,
        Tex1D,
        Tex2D,
        Tex3D,
        Cubemap,
        Tex2DArray,
    };

    class FrameResource;

    class Texture final : public Resource {
    public:
        enum class TextureUsage : uint {
            None            = 0,
            RenderTarget    = 0x1,
            DepthStencil    = 0x2,
            UnorderedAccess = 0x4,
            GenericColor    = (0x4 | 0x1), // Both render target and unordered access
        };

        static float             CLEAR_COLOR[4];
        static constexpr float   CLEAR_DEPTH   = 1;
        static constexpr uint8_t CLEAR_STENCIL = 0;

        static void setClearColor(const DirectX::XMFLOAT4& color) {
            CLEAR_COLOR[0] = color.x;
            CLEAR_COLOR[1] = color.y;
            CLEAR_COLOR[2] = color.z;
            CLEAR_COLOR[3] = color.w;
        }

    private:
        D3D12_RESOURCE_STATES  initState;
        TextureUsage           usage;
        TextureDimension       dimension;
        ComPtr<ID3D12Resource> resource;

    public:
        Texture(
            ID3D12Device*         device,
            uint                  width,
            uint                  height,
            DXGI_FORMAT           format,
            TextureDimension      dimension,
            uint                  depth,
            uint                  mip,
            TextureUsage          usage,
            D3D12_RESOURCE_STATES resourceState);

        Texture(
            ID3D12Device*    device,
            IDXGISwapChain3* swapchain,
            uint             frame);

        ~Texture();

        ID3D12Resource* GetResource() const override {
            return resource.Get();
        }
        D3D12_RESOURCE_STATES GetInitState() const override {
            return initState;
        }
        ID3D12Resource** ReleaseAndGetAddress() override {
            return resource.ReleaseAndGetAddressOf();
        }

        D3D12_SHADER_RESOURCE_VIEW_DESC  GetColorSrvDesc(uint mipOffset) const;
        D3D12_UNORDERED_ACCESS_VIEW_DESC GetColorUavDesc(uint targetMipLevel) const;
        void                             DelayDispose(FrameResource* frameRes) const override;

        uint GetWidth() { return resource->GetDesc().Width; }
        uint GetHeight() { return resource->GetDesc().Height; };
    };
} // namespace Natsu::GFX