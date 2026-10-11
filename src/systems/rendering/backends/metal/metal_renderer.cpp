#include "metal_renderer.h"
#include "GLFW/glfw3.h"
#include "Metal/MTLArgument.hpp"
#include "Metal/MTLDepthStencil.hpp"
#include "Metal/MTLPixelFormat.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPass.hpp"
#include "Metal/MTLResource.hpp"
#include "drivers/metal/metal_swapchain.h"
#include "drivers/metal/metal_ui_backend.h"
#include "editor/ui/ui_context.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "glm/matrix.hpp"
#include "platform/macos/cocoa_bridge.h"

#include "platform/macos/glfw_ui_backend.h"
#include "systems/display/iwindow.h"
#include "systems/rendering/passes/scene_render_pass.h"
#include "systems/rendering/rhi/framebuffer.h"
#include "drivers/metal/metal_command_list.h"

#include "drivers/metal/metal_device.h"

#include <CoreGraphics/CoreGraphics.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/device.h"
#include "systems/rendering/rhi/pipeline_state.h"
#include "systems/rendering/rhi/swapchain.h"
#include "systems/rendering/rhi/texture_2d.h"
#include "systems/rendering/shaders/shader_compiler.h"
#include "vertex_data.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <simd/simd.h>
#include <cmath>

namespace Primitive {
    struct MetalRenderer::Impl {
        MTL::Device* raw_metal_device = nullptr;
        std::unique_ptr<RHI::IDevice> rhi_device;

        MTL::CommandQueue* metal_command_queue = nullptr;
        MTL::CommandBuffer* metal_command_buffer = nullptr;

        std::shared_ptr<RHI::IBuffer> vertex_buffer;
        std::shared_ptr<RHI::IBuffer> index_buffer;
        std::shared_ptr<RHI::IBuffer> transform_buffer;
        std::shared_ptr<RHI::IPipelineState> pipeline_state;
        std::shared_ptr<RHI::ITexture2D> viewport_texture;

        std::shared_ptr<RHI::IFramebuffer> framebuffer;

        MTL::Library* metal_vertex_library = nullptr;
        MTL::Library* metal_fragment_library = nullptr;

        glm::ivec2 viewport_size = {1, 1};
        int sample_count = 4;

        std::unique_ptr<SceneRenderPass> m_scene_render_pass;
        std::unique_ptr<UIContext> ui;

        std::unique_ptr<RHI::ISwapchain> swapchain;
        NS::AutoreleasePool* frame_pool = nullptr;
        MTL::RenderPassDescriptor* ui_pass_descriptor = nullptr;
        bool frame_active = false;
    };

    MetalRenderer::MetalRenderer() : metal_backend( new Impl() ) {}

    MetalRenderer::~MetalRenderer() {
        shutdown();
        delete metal_backend;
    }

    void MetalRenderer::set_viewport_size(int width, int height) {
        if (width <= 0 || height <= 0) return;

        if (metal_backend->viewport_size.x == width &&
            metal_backend->viewport_size.y == height) return;

        metal_backend->viewport_size = { width, height };

        if (metal_backend->framebuffer) {
            metal_backend->framebuffer->resize(width, height);
        }
    }


    void MetalRenderer::init(IWindow& window)
    {
        init_device();
        init_window(window);
        create_buffers();

        metal_backend->ui = std::make_unique<UIContext>(std::make_unique<GLFWUIBackend>(), std::make_unique<RHI::Metal::MetalUIBackend>(metal_backend->raw_metal_device));
        metal_backend->ui->init(window);

        create_cube();
        create_default_library();
        create_command_queue();

        const auto color_format = static_cast<MTL::PixelFormat>(metal_backend->swapchain->get_color_format());

        RHI::FramebufferDesc fb_desc{};
        fb_desc.width = metal_backend->viewport_size.x;
        fb_desc.height = metal_backend->viewport_size.y;
        fb_desc.sample_count = metal_backend->sample_count;
        fb_desc.color_attachment.format = color_format;
        fb_desc.depth_attachment.format = MTL::PixelFormatDepth32Float;

        metal_backend->framebuffer = metal_backend->rhi_device->create_framebuffer(fb_desc);


        create_render_pipeline();

        metal_backend->m_scene_render_pass = std::make_unique<SceneRenderPass>(
            metal_backend->vertex_buffer,
            metal_backend->index_buffer,
            metal_backend->transform_buffer,
            metal_backend->pipeline_state
        );

    }


