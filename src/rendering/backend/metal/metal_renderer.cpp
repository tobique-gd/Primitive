#include "metal_renderer.h"
#include "Metal/MTLArgument.hpp"
#include "Metal/MTLDepthStencil.hpp"
#include "Metal/MTLPixelFormat.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPass.hpp"
#include "Metal/MTLResource.hpp"
#include "cocoa_bridge.h"

#include "core/iwindow.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"
#include "platform/multi/GLFWWindow.h"

#include "editor/ui_context.h"

#include <CoreGraphics/CoreGraphics.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "rendering/itexture_2d.h"
#include "rendering/backend/metal/metal_texture_2d.h"
#include "rendering/shaders/shader_compiler.h"
#include "vertex_data.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <simd/simd.h>
#include <cmath>

namespace Engine
{
    struct MetalRenderer::Impl
    {
        MTL::Device* metal_device = nullptr;

        CA::MetalLayer* metal_layer = nullptr;
        CA::MetalDrawable* metal_drawable = nullptr;
        NS::SharedPtr<MTL::Texture> internal_render_target;
        std::shared_ptr<MetalTexture2D> viewport_texture;

        MTL::Library* metal_vertex_library = nullptr;
        MTL::Library* metal_fragment_library = nullptr;
        MTL::CommandQueue* metal_command_queue = nullptr;
        MTL::CommandBuffer* metal_command_buffer = nullptr;
        MTL::RenderPipelineState* metal_render_pso = nullptr;
        MTL::Buffer* vertex_buffer = nullptr;
        MTL::Buffer* index_buffer = nullptr;
        MTL::Buffer* transform_buffer = nullptr;
        MTL::SamplerState* sampler_state = nullptr;

        void* cocoa_window = nullptr;
        glm::ivec2 frame_buffer_size;
        glm::ivec2 viewport_size = {1, 1};

        MTL::DepthStencilState* depth_stencil_state;
        MTL::RenderPassDescriptor* render_pass_descriptor = nullptr;
        MTL::RenderPassDescriptor* ui_render_pass_descriptor = nullptr;

        MTL::Texture* msaa_render_target_texture = nullptr;
        MTL::Texture* depth_texture;
        int sample_count = 4;

    };

    MetalRenderer::MetalRenderer()
        : metal_backend(new Impl())
    {
    }

    MetalRenderer::~MetalRenderer()
    {
        shutdown();
        delete metal_backend;
    }

    void MetalRenderer::set_viewport_size(int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        if (metal_backend->viewport_size.x == width &&
            metal_backend->viewport_size.y == height)
            return;

        metal_backend->viewport_size = { width, height };

        create_depth_and_MSAA_textures();
        create_internal_render_target();
        create_render_pass_descriptor();
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


        create_depth_and_MSAA_textures();
        create_internal_render_target();

        create_render_pipeline();
        create_render_pass_descriptor();

    }

    void MetalRenderer::init_ui(IWindow& window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.Fonts->AddFontFromFileTTF("assets/fonts/IBMPlexSans-Regular.ttf");

        UIStyle::setup_ui_style();

        auto* glfw_window = dynamic_cast<GLFWWindow*>(&window);

        if (!glfw_window)
            throw std::runtime_error("MetalRenderer requires GLFWwindow");

        ImGui_ImplGlfw_InitForOther(
            glfw_window->get_glfw_window(),
            true
        );

        ImGui_ImplMetal_Init(metal_backend->metal_device);
    }


    void MetalRenderer::init_device()
    {
        metal_backend->metal_device = MTL::CreateSystemDefaultDevice();
    }

    void MetalRenderer::init_window(IWindow& window)
    {
        void* native_window_handle = window.get_native_window();
        glm::ivec2 size = window.get_frame_buffer_size();
        metal_backend->frame_buffer_size = size;

        if (!native_window_handle)
            throw std::runtime_error("Invalid native window handle");

        if (!metal_backend->metal_device)
            throw std::runtime_error("Metal device is not initialized");

        metal_backend->metal_layer = CA::MetalLayer::layer();

        if (!metal_backend->metal_layer)
            throw std::runtime_error("Failed to create Metal layer");

        metal_backend->metal_layer->setDevice(metal_backend->metal_device);
        metal_backend->metal_layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        metal_backend->metal_layer->setDrawableSize(
            CGSizeMake(size.x, size.y)
        );

        metal_backend->cocoa_window = native_window_handle;

        CocoaBridge::AddLayerToWindow(
            metal_backend->cocoa_window,
            metal_backend->metal_layer
        );
    }

