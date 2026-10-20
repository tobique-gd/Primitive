#pragma once

#include "Metal/MTLDepthStencil.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPipeline.hpp"
#include "systems/rendering/rhi/render_pipeline_state.h"
#include <string>
namespace Primitive {
    class MetalRenderPipelineState : public IRenderPipelineState {
        public:
            MetalRenderPipelineState(MTL::RenderPipelineState* pso, MTL::DepthStencilState* depth_stecil, MTL::Winding winding, MTL::CullMode cull_mode, std::string debug_name)
            : render_pso_(pso), depth_stencil_state_(depth_stecil), winding_(winding), cull_mode_(cull_mode), debug_name_(debug_name) {}

            ~MetalRenderPipelineState() override {
                if (render_pso_) render_pso_->release();
                if (depth_stencil_state_) depth_stencil_state_->release();
            }

            const std::string& get_debug_name() const override { return debug_name_;}

            MTL::RenderPipelineState* get_pso() const { return render_pso_; }
            MTL::DepthStencilState* get_depth_stencil() const { return depth_stencil_state_; }
            MTL::Winding get_winding() const { return winding_; }
            MTL::CullMode get_cull_mode() const { return cull_mode_; }

        private:
            MTL::RenderPipelineState* render_pso_ = nullptr;
            MTL::DepthStencilState* depth_stencil_state_ = nullptr;
            MTL::Winding winding_ = MTL::WindingClockwise;
            MTL::CullMode cull_mode_ = MTL::CullModeBack;
            std::string debug_name_;

    };
}
