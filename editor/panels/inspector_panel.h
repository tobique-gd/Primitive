#pragma once

#include "core/events/ievent.h"
#include "core/project/project_manager.h"
#include "editor/panels/ieditorpanel.h"
#include <memory>

namespace Primitive
{
    class InspectorPanel : public IEditorPanel {
        public:
            InspectorPanel(std::shared_ptr<EditorState> editor_state, std::shared_ptr<ProjectManager> project_manager) : m_project_manager(project_manager), m_editor_state(editor_state) {};
            ~InspectorPanel() = default;
            void on_imgui_render() override;

            void on_event(std::shared_ptr<IEvent> event) override {} ;

        private:
            std::shared_ptr<ProjectManager> m_project_manager;
            std::shared_ptr<EditorState> m_editor_state;

    };
}