    void MetalRenderer::create_render_pass_descriptor() {
        metal_backend->render_pass_descriptor = MTL::RenderPassDescriptor::alloc()->init();

        MTL::RenderPassColorAttachmentDescriptor* colorAttachment = metal_backend->render_pass_descriptor->colorAttachments()->object(0);
        MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = metal_backend->render_pass_descriptor->depthAttachment();

        colorAttachment->setTexture(metal_backend->msaa_render_target_texture);
        colorAttachment->setResolveTexture(metal_backend->internal_render_target.get());
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setClearColor(MTL::ClearColor(41.0f/255.0f, 42.0f/255.0f, 48.0f/255.0f, 1.0));
        colorAttachment->setStoreAction(MTL::StoreActionMultisampleResolve);

        depthAttachment->setTexture(metal_backend->depth_texture);
        depthAttachment->setLoadAction(MTL::LoadActionClear);
        depthAttachment->setStoreAction(MTL::StoreActionDontCare);
        depthAttachment->setClearDepth(1.0);
    }

    void MetalRenderer::update_render_pass_descriptor() {
        metal_backend->render_pass_descriptor->colorAttachments()->object(0)->setTexture(metal_backend->msaa_render_target_texture);
        metal_backend->render_pass_descriptor->colorAttachments()->object(0)->setResolveTexture(metal_backend->internal_render_target.get());
        metal_backend->render_pass_descriptor->depthAttachment()->setTexture(metal_backend->depth_texture);
    }


