#pragma once

#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/framebuffer.h"
#include "systems/rendering/rhi/pipeline_state.h"
#include <memory>

namespace Primitive::RHI {
    class IDevice {
        public:
            virtual ~IDevice() = default;
            virtual std::shared_ptr<IBuffer> create_buffer(const BufferDesc& desc) = 0;
            //virtual std::shared_ptr<ITexture2D> create_texture_2d() = 0;
            virtual std::shared_ptr<IPipelineState> create_pipeline_state(const PipelineDesc& desc) = 0;
            virtual std::shared_ptr<IFramebuffer> create_framebuffer(const FramebufferDesc& desc) = 0;


    };
}
