#pragma once

#include "core/events/ievent.h"
#include "core/layers/ilayer.h"
#include "core/project/project_manager.h"
#include "editor/editor_state.h"
#include "editor/panels/ieditorpanel.h"
#include "editor/ui/ui_context.h"
#include "systems/display/iwindow.h"
#include "editor/panels/viewport_panel.h"
#include "systems/rendering/rendering_system.h"
#include <array>
#include <memory>

namespace Primitive {
    class EditorLayer : public ILayer {
        public:
            EditorLayer(std::shared_ptr<IWindow> window, std::shared_ptr<ProjectManager> project_manager, std::shared_ptr<RenderingSystem> renderer);

            virtual void on_attach() override;
            virtual void on_detach() override;
            virtual void on_update() override;
            virtual void on_event(std::shared_ptr<IEvent> event) override;

            virtual void set_event_dispatcher(IEventDispatcher* dispatcher) override;



        private:
            std::shared_ptr<IWindow> m_window;
            std::shared_ptr<ProjectManager> m_project_manager;

            std::unique_ptr<UIContext> _ui;
            std::shared_ptr<RenderingSystem> m_renderer;

            std::shared_ptr<EditorState> m_editor_state;
            std::array<std::unique_ptr<IEditorPanel>, 4> m_editor_panels;

            std::unique_ptr<UIContext> m_ui_context;




            void create_ui_layer();
    };
}
