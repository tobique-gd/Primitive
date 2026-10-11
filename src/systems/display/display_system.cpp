#include "systems/display/display_system.h"
#include "GLFW/glfw3.h"
#include "platform/macos/GLFWWindow.h"

namespace Primitive {

    DisplaySystem::DisplaySystem() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
    };

    DisplaySystem::~DisplaySystem() {
        glfwTerminate();
    }

    std::shared_ptr<IWindow> DisplaySystem::create_window(const WindowProps& props) {
        return std::make_shared<GLFWWindow>(props);
    }

}
