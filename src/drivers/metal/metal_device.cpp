#include "drivers/metal/metal_device.h"
#include "Metal/MTLDepthStencil.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPipeline.hpp"
#include "systems/rendering/rhi/framebuffer.h"
#include "drivers/metal/metal_buffer.h"
#include "drivers/metal/metal_framebuffer.h"
#include "drivers/metal/metal_pipeline_state.h"
#include "systems/rendering/rhi/pipeline_state.h"

#include <iostream>
#include <memory>
namespace Primitive {

    std::shared_ptr<IBuffer> MetalDevice::create_buffer(const BufferDesc& desc) {
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

    std::shared_ptr<IFramebuffer> MetalDevice::create_framebuffer(const FramebufferDesc& desc) {
        return std::make_shared<MetalFramebuffer>(metal_device_, desc);
    }

    std::shared_ptr<IPipelineState> MetalDevice::create_pipeline_state(const PipelineDesc& desc) {
        if (desc.pipeline_type == PipelineType::Standard) {
            auto vertex_function = static_cast<MTL::Function*>(desc.vertex_shader);
            auto fragment_function = static_cast<MTL::Function*>(desc.fragment_shader);

            auto* pso_desc = MTL::RenderPipelineDescriptor::alloc()->init();
            pso_desc->setVertexFunction(vertex_function);
            pso_desc->setFragmentFunction(fragment_function);
            pso_desc->colorAttachments()->object(0)->setPixelFormat(static_cast<MTL::PixelFormat>(desc.color_format));
            pso_desc->setSampleCount(desc.sample_count);
            pso_desc->setDepthAttachmentPixelFormat(static_cast<MTL::PixelFormat>(desc.depth_format));

            NS::Error* error = nullptr;
            MTL::RenderPipelineState* pso =
                metal_device_->newRenderPipelineState(pso_desc, &error);


            if (!pso) {
                std::cerr << "Failed to create pipeline '"
                            << desc.debug_name << "': "
                            << (error ? error->localizedDescription()->utf8String()
                                    : "Unknown Metal error")
                            << '\n';

                pso_desc->release();
                return nullptr;
            }

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

            //MTL::CullMode cull = (desc.rasterizer.cull_mode == CullMode::Back) ? MTL::CullModeBack : (desc.rasterizer.cull_mode == CullMode::Front ? MTL::CullModeFront : MTL::CullModeNone);
            MTL::CullMode cull = MTL::CullModeNone;

            return std::make_shared<MetalPipelineState>(pso, dss, winding, cull, desc.debug_name);

        } else if (desc.pipeline_type == PipelineType::Mesh) {

            auto mesh_function = static_cast<MTL::Function*>(desc.mesh_shader);
            auto fragment_function = static_cast<MTL::Function*>(desc.fragment_shader);

            auto* pso_desc = MTL::MeshRenderPipelineDescriptor::alloc()->init();
            pso_desc->setMeshFunction(mesh_function);
            pso_desc->setFragmentFunction(fragment_function);
            pso_desc->colorAttachments()->object(0)->setPixelFormat(static_cast<MTL::PixelFormat>(desc.color_format));
            pso_desc->setRasterSampleCount(desc.sample_count);
            pso_desc->setDepthAttachmentPixelFormat(static_cast<MTL::PixelFormat>(desc.depth_format));

            pso_desc->setMaxTotalThreadsPerMeshThreadgroup(32);
            pso_desc->setMaxTotalThreadsPerObjectThreadgroup(0);
            pso_desc->setMeshThreadgroupSizeIsMultipleOfThreadExecutionWidth(true);

            NS::Error* error = nullptr;
            MTL::RenderPipelineState* pso = metal_device_->newRenderPipelineState(pso_desc, MTL::PipelineOptionNone, nullptr, &error);


            if (!pso) {
                std::cerr << "Failed to create pipeline '"
                            << desc.debug_name << "': "
                            << (error ? error->localizedDescription()->utf8String()
                                    : "Unknown Metal error")
                            << '\n';

                pso_desc->release();
                return nullptr;
            }

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

            //MTL::CullMode cull = (desc.rasterizer.cull_mode == CullMode::Back) ? MTL::CullModeBack : (desc.rasterizer.cull_mode == CullMode::Front ? MTL::CullModeFront : MTL::CullModeNone);
            MTL::CullMode cull = MTL::CullModeNone;

            return std::make_shared<MetalPipelineState>(pso, dss, winding, cull, desc.debug_name);

        };

    }
}
