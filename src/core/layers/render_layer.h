#pragma once

#include "core/layers/ilayer.h"
#include "core/project/project_manager.h"
#include "systems/rendering/rendering_system.h"

#include <memory>

namespace Primitive {
    class RenderLayer : public ILayer {
        public:
            RenderLayer(std::shared_ptr<RenderingSystem> renderer, std::shared_ptr<ProjectManager> project_manager);

            virtual void on_attach() override;
            virtual void on_detach() override;
            virtual void on_update() override;
            virtual void on_event(std::shared_ptr<IEvent> event) override;

        private:
            std::shared_ptr<ProjectManager> m_project_manager;
            std::shared_ptr<RenderingSystem> m_renderer;
    };
}
