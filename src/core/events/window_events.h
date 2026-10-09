#pragma once

#include "core/events/event_type.h"
#include "core/events/ievent.h"

namespace Engine {

    struct WindowResizeEvent : public IEvent {
        int width;
        int height;

        WindowResizeEvent(int v_width, int v_height) : width(v_width), height(v_height) {}

        inline EventType get_type() const override { return EventType::WINDOW_RESIZE; }
    };
}
