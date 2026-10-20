#pragma once

#include "systems/rendering/rhi/compute_pipeline_state.h"
#include <string>

namespace Primitive {
    class MetalComputePipelineState : public IComputePipelineState {
        public:
            MetalComputePipelineState(MTL::ComputePipelineState* pso, std::string debug_name)
            : compute_pso_(pso), debug_name_(debug_name) {}

            ~MetalComputePipelineState() override {
                if (compute_pso_) compute_pso_->release();
            }

            const std::string& get_debug_name() const override { return debug_name_;}


        private:
            MTL::ComputePipelineState* compute_pso_ = nullptr;
            std::string debug_name_;

    };
}
