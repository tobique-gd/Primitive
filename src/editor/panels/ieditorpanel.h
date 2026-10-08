#pragma once

#include "core/events/ievent.h"
#include "imgui.h"
#include <memory>

namespace Engine
{
    class IEditorPanel {
        public:
            IEditorPanel() = default;
            ~IEditorPanel() = default;
            virtual void on_imgui_render() = 0;
            virtual void on_event(std::shared_ptr<IEvent> event) {};
    };
}
