#pragma once

#include "core/events/window_events.h"
#include "systems/display/iwindow.h"
#include "editor/layers/editor_layer.h"
#include "core/layers/layer_stack.h"
#include <memory>
#include <shared_mutex>
#include "systems/rendering/backends/metal/metal_renderer.h"
#include "systems/display/display_system.h"
#include "systems/rendering/rendering_system.h"

namespace Primitive {
    class Application
    {
    public:
        Application()
        {
            _display_system = std::make_shared<DisplaySystem>();
            _window = _display_system->create_window();
            _window->set_event_callback([this](IEvent& e) { on_event(e); });

            _project_manager = std::make_shared<ProjectManager>();
            _layer_stack = std::make_shared<LayerStack>();
            _renderer = std::make_shared<MetalRenderer>();
            _renderer->init(*_window);
        }

        virtual ~Application()
        {
            _layer_stack.reset();
            _window.reset();

            //Platform::shutdown();
        }

        void on_event(IEvent& e)
        {
            if (e.get_type() == EventType::WINDOW_RESIZE)
            {
                auto& resize = static_cast<WindowResizeEvent&>(e);
                _renderer->on_window_resize(resize.width, resize.height);
            }

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
        std::shared_ptr<RenderingSystem> _renderer;
        std::shared_ptr<DisplaySystem> _display_system;
    };
}