    void MetalRenderer::init_device()
    {
        metal_backend->raw_metal_device = MTL::CreateSystemDefaultDevice();
        metal_backend->rhi_device = std::make_unique<RHI::Metal::MetalDevice>(metal_backend->raw_metal_device);
    }

    void MetalRenderer::init_window(IWindow& window)
    {
        glm::ivec2 size = window.get_frame_buffer_size();

        RHI::SwapchainDesc desc{};
        desc.native_window = window.get_native_window();
        desc.width = size.x;
        desc.height = size.y;
        desc.vsync = window.is_vsync();

        metal_backend->swapchain = std::make_unique<RHI::Metal::MetalSwapchain>( metal_backend->raw_metal_device, desc);

    }

    void MetalRenderer::create_default_library() {
      NS::Error* error = nullptr;

      try {
          ShaderCompiler compiler;

          std::string vert_msl = compiler.compile_glsl_file_to_msl("src/systems/rendering/shaders/forward_3d.vert.glsl");
          std::string frag_msl = compiler.compile_glsl_file_to_msl("src/systems/rendering/shaders/forward_3d.frag.glsl");


          std::cout << "trasnlsateed vertex msl:\n" << vert_msl;
          std::cout << "translated fragment msl:\n" << frag_msl;


          metal_backend->metal_vertex_library = metal_backend->raw_metal_device->newLibrary(
            NS::String::string(vert_msl.c_str(), NS::UTF8StringEncoding),
            nullptr,
            &error
          );

          if (!metal_backend->metal_vertex_library) {
            std::cerr << "Failed to compile translated vertex shader to Metal";
            if (error != nullptr) {
              std::cerr << "\nMetal error: " << error->localizedDescription()->utf8String();
            }
            std::exit(-1);
          }

          error = nullptr;
          metal_backend->metal_fragment_library = metal_backend->raw_metal_device->newLibrary(
            NS::String::string(frag_msl.c_str(), NS::UTF8StringEncoding),
            nullptr,
            &error
          );

          if (!metal_backend->metal_fragment_library) {
            std::cerr << "Failed to compile translated fragment shader to Metal";
            if (error != nullptr) {
              std::cerr << "\nMetal error: " << error->localizedDescription()->utf8String();
            }
            std::exit(-1);
          }

      } catch (const std::exception& e) {
          std::cerr << "Failed to compile shaders: " << e.what() << std::endl;
          std::exit(-1);
      }
    }



    void MetalRenderer::shutdown()
    {
        if (!metal_backend || !metal_backend->raw_metal_device)
            return;

        auto& m = *metal_backend;

        if (m.metal_command_queue) {
            if (auto* cmd = m.metal_command_queue->commandBuffer()) {
                cmd->commit();
                cmd->waitUntilCompleted();
            }
        }

        m.m_scene_render_pass.reset();
        m.pipeline_state.reset();
        m.framebuffer.reset();
        m.vertex_buffer.reset();
        m.index_buffer.reset();
        m.transform_buffer.reset();

        m.ui.reset();
        m.swapchain.reset();

        if (m.metal_vertex_library)   { m.metal_vertex_library->release();   m.metal_vertex_library = nullptr; }
        if (m.metal_fragment_library) { m.metal_fragment_library->release(); m.metal_fragment_library = nullptr; }
        if (m.metal_command_queue)    { m.metal_command_queue->release();    m.metal_command_queue = nullptr; }

        m.rhi_device.reset();

        m.raw_metal_device->release();
        m.raw_metal_device = nullptr;
    }


    void MetalRenderer::begin_frame()
    {
        auto& m = *metal_backend;

        m.frame_pool = NS::AutoreleasePool::alloc()->init();
        m.frame_active = m.swapchain->acquire_next_frame();

        if (!m.frame_active)
            return;

        m.ui_pass_descriptor = static_cast<MTL::RenderPassDescriptor*>(m.swapchain->get_native_render_pass_descriptor(RHI::LoadOp::Clear));

        m.ui->begin_frame(m.ui_pass_descriptor);
    }


    void MetalRenderer::end_frame()
    {
        auto& m = *metal_backend;

        if (m.frame_active)
        {
            m.ui->end_frame();

            auto* command_buffer = m.metal_command_queue->commandBuffer();
            auto* encoder = command_buffer->renderCommandEncoder(m.ui_pass_descriptor);

            m.ui->render(command_buffer, encoder);

            encoder->endEncoding();
            m.swapchain->present(command_buffer);
            command_buffer->commit();
        }

        m.frame_active = false;
        m.ui_pass_descriptor = nullptr;

        if (m.frame_pool) {
            m.frame_pool->release();
            m.frame_pool = nullptr;
        }
    }

