// src/drivers/metal/metal_swapchain.cpp
#include "drivers/metal/metal_swapchain.h"
#include "platform/macos/cocoa_bridge.h"

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <CoreGraphics/CoreGraphics.h>
#include <stdexcept>

namespace Primitive::RHI::Metal
{
    static MTL::LoadAction to_mtl(LoadOp op)
    {
        switch (op) {
            case LoadOp::Load:     return MTL::LoadActionLoad;
            case LoadOp::Clear:    return MTL::LoadActionClear;
            case LoadOp::DontCare: return MTL::LoadActionDontCare;
        }
        return MTL::LoadActionClear;
    }

    MetalSwapchain::MetalSwapchain(MTL::Device* device, const SwapchainDesc& desc)
        : m_width(desc.width), m_height(desc.height)
    {
        if (!device) throw std::runtime_error("MetalSwapchain: null device");
        if (!desc.native_window) throw std::runtime_error("MetalSwapchain: null native window");

        m_layer = CA::MetalLayer::layer();
        if (!m_layer) throw std::runtime_error("MetalSwapchain: failed to create CAMetalLayer");
        m_layer->retain();

        m_layer->setDevice(device);
        m_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        m_layer->setDrawableSize(CGSizeMake(desc.width, desc.height));
        m_layer->setDisplaySyncEnabled(desc.vsync);

        CocoaBridge::AddLayerToWindow(desc.native_window, m_layer);
    }

    MetalSwapchain::~MetalSwapchain()
    {
        if (m_layer) m_layer->release();
    }

    bool MetalSwapchain::acquire_next_frame()
    {
        m_drawable = m_layer->nextDrawable();
        return m_drawable != nullptr;
    }

    void MetalSwapchain::present(void* native_command_buffer)
    {
        if (!m_drawable || !native_command_buffer) return;

        static_cast<MTL::CommandBuffer*>(native_command_buffer)->presentDrawable(m_drawable);
        m_drawable = nullptr;
    }

    void MetalSwapchain::resize(int width, int height)
    {
        if (width <= 0 || height <= 0) return;
        if (width == m_width && height == m_height) return;

        m_width  = width;
        m_height = height;
        m_layer->setDrawableSize(CGSizeMake(width, height));
    }

    void* MetalSwapchain::get_native_render_pass_descriptor(LoadOp load)
    {
        if (!m_drawable) return nullptr;

        auto* desc  = MTL::RenderPassDescriptor::renderPassDescriptor();
        auto* color = desc->colorAttachments()->object(0);
        color->setTexture(m_drawable->texture());
        color->setLoadAction(to_mtl(load));
        color->setStoreAction(MTL::StoreActionStore);
        color->setClearColor(MTL::ClearColor(0, 0, 0, 1));
        return desc;
    }

    unsigned MetalSwapchain::get_color_format() const
    {
        return static_cast<unsigned>(m_layer->pixelFormat());
    }
}
