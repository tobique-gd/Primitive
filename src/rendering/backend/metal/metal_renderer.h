#pragma once

#include <QuartzCore/CAMetalLayer.hpp>
#include <memory>


#include "core/iwindow.h"
#include "platform/multi/GLFWWindow.h"
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
        void encode_render_command(MTL::RenderCommandEncoder* render_command_encoder);

        void create_buffers();


        void create_depth_and_MSAA_textures();
        void create_render_pass_descriptor();
        void create_internal_render_target();

        void update_render_pass_descriptor();
        void init_ui(IWindow& window);


        struct Impl;
        Impl* metal_backend = nullptr;

    };
}
