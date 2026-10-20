#pragma once

#include "Metal/MTLDepthStencil.hpp"
#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/compute_pipeline_state.h"
#include "systems/rendering/rhi/device.h"
#include "systems/rendering/rhi/framebuffer.h"
#include "systems/rendering/rhi/render_pipeline_state.h"


#include <memory>
namespace Primitive {
    class MetalDevice : public IDevice {
        public:
            MetalDevice(MTL::Device* device) {
                metal_device_ = device;
            };

            std::shared_ptr<IBuffer> create_buffer(const BufferDesc& desc) override;
            std::shared_ptr<IRenderPipelineState> create_pipeline_state(const RenderPipelineDesc& desc) override;
            std::shared_ptr<IFramebuffer> create_framebuffer(const FramebufferDesc& desc) override;

        protected:
            MTL::Device* metal_device_;
            MTL::CompareFunction to_metal_compare_function(const CompareFunc& func);
            std::shared_ptr<IRenderPipelineState> create_mesh_pipeline_state(const RenderPipelineDesc& desc);
            std::shared_ptr<IRenderPipelineState> create_vertex_pipeline_state(const RenderPipelineDesc& desc);
            std::shared_ptr<IComputePipelineState> create_compute_pipeline_state(const ComputePipelineDesc& desc);
    };
}
