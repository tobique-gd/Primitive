#pragma once

#include "core/events/ievent.h"
#include "core/events/ieventdispatcher.h"
#include <memory>

namespace Engine
{
    class IEditorPanel {
        public:
            IEditorPanel() = default;
            ~IEditorPanel() = default;
            virtual void on_imgui_render() = 0;
            virtual void on_event(std::shared_ptr<IEvent> event) {};

            void set_event_dispatcher(IEventDispatcher* event_dispatcher) {
                m_event_dispatcher = event_dispatcher;
            }

        protected:
            IEventDispatcher* m_event_dispatcher = nullptr;
    };
}
