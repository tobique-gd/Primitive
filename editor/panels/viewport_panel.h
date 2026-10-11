#pragma once

#include "core/events/ievent.h"
#include "editor/panels/ieditorpanel.h"
#include "systems/rendering/rhi/texture_2d.h"
#include <memory>

namespace Primitive {
    class ViewportPanel : public IEditorPanel {
        public:
            ViewportPanel(std::shared_ptr<EditorState> editor_state) : m_editor_state(editor_state) {};
            ~ViewportPanel() = default;
            void on_imgui_render() override;

            void set_texture(std::shared_ptr<RHI::ITexture2D> texture);
            void on_event(std::shared_ptr<IEvent> event) override ;

            bool m_toolbar_visible = true;
            float m_toolbar_offset = 0.0f;


        private:
            std::shared_ptr<RHI::ITexture2D> m_texture;
            std::shared_ptr<EditorState> m_editor_state;
    };
}
