#pragma once

#include "Metal/MTLArgument.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "drivers/metal/metal_buffer.h"
#include "drivers/metal/metal_render_pipeline_state.h"
#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/command_list.h"
#include <cstdint>
#include <iostream>
#include <memory>

namespace Primitive {
    class MetalCommandList : public ICommandList {
        public:
            explicit MetalCommandList(MTL::RenderCommandEncoder* encoder) : encoder_(encoder) {}

            void set_pipeline_state(std::shared_ptr<IRenderPipelineState> state) override {
                if (!encoder_) {
                    std::cerr << "MetalCommandList: encoder is null\n";
                    return;
                }

                if (!state) {
                    std::cerr << "MetalCommandList: pipeline state is null\n";
                    return;
                }

                auto metal_pso = std::static_pointer_cast<MetalRenderPipelineState>(state);

                if (!metal_pso->get_pso()) {
                    std::cerr << "MetalCommandList: native pipeline is null\n";
                    return;
                }

                encoder_->setRenderPipelineState(metal_pso->get_pso());

                if (metal_pso->get_depth_stencil()) {
                    encoder_->setDepthStencilState(metal_pso->get_depth_stencil());
                }

                encoder_->setFrontFacingWinding(metal_pso->get_winding());
                encoder_->setCullMode(metal_pso->get_cull_mode());
            }

            void set_vertex_buffer(std::shared_ptr<IBuffer> buffer, uint32_t index) override {
                encoder_->setVertexBuffer(std::static_pointer_cast<MetalBuffer>(buffer)->get_native_handle(), 0, index);
            }

            void set_mesh_buffer(std::shared_ptr<IBuffer> buffer, uint32_t index) override {

                encoder_->setMeshBuffer(std::static_pointer_cast<MetalBuffer>(buffer)->get_native_handle(), 0, index);
            }

            void set_index_buffer(std::shared_ptr<IBuffer> buffer) override {
                bound_index_buffer_ = std::static_pointer_cast<MetalBuffer>(buffer);
            }

            void draw_indexed(uint32_t index_count, uint32_t first_index) override {
                if (!bound_index_buffer_) return;

                size_t offset = first_index * sizeof(uint32_t);
                encoder_->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, index_count, MTL::IndexTypeUInt32, bound_index_buffer_->get_native_handle(), offset);
            }

            void draw_mesh_threadgroups(MTL::Size threadgroups_per_grid, MTL::Size threads_per_threadgroup, MTL::Size object_threads_per_threadgroup) override {
                encoder_->drawMeshThreadgroups(threadgroups_per_grid, threads_per_threadgroup, object_threads_per_threadgroup);
            }

            void draw(uint32_t vertex_count, uint32_t first_vertex) override {

            }
            void set_viewport(
                uint32_t x,
                uint32_t y,
                uint32_t width,
                uint32_t height
            ) override {
                if (!encoder_ || width == 0 || height == 0) return;

                encoder_->setViewport(MTL::Viewport{
                    static_cast<double>(x),
                    static_cast<double>(y),
                    static_cast<double>(width),
                    static_cast<double>(height),
                    0.0,
                    1.0
                });
            }


        private:
            MTL::RenderCommandEncoder* encoder_ = nullptr;
            std::shared_ptr<MetalBuffer> bound_index_buffer_ = nullptr;
    };
}
