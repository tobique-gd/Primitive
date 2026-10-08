#pragma once

#include "cocoa_bridge.h"
#include <QuartzCore/CAMetalLayer.hpp>
#include <memory>


#include "rendering/irenderer.h"
#include "rendering/itexture_2d.h"

namespace MTL {
    class RenderCommandEncoder;
    class DepthStencilState;
    class RenderPassDescriptor;
}


namespace Engine
{

    class MetalRenderer : public IRenderer
    {
    public:
        MetalRenderer();
        ~MetalRenderer() override;

        void init(IWindow& window) override;
        void shutdown() override;

        void begin_imgui() override;
        void end_imgui() override;

        std::shared_ptr<ITexture2D> render_viewport() override;
        void render_frame() override;

    private:
        void init_window(IWindow& window);
        void init_device();

        void create_triangle();
        void create_cube();

        void create_default_library();
        void create_command_queue();
        void create_render_pipeline();
        void send_render_command();
        void encode_render_command(MTL::RenderCommandEncoder* render_command_encoder);

        void create_buffers();

        void create_depth_and_MSAA_textures();
        void create_render_pass_descriptor();
        void create_internal_render_target();

        void update_render_pass_descriptor();
        MTL::DepthStencilState* depth_stencil_state;
        MTL::RenderPassDescriptor* render_pass_descriptor = nullptr;
        MTL::RenderPassDescriptor* ui_render_pass_descriptor = nullptr;

        MTL::Texture* msaa_render_target_texture = nullptr;
        MTL::Texture* depth_texture;
        int sample_count = 4;

        void present_viewport_debug();


        struct Impl;
        Impl* metal_backend = nullptr;

    };
}
