
#include "editor/panels/viewport_panel.h"
#include "core/events/event_type.h"
#include "core/events/rendering_events.h"
#include "editor/ui/IconsFontAwesome6.h"
#include "editor/ui/ui_theme.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "systems/rendering/rhi/texture_2d.h"
#include <cmath>
#include <iostream>
#include <memory>

namespace Primitive {


    void ViewportPanel::set_texture(std::shared_ptr<Texture2D> texture) {
        m_texture = texture;
    }


    void ViewportPanel::on_imgui_render() {
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoTitleBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport", nullptr, windowFlags);
        ImGui::PopStyleVar();

        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
            if (ImGui::IsKeyPressed(ImGuiKey_T)) m_toolbar_visible = !m_toolbar_visible;
            if (ImGui::IsKeyPressed(ImGuiKey_Q)) m_editor_state->temporary.selected_tool = 0;
            if (ImGui::IsKeyPressed(ImGuiKey_W)) m_editor_state->temporary.selected_tool = 1;
            if (ImGui::IsKeyPressed(ImGuiKey_E)) m_editor_state->temporary.selected_tool = 2;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) m_editor_state->temporary.selected_tool = 3;
        }


        ImVec2 viewport_pos = ImGui::GetCursorScreenPos();
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 scale = ImGui::GetIO().DisplayFramebufferScale;

        int width = static_cast<int>(std::round(avail.x * scale.x));
        int height = static_cast<int>(std::round(avail.y * scale.y));

        if (width > 0 && height > 0 &&
            (width != m_last_viewport_width ||
             height != m_last_viewport_height)) {
            if (m_event_dispatcher) {
                m_event_dispatcher->dispatch_event(
                    std::make_shared<ViewportSizeChangedEvent>(width, height)
                );

                m_last_viewport_width = width;
                m_last_viewport_height = height;
            }
        }

        if (m_texture && avail.x > 0.0f && avail.y > 0.0f) {
            ImGui::Image(m_texture->get_native_handle(), avail);
        }

        float toolbar_width = 46.0f;
        float visible_x = viewport_pos.x + 12.0f;
        float hidden_x = viewport_pos.x - toolbar_width - 2.0f;
        float target_x = m_toolbar_visible ? visible_x : hidden_x;

        float speed = 12.0f;
        float t = 1.0f - std::exp(-speed * ImGui::GetIO().DeltaTime);
        m_toolbar_offset += (target_x - m_toolbar_offset) * t;

        ImGui::SetCursorScreenPos(ImVec2(m_toolbar_offset, viewport_pos.y + 12.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Primitive::UIColors::background_tp);

        ImGuiChildFlags child_flags =
            ImGuiChildFlags_AutoResizeY |
            ImGuiChildFlags_AutoResizeX |
            ImGuiChildFlags_AlwaysUseWindowPadding;

        ImGui::BeginChild(
            "ViewportToolbar",
            ImVec2(0, 0),
            child_flags,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove
        );

        const char* tools[] = {
            ICON_FA_ARROW_POINTER,
            ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT,
            ICON_FA_ARROW_ROTATE_LEFT,
            ICON_FA_UP_RIGHT_FROM_SQUARE
        };

        ImVec2 button_size(34.0f, 34.0f);

        for (int i = 0; i < 4; i++) {
            bool is_selected = (m_editor_state->temporary.selected_tool == i);

            if (is_selected) {
                ImGui::PushStyleColor(ImGuiCol_Button, Primitive::UIColors::content);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Primitive::UIColors::panel);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, Primitive::UIColors::button_active);
                ImGui::PushStyleColor(ImGuiCol_Text, Primitive::UIColors::accent);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, Primitive::UIColors::transparent);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Primitive::UIColors::button_hover);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, Primitive::UIColors::button_active);
                ImGui::PushStyleColor(ImGuiCol_Text, Primitive::UIColors::text);
            }

            if (ImGui::Button(tools[i], button_size)) {
                m_editor_state->temporary.selected_tool = i;
            }

            ImGui::PopStyleColor(4);
        }

        ImGui::EndChild();

        ImGui::PopStyleColor();
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