    void MetalRenderer::create_default_library() {
      NS::Error* error = nullptr;

      try {
      ShaderCompiler compiler;

      std::string vert_msl = compiler.compile_glsl_file_to_msl("src/rendering/shaders/forward_3d.vert.glsl");
      std::string frag_msl = compiler.compile_glsl_file_to_msl("src/rendering/shaders/forward_3d.frag.glsl");


      std::cout << "trasnlsateed vertex msl:\n" << vert_msl;
      std::cout << "translated fragment msl:\n" << frag_msl;


      metal_backend->metal_vertex_library = metal_backend->metal_device->newLibrary(
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
      metal_backend->metal_fragment_library = metal_backend->metal_device->newLibrary(
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

      MTL::SamplerDescriptor* sampler_descriptor = MTL::SamplerDescriptor::alloc()->init();
      sampler_descriptor->setMinFilter(MTL::SamplerMinMagFilterNearest);
      sampler_descriptor->setMagFilter(MTL::SamplerMinMagFilterNearest);
      sampler_descriptor->setSAddressMode(MTL::SamplerAddressModeClampToEdge);
      sampler_descriptor->setTAddressMode(MTL::SamplerAddressModeClampToEdge);
      metal_backend->sampler_state = metal_backend->metal_device->newSamplerState(sampler_descriptor);
      sampler_descriptor->release();

      if (!metal_backend->sampler_state) {
        std::cerr << "Failed to create Metal sampler state" << std::endl;
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

        if (metal_backend->metal_device)
        {
            metal_backend->metal_device->release();
            metal_backend->metal_device = nullptr;
        }

        metal_backend->transform_buffer->release();
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

    void MetalRenderer::create_internal_render_target()
    {
        glm::ivec2 size = metal_backend->viewport_size;

        auto* descriptor = MTL::TextureDescriptor::alloc()->init();

        descriptor->setTextureType(MTL::TextureType2D);
        descriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        descriptor->setWidth(size.x);
        descriptor->setHeight(size.y);
        descriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        descriptor->setStorageMode(MTL::StorageModePrivate);

        metal_backend->internal_render_target = NS::TransferPtr(metal_backend->metal_device->newTexture(descriptor));

        descriptor->release();


        metal_backend->viewport_texture = std::make_shared<MetalTexture2D>(metal_backend->internal_render_target);


    }

    std::shared_ptr<ITexture2D> MetalRenderer::render_viewport()
    {
        if (!metal_backend || !metal_backend->internal_render_target)
            return nullptr;

        NS::AutoreleasePool* frame_pool =
            NS::AutoreleasePool::alloc()->init();

        send_render_command();

        frame_pool->release();

        //present_viewport_debug();

        return metal_backend->viewport_texture;


    }

    void MetalRenderer::create_depth_and_MSAA_textures() {
        MTL::TextureDescriptor* msaa_texture_descriptor = MTL::TextureDescriptor::alloc()->init();
        msaa_texture_descriptor->setTextureType(MTL::TextureType2DMultisample);
        msaa_texture_descriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        msaa_texture_descriptor->setWidth(metal_backend->viewport_size.x);
        msaa_texture_descriptor->setHeight(metal_backend->viewport_size.y);
        msaa_texture_descriptor->setSampleCount(metal_backend->sample_count);
        msaa_texture_descriptor->setUsage(MTL::TextureUsageRenderTarget);

        metal_backend->msaa_render_target_texture = metal_backend->metal_device->newTexture(msaa_texture_descriptor);

        MTL::TextureDescriptor* depthTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
        depthTextureDescriptor->setTextureType(MTL::TextureType2DMultisample);
        depthTextureDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
        depthTextureDescriptor->setWidth(metal_backend->viewport_size.x);
        depthTextureDescriptor->setHeight(metal_backend->viewport_size.y);
        depthTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
        depthTextureDescriptor->setSampleCount(metal_backend->sample_count);

        metal_backend->depth_texture = metal_backend->metal_device->newTexture(depthTextureDescriptor);

        msaa_texture_descriptor->release();
        depthTextureDescriptor->release();
    }


    void MetalRenderer::send_render_command() {
        metal_backend->metal_command_buffer = metal_backend->metal_command_queue->commandBuffer();

        update_render_pass_descriptor();

        MTL::RenderCommandEncoder* render_command_encoder = metal_backend->metal_command_buffer->renderCommandEncoder(metal_backend->render_pass_descriptor);

        encode_render_command(render_command_encoder);
        render_command_encoder->endEncoding();


        metal_backend->metal_command_buffer->commit();

    }

    matrix_float4x4 matrix4x4_translation(float x, float y, float z)
    {
        matrix_float4x4 matrix = {};

        matrix.columns[0] = {1, 0, 0, 0};
        matrix.columns[1] = {0, 1, 0, 0};
        matrix.columns[2] = {0, 0, 1, 0};
        matrix.columns[3] = {x, y, z, 1};

        return matrix;
    }

    matrix_float4x4 matrix4x4_rotation(float radians, float x, float y, float z)
    {
        simd::float3 axis = simd::normalize(simd::float3{x, y, z});

        float c = std::cos(radians);
        float s = std::sin(radians);
        float t = 1.0f - c;

        matrix_float4x4 matrix = {};

        matrix.columns[0] = {
            t * axis.x * axis.x + c,
            t * axis.x * axis.y + s * axis.z,
            t * axis.x * axis.z - s * axis.y,
            0
        };

        matrix.columns[1] = {
            t * axis.x * axis.y - s * axis.z,
            t * axis.y * axis.y + c,
            t * axis.y * axis.z + s * axis.x,
            0
        };

        matrix.columns[2] = {
            t * axis.x * axis.z + s * axis.y,
            t * axis.y * axis.z - s * axis.x,
            t * axis.z * axis.z + c,
            0
        };

        matrix.columns[3] = {0, 0, 0, 1};

        return matrix;
    }

    matrix_float4x4 matrix_make_rows(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    )
    {
        matrix_float4x4 matrix = {};

        matrix.columns[0] = {m00, m10, m20, m30};
        matrix.columns[1] = {m01, m11, m21, m31};
        matrix.columns[2] = {m02, m12, m22, m32};
        matrix.columns[3] = {m03, m13, m23, m33};

        return matrix;
    }

    matrix_float4x4 matrix_perspective_right_hand(
        float fovY,
        float aspect,
        float nearZ,
        float farZ
    )
    {
        float yScale = 1.0f / std::tan(fovY * 0.5f);
        float xScale = yScale / aspect;

        matrix_float4x4 matrix = {};

        matrix.columns[0] = {xScale, 0, 0, 0};
        matrix.columns[1] = {0, yScale, 0, 0};
        matrix.columns[2] = {
            0,
            0,
            (farZ + nearZ) / (nearZ - farZ),
            -1
        };
        matrix.columns[3] = {
            0,
            0,
            (2.0f * farZ * nearZ) / (nearZ - farZ),
            0
        };

        return matrix;
    }



    void MetalRenderer::encode_render_command(MTL::RenderCommandEncoder* render_command_encoder)
    {
        matrix_float4x4 translation_matrix = matrix4x4_translation(0, 0, -1.0f);

        float angle_in_degrees = 90.0f / 2.0f * 45.0f;
        float angle_in_radians = angle_in_degrees * M_PI / 180.0f;

        matrix_float4x4 rotation_matrix = matrix4x4_rotation(angle_in_radians, 0.0f, 1.0f, 0.0f);

        matrix_float4x4 model_matrix = simd_mul(translation_matrix, rotation_matrix);

        simd::float3 R = {1, 0, 0};
        simd::float3 U = {0, 1, 0};
        simd::float3 F = {0, 0, -1};
        simd::float3 P = {0, 0, 1};

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

        memcpy(metal_backend->transform_buffer->contents(), &transformation_data, sizeof(transformation_data));

        render_command_encoder->setFrontFacingWinding(MTL::WindingClockwise);
        render_command_encoder->setCullMode(MTL::CullModeBack);

        //render_command_encoder->setTriangleFillMode(MTL::TriangleFillModeLines);
        render_command_encoder->setRenderPipelineState(metal_backend->metal_render_pso);
        render_command_encoder->setDepthStencilState(metal_backend->depth_stencil_state);
        render_command_encoder->setVertexBuffer(metal_backend->vertex_buffer, 0, 0);

        render_command_encoder->setVertexBuffer(metal_backend->transform_buffer, 0, 1);

        MTL::PrimitiveType type_triangle = MTL::PrimitiveTypeTriangle;
        MTL::IndexType index_type_uint32 = MTL::IndexTypeUInt32;
        render_command_encoder->drawIndexedPrimitives(type_triangle, 36, index_type_uint32, metal_backend->index_buffer, 0);
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


        metal_backend->vertex_buffer = metal_backend->metal_device->newBuffer(cubeVertices, sizeof(cubeVertices), MTL::ResourceStorageModeShared);
        metal_backend->index_buffer = metal_backend->metal_device->newBuffer(cubeIndices, sizeof(cubeIndices), MTL::ResourceStorageModeShared);
    }

    void MetalRenderer::create_buffers() {
        metal_backend->transform_buffer = metal_backend->metal_device->newBuffer(sizeof(TransformationData), MTL::ResourceStorageModeShared);
    }


    void MetalRenderer::create_command_queue() {
        metal_backend->metal_command_queue = metal_backend->metal_device->newCommandQueue();
    }

    void MetalRenderer::create_render_pipeline() {
        MTL::Function* vertex_shader = metal_backend->metal_vertex_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
        assert(vertex_shader);
        MTL::Function* fragment_shader = metal_backend->metal_fragment_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
        assert(fragment_shader);

        MTL::RenderPipelineDescriptor* render_pipeline_descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        render_pipeline_descriptor->setLabel(NS::String::string("Triangle Rendering Pipeline", NS::ASCIIStringEncoding));
        render_pipeline_descriptor->setVertexFunction(vertex_shader);
        render_pipeline_descriptor->setFragmentFunction(fragment_shader);
        assert(render_pipeline_descriptor);

        MTL::PixelFormat pixel_format = (MTL::PixelFormat) metal_backend->metal_layer->pixelFormat();
        render_pipeline_descriptor->colorAttachments()->object(0)->setPixelFormat(pixel_format);
        render_pipeline_descriptor->setSampleCount(metal_backend->sample_count);
        render_pipeline_descriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);

        NS::Error* error;
        metal_backend->metal_render_pso = metal_backend->metal_device->newRenderPipelineState(render_pipeline_descriptor, &error);

        if (metal_backend->metal_render_pso == nil) {
            std::cout << "Error creating render pipeline state: " << error << std::endl;
            std::exit(0);
        }

        MTL::DepthStencilDescriptor* depth_stencil_descriptor = MTL::DepthStencilDescriptor::alloc()->init();
        depth_stencil_descriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
        depth_stencil_descriptor->setDepthWriteEnabled(true);
        metal_backend->depth_stencil_state = metal_backend->metal_device->newDepthStencilState(depth_stencil_descriptor);

        render_pipeline_descriptor->release();
        vertex_shader->release();
        fragment_shader->release();

    }
}
