#include "metal_renderer.h"
#include "GLFW/glfw3.h"
#include "Metal/MTLArgument.hpp"
#include "Metal/MTLDepthStencil.hpp"
#include "Metal/MTLPixelFormat.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPass.hpp"
#include "Metal/MTLResource.hpp"
#include "platform/macos/cocoa_bridge.h"

#include "editor/IconsFontAwesome6.h"

#include "systems/display/iwindow.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#include "platform/macos/GLFWWindow.h"
#include "systems/rendering/backends/metal/math_util.h"
#include "systems/rendering/passes/render_pass.h"
#include "systems/rendering/passes/scene_render_pass.h"
#include "systems/rendering/rhi/framebuffer.h"
#include "drivers/metal/metal_command_list.h"

#include "editor/ui_context.h"
#include "drivers/metal/metal_device.h"

#include <CoreGraphics/CoreGraphics.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/device.h"
#include "systems/rendering/rhi/pipeline_state.h"
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

namespace Engine
{
    struct MetalRenderer::Impl {
        MTL::Device* raw_metal_device = nullptr;
        std::unique_ptr<RHI::IDevice> rhi_device;

        CA::MetalLayer* metal_layer = nullptr;
        CA::MetalDrawable* metal_drawable = nullptr;
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

        MTL::RenderPassDescriptor* render_pass_descriptor = nullptr;
        MTL::RenderPassDescriptor* ui_render_pass_descriptor = nullptr;

        MTL::Texture* msaa_render_target_texture = nullptr;
        MTL::Texture* depth_texture = nullptr;

        void* cocoa_window = nullptr;
        glm::ivec2 frame_buffer_size;
        glm::ivec2 viewport_size = {1, 1};
        int sample_count = 2;

        std::unique_ptr<SceneRenderPass> m_scene_render_pass;
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
        init_ui(window);

        create_cube();
        create_default_library();
        create_command_queue();

        RHI::FramebufferDesc fb_desc{};
        fb_desc.width = metal_backend->viewport_size.x;
        fb_desc.height = metal_backend->viewport_size.y;
        fb_desc.sample_count = metal_backend->sample_count;
        fb_desc.color_attachment.format = metal_backend->metal_layer->pixelFormat();
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

    void MetalRenderer::init_ui(IWindow& window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImFontConfig font_config;
        font_config.FontDataOwnedByAtlas = false;

        io.Fonts->AddFontFromFileTTF(
            "assets/fonts/IBMPlexSans-Regular.ttf",
            16.0f
        );

        static const ImWchar icons_ranges[] = {
            ICON_MIN_FA,
            ICON_MAX_16_FA,
            0
        };

        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;

        io.Fonts->AddFontFromFileTTF(
            FONT_ICON_FILE_NAME_FAS,
            16.0f,
            &icons_config,
            icons_ranges
        );

        UIStyle::setup_ui_style();

        auto* glfw_window = dynamic_cast<GLFWWindow*>(&window);

        if (!glfw_window)
            throw std::runtime_error("MetalRenderer requires GLFWwindow");

        ImGui_ImplGlfw_InitForOther(
            glfw_window->get_glfw_window(),
            true
        );

        ImGui_ImplMetal_Init(metal_backend->raw_metal_device);
    }


    void MetalRenderer::init_device()
    {
        metal_backend->raw_metal_device = MTL::CreateSystemDefaultDevice();
        metal_backend->rhi_device = std::make_unique<RHI::Metal::MetalDevice>(metal_backend->raw_metal_device);
    }

    void MetalRenderer::init_window(IWindow& window)
    {
        void* native_window_handle = window.get_native_window();
        glm::ivec2 size = window.get_frame_buffer_size();
        metal_backend->frame_buffer_size = size;

        if (!native_window_handle)
            throw std::runtime_error("Invalid native window handle");

        if (!metal_backend->raw_metal_device)
            throw std::runtime_error("Metal device is not initialized");

        metal_backend->metal_layer = CA::MetalLayer::layer();

        if (!metal_backend->metal_layer)
            throw std::runtime_error("Failed to create Metal layer");

        metal_backend->metal_layer->setDevice(metal_backend->raw_metal_device);
        metal_backend->metal_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        metal_backend->metal_layer->setDrawableSize(CGSizeMake(size.x, size.y));

        metal_backend->cocoa_window = native_window_handle;

        CocoaBridge::AddLayerToWindow(metal_backend->cocoa_window, metal_backend->metal_layer);
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
        if (!metal_backend)
            return;

        if (metal_backend->raw_metal_device)
        {
            metal_backend->raw_metal_device->release();
            metal_backend->raw_metal_device = nullptr;
        }

        metal_backend->msaa_render_target_texture->release();
        metal_backend->depth_texture->release();
        metal_backend->render_pass_descriptor->release();
    }

