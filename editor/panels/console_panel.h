#pragma once

#include "core/events/ievent.h"
#include "editor/panels/ieditorpanel.h"
#include <memory>

namespace Engine
{
    class ConsolePanel : public IEditorPanel {
        public:
            ConsolePanel() = default;
            ~ConsolePanel() = default;
            void on_imgui_render() override;

            void on_event(std::shared_ptr<IEvent> event) override {} ;


    };
}
