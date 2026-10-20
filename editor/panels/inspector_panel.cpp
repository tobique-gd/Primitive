
#include "editor/panels/inspector_panel.h"
#include "imgui.h"

namespace Primitive {
    void InspectorPanel::on_imgui_render() {

        ImGui::Begin("Node Inspector");

        if (m_editor_state->temporary.selected_node == nullptr) {
            ImGui::End();
            return;
        }

        auto scene = m_project_manager->get_scene_manager()->get_open_scene();
/*
        entt::registry* active_registry = scene->get_registry();
        entt::entity selected_entity = m_editor_state->temporary.selected_entity;
        EntityHandle entity(selected_entity, active_registry);

		// Laura engine https://github.com/jakubg05/Laura/blob/main/LauraEditor/src/Panels/InspectorPanel/InspectorPanel.cpp
        if (entity.has_component<TagComponent>()) {
			std::string& tag = entity.get_component<TagComponent>().tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer, tag.c_str());
			ImGui::AlignTextToFramePadding();
			ImGui::Text(ICON_FA_TAG " Name:"); ImGui::SameLine();

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputTextWithHint("##Tag Component", "Entity Name", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		if (entity.has_component<IDComponent>()) {
			GUID guid = entity.get_component<IDComponent>().guid;

			ImGui::AlignTextToFramePadding();
			ImGui::Text("Guid:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::Text(guid.string().c_str());
		}
		//
		// */

        ImGui::End();

    }


}