    void MetalRenderer::begin_imgui()
    {
        metal_backend->metal_drawable = metal_backend->metal_layer->nextDrawable();


        if (!metal_backend->metal_drawable) return;

        metal_backend->ui_render_pass_descriptor = MTL::RenderPassDescriptor::alloc()->init();

        auto* color = metal_backend->ui_render_pass_descriptor->colorAttachments()->object(0);

        color->setTexture(metal_backend->metal_drawable->texture());
        color->setLoadAction(MTL::LoadActionClear);
        color->setStoreAction(MTL::StoreActionStore);
        color->setClearColor(MTL::ClearColor(0,0,0,1));

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplMetal_NewFrame(metal_backend->ui_render_pass_descriptor);
        ImGui::NewFrame();

    }

    void MetalRenderer::end_imgui()
    {
        ImGui::Render();

        auto* command_buffer = metal_backend->metal_command_queue->commandBuffer();

        auto* encoder = command_buffer->renderCommandEncoder(metal_backend->ui_render_pass_descriptor);
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), command_buffer, encoder);

        encoder->endEncoding();

        command_buffer->presentDrawable(metal_backend->metal_drawable);
        command_buffer->commit();

        metal_backend->ui_render_pass_descriptor->release();
        metal_backend->ui_render_pass_descriptor = nullptr;

    }

    void MetalRenderer::render_frame()
    {
        metal_backend->metal_drawable =
            metal_backend->metal_layer->nextDrawable();

        if (!metal_backend->metal_drawable)
            return;

        MTL::RenderPassDescriptor* descriptor = MTL::RenderPassDescriptor::alloc()->init();

        auto* color = descriptor->colorAttachments()->object(0);

        color->setTexture(metal_backend->metal_drawable->texture());
        color->setLoadAction(MTL::LoadActionClear);
        color->setStoreAction(MTL::StoreActionStore);
        color->setClearColor(MTL::ClearColor(0, 0, 0, 1));

        auto* command_buffer =
            metal_backend->metal_command_queue->commandBuffer();

        auto* encoder =
            command_buffer->renderCommandEncoder(descriptor);

        ImGui_ImplMetal_RenderDrawData(
            ImGui::GetDrawData(),
            command_buffer,
            encoder
        );

        encoder->endEncoding();

        command_buffer->presentDrawable(metal_backend->metal_drawable);
        command_buffer->commit();

        descriptor->release();
    }

    void MetalRenderer::on_window_resize(int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        metal_backend->frame_buffer_size = { width, height };

        if (metal_backend->metal_layer)
        {
            metal_backend->metal_layer->setDrawableSize(CGSizeMake(width, height));
        }
    }

    std::shared_ptr<RHI::ITexture2D> MetalRenderer::render_viewport()
    {
        if (!metal_backend || !metal_backend->framebuffer) return nullptr;

        NS::AutoreleasePool* frame_pool = NS::AutoreleasePool::alloc()->init();

        send_render_command();

        frame_pool->release();

        return std::static_pointer_cast<RHI::ITexture2D>(metal_backend->framebuffer->get_color_attachment());


    }

    void MetalRenderer::update_scene_transforms() {
        matrix_float4x4 translation_matrix = matrix4x4_translation(0, 0, -1.0f);
        float angle_in_degrees = 45.0f * glfwGetTime() / 2.0;
        float angle_in_radians = angle_in_degrees * M_PI / 180.0f;
        matrix_float4x4 rotation_matrix = matrix4x4_rotation(angle_in_radians, 1.0f, 1.0f, 1.0f);
        matrix_float4x4 model_matrix = simd_mul(translation_matrix, rotation_matrix);

        simd::float3 R = {1, 0, 0}, U = {0, 1, 0}, F = {0, 0, -1}, P = {0, 0, 1};

        matrix_float4x4 view_matrix =
            matrix_make_rows(
                R.x, R.y, R.z, dot(-R, P),
                U.x, U.y, U.z, dot(-U, P),
                -F.x, -F.y, -F.z, dot(F, P),
                0, 0, 0, 1
            );

        float width = metal_backend->viewport_size.x;
        float height = metal_backend->viewport_size.y;

        float aspect_ratio = width / height;

        float fov = 90.0f * (M_PI / 180.0f);
        float near_z = 0.1f;
        float far_z = 100.0f;

        matrix_float4x4 perspective_matrix = matrix_perspective_right_hand( fov, aspect_ratio, near_z,far_z);

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



    void MetalRenderer::encode_render_command(MTL::RenderCommandEncoder* raw_encoder)
    {
        RHI::Metal::MetalCommandList cmd(raw_encoder);

        update_scene_transforms();

        cmd.set_pipeline_state(metal_backend->pipeline_state);
        cmd.set_vertex_buffer(metal_backend->vertex_buffer, 0);
        cmd.set_vertex_buffer(metal_backend->transform_buffer, 1);
        cmd.set_index_buffer(metal_backend->index_buffer);

        cmd.draw_indexed(36, 0);



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
        pso_desc.color_format = metal_backend->metal_layer->pixelFormat();
        pso_desc.depth_format = MTL::PixelFormatDepth32Float;
        pso_desc.sample_count = metal_backend->sample_count;
        pso_desc.depth_stencil.depth_write_enable = true;

        metal_backend->pipeline_state = metal_backend->rhi_device->create_pipeline_state(pso_desc);

        vertex_fn->release();
        fragment_fn->release();


    }
}
