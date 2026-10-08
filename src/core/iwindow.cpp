#include "platform/multi/GLFWWindow.h"

namespace Engine
{
    std::shared_ptr<IWindow> IWindow::create_window(const WindowProps& props)
    {
        return std::make_shared<GLFWWindow>(props);
    }
}
