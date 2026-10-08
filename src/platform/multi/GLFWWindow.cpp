#include "GLFWWindow.h"
#include "GLFW/glfw3.h"
#include <string>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace Engine
{
    GLFWWindow::GLFWWindow(const WindowProps& props)
        : m_props(props), m_vsync(props.vsync)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window = glfwCreateWindow(
            props.width,
            props.height,
            props.title.c_str(),
            nullptr,
            nullptr
        );

        if (!m_window)
            throw std::runtime_error("Failed to create GLFW window");

        glfwSetWindowUserPointer(m_window, this);

        glfwGetWindowPos(m_window, &m_windowed_x, &m_windowed_y);

        m_windowed_width = props.width;
        m_windowed_height = props.height;


    }

    GLFWWindow::~GLFWWindow()
    {
        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

    }

    void GLFWWindow::on_update() {
        glfwPollEvents();
    }

    bool GLFWWindow::should_close() {
        return glfwWindowShouldClose(m_window);
    }

    void GLFWWindow::set_title(const std::string& title) {
        glfwSetWindowTitle(m_window, title.c_str());
    }

    glm::ivec2 GLFWWindow::get_frame_buffer_size() const {
        int height;
        int width;

        glfwGetFramebufferSize(m_window, &width, &height);

        return {width, height};
    }

    void* GLFWWindow::get_native_window() const {
        return glfwGetCocoaWindow(m_window);
    }

    GLFWwindow* GLFWWindow::get_glfw_window() {
        return m_window;
    }

    bool GLFWWindow::is_vsync() const {
        return m_vsync;
    }

    bool GLFWWindow::is_fullscreen() const {
        return m_fullscreen;
    }

    void GLFWWindow::set_fullscreen(bool enabled) {
        if (m_fullscreen == enabled) {
            return;
        }

        if (enabled) {
            glfwGetWindowPos(m_window, &m_windowed_x, &m_windowed_y);
            glfwGetWindowSize(m_window, &m_windowed_width, &m_windowed_height);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);

            glfwSetWindowMonitor(m_window, monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
        } else {
            glfwSetWindowMonitor(m_window, nullptr, m_windowed_x, m_windowed_y, m_windowed_width, m_windowed_height, 0);


        }

        m_fullscreen = enabled;
    }

    void GLFWWindow::minimize()
    {
        glfwIconifyWindow(m_window);
    }

    void GLFWWindow::maximize()
    {
        glfwMaximizeWindow(m_window);
    }

    void GLFWWindow::restore()
    {
        glfwRestoreWindow(m_window);
    }

    bool GLFWWindow::is_maximized() const
    {
        return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
    }

    void GLFWWindow::close()
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }

    void GLFWWindow::set_position(int x, int y)
    {
        glfwSetWindowPos(m_window, x, y);
    }

    glm::ivec2 GLFWWindow::get_position() const
    {
        int x;
        int y;

        glfwGetWindowPos(m_window, &x, &y);

        return { x, y };
    }
}
