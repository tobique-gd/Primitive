
#include "editor/panels/viewport_panel.h"
#include "core/events/event_type.h"
#include "core/events/rendering_events.h"
#include "editor/ui/IconsFontAwesome6.h"
#include "editor/ui/ui_theme.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "systems/rendering/rhi/texture_2d.h"
#include <cmath>
#include <memory>

namespace Primitive {


    void ViewportPanel::set_texture(std::shared_ptr<RHI::ITexture2D> texture) {
        m_texture = texture;
    }


    void ViewportPanel::on_imgui_render() {

        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoTitleBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport", nullptr, windowFlags);
        ImGui::PopStyleVar();

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsKeyPressed(ImGuiKey_T)) {
            m_toolbar_visible = !m_toolbar_visible;
        }
        ImVec2 viewport_pos = ImGui::GetCursorScreenPos();
        ImVec2 avail = ImGui::GetContentRegionAvail();

        if (m_event_dispatcher) {
            m_event_dispatcher->dispatch_event(
                std::make_shared<ViewportSizeChangedEvent>(
                    avail.x, avail.y
                )
            );
        }

        if (m_texture) {
            ImGui::Image(
                m_texture->get_native_handle(),
                avail
            );
        }

        ImGui::SetCursorScreenPos(
            ImVec2(viewport_pos.x + 12.0f, viewport_pos.y + 12.0f)
        );

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.14f, 0.25f));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));

        float toolbar_width = 46.0f;
        float visible_x = viewport_pos.x + 12.0f;
        float hidden_x = viewport_pos.x - toolbar_width - 2.0f;

        float target_x = m_toolbar_visible ? visible_x : hidden_x;

        float speed = 12.0f;
        float t = 1.0f - std::exp(-speed * ImGui::GetIO().DeltaTime);

        m_toolbar_offset += (target_x - m_toolbar_offset) * t;

        ImGui::SetCursorScreenPos(
            ImVec2(m_toolbar_offset, viewport_pos.y + 12.0f)
        );

        ImGui::BeginChild(
            "ViewportToolbar",
            ImVec2(0, 0),
            ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX,
            ImGuiWindowFlags_NoScrollbar
        );

        ImGui::Button(ICON_FA_ARROW_POINTER, ImVec2(34, 34));
        ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, ImVec2(34, 34));
        ImGui::Button(ICON_FA_ARROW_ROTATE_RIGHT, ImVec2(34, 34));
        ImGui::Button(ICON_FA_UP_RIGHT_FROM_SQUARE, ImVec2(34, 34));

        ImGui::EndChild();

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(4);

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
