#include "editor/layers/editor_layer.h"
#include "core/events/ievent.h"
#include "editor/panels/inspector_panel.h"
#include "editor/ui/IconsFontAwesome6.h"
#include "editor/panels/hierarchy_panel.h"
#include "editor/panels/viewport_panel.h"
#include "editor/panels/console_panel.h"
#include "editor/ui/backend/glfw_ui_backend.h"
#include "editor/ui/backend/metal_ui_backend.h"
#include "editor/ui/ui_theme.h"
#include "imgui_internal.h"
#include "imgui.h"
#include "systems/rendering/rendering_system.h"

#include <iostream>
#include <memory>
namespace Primitive {
    EditorLayer::EditorLayer(std::shared_ptr<IWindow> window,
                            std::shared_ptr<ProjectManager> project_manager,
                            std::shared_ptr<RenderingSystem> renderer)
                            : m_window(window)
                            , m_project_manager(project_manager)
                            , m_renderer(renderer)
                            , m_editor_state(std::make_shared<EditorState>())
                            , m_editor_panels({std::make_unique<ViewportPanel>(m_editor_state),
                                std::make_unique<ConsolePanel>(),
                                std::make_unique<HierarchyPanel>(m_editor_state, m_project_manager),
                                std::make_unique<InspectorPanel>(m_editor_state, m_project_manager)})
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

    void EditorLayer::on_attach() {
        GUID s_guid = m_project_manager->get_scene_manager()->create_scene();
        m_project_manager->get_scene_manager()->set_open_scene_guid(s_guid);

        m_ui_context = std::make_unique<UIContext>(
            std::make_unique<GLFWUIBackend>(),
            std::make_unique<MetalUIBackend>(
                static_cast<MTL::Device*>(m_renderer->get_native_device())
            )
        );

        m_ui_context->init(*m_window);
    }

    void EditorLayer::on_detach()
    {
        if (m_ui_context) {
            m_ui_context->shutdown();
            m_ui_context.reset();
        }
    }

    void EditorLayer::on_update() {

        void* pass_descriptor =
                m_renderer->get_overlay_render_pass_descriptor();

            if (!pass_descriptor)
                return;

        m_ui_context->begin_frame(pass_descriptor);

        create_ui_layer();


        for (auto& panel : m_editor_panels) {
            panel->on_imgui_render();
        }

        m_ui_context->end_frame();

        m_renderer->set_overlay_callback(
            [this](void* command_buffer, void* encoder) {
                m_ui_context->render(command_buffer, encoder);
            }
        );


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


        const float item_height = 24.0f;
        const float bar_padding_y = 4.0f;
        const float bar_height = item_height + bar_padding_y * 2.0f;
        const float frame_pad_y = (bar_height - ImGui::GetFontSize()) * 0.5f;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, frame_pad_y));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, UIColors::background);
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, UIColors::background);

        bool visible = ImGui::Begin("Main", nullptr, flags);

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(4);

        if (visible && ImGui::BeginMenuBar()) {
            const float bar_top = ImGui::GetCursorPosY();
            const float menu_bar_w = ImGui::GetWindowWidth();
            const float text_h = ImGui::GetTextLineHeight();
            const float item_y = bar_top + bar_padding_y;

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, frame_pad_y));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_MenuItemRounding, 0.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, UIColors::text_disabled);
            ImGui::PushStyleColor(ImGuiCol_Header, UIColors::selection);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, UIColors::panel_hover);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, UIColors::selection);

            auto begin_top_menu = [&](const char* label) {
                ImGui::SetCursorPosY(bar_top);
                ImGui::AlignTextToFramePadding();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, bar_height - text_h));
                bool open = ImGui::BeginMenu(label);
                ImGui::PopStyleVar();
                return open;
            };

            ImGui::SetCursorPosY(item_y);
            ImGui::Button("L", ImVec2(28.0f, item_height));

            if (begin_top_menu("Scene")) {
                ImGui::PushStyleColor(ImGuiCol_Text, UIColors::text);
                if (ImGui::MenuItem("New Scene")) {
                }
                if (ImGui::MenuItem("Open Scene")) {
                }
                ImGui::PopStyleColor();
                ImGui::EndMenu();
            }
            if (begin_top_menu("Project"))   { ImGui::EndMenu(); }
            if (begin_top_menu("View"))   { ImGui::EndMenu(); }
            if (begin_top_menu("Window")) { ImGui::EndMenu(); }

            const char* icon = ICON_FA_PLAY;
            const ImVec2 button_size(32.0f, item_height);

            ImGui::SetCursorPos(ImVec2((menu_bar_w - button_size.x) * 0.5f, item_y));

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            bool clicked = ImGui::Button("##Play", button_size);
            ImGui::PopStyleColor(3);

            const bool hovered = ImGui::IsItemHovered();
            const bool active  = ImGui::IsItemActive();

            ImVec4 icon_color = UIColors::text_disabled;
            if (active) {
                icon_color = UIColors::text;
            } else if (hovered) {
                const float t = 0.5f;
                icon_color = ImVec4(
                    UIColors::text_disabled.x + (UIColors::text.x - UIColors::text_disabled.x) * t,
                    UIColors::text_disabled.y + (UIColors::text.y - UIColors::text_disabled.y) * t,
                    UIColors::text_disabled.z + (UIColors::text.z - UIColors::text_disabled.z) * t,
                    UIColors::text_disabled.w + (UIColors::text.w - UIColors::text_disabled.w) * t
                );
            }

            const ImVec2 icon_size = ImGui::CalcTextSize(icon);
            const ImVec2 rect_min  = ImGui::GetItemRectMin();
            ImGui::GetWindowDrawList()->AddText(
                ImVec2(rect_min.x + (button_size.x - icon_size.x) * 0.5f,
                       rect_min.y + (button_size.y - icon_size.y) * 0.5f),
                ImGui::ColorConvertFloat4ToU32(icon_color),
                icon
            );

            if (clicked) {
            }

            const char* breadcrumb = "Project Name > Scene 8";
            const ImVec2 bc_size = ImGui::CalcTextSize(breadcrumb);
            const ImVec2 win_pos = ImGui::GetWindowPos();
            ImGui::GetWindowDrawList()->AddText(
                ImVec2(win_pos.x + menu_bar_w - bc_size.x - 16.0f,
                       win_pos.y + bar_top + (bar_height - bc_size.y) * 0.5f),
                ImGui::ColorConvertFloat4ToU32(UIColors::text_disabled),
                breadcrumb
            );

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar(4);

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
