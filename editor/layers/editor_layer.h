#pragma once

#include "core/events/ievent.h"
#include "core/layers/ilayer.h"
#include "core/project/project_manager.h"
#include "editor/panels/ieditorpanel.h"
#include "systems/display/iwindow.h"
#include "editor/panels/viewport_panel.h"
#include "systems/rendering/irenderer.h"
#include <array>
#include <memory>

namespace Engine
{
    class EditorLayer : public ILayer {
        public:
            EditorLayer(std::shared_ptr<IWindow> window, std::shared_ptr<ProjectManager> project_manager, std::shared_ptr<IRenderer> renderer);

            virtual void on_attach() override;
            virtual void on_detach() override;
            virtual void on_update() override;
            virtual void on_event(std::shared_ptr<IEvent> event) override;

            virtual void set_event_dispatcher(IEventDispatcher* dispatcher) override;



        private:
            std::shared_ptr<IWindow> m_window;
            std::shared_ptr<ProjectManager> m_project_manager;

            std::array<std::unique_ptr<IEditorPanel>, 3> m_editor_panels;
            std::shared_ptr<IRenderer> m_renderer;

            void create_ui_layer();
    };
}
