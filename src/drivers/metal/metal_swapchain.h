// src/drivers/metal/metal_swapchain.h
#pragma once

#include "systems/rendering/rhi/swapchain.h"

namespace MTL { class Device; class RenderPassDescriptor; }
namespace CA  { class MetalLayer; class MetalDrawable; }

namespace Primitive::RHI::Metal
{
    class MetalSwapchain final : public ISwapchain
    {
    public:
        MetalSwapchain(MTL::Device* device, const SwapchainDesc& desc);
        ~MetalSwapchain() override;

        MetalSwapchain(const MetalSwapchain&) = delete;
        MetalSwapchain& operator=(const MetalSwapchain&) = delete;

        bool  acquire_next_frame() override;
        void  present(void* native_command_buffer) override;
        void  resize(int width, int height) override;
        void* get_native_render_pass_descriptor(LoadOp load) override;

        unsigned get_color_format() const override;
        int get_width()  const override { return m_width; }
        int get_height() const override { return m_height; }

    private:
        CA::MetalLayer*    m_layer    = nullptr;
        CA::MetalDrawable* m_drawable = nullptr;
        int m_width  = 0;
        int m_height = 0;
    };
}
