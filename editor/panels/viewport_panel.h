#pragma once

#include "core/events/ievent.h"
#include "editor/panels/ieditorpanel.h"
#include "systems/rendering/rhi/texture_2d.h"
#include <memory>

namespace Engine
{
    class ViewportPanel : public IEditorPanel {
        public:
            ViewportPanel() = default;
            ~ViewportPanel() = default;
            void on_imgui_render() override;

            void set_texture(std::shared_ptr<RHI::ITexture2D> texture);
            void on_event(std::shared_ptr<IEvent> event) override ;



        private:
            std::shared_ptr<RHI::ITexture2D> m_texture;
    };
}
