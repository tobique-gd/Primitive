#pragma once

#include "rendering/rhi/rhi_buffer.h"
#include "rendering/rhi/rhi_pipeline_state.h"
#include <memory>
namespace Engine::RHI {
    class IRHICommandList {
        public:
            virtual ~IRHICommandList() = default;
            virtual void set_pipeline_state(std::shared_ptr<IRHIPipelineState> state) = 0;
            virtual void set_vertex_buffer(std::shared_ptr<IRHIBuffer> buffer, uint32_t index) = 0;
            virtual void set_index_buffer(std::shared_ptr<IRHIBuffer> buffer) = 0;
            virtual void draw_indexed(uint32_t index_count, uint32_t first_index) = 0;



    };
}
