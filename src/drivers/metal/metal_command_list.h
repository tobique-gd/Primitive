#pragma once

#include "Metal/MTLArgument.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "drivers/metal/metal_buffer.h"
#include "drivers/metal/metal_pipeline_state.h"
#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/command_list.h"
#include <cstdint>
#include <memory>

namespace Primitive::RHI::Metal {
    class MetalCommandList : public ICommandList {
        public:
            explicit MetalCommandList(MTL::RenderCommandEncoder* encoder) : encoder_(encoder) {}

            void set_pipeline_state(std::shared_ptr<IPipelineState> state) override {
                auto metal_pso = std::static_pointer_cast<MetalPipelineState>(state);
                encoder_->setRenderPipelineState(metal_pso->get_pso());
                if (metal_pso->get_depth_stencil()) {
                    encoder_->setDepthStencilState(metal_pso->get_depth_stencil());
                }
                encoder_->setTriangleFillMode(MTL::TriangleFillModeLines);
                encoder_->setFrontFacingWinding(metal_pso->get_winding());
                encoder_->setCullMode(metal_pso->get_cull_mode());
            }

            void set_vertex_buffer(std::shared_ptr<IBuffer> buffer, uint32_t index) override {
                encoder_->setVertexBuffer(std::static_pointer_cast<MetalBuffer>(buffer)->get_native_handle(), 0, index);
            }

            void set_index_buffer(std::shared_ptr<IBuffer> buffer) override {
                bound_index_buffer_ = std::static_pointer_cast<MetalBuffer>(buffer);
            }

            void draw_indexed(uint32_t index_count, uint32_t first_index) override {
                if (!bound_index_buffer_) return;

                size_t offset = first_index * sizeof(uint32_t);
                encoder_->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, index_count, MTL::IndexTypeUInt32, bound_index_buffer_->get_native_handle(), offset);
            }

            void draw(uint32_t vertex_count, uint32_t first_vertex) override {

            }


        private:
            MTL::RenderCommandEncoder* encoder_ = nullptr;
            std::shared_ptr<MetalBuffer> bound_index_buffer_ = nullptr;
    };
}
