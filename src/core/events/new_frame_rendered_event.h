#pragma once

#include "core/events/event_type.h"
#include "core/events/ievent.h"
#include "rendering/itexture_2d.h"
#include <memory>

namespace Engine {
    struct NewFrameRenderedEvent : public IEvent {
        std::shared_ptr<ITexture2D> frame;

        NewFrameRenderedEvent(std::shared_ptr<ITexture2D> frame) : frame(std::move(frame)) {}

        inline EventType get_type() const override { return EventType::NEW_FRAME_RENDERED; }
    };
}
