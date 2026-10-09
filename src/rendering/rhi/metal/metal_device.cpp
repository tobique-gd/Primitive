#include "rendering/rhi/metal/metal_device.h"
#include "Metal/MTLDepthStencil.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPipeline.hpp"
#include "rendering/rhi/metal/metal_buffer.h"
#include "rendering/rhi/metal/metal_pipeline_state.h"
#include "rendering/rhi/rhi_pipeline_state.h"

#include <memory>
namespace Engine::RHI::Metal {
    std::shared_ptr<IRHIBuffer> MetalDevice::create_buffer(const BufferDesc& desc) {
        return std::make_shared<MetalBuffer>(metal_device_, desc);
    }

    MTL::CompareFunction MetalDevice::to_metal_compare_function(const CompareFunc& func) {
        switch (func) {
            case CompareFunc::Never:
                return MTL::CompareFunctionNever;
                break;
            case CompareFunc::Less:
                return MTL::CompareFunctionNever;
                break;
            case CompareFunc::Equal:
                return MTL::CompareFunctionEqual;
                break;
            case CompareFunc::LessEqual:
                return MTL::CompareFunctionLessEqual;
                break;

            case CompareFunc::Greater:
                return MTL::CompareFunctionGreater;
                break;
            case CompareFunc::GreaterEqual:
                return MTL::CompareFunctionGreaterEqual;
                break;

            case CompareFunc::NotEqual:
                return MTL::CompareFunctionNotEqual;
                break;

            case CompareFunc::Always:
                return MTL::CompareFunctionAlways;
                break;

            default:
                return MTL::CompareFunctionAlways;
                break;
        }
    }

    std::shared_ptr<IRHIPipelineState> MetalDevice::create_pipeline_state(const PipelineDesc& desc) {
        auto vertex_function = static_cast<MTL::Function*>(desc.vertex_shader);
        auto fragment_function = static_cast<MTL::Function*>(desc.fragment_shader);

        auto* pso_desc = MTL::RenderPipelineDescriptor::alloc()->init();
        pso_desc->setVertexFunction(vertex_function);
        pso_desc->setFragmentFunction(fragment_function);
        pso_desc->colorAttachments()->object(0)->setPixelFormat(static_cast<MTL::PixelFormat>(desc.color_format));
        pso_desc->setSampleCount(desc.sample_count);
        pso_desc->setDepthAttachmentPixelFormat(static_cast<MTL::PixelFormat>(desc.depth_format));

        NS::Error* error = nullptr;
        MTL::RenderPipelineState* pso = metal_device_->newRenderPipelineState(pso_desc, &error);
        pso_desc->release();

        MTL::DepthStencilState* dss = nullptr;
        if (desc.depth_stencil.depth_test_enable) {
            auto* depth_stencil_descriptor = MTL::DepthStencilDescriptor::alloc()->init();
            depth_stencil_descriptor->setDepthCompareFunction(to_metal_compare_function(desc.depth_stencil.depth_compare_op));
            depth_stencil_descriptor->setDepthWriteEnabled(desc.depth_stencil.depth_write_enable);
            dss = metal_device_->newDepthStencilState(depth_stencil_descriptor);
            depth_stencil_descriptor->release();
        }

        MTL::Winding winding = (desc.rasterizer.winding_order == WindingOrder::Clockwise) ? MTL::WindingClockwise : MTL::WindingCounterClockwise;

        MTL::CullMode cull = (desc.rasterizer.cull_mode == CullMode::Back) ? MTL::CullModeBack : (desc.rasterizer.cull_mode == CullMode::Front ? MTL::CullModeFront : MTL::CullModeNone);

        return std::make_shared<MetalPipelineState>(pso, dss, winding, cull, desc.debug_name);
    }
}
