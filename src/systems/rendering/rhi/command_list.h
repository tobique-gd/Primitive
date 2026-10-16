#pragma once

#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/pipeline_state.h"
#include <memory>
namespace Primitive {
    class ICommandList {
        public:
            virtual ~ICommandList() = default;
            virtual void set_pipeline_state(std::shared_ptr<IPipelineState> state) = 0;
            virtual void set_vertex_buffer(std::shared_ptr<IBuffer> buffer, uint32_t index) = 0;
            virtual void set_index_buffer(std::shared_ptr<IBuffer> buffer) = 0;
            virtual void draw_indexed(uint32_t index_count, uint32_t first_index) = 0;
            virtual void draw(uint32_t vertex_count, uint32_t first_vertex) = 0;


    };
}
