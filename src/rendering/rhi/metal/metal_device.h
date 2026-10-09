#include "Metal/MTLDepthStencil.hpp"
#include "rendering/rhi/metal/metal_buffer.h"
#include "rendering/rhi/rhi_buffer.h"
#include "rendering/rhi/rhi_device.h"
#include "rendering/rhi/rhi_pipeline_state.h"


#include <memory>
namespace Engine::RHI::Metal {
    class MetalDevice : public IRHIDevice {
        public:
            std::shared_ptr<RHI::IRHIBuffer> create_buffer(const BufferDesc& desc) override;
            std::shared_ptr<IRHIPipelineState> create_pipeline_state(const PipelineDesc& desc) override;

    private:
        MTL::Device* metal_device_;
        MTL::CompareFunction to_metal_compare_function(const CompareFunc& func);
    };
}
