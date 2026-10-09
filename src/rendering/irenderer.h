#pragma once

#include "core/iwindow.h"
#include "rendering/itexture_2d.h"
#include <memory>

namespace Engine
{
    class IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        virtual void init(IWindow& window) = 0;
        virtual void shutdown() = 0;

        virtual void begin_imgui() = 0;
        virtual void end_imgui() = 0;

        virtual std::shared_ptr<ITexture2D> render_viewport() = 0;
        virtual void render_frame() = 0;
        virtual void set_viewport_size(int width, int height) = 0;
        virtual void on_window_resize(int width, int height) = 0;
    };
}
