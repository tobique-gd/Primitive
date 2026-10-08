#include "core/layers/editor_layer.h"
#include "core/events/event_type.h"
#include "core/events/ievent.h"
#include "core/events/new_frame_rendered_event.h"
#include "editor/panels/viewport_panel.h"
#include "rendering/irenderer.h"

#include <iostream>
#include <memory>
namespace Engine
{
    EditorLayer::EditorLayer(std::shared_ptr<IWindow> window,
                            std::shared_ptr<ProjectManager> project_manager,
                            std::shared_ptr<IRenderer> renderer)
                            : m_window(window)
                            , m_project_manager(project_manager)
                            , m_renderer(renderer)
                            , m_editor_panels({std::make_unique<ViewportPanel>()}) {

    }

    void EditorLayer::on_attach() {}

    void EditorLayer::on_detach() {}

    void EditorLayer::on_update() {
        m_renderer->begin_imgui();

        for (auto& panel : m_editor_panels) {
            panel->on_imgui_render();
        }

        m_renderer->end_imgui();
    }

    void EditorLayer::on_event(std::shared_ptr<IEvent> event) {
        for (auto& panels : m_editor_panels) {
            panels->on_event(event);
        }

    }

}
