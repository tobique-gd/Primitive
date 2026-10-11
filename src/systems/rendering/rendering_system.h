#pragma once

#include "systems/display/iwindow.h"
#include "systems/rendering/rhi/texture_2d.h"
#include <memory>

namespace Primitive {
    class RenderingSystem
    {
    public:
        virtual ~RenderingSystem() = default;

        virtual void init(IWindow& window) = 0;
        virtual void shutdown() = 0;

        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;

        virtual void draw() = 0;

        virtual std::shared_ptr<RHI::ITexture2D> get_viewport_texture() = 0;
        virtual void set_viewport_size(int width, int height) = 0;
        virtual void on_window_resize(int width, int height) = 0;
    };
}
