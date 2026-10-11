#pragma once

#include <memory>

struct ImDrawData;

namespace Primitive {
    class IWindow;

    class IUIPlatformBackend
    {
    public:
        virtual ~IUIPlatformBackend() = default;
        virtual void init(IWindow& window) = 0;
        virtual void new_frame() = 0;
        virtual void shutdown() = 0;
    };

    class IUIRendererBackend
    {
    public:
        virtual ~IUIRendererBackend() = default;
        virtual void init() = 0;
        virtual void new_frame(void* native_pass_descriptor) = 0;
        virtual void render(ImDrawData* draw_data,
                            void* native_command_buffer,
                            void* native_encoder) = 0;
        virtual void shutdown() = 0;
    };

    class UIContext
    {
    public:
        UIContext(std::unique_ptr<IUIPlatformBackend> platform,
                  std::unique_ptr<IUIRendererBackend> renderer);
        ~UIContext();

        UIContext(const UIContext&) = delete;
        UIContext& operator=(const UIContext&) = delete;

        void init(IWindow& window);
        void shutdown();

        void begin_frame(void* target_pass_descriptor);

        void end_frame();

        ImDrawData* draw_data() const;

        void render(void* native_command_buffer, void* native_encoder);

        bool is_initialized() const { return m_initialized; }

    private:
        void load_fonts();

        std::unique_ptr<IUIPlatformBackend> m_platform;
        std::unique_ptr<IUIRendererBackend> m_renderer;
        bool m_initialized = false;
    };
}
