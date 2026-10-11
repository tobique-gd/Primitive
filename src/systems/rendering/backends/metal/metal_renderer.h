#pragma once

#include <QuartzCore/CAMetalLayer.hpp>
#include <memory>


#include "systems/display/iwindow.h"
#include "systems/rendering/rendering_system.h"
#include "systems/rendering/rhi/texture_2d.h"

namespace MTL {
    class RenderCommandEncoder;
    class DepthStencilState;
    class RenderPassDescriptor;
}


namespace Primitive {

    class MetalRenderer : public RenderingSystem
    {
    public:
        MetalRenderer();
        ~MetalRenderer() override;

        void init(IWindow& window) override;
        void shutdown() override;

        void begin_frame() override;
        void end_frame() override;

        void draw() override;

        std::shared_ptr<RHI::ITexture2D> get_viewport_texture() override;
        void set_viewport_size(int width, int height) override;
        void on_window_resize(int width, int height) override;

    private:
        void init_window(IWindow& window);
        void init_device();

        void create_cube();

        void create_default_library();
        void create_command_queue();
        void create_render_pipeline();
        void send_render_command();

        void create_buffers();


        void create_depth_and_MSAA_textures();
        void create_render_pass_descriptor();
        void create_internal_render_target();

        void update_render_pass_descriptor();
        void update_scene_transforms();


        struct Impl;
        Impl* metal_backend = nullptr;

    };
}
