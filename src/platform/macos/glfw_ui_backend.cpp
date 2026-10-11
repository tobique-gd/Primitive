#include "platform/macos/glfw_ui_backend.h"
#include "platform/macos/GLFWWindow.h"
#include "systems/display/iwindow.h"

#include "GLFW/glfw3.h"
#include "imgui_impl_glfw.h"

#include <stdexcept>

namespace Primitive {
    void GLFWUIBackend::init(IWindow& window)
    {
        auto* glfw_window = dynamic_cast<GLFWWindow*>(&window);
        if (!glfw_window)
            throw std::runtime_error("GLFWUIBackend requires a GLFWWindow");

        m_window = glfw_window->get_glfw_window();
        ImGui_ImplGlfw_InitForOther(m_window, true);
    }

    void GLFWUIBackend::new_frame()
    {
        ImGui_ImplGlfw_NewFrame();
    }

    void GLFWUIBackend::shutdown()
    {
        ImGui_ImplGlfw_Shutdown();
        m_window = nullptr;
    }
}
