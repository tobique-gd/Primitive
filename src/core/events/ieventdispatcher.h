#pragma once

#include "core/events/ievent.h"

#include <memory>
namespace Primitive {
    class IEventDispatcher {
        public:
            virtual ~IEventDispatcher() = default;
            virtual void dispatch_event(std::shared_ptr<IEvent> event) = 0;
    };
}