    void MetalRenderer::on_window_resize(int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        metal_backend->swapchain->resize(width, height);
    }


    void MetalRenderer::draw() {
        if (!metal_backend || !metal_backend->framebuffer) return;

        NS::AutoreleasePool* frame_pool = NS::AutoreleasePool::alloc()->init();

        send_render_command();

        frame_pool->release();

    }

    std::shared_ptr<RHI::ITexture2D> MetalRenderer::get_viewport_texture()
    {
        return std::static_pointer_cast<RHI::ITexture2D>(metal_backend->framebuffer->get_color_attachment());
    }

    void MetalRenderer::update_scene_transforms() {
        glm::mat4x4 transform = glm::mat4x4(1.0f);
        transform = glm::translate(transform, glm::vec3(0, 0, -1.0f));

        float angle_in_degrees = 45.0f * glfwGetTime() / 2.0;
        float angle_in_radians = angle_in_degrees * M_PI / 180.0f;
        transform = glm::rotate(transform, angle_in_radians, glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4x4 model_matrix = transform;

        glm::vec3 R = {1, 0, 0}, U = {0, 1, 0}, F = {0, 0, -1}, P = {0, 0, 1};

        glm::mat4x4 view_matrix = glm::mat4x4(
                glm::vec4(R.x, U.x, -F.x, 0.0f),
                glm::vec4(R.y, U.y, -F.y, 0.0f),
                glm::vec4(R.z, U.z, -F.z, 0.0f),
                glm::vec4(dot(-R, P), dot(-U, P), dot(F, P), 1.0f)
            );

        float width = metal_backend->viewport_size.x;
        float height = metal_backend->viewport_size.y;

        float aspect_ratio = width / height;

        float fov = 90.0f * (M_PI / 180.0f);
        float near_z = 0.1f;
        float far_z = 100.0f;

        glm::mat4x4 perspective_matrix = glm::perspectiveRH_ZO(fov, aspect_ratio, near_z, far_z); // matrix_perspective_right_hand( fov, aspect_ratio, near_z,far_z);

        TransformationData transformation_data = {
            model_matrix,
            view_matrix,
            perspective_matrix
        };

        metal_backend->transform_buffer->update_data(&transformation_data, sizeof(transformation_data), 0);
    }

    void MetalRenderer::send_render_command() {
        metal_backend->metal_command_buffer = metal_backend->metal_command_queue->commandBuffer();

        auto* pass_desc = static_cast<MTL::RenderPassDescriptor*>(metal_backend->framebuffer->get_native_render_pass_descriptor());

        MTL::RenderCommandEncoder* encoder = metal_backend->metal_command_buffer->renderCommandEncoder(pass_desc);

        RHI::Metal::MetalCommandList command_list(encoder);
        RHI::RenderContext context(command_list);

        update_scene_transforms();
        metal_backend->m_scene_render_pass->execute(context);

        encoder->endEncoding();
        metal_backend->metal_command_buffer->commit();



    }

    void MetalRenderer::create_cube() {
        VertexData cubeVertices[] = {
            {{-0.5, -0.5,  0.5, 1.0}, { 0.0,  0.0,  1.0}, {0.0, 0.0}},
            {{ 0.5, -0.5,  0.5, 1.0}, { 0.0,  0.0,  1.0}, {1.0, 0.0}},
            {{ 0.5,  0.5,  0.5, 1.0}, { 0.0,  0.0,  1.0}, {1.0, 1.0}},
            {{-0.5,  0.5,  0.5, 1.0}, { 0.0,  0.0,  1.0}, {0.0, 1.0}},

            {{ 0.5, -0.5, -0.5, 1.0}, { 0.0,  0.0, -1.0}, {0.0, 0.0}},
            {{-0.5, -0.5, -0.5, 1.0}, { 0.0,  0.0, -1.0}, {1.0, 0.0}},
            {{-0.5,  0.5, -0.5, 1.0}, { 0.0,  0.0, -1.0}, {1.0, 1.0}},
            {{ 0.5,  0.5, -0.5, 1.0}, { 0.0,  0.0, -1.0}, {0.0, 1.0}},

            {{-0.5,  0.5,  0.5, 1.0}, { 0.0,  1.0,  0.0}, {0.0, 0.0}},
            {{ 0.5,  0.5,  0.5, 1.0}, { 0.0,  1.0,  0.0}, {1.0, 0.0}},
            {{ 0.5,  0.5, -0.5, 1.0}, { 0.0,  1.0,  0.0}, {1.0, 1.0}},
            {{-0.5,  0.5, -0.5, 1.0}, { 0.0,  1.0,  0.0}, {0.0, 1.0}},

            {{-0.5, -0.5, -0.5, 1.0}, { 0.0, -1.0,  0.0}, {0.0, 0.0}},
            {{ 0.5, -0.5, -0.5, 1.0}, { 0.0, -1.0,  0.0}, {1.0, 0.0}},
            {{ 0.5, -0.5,  0.5, 1.0}, { 0.0, -1.0,  0.0}, {1.0, 1.0}},
            {{-0.5, -0.5,  0.5, 1.0}, { 0.0, -1.0,  0.0}, {0.0, 1.0}},

            {{ 0.5, -0.5,  0.5, 1.0}, { 1.0,  0.0,  0.0}, {0.0, 0.0}},
            {{ 0.5, -0.5, -0.5, 1.0}, { 1.0,  0.0,  0.0}, {1.0, 0.0}},
            {{ 0.5,  0.5, -0.5, 1.0}, { 1.0,  0.0,  0.0}, {1.0, 1.0}},
            {{ 0.5,  0.5,  0.5, 1.0}, { 1.0,  0.0,  0.0}, {0.0, 1.0}},

            {{-0.5, -0.5, -0.5, 1.0}, {-1.0,  0.0,  0.0}, {0.0, 0.0}},
            {{-0.5, -0.5,  0.5, 1.0}, {-1.0,  0.0,  0.0}, {1.0, 0.0}},
            {{-0.5,  0.5,  0.5, 1.0}, {-1.0,  0.0,  0.0}, {1.0, 1.0}},
            {{-0.5,  0.5, -0.5, 1.0}, {-1.0,  0.0,  0.0}, {0.0, 1.0}}
        };

        uint32_t cubeIndices[] = {
            0,  2,  1,    0,  3,  2,
            4,  6,  5,    4,  7,  6,
            8, 10,  9,    8, 11, 10,
            12, 14, 13,   12, 15, 14,
            16, 18, 17,   16, 19, 18,
            20, 22, 21,   20, 23, 22
        };



        RHI::BufferDesc v_description{};
        v_description.size = sizeof(cubeVertices);
        v_description.buffer_usage = RHI::BufferUsage::Vertex;
        v_description.memory_usage = RHI::MemoryUsage::CPUToGPU;
        v_description.debug_name = "VertexBuffer";
        v_description.initial_data = cubeVertices;

        metal_backend->vertex_buffer = metal_backend->rhi_device->create_buffer(v_description);

        RHI::BufferDesc i_description{};
        i_description.size = sizeof(cubeIndices);
        i_description.buffer_usage = RHI::BufferUsage::Index;
        i_description.memory_usage = RHI::MemoryUsage::CPUToGPU;
        i_description.debug_name = "IndexBuffer";
        i_description.initial_data = cubeIndices;

        metal_backend->index_buffer = metal_backend->rhi_device->create_buffer(i_description);

    }

    void MetalRenderer::create_buffers() {
        RHI::BufferDesc transform_desc{};
        transform_desc.size = sizeof(TransformationData);
        transform_desc.buffer_usage = RHI::BufferUsage::Uniform;
        transform_desc.memory_usage = RHI::MemoryUsage::CPUToGPU;
        transform_desc.debug_name = "TransformBuffer";

        metal_backend->transform_buffer = metal_backend->rhi_device->create_buffer(transform_desc);
    }


    void MetalRenderer::create_command_queue() {
        metal_backend->metal_command_queue = metal_backend->raw_metal_device->newCommandQueue();
    }

    void MetalRenderer::create_render_pipeline() {
        MTL::Function* vertex_fn = metal_backend->metal_vertex_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
        MTL::Function* fragment_fn = metal_backend->metal_fragment_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));

        RHI::PipelineDesc pso_desc{};
        pso_desc.vertex_shader = vertex_fn;
        pso_desc.fragment_shader = fragment_fn;
        pso_desc.color_format = static_cast<MTL::PixelFormat>(metal_backend->swapchain->get_color_format());
        pso_desc.depth_format = MTL::PixelFormatDepth32Float;
        pso_desc.sample_count = metal_backend->sample_count;
        pso_desc.depth_stencil.depth_write_enable = true;

        metal_backend->pipeline_state = metal_backend->rhi_device->create_pipeline_state(pso_desc);

        vertex_fn->release();
        fragment_fn->release();


    }
}
