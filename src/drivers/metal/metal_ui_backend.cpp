#include "drivers/metal/metal_ui_backend.h"

#include <Metal/Metal.hpp>
#include "imgui.h"
#include "imgui_impl_metal.h"

namespace Primitive::RHI::Metal
{
    void MetalUIBackend::init()
    {
        ImGui_ImplMetal_Init(m_device);
    }

    void MetalUIBackend::new_frame(void* native_pass_descriptor)
    {
        ImGui_ImplMetal_NewFrame(static_cast<MTL::RenderPassDescriptor*>(native_pass_descriptor));
    }

    void MetalUIBackend::render(ImDrawData* draw_data,
                                void* native_command_buffer,
                                void* native_encoder)
    {
        ImGui_ImplMetal_RenderDrawData(
            draw_data,
            static_cast<MTL::CommandBuffer*>(native_command_buffer),
            static_cast<MTL::RenderCommandEncoder*>(native_encoder));
    }

    void MetalUIBackend::shutdown()
    {
        ImGui_ImplMetal_Shutdown();
    }
}
