#include "core/layers/editor_layer.h"
#include "core/events/ievent.h"
#include "editor/panels/hierarchy_panel.h"
#include "editor/panels/viewport_panel.h"
#include "editor/panels/console_panel.h"
#include "editor/ui_context.h"
#include "rendering/irenderer.h"
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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, UIColors::background);
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, UIColors::background);

        bool visible = ImGui::Begin("Main", nullptr, flags);

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(3);

        if (visible && ImGui::BeginMenuBar()) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 7.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));

            ImGui::PushStyleColor(ImGuiCol_Header, UIColors::selection);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, UIColors::panel_hover);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, UIColors::selection);


            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                }
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

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);

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
