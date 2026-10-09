#pragma once

#include "rendering/irenderer.h"
#include "core/layers/ilayer.h"
#include "core/project/project_manager.h"

#include <memory>

namespace Engine
{
    class RenderLayer : public ILayer {
        public:
            RenderLayer(std::shared_ptr<IRenderer> renderer, std::shared_ptr<ProjectManager> project_manager);

            virtual void on_attach() override;
            virtual void on_detach() override;
            virtual void on_update() override;
            virtual void on_event(std::shared_ptr<IEvent> event) override;

        private:
            std::shared_ptr<ProjectManager> m_project_manager;
            std::shared_ptr<IRenderer> m_renderer;
    };
}
