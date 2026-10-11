#pragma once

#include "core/events/ieventdispatcher.h"
#include <core/events/ievent.h>
#include <memory>

namespace Primitive {
    class ILayer{
        public:
            virtual ~ILayer() = default;
            virtual void on_attach() {};
            virtual void on_detach() {};
            virtual void on_update() {};
            virtual void on_event(std::shared_ptr<IEvent> event) {};

            virtual void set_event_dispatcher(IEventDispatcher* dispatcher) {
                m_event_dispatcher = dispatcher;
            }

        protected:
            IEventDispatcher* m_event_dispatcher = nullptr;
    };
}
