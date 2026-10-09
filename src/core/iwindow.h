#pragma once

#include "core/events/ievent.h"
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace Engine
{
    struct WindowProps {
        std::string title;
        int width, height;
        bool vsync;
        WindowProps(const std::string& title = std::string("Engine Name"),
        int width = 1280, int height = 720, bool vsync = false) : title(title), width(width), height(height), vsync(vsync) {

        }
    };

    class IWindow {
        public:
            using EventCallbackFn = std::function<void(IEvent&)>;

            virtual ~IWindow() = default;
            static std::shared_ptr<IWindow> create_window(const WindowProps& props = WindowProps());

            virtual void on_update() = 0;
            virtual bool should_close() = 0;
            virtual void set_title(const std::string& title) = 0;
            virtual glm::ivec2 get_frame_buffer_size() const = 0;
            virtual void* get_native_window() const = 0;
            virtual bool is_vsync() const = 0;
            virtual bool is_fullscreen() const = 0;
            virtual void set_fullscreen(bool enabled) = 0;

            virtual void minimize() = 0;
            virtual void maximize() = 0;
            virtual void restore() = 0;
            virtual bool is_maximized() const = 0;
            virtual void close() = 0;
            virtual void set_position(int x, int y) = 0;
            virtual glm::ivec2 get_position() const = 0;
            virtual void set_event_callback(const EventCallbackFn& callback) = 0;

            //virtual bool is_key_pressed(KeyCode key) = 0;





    };
}
