#pragma once

#include "rendering/rhi/rhi_buffer.h"
#include "rendering/rhi/rhi_pipeline_state.h"
#include "rendering/rhi/rhi_texture_2d.h"
#include <memory>

namespace Engine::RHI {
    class IRHIDevice {
        public:
            virtual ~IRHIDevice() = default;
            virtual std::shared_ptr<IRHIBuffer> create_buffer(const BufferDesc& desc) = 0;
            //virtual std::shared_ptr<IRHITexture2D> create_texture_2d() = 0;
            virtual std::shared_ptr<IRHIPipelineState> create_pipeline_state(const PipelineDesc& desc) = 0;


    };
}
