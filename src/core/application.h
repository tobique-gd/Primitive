#pragma once

#include "core/iwindow.h"
#include "core/layers/editor_layer.h"
#include "core/layers/layer_stack.h"
#include <memory>
#include <shared_mutex>
#include "platform/platform.h"
#include "rendering/backend/metal/metal_renderer.h"
#include "rendering/irenderer.h"

namespace Engine
{
    class Application
    {
    public:
        Application()
        {
            Platform::init();

            _window = IWindow::create_window();

            _project_manager = std::make_shared<ProjectManager>();
            _layer_stack = std::make_shared<LayerStack>();
            _renderer = std::make_shared<MetalRenderer>();
            _renderer->init(*_window);
        }

        virtual ~Application()
        {
            _layer_stack.reset();
            _window.reset();

            Platform::shutdown();
        }

        void run()
        {
            while (!_window->should_close()) {
                _window->on_update();
                _layer_stack->on_update();
            }
        }

    protected:
        std::shared_ptr<IWindow> _window;
        std::shared_ptr<LayerStack> _layer_stack;
        std::shared_ptr<ProjectManager> _project_manager;
        std::shared_ptr<IRenderer> _renderer;
    };
}
