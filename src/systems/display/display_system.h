#pragma once

#include "systems/display/iwindow.h"
#include <memory>

namespace Engine {

    class DisplaySystem {
    public:
        DisplaySystem();
        ~DisplaySystem();

        DisplaySystem(const DisplaySystem&) = delete;
        DisplaySystem& operator=(const DisplaySystem&) = delete;

        std::shared_ptr<IWindow> create_window(const WindowProps& props = WindowProps());
    };

}
