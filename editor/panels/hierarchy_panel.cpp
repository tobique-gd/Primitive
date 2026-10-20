
#include "editor/panels/hierarchy_panel.h"
#include "editor/editor_state.h"
#include "editor/ui/IconsFontAwesome6.h"
#include "imgui.h"
#include "scene/main/node.h"
#include <memory>
#include <queue>
#include <stack>

namespace Primitive {

    void HierarchyPanel::on_imgui_render() {
        ImGui::Begin("World Inspector");

        std::shared_ptr<Scene> scene = m_project_manager->get_scene_manager()->get_open_scene();

        if (!scene) {
            ImGui::End();
            return;
        }

        if (ImGui::BeginMenu("Add")) {
            if (ImGui::MenuItem("Node")) {
                scene->create_node("Node");
            }

            ImGui::EndMenu();
        }

        Node* root = scene->get_root();

        if (root) {
            render_node(root);
        }

        if (ImGui::IsWindowHovered() &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            !ImGui::IsAnyItemHovered()) {
            m_editor_state->temporary.selected_node = nullptr;
        }

        ImGui::End();
    }

    void HierarchyPanel::render_node(Node* node) {
        if (!node)
            return;

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_AllowOverlap;

        const auto& children = node->get_children();

        if (children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf;

        if (m_editor_state->temporary.selected_node == node)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool opened = ImGui::TreeNodeEx(
            static_cast<void*>(node),
            flags,
            "%s",
            node->get_name().c_str()
        );

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            m_editor_state->temporary.selected_node = node;

        if (opened) {
            for (const auto& child : children)
                render_node(child.get());

            ImGui::TreePop();
        }
    }

}
