#include "core/layers/render_layer.h"
#include "core/events/ievent.h"
#include "core/events/rendering_events.h"
#include "systems/rendering/rendering_system.h"
#include <iostream>
#include <memory>

namespace Primitive {
    RenderLayer::RenderLayer(std::shared_ptr<RenderingSystem> renderer,
                            std::shared_ptr<ProjectManager> project_manager
                            )
                            : m_project_manager(project_manager)
                            , m_renderer(renderer) {

    }

    void RenderLayer::on_attach() {}

    void RenderLayer::on_detach() {}

    void RenderLayer::on_update()
    {
        m_renderer->draw();
        auto texture = m_renderer->get_viewport_texture();

        m_event_dispatcher->dispatch_event(
            std::make_shared<NewFrameRenderedEvent>(texture)
        );
    }

    void RenderLayer::on_event(std::shared_ptr<IEvent> event) {
        if (event->get_type() != EventType::VIEWPORT_SIZE_CHANGED) {
            return;
        }

        auto viewport_event = std::static_pointer_cast<ViewportSizeChangedEvent>(event);

        m_renderer->set_viewport_size(viewport_event->width, viewport_event->height);
        event->consume();

    }

}
