#pragma once

#include "editor/ui/ui_context.h"

namespace MTL { class Device; }

namespace Primitive
{
    class MetalUIBackend final : public Primitive::IUIRendererBackend
    {
    public:
        explicit MetalUIBackend(MTL::Device* device) : m_device(device) {}

        void init() override;
        void new_frame(void* native_pass_descriptor) override;
        void render(ImDrawData* draw_data,
                    void* native_command_buffer,
                    void* native_encoder) override;
        void shutdown() override;

    private:
        MTL::Device* m_device = nullptr;
    };
}
