
#include "editor/panels/hierarchy_panel.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "editor/editor_state.h"
#include "editor/ui/IconsFontAwesome6.h"
#include "imgui.h"

namespace Primitive {
    void HierarchyPanel::on_imgui_render() {

        ImGui::Begin("Hierarchy");

        std::shared_ptr<Scene> scene = m_project_manager->get_scene_manager()->get_open_scene();

        if (!scene) {
            ImGui::End();
            return;
        }

        if (ImGui::BeginMenu("Add")) {
            if (ImGui::MenuItem("Empty")) {
                scene->create_entity();
            }
            ImGui::EndMenu();
        }

        entt::registry* active_registry = scene->get_registry();

        auto view = active_registry->view<entt::entity>();




        for (auto entity_id: view) {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap;
            EntityHandle entity(entity_id, active_registry);
            std::string& tag = entity.get_component<TagComponent>().tag;

            if (entity_id == m_editor_state->temporary.selected_entity) {
                flags |= ImGuiTreeNodeFlags_Selected;
                ImGui::TreeNodeEx((void*)(uint64_t)entity_id, flags, tag.c_str());

                // TODO: Impment deletion of entities

                ImGui::TreePop();

            } else {
                if (ImGui::TreeNodeEx((void*)(uint64_t) entity_id, flags, tag.c_str())) {
                    if (ImGui::IsItemClicked()) {
                        m_editor_state->temporary.selected_entity = entity_id;
                    }

                    ImGui::TreePop();
                };
            }


        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
			m_editor_state->temporary.selected_entity = entt::null;
		}

        ImGui::End();

    }


}
