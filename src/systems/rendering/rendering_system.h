#pragma once

#include "core/project/project_manager.h"
#include "systems/display/iwindow.h"
#include "systems/rendering/rhi/device.h"
#include "systems/rendering/rhi/texture_2d.h"
#include <memory>
#include <functional>

namespace Primitive {
    class RenderingSystem
    {
    public:
        virtual ~RenderingSystem() = default;

        virtual void init(IWindow& window, std::shared_ptr<ProjectManager> project_manager) = 0;
        virtual void shutdown() = 0;

        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;

        virtual void draw() = 0;

        virtual std::shared_ptr<Texture2D> get_viewport_texture() = 0;
        virtual void set_viewport_size(int width, int height) = 0;
        virtual void on_window_resize(int width, int height) = 0;

        virtual void* get_native_device() const = 0;

        virtual void render_overlay(const std::function<void(void*, void*)>& callback) = 0;
        virtual void* get_overlay_render_pass_descriptor() const = 0;
        virtual void set_overlay_callback(
            std::function<void(void*, void*)> callback
        ) = 0;


    };
}
