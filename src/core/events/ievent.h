#pragma once

#include "core/events/event_type.h"

namespace Engine {

    struct IEvent {
        virtual ~IEvent() = default;
        virtual EventType get_type() const = 0;
        virtual inline bool is_input_event() const { return false; }

        inline void consume() { m_is_consumed = true; }
        inline bool is_consumed() const { return m_is_consumed; }

        private:
            bool m_is_consumed = false ;
    };
}
