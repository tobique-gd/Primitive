#pragma once

#include "core/events/ievent.h"
#include "editor/panels/ieditorpanel.h"
#include <memory>

namespace Engine
{
    class HierarchyPanel : public IEditorPanel {
        public:
            HierarchyPanel() = default;
            ~HierarchyPanel() = default;
            void on_imgui_render() override;

            void on_event(std::shared_ptr<IEvent> event) override {} ;


    };
}
