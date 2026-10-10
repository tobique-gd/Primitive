#pragma once

#include "systems/display/iwindow.h"
#include <GLFW/glfw3.h>

namespace Engine
{
    class GLFWWindow : public IWindow
    {
    public:
        using FramebufferSizeCallback = std::function<void(int, int)>;

        GLFWWindow(const WindowProps& props);
        ~GLFWWindow() override;

        void on_update() override;
        bool should_close() const override;
        void set_title(const std::string& title) override;
        glm::ivec2 get_frame_buffer_size() const override;
        void* get_native_window() const override;
        GLFWwindow* get_glfw_window();
        bool is_vsync() const override;
        bool is_fullscreen() const override;
        void set_fullscreen(bool enabled) override;

        void minimize() override;
        void maximize() override;
        void restore() override;
        bool is_maximized() const override;
        void close() override;
        void set_position(int x, int y) override;
        glm::ivec2 get_position() const override;
        void set_event_callback(const EventCallbackFn& callback) override {
            m_data.event_callback = callback;
        }


    private:
        GLFWwindow* m_window = nullptr;

        struct WindowData
        {
            WindowProps props;
            EventCallbackFn event_callback;
        };

        WindowData m_data;

        bool m_vsync = false;
        bool m_fullscreen = false;

        int m_windowed_x = 0;
        int m_windowed_y = 0;
        int m_windowed_width = 0;
        int m_windowed_height = 0;

        FramebufferSizeCallback m_resize_callback;
    };
}
