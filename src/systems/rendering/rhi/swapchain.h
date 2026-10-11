// src/systems/rendering/rhi/swapchain.h
#pragma once

namespace Primitive::RHI
{
    enum class LoadOp { Load, Clear, DontCare };

    struct SwapchainDesc
    {
        void* native_window = nullptr;  // NSWindow* / HWND / etc.
        int   width  = 1;
        int   height = 1;
        bool  vsync  = true;
    };

    class ISwapchain
    {
    public:
        virtual ~ISwapchain() = default;
        virtual bool acquire_next_frame() = 0;
        virtual void present(void* native_command_buffer) = 0;

        virtual void resize(int width, int height) = 0;
        virtual void* get_native_render_pass_descriptor(LoadOp load) = 0;

        virtual unsigned get_color_format() const = 0;
        virtual int get_width()  const = 0;
        virtual int get_height() const = 0;
    };
}
