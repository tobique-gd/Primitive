#include "editor/layers/editor_layer.h"
#include "core/events/ievent.h"
#include "editor/IconsFontAwesome6.h"
#include "editor/panels/hierarchy_panel.h"
#include "editor/panels/viewport_panel.h"
#include "editor/panels/console_panel.h"
#include "editor/ui_context.h"
#include "imgui_internal.h"
#include "systems/rendering/irenderer.h"
#include "imgui.h"

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
                            , m_editor_panels({std::make_unique<ViewportPanel>(),
                                std::make_unique<ConsolePanel>(),
                                std::make_unique<HierarchyPanel>()})
                             {


    }

    void EditorLayer::set_event_dispatcher(IEventDispatcher* dispatcher)
    {
        ILayer::set_event_dispatcher(dispatcher);

        for (auto& panel : m_editor_panels)
        {
            panel->set_event_dispatcher(dispatcher);
        }
    }

    void EditorLayer::on_attach() {}

    void EditorLayer::on_detach() {}

    void EditorLayer::on_update() {
        m_renderer->begin_imgui();
        create_ui_layer();

        for (auto& panel : m_editor_panels) {
            panel->on_imgui_render();
        }

        m_renderer->end_imgui();
    }


    void EditorLayer::create_ui_layer() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_MenuBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, UIColors::background);
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, UIColors::background);

        bool visible = ImGui::Begin("Main", nullptr, flags);

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);


        if (visible && ImGui::BeginMenuBar()) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_MenuItemRounding, 0.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, UIColors::text_disabled);
            ImGui::PushStyleColor(ImGuiCol_Header, UIColors::selection);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, UIColors::panel_hover);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, UIColors::selection);

            ImGui::Button("L", ImVec2(16.0, 16.0));

            if (ImGui::BeginMenu("Scene")) {
                ImGui::PushStyleColor(ImGuiCol_Text, UIColors::text);

                if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " New Scene")) {
                }
                if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open Scene")) {
                }

                ImGui::PopStyleColor();
                ImGui::EndMenu();

            }

            if (ImGui::BeginMenu("Edit")) {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window")) {
                ImGui::EndMenu();
            }

            float menu_bar_width = ImGui::GetWindowWidth();
            float button_sizex = 32.0f;
            float center_x = (menu_bar_width - button_sizex) * 0.5f;

            ImGui::SetCursorPosX(center_x);

            const char* icon = ICON_FA_PLAY;
            ImVec2 button_size(32.0f, 32.0f);
            ImVec2 icon_size = ImGui::CalcTextSize(icon);
            ImVec2 button_pos = ImGui::GetCursorScreenPos();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

            bool clicked = ImGui::Button("##Play", button_size);
            bool hovered = ImGui::IsItemHovered();
            bool active = ImGui::IsItemActive();

            ImGui::PopStyleColor(3);

            ImVec4 icon_color = UIColors::text_disabled;

            if (active) {
                icon_color = UIColors::text;
            } else if (hovered) {
                float t = 0.5f;
                icon_color = ImVec4(
                    UIColors::text_disabled.x + (UIColors::text.x - UIColors::text_disabled.x) * t,
                    UIColors::text_disabled.y + (UIColors::text.y - UIColors::text_disabled.y) * t,
                    UIColors::text_disabled.z + (UIColors::text.z - UIColors::text_disabled.z) * t,
                    UIColors::text_disabled.w + (UIColors::text.w - UIColors::text_disabled.w) * t
                );
            }

            float offset_x = (button_size.x - icon_size.x) * 0.5f;
            float offset_y = (button_size.y - ImGui::GetTextLineHeight()) * 0.5f;

            ImGui::GetWindowDrawList()->AddText(
                ImVec2(button_pos.x + offset_x, button_pos.y + offset_y),
                ImGui::ColorConvertFloat4ToU32(icon_color),
                icon
            );

            if (clicked) {
            }

            const char* breadcrumb = "Project Name > Scene 8";
            float breadcrumb_width = ImGui::CalcTextSize(breadcrumb).x;
            float breadcrumb_x = menu_bar_width - breadcrumb_width - 16.0f;

            ImGui::SameLine();
            ImGui::SetCursorPosX(breadcrumb_x);
            ImGui::TextDisabled("%s", breadcrumb);

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar(3);

            ImGui::EndMenuBar();
        }

        ImGui::DockSpace(
            ImGui::GetID("Dockspace"),
            ImVec2(0.0f, 0.0f),
            ImGuiDockNodeFlags_PassthruCentralNode
        );

        ImGui::End();
    }

    void EditorLayer::on_event(std::shared_ptr<IEvent> event) {
        for (auto& panels : m_editor_panels) {
            panels->on_event(event);
        }

    }

}
