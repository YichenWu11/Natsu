#pragma once

#include "natsu_gfx/render_pass.h"

namespace Natsu::Runtime {
    class TestPass : public GFX::RenderPass {
    public:
        TestPass()  = default;
        ~TestPass() = default;

        void PreLoad() override;
        void OnResize() override;
        void Execute(GFX::FrameResource& frameRes, uint32_t frameIndex) override;
        void DelayDisposeResource(GFX::FrameResource& frameRes) override;

    private:
        static const uint32_t s_frame_count = 3;
        static DXGI_FORMAT    s_colorFormat;
        static DXGI_FORMAT    s_depthFormat;

    private:
        std::unique_ptr<GFX::Texture> m_renderTargets[s_frame_count];
        std::unique_ptr<GFX::Texture> m_depthTargets[s_frame_count];

        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT     m_scissorRect;
    };
} // namespace Natsu::Runtime