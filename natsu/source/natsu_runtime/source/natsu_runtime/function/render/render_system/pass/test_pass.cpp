#include "natsu_runtime/function/render/render_system/pass/test_pass.h"

#include "natsu_gfx/util/bind_property.h"
#include "natsu_runtime/function/render/render_system/render_system.h"
#include "natsu_runtime/resource/config_manager/config_manager.h"
#include "natsu_runtime/resource/resource/mesh.h"
#include "natsu_runtime/resource/resource_manager.h"

namespace Natsu::Runtime {
    DXGI_FORMAT TestPass::s_colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT TestPass::s_depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    void TestPass::PreLoad() {
        auto context =
            g_runtime_global_context.m_render_system->GetCommandContext();
        auto window =
            g_runtime_global_context.m_window_system.get();

        m_viewport    = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));
        m_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(window->GetWidth()), static_cast<LONG>(window->GetHeight()));

        // build resources
        {
            uint32_t rtvHandleIdx(context->m_resourceMngr.AllocateHandle(GFX::RTV));
            uint32_t dsvHandleIdx(context->m_resourceMngr.AllocateHandle(GFX::DSV));

            for (int n = 0; n < s_frame_count; ++n) {
                uint32_t rtvHandleIdx(context->m_resourceMngr.AllocateHandle(GFX::RTV, "MainRT" + std::to_string(n)));
                uint32_t dsvHandleIdx(context->m_resourceMngr.AllocateHandle(GFX::DSV, "MainDS" + std::to_string(n)));

                m_renderTargets[n] = std::unique_ptr<GFX::Texture>(
                    new GFX::Texture(context->m_device.DxDevice(), context->m_SwapChain.Get(), n));
                m_depthTargets[n] = std::unique_ptr<GFX::Texture>(
                    new GFX::Texture(
                        context->m_device.DxDevice(),
                        m_scissorRect.right,
                        m_scissorRect.bottom,
                        DXGI_FORMAT_D24_UNORM_S8_UINT,
                        GFX::TextureDimension::Tex2D,
                        1,
                        1,
                        GFX::Texture::TextureUsage::DepthStencil,
                        D3D12_RESOURCE_STATE_DEPTH_READ));

                context->m_device.DxDevice()->CreateRenderTargetView(
                    m_renderTargets[n]->GetResource(),
                    nullptr,
                    context->m_resourceMngr.GetRTVCpuHandle(rtvHandleIdx));
                context->m_device.DxDevice()->CreateDepthStencilView(
                    m_depthTargets[n]->GetResource(),
                    nullptr,
                    context->m_resourceMngr.GetDSVCpuHandle(rtvHandleIdx));
            }
        }

        // build shaders
        {
            auto shaderPath =
                g_runtime_global_context.m_config_manager->GetShaderFolder() / "common/test.hlsl";
            std::vector<std::pair<std::string, GFX::Shader::Property>> properties;
            m_basicShaderMap["test"] = new GFX::BasicShader(properties, context->m_device.DxDevice());
            m_basicShaderMap["test"]->SetVsShader(shaderPath.c_str());
            m_basicShaderMap["test"]->SetPsShader(shaderPath.c_str());
            m_basicShaderMap["test"]->rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            m_basicShaderMap["test"]->blendState      = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            auto&& depthStencilState =
                m_basicShaderMap["test"]->depthStencilState;
            depthStencilState.DepthEnable    = true;
            depthStencilState.DepthFunc      = D3D12_COMPARISON_FUNC_LESS;
            depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            depthStencilState.StencilEnable  = false;
        }
    }

    void TestPass::OnResize() {
    }

    void TestPass::Execute(GFX::FrameResource& frameRes, uint32_t frameIndex) {
        // LOG_INFO("TestPass Execute");

        auto context =
            g_runtime_global_context.m_render_system->GetCommandContext();
        auto& currCmdList =
            context->m_frameResourceMngr.GetCurrentFrameResource()->GetCmdList();

        {
            context->m_StateTracker.RecordState(
                m_renderTargets[frameIndex].get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET);

            context->m_StateTracker.RecordState(
                m_depthTargets[frameIndex].get(),
                D3D12_RESOURCE_STATE_DEPTH_WRITE);

            context->m_StateTracker.UpdateState(currCmdList.GetCommandList());

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
                context->m_resourceMngr.GetRTVCpuHandle(
                    context->m_resourceMngr.GetHandleOffset(GFX::RTV, "MainRT" + std::to_string(frameIndex))));
            CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(
                context->m_resourceMngr.GetDSVCpuHandle(
                    context->m_resourceMngr.GetHandleOffset(GFX::DSV, "MainDS" + std::to_string(frameIndex))));

            frameRes.SetRenderTarget(
                m_renderTargets[frameIndex].get(),
                &rtvHandle,
                &dsvHandle);
            frameRes.ClearRTV(rtvHandle);
            frameRes.ClearDSV(dsvHandle);

            // bind descriptorHeap
            currCmdList->SetDescriptorHeaps(1, get_rvalue_ptr(context->m_resourceMngr.GetCSUDescriptorHeap()));

            // bind shader params

            // drawcall
            auto mesh =
                static_cast<Resource<ResourceType::Mesh>*>(g_runtime_global_context.m_resource_manager->get<ResourceType::Mesh>("square"))->GetMesh();

            frameRes.DrawMesh(
                m_basicShaderMap["test"],
                &(context->m_psoMngr),
                mesh,
                s_colorFormat,
                s_depthFormat,
                std::span<GFX::BindProperty>{});

            for (auto& rData : g_runtime_global_context.m_render_system->m_render_data) {
                auto mesh =
                    static_cast<Resource<ResourceType::Mesh>*>(g_runtime_global_context.m_resource_manager->get<ResourceType::Mesh>(rData.mesh_name))->GetMesh();
                std::vector<GFX::BindProperty> bind_properties;

                frameRes.DrawMesh(
                    m_basicShaderMap["test"],
                    &(context->m_psoMngr),
                    mesh,
                    s_colorFormat,
                    s_depthFormat,
                    bind_properties);
            }

            context->m_StateTracker.RestoreState(currCmdList.GetCommandList());
        }
    }

    void TestPass::DelayDisposeResource(GFX::FrameResource& frameRes) {
    }

} // namespace Natsu::Runtime