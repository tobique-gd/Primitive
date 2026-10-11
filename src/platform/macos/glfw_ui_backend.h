#pragma once

#include "editor/ui/ui_context.h"

struct GLFWwindow;

namespace Primitive {
    class GLFWUIBackend final : public IUIPlatformBackend {
    public:
        void init(IWindow& window) override;
        void new_frame() override;
        void shutdown() override;

    private:
        GLFWwindow* m_window = nullptr;
    };
}
