#pragma once

#include "core/events/event_type.h"
#include "core/events/ievent.h"
#include "systems/rendering/rhi/texture_2d.h"
#include <memory>

namespace Engine {
    struct NewFrameRenderedEvent : public IEvent {
        std::shared_ptr<RHI::ITexture2D> frame;

        NewFrameRenderedEvent(std::shared_ptr<RHI::ITexture2D> frame) : frame(std::move(frame)) {}

        inline EventType get_type() const override { return EventType::NEW_FRAME_RENDERED; }
    };

    struct ViewportSizeChangedEvent : public IEvent {
        int width;
        int height;

        ViewportSizeChangedEvent(int v_width, int v_height) : width(v_width), height(v_height) {}

        inline EventType get_type() const override { return EventType::VIEWPORT_SIZE_CHANGED; }
    };
}
