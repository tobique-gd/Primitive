#pragma once

#include <QuartzCore/CAMetalLayer.hpp>
#include <memory>


#include "drivers/metal/metal_device.h"
#include "imgui.h"
#include "systems/display/iwindow.h"
#include "systems/rendering/rendering_system.h"
#include "systems/rendering/rhi/device.h"
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

        void init(IWindow& window, std::shared_ptr<ProjectManager> project_manager) override;
        void shutdown() override;

        void begin_frame() override;
        void end_frame() override;

        void draw() override;

        std::shared_ptr<Texture2D> get_viewport_texture() override;
        void set_viewport_size(int width, int height) override;
        void on_window_resize(int width, int height) override;
        //void render_to_swapchain() override;
        //
        void* get_overlay_render_pass_descriptor() const override;

        void render_overlay(
            const std::function<void(void*, void*)>& callback
        ) override;

        void set_overlay_callback(
            std::function<void(void*, void*)> callback
        ) override;

        void* get_native_device() const override;



    private:
        void init_device();

        void create_cube();

        void create_default_library();
        void create_command_queue();
        void create_render_pipeline();
        void send_render_command();

        void create_buffers();

        void update_scene_transforms();
        void update_sky_transforms();


        struct Impl;
        Impl* metal_backend = nullptr;

    };
}
