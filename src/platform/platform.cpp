#include "platform.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace Engine
{
    void Platform::init()
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");

    }

    void Platform::shutdown()
    {
        glfwTerminate();
    }
}
