#pragma once

namespace Engine
{
    enum EventType {
        KEY_PRESS,
        KEY_RELEASE,
        MOUSE_MOVE,
        MOUSE_BUTTON_PRESS,
        MOUSE_BUTTON_RELEASE,
        MOUSE_SCROLL,
        WINDOW_RESIZE,
        NEW_FRAME_RENDERED,
        VIEWPORT_SIZE_CHANGED,
    };
}
