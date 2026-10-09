
#include "editor/panels/viewport_panel.h"
#include "core/events/event_type.h"
#include "core/events/rendering_events.h"
#include "editor/ui_context.h"
#include "imgui.h"
#include "rendering/itexture_2d.h"
#include <memory>

namespace Engine {
    void ViewportPanel::set_texture(std::shared_ptr<ITexture2D> texture) {
        m_texture = texture;
    }

    void ViewportPanel::on_imgui_render() {

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin("Viewport", NULL, windowFlags);
        ImGui::PopStyleVar();

        if (m_texture) {

            int width = m_texture->get_width();
            int height = m_texture->get_height();

            ImVec2 avail = ImGui::GetContentRegionAvail();

            if (m_event_dispatcher) {
                m_event_dispatcher->dispatch_event(std::make_shared<ViewportSizeChangedEvent>(avail.x, avail.y));
            }

            ImGui::Image(m_texture->get_native_handle(), ImVec2(width, height));
        }
        ImGui::End();

    }

    void ViewportPanel::on_event(std::shared_ptr<IEvent> event) {
        if (event->get_type() != EventType::NEW_FRAME_RENDERED) {
            return;
        }

        auto viewport_event = std::static_pointer_cast<NewFrameRenderedEvent>(event);

        set_texture(viewport_event->frame);
    }
}
