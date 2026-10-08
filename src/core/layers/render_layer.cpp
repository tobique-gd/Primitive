#include "core/layers/render_layer.h"
#include "core/events/new_frame_rendered_event.h"

namespace Engine
{
    RenderLayer::RenderLayer(std::shared_ptr<IRenderer> renderer,
                            std::shared_ptr<ProjectManager> project_manager
                            )
                            : m_project_manager(project_manager)
                            , m_renderer(renderer) {

    }

    void RenderLayer::on_attach() {}

    void RenderLayer::on_detach() {}

    void RenderLayer::on_update()
    {
        auto texture = m_renderer->render_viewport();

        m_event_dispatcher->dispatch_event(
            std::make_shared<NewFrameRenderedEvent>(texture)
        );
    }

}
