#include "metal_renderer.h"

#include "Foundation/NSError.hpp"
#include "GLFW/glfw3.h"

#include <CoreGraphics/CoreGraphics.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "Metal/MTLDepthStencil.hpp"
#include "Metal/MTLDevice.hpp"
#include "Metal/MTLHeaderBridge.hpp"
#include "Metal/MTLPixelFormat.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPass.hpp"
#include "Metal/MTLRenderPipeline.hpp"
#include "Metal/MTLResource.hpp"

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/matrix.hpp>

#include "drivers/metal/metal_swapchain.h"
#include "drivers/metal/metal_device.h"
#include "drivers/metal/metal_command_list.h"

#include "editor/import/resource_importer_scene.h"
#include "meshoptimizer.h"
#include "systems/display/iwindow.h"
#include "systems/rendering/frame_graph/render_graph.h"
#include "systems/rendering/passes/scene_render_pass.h"
#include "systems/rendering/passes/sky_pass.h"
#include "systems/rendering/rhi/framebuffer.h"
#include "scene/3d/geometry_instance.h"

/* #include "editor/ui/ui_context.h" */

#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/device.h"
#include "systems/rendering/rhi/pipeline_state.h"
#include "systems/rendering/rhi/render_context.h"
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
#include <vector>

namespace Primitive {
    struct MetalRenderer::Impl {
        MTL::Device* raw_metal_device = nullptr;
        std::unique_ptr<IDevice> rhi_device;

        MTL::CommandQueue* metal_command_queue = nullptr;
        MTL::CommandBuffer* metal_command_buffer = nullptr;

        std::shared_ptr<IBuffer> vertex_buffer;
        std::shared_ptr<IBuffer> index_buffer;
        std::shared_ptr<IBuffer>  meshlet_buffer;
        std::shared_ptr<IBuffer>  meshlet_vertices;
        std::shared_ptr<IBuffer>  meshlet_indices;
        uint32_t meshlet_count;

        std::shared_ptr<IBuffer> cube_vertex_buffer;
        std::shared_ptr<IBuffer> cube_index_buffer;

        std::shared_ptr<IBuffer> transform_buffer;
        std::shared_ptr<IBuffer> sky_transform_buffer;
        std::shared_ptr<IPipelineState> pipeline_state;
        std::shared_ptr<Texture2D> viewport_texture;

        std::shared_ptr<IFramebuffer> framebuffer;

        MTL::Library* metal_vertex_library = nullptr;
        MTL::Library* metal_fragment_library = nullptr;
        MTL::Library* metal_mesh_library = nullptr;

        MTL::Library* sky_vertex_library = nullptr;
        MTL::Library* sky_fragment_library = nullptr;
        std::shared_ptr<IPipelineState> sky_pipeline_state;

        glm::ivec2 viewport_size = {1, 1};
        int sample_count = 4;

        std::unique_ptr<SceneRenderPass> m_scene_render_pass;
        std::unique_ptr<SkyPass> m_sky_pass;
        MTL::RenderPassDescriptor* ui_pass_descriptor;

        std::unique_ptr<ISwapchain> swapchain;
        NS::AutoreleasePool* frame_pool = nullptr;
        bool frame_active = false;

        std::function<void(void*, void*)> overlay_callback;
        std::unique_ptr<RenderGraph> render_graph;

    };

    MetalRenderer::MetalRenderer() : metal_backend( new Impl() ) {}

    MetalRenderer::~MetalRenderer() {
        shutdown();
        delete metal_backend;
    }

    void MetalRenderer::set_overlay_callback(
        std::function<void(void*, void*)> callback
    )
    {
        metal_backend->overlay_callback = std::move(callback);
    }

    void MetalRenderer::render_overlay(const std::function<void(void*, void*)>& callback) {
        auto& m = *metal_backend;

        if (!m.frame_active)
            return;

        auto* command_buffer =
            m.metal_command_queue->commandBuffer();

        auto* encoder = command_buffer->renderCommandEncoder(
            m.ui_pass_descriptor
        );

        callback(command_buffer, encoder);

        encoder->endEncoding();
        command_buffer->commit();
    }


    void* MetalRenderer::get_native_device() const
    {
        return metal_backend->raw_metal_device;
    }

    void* MetalRenderer::get_overlay_render_pass_descriptor() const
    {
        if (!metal_backend->frame_active)
            return nullptr;

        return metal_backend->ui_pass_descriptor;
    }

    void MetalRenderer::set_viewport_size(int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        auto& m = *metal_backend;

        if (m.viewport_size.x == width && m.viewport_size.y == height) return;

        m.viewport_size = {width, height};

        if (m.framebuffer) m.framebuffer->resize(width, height);

        if (m.m_scene_render_pass) m.m_scene_render_pass->resize(width, height);

        if (m.m_sky_pass) m.m_sky_pass->resize(width, height);

    }

    void MetalRenderer::on_window_resize(int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        metal_backend->swapchain->resize(width, height);
    }


    void MetalRenderer::init(IWindow& window, std::shared_ptr<ProjectManager> project_manager)
    {
        init_device();

        ShaderCompiler compiler;

        glm::ivec2 size = window.get_frame_buffer_size();

        SwapchainDesc desc{};
        desc.native_window = window.get_native_window();
        desc.width = size.x;
        desc.height = size.y;
        desc.vsync = window.is_vsync();

        metal_backend->swapchain = std::make_unique<MetalSwapchain>( metal_backend->raw_metal_device, desc);

        create_buffers();

        EditorSceneFormatImporter imp = EditorSceneFormatImporter(project_manager);
        std::filesystem::path p = std::filesystem::path("editor/import/xyzrgb_dragon.obj");
        GUID s = imp.import_scene(p);

        project_manager->get_scene_manager()->set_open_scene_guid(s);

        auto scene = project_manager->get_scene_manager()->get_open_scene();
        if (scene) {
            const auto& nodes = scene->get_root()->get_children();
            for (const auto& child_ptr : nodes) {
                Node* node = child_ptr.get();

                if (auto geo_node = dynamic_cast<GeometryInstance*>(node)) {
                    auto& mesh = geo_node->mesh;

                    BufferDesc vertex_desc{};
                    vertex_desc.size = mesh.vertices.size() * sizeof(VertexData);
                    vertex_desc.buffer_usage = BufferUsage::Storage;
                    vertex_desc.memory_usage = MemoryUsage::CPUToGPU;
                    vertex_desc.debug_name = "MeshVertexBuffer";
                    vertex_desc.initial_data = mesh.vertices.data();

                    metal_backend->vertex_buffer =
                        metal_backend->rhi_device->create_buffer(vertex_desc);

                    BufferDesc index_desc{};
                    index_desc.size = mesh.indices.size() * sizeof(uint32_t);
                    index_desc.buffer_usage = BufferUsage::Index;
                    index_desc.memory_usage = MemoryUsage::CPUToGPU;
                    index_desc.debug_name = "MeshIndexBuffer";
                    index_desc.initial_data = mesh.indices.data();

                    metal_backend->index_buffer =
                        metal_backend->rhi_device->create_buffer(index_desc);

                    std::vector<GPUMeshlet> gpu_meshlets;
                    gpu_meshlets.reserve(mesh.meshlets.size());

                    for (const meshopt_Meshlet& meshlet : mesh.meshlets) {
                        gpu_meshlets.push_back({
                            static_cast<uint32_t>(meshlet.vertex_offset),
                            static_cast<uint32_t>(meshlet.triangle_offset),
                            static_cast<uint32_t>(meshlet.vertex_count),
                            static_cast<uint32_t>(meshlet.triangle_count)
                        });
                    }

                    BufferDesc meshlet_desc{};
                    meshlet_desc.size = gpu_meshlets.size() * sizeof(GPUMeshlet);
                    meshlet_desc.buffer_usage = BufferUsage::Storage;
                    meshlet_desc.memory_usage = MemoryUsage::CPUToGPU;
                    meshlet_desc.debug_name = "MeshletBuffer";
                    meshlet_desc.initial_data = gpu_meshlets.data();

                    metal_backend->meshlet_buffer =
                        metal_backend->rhi_device->create_buffer(meshlet_desc);

                    BufferDesc meshlet_vertices_desc{};
                    meshlet_vertices_desc.size =
                        mesh.meshlet_vertices.size() * sizeof(uint32_t);
                    meshlet_vertices_desc.buffer_usage = BufferUsage::Storage;
                    meshlet_vertices_desc.memory_usage = MemoryUsage::CPUToGPU;
                    meshlet_vertices_desc.debug_name = "MeshletVertexBuffer";
                    meshlet_vertices_desc.initial_data = mesh.meshlet_vertices.data();

                    metal_backend->meshlet_vertices =
                        metal_backend->rhi_device->create_buffer(meshlet_vertices_desc);

                    std::vector<uint32_t> packed_triangles(
                        (mesh.meshlet_triangles.size() + 3) / 4,
                        0
                    );

                    if (!mesh.meshlet_triangles.empty()) {
                        std::memcpy(
                            packed_triangles.data(),
                            mesh.meshlet_triangles.data(),
                            mesh.meshlet_triangles.size()
                        );
                    }

                    BufferDesc meshlet_triangles_desc{};
                    meshlet_triangles_desc.size = packed_triangles.size() * sizeof(uint32_t);
                    meshlet_triangles_desc.buffer_usage = BufferUsage::Storage;
                    meshlet_triangles_desc.memory_usage = MemoryUsage::CPUToGPU;
                    meshlet_triangles_desc.debug_name = "MeshletTriangleBuffer";
                    meshlet_triangles_desc.initial_data = packed_triangles.data();

                    metal_backend->meshlet_indices =
                        metal_backend->rhi_device->create_buffer(meshlet_triangles_desc);

                    metal_backend->meshlet_count =
                        static_cast<uint32_t>(mesh.meshlets.size());

                }
            }
        }

        create_cube();

        create_default_library();
        create_command_queue();

        const auto color_format = static_cast<MTL::PixelFormat>(metal_backend->swapchain->get_color_format());

        FramebufferDesc fb_desc{};
        fb_desc.width = metal_backend->viewport_size.x;
        fb_desc.height = metal_backend->viewport_size.y;
        fb_desc.sample_count = metal_backend->sample_count;
        fb_desc.color_attachment.format = color_format;
        fb_desc.depth_attachment.format = MTL::PixelFormatDepth32Float;

        metal_backend->framebuffer = metal_backend->rhi_device->create_framebuffer(fb_desc);


        MTL::Function* mesh_fn = metal_backend->metal_mesh_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
        MTL::Function* vertex_fn = metal_backend->sky_vertex_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
        MTL::Function* fragment_fn = metal_backend->sky_fragment_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
        MTL::Function* scene_fragment_fn = metal_backend->metal_fragment_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));

        NS::Error* error = nullptr;

        auto& m = *metal_backend;

        m.metal_command_buffer = m.metal_command_queue->commandBuffer();

        auto* pass_desc = static_cast<MTL::RenderPassDescriptor*>(m.framebuffer->get_native_render_pass_descriptor());

        auto* encoder =m.metal_command_buffer->renderCommandEncoder(pass_desc);

        MetalCommandList command_list(encoder);

        RenderContext context(command_list);

        PipelineDesc pso_desc{};
        pso_desc.mesh_shader = mesh_fn;
        pso_desc.vertex_shader = vertex_fn;
        pso_desc.fragment_shader = scene_fragment_fn;
        pso_desc.color_format = static_cast<MTL::PixelFormat>(metal_backend->swapchain->get_color_format());
        pso_desc.depth_format = MTL::PixelFormatDepth32Float;
        pso_desc.sample_count = metal_backend->sample_count;
        pso_desc.depth_stencil.depth_write_enable = true;
        pso_desc.cull_mode = CullMode::Back;
        pso_desc.pipeline_type = PipelineType::Mesh;

        auto pso = metal_backend->rhi_device->create_pipeline_state(pso_desc);

        metal_backend->m_scene_render_pass = std::make_unique<SceneRenderPass>(
            metal_backend->vertex_buffer,
            metal_backend->transform_buffer,
            metal_backend->meshlet_buffer,
            metal_backend->meshlet_vertices,
            metal_backend->meshlet_indices,
            pso,
            metal_backend->meshlet_count
        );

        PipelineDesc pso_desc2{};
        pso_desc2.vertex_shader = vertex_fn;
        pso_desc2.fragment_shader = fragment_fn;
        pso_desc2.color_format = static_cast<MTL::PixelFormat>(metal_backend->swapchain->get_color_format());
        pso_desc2.depth_format = MTL::PixelFormatDepth32Float;
        pso_desc2.sample_count = metal_backend->sample_count;
        pso_desc2.depth_stencil.depth_write_enable = false;
        pso_desc2.cull_mode = CullMode::None;
        pso_desc2.pipeline_type = PipelineType::Standard;

        metal_backend->sky_pipeline_state = metal_backend->rhi_device->create_pipeline_state(pso_desc2);

        vertex_fn->release();
        fragment_fn->release();
        mesh_fn->release();
        scene_fragment_fn->release();

        BufferDesc transform_desc{};
        transform_desc.size = sizeof(TransformationData);
        transform_desc.buffer_usage = BufferUsage::Uniform;
        transform_desc.memory_usage = MemoryUsage::CPUToGPU;
        transform_desc.debug_name = "CubeTransformBuffer";

        metal_backend->sky_transform_buffer = metal_backend->rhi_device->create_buffer(transform_desc);

        metal_backend->m_sky_pass = std::make_unique<SkyPass>(
            metal_backend->cube_vertex_buffer,
            metal_backend->cube_index_buffer,
            metal_backend->sky_transform_buffer,
            metal_backend->sky_pipeline_state
        );
    }

    void MetalRenderer::init_device()
    {
        metal_backend->raw_metal_device = MTL::CreateSystemDefaultDevice();
        metal_backend->rhi_device = std::make_unique<MetalDevice>(metal_backend->raw_metal_device);
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

        BufferDesc v_description{};
        v_description.size = sizeof(cubeVertices);
        v_description.buffer_usage = BufferUsage::Vertex;
        v_description.memory_usage = MemoryUsage::CPUToGPU;
        v_description.debug_name = "VertexBuffer";
        v_description.initial_data = cubeVertices;

        metal_backend->cube_vertex_buffer = metal_backend->rhi_device->create_buffer(v_description);

        BufferDesc i_description{};
        i_description.size = sizeof(cubeIndices);
        i_description.buffer_usage = BufferUsage::Index;
        i_description.memory_usage = MemoryUsage::CPUToGPU;
        i_description.debug_name = "IndexBuffer";
        i_description.initial_data = cubeIndices;

        metal_backend->cube_index_buffer = metal_backend->rhi_device->create_buffer(i_description);

    }

    void MetalRenderer::create_buffers() {
        BufferDesc transform_desc{};
        transform_desc.size = sizeof(TransformationData);
        transform_desc.buffer_usage = BufferUsage::Uniform;
        transform_desc.memory_usage = MemoryUsage::CPUToGPU;
        transform_desc.debug_name = "TransformBuffer";

        metal_backend->transform_buffer = metal_backend->rhi_device->create_buffer(transform_desc);
    }


    void MetalRenderer::create_command_queue() {
        metal_backend->metal_command_queue = metal_backend->raw_metal_device->newCommandQueue();
    }

    void MetalRenderer::create_render_pipeline() {
        MTL::Function* vertex_fn = metal_backend->metal_vertex_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
        MTL::Function* fragment_fn = metal_backend->metal_fragment_library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));

        PipelineDesc pso_desc{};
        pso_desc.vertex_shader = vertex_fn;
        pso_desc.fragment_shader = fragment_fn;
        pso_desc.color_format = static_cast<MTL::PixelFormat>(metal_backend->swapchain->get_color_format());
        pso_desc.depth_format = MTL::PixelFormatDepth32Float;
        pso_desc.sample_count = metal_backend->sample_count;
        pso_desc.depth_stencil.depth_write_enable = true;
        pso_desc.pipeline_type = PipelineType::Standard;

        metal_backend->pipeline_state = metal_backend->rhi_device->create_pipeline_state(pso_desc);

        vertex_fn->release();
        fragment_fn->release();
    }

    void MetalRenderer::create_default_library() {
      NS::Error* error = nullptr;

      try {
          ShaderCompiler compiler;

          std::string vert_msl = compiler.compile_glsl_file_to_msl("src/systems/rendering/shaders/forward_3d.vert.glsl");
          std::string frag_msl = compiler.compile_glsl_file_to_msl("src/systems/rendering/shaders/forward_3d.frag.glsl");
          std::string mesh_msl = compiler.compile_glsl_file_to_msl("src/systems/rendering/shaders/forward_3d.mesh.glsl");

          std::string vert_sky_msl = compiler.compile_glsl_file_to_msl("src/systems/rendering/shaders/sky_3d.vert.glsl");
          std::string frag_sky_msl = compiler.compile_glsl_file_to_msl("src/systems/rendering/shaders/sky_3d.frag.glsl");

          //std::cout << mesh_msl << "\n";

          metal_backend->metal_vertex_library = metal_backend->raw_metal_device->newLibrary(
            NS::String::string(vert_msl.c_str(), NS::UTF8StringEncoding),
            nullptr,
            &error
          );

          metal_backend->metal_mesh_library = metal_backend->raw_metal_device->newLibrary(
            NS::String::string(mesh_msl.c_str(), NS::UTF8StringEncoding),
            nullptr,
            &error
          );

          metal_backend->sky_vertex_library = metal_backend->raw_metal_device->newLibrary(
            NS::String::string(vert_sky_msl.c_str(), NS::UTF8StringEncoding),
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

          metal_backend->sky_fragment_library = metal_backend->raw_metal_device->newLibrary(
            NS::String::string(frag_sky_msl.c_str(), NS::UTF8StringEncoding),
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

    void MetalRenderer::begin_frame()
    {
        auto& m = *metal_backend;

        m.frame_pool = NS::AutoreleasePool::alloc()->init();
        m.frame_active = m.swapchain->acquire_next_frame();

        if (!m.frame_active) {
            m.frame_pool->release();
            m.frame_pool = nullptr;
            return;
        }

        m.ui_pass_descriptor = static_cast<MTL::RenderPassDescriptor*>(m.swapchain->get_native_render_pass_descriptor(LoadOp::Clear));
    }

    void MetalRenderer::draw() {
        if (!metal_backend || !metal_backend->framebuffer) return;

        NS::AutoreleasePool* frame_pool = NS::AutoreleasePool::alloc()->init();

        send_render_command();

        frame_pool->release();

    }

    void MetalRenderer::send_render_command()
    {
        auto& m = *metal_backend;

        m.metal_command_buffer = m.metal_command_queue->commandBuffer();

        auto* pass_desc = static_cast<MTL::RenderPassDescriptor*>(m.framebuffer->get_native_render_pass_descriptor());

        auto* encoder =m.metal_command_buffer->renderCommandEncoder(pass_desc);

        MetalCommandList command_list(encoder);

        RenderContext context(command_list);

        update_sky_transforms();
        update_scene_transforms();

        RenderGraph graph;

        auto scene_color = graph.create_texture({
            .name = "SceneColor",
            .texture = {
                .width = static_cast<uint32_t>(m.viewport_size.x),
                .height = static_cast<uint32_t>(m.viewport_size.y),
                .sample_count = static_cast<uint32_t>(m.sample_count),
                .format = TextureFormat::RGBA8
            }
        });

        graph.add_pass({
            .name = "Sky",
            .setup = [scene_color](RenderGraphBuilder& builder) {
                builder.write(scene_color);
            },
            .execute = [&m](RenderContext& context) {m.m_sky_pass->execute(context); }
        });


        graph.add_pass({
            .name = "Scene",
            .setup = [scene_color](RenderGraphBuilder& builder) {
                builder.read_write(scene_color);
            },
            .execute = [&m](RenderContext& context) { m.m_scene_render_pass->execute(context);}
        });

        graph.compile();

        graph.execute(context);

        encoder->endEncoding();
        m.metal_command_buffer->commit();
    }

    void MetalRenderer::end_frame()
    {
        auto& m = *metal_backend;

        if (m.frame_active) {
            auto* command_buffer = m.metal_command_queue->commandBuffer();
            auto* encoder = command_buffer->renderCommandEncoder(
                m.ui_pass_descriptor
            );

            if (m.overlay_callback) {
                m.overlay_callback(command_buffer, encoder);
            }

            encoder->endEncoding();

            m.swapchain->present(command_buffer);
            command_buffer->commit();
        }

        m.overlay_callback = {};
        m.frame_active = false;
        m.ui_pass_descriptor = nullptr;

        if (m.frame_pool) {
            m.frame_pool->release();
            m.frame_pool = nullptr;
        }
    }

    void MetalRenderer::update_sky_transforms()
    {
        glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f));
        glm::mat4 view_matrix(1.0f);


        float width = static_cast<float>(metal_backend->viewport_size.x);
        float height = static_cast<float>(metal_backend->viewport_size.y);
        float aspect_ratio = width / height;

        float fov = glm::radians(90.0f);
        float near_z = 0.1f;
        float far_z = 100.0f;

        glm::mat4 projection_matrix = glm::perspectiveRH_ZO(
            fov,
            aspect_ratio,
            near_z,
            far_z
        );

        TransformationData data{
            model_matrix,
            view_matrix,
            projection_matrix
        };

        metal_backend->sky_transform_buffer->update_data(
            &data,
            sizeof(data),
            0
        );
    }

    void MetalRenderer::update_scene_transforms() {
        glm::mat4x4 transform = glm::mat4x4(1.0f);
        transform = glm::translate(transform, glm::vec3(0, 0, 0.0f));
        transform = glm::scale(transform, glm::vec3(0.01, 0.01, 0.01));

        float angle_in_degrees = 45.0f * glfwGetTime() / 2.0;
        float angle_in_radians = angle_in_degrees * M_PI / 180.0f;
        transform = glm::rotate(transform, angle_in_radians, glm::vec3(0.0f, 1.0f, 0.0f));
        //transform = glm::translate(transform, glm::vec3(0.0f, -0.5f, 0.0f));
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

        float fov = 80.0f * (M_PI / 180.0f);
        float near_z = 0.1f;
        float far_z = 100.0f;

        glm::mat4x4 perspective_matrix = glm::perspectiveRH_ZO(fov, aspect_ratio, near_z, far_z);

        TransformationData transformation_data = {
            model_matrix,
            view_matrix,
            perspective_matrix
        };

        metal_backend->transform_buffer->update_data(&transformation_data, sizeof(transformation_data), 0);
    }


    std::shared_ptr<Texture2D> MetalRenderer::get_viewport_texture()
    {
        return std::static_pointer_cast<Texture2D>(metal_backend->framebuffer->get_color_attachment());
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
        m.sky_transform_buffer.reset();
        m.m_sky_pass.reset();

        /* m.ui.reset(); */
        m.swapchain.reset();

        if (m.metal_vertex_library)   { m.metal_vertex_library->release();   m.metal_vertex_library = nullptr; }
        if (m.metal_mesh_library)   { m.metal_mesh_library->release();   m.metal_mesh_library = nullptr; }
        if (m.metal_fragment_library) { m.metal_fragment_library->release(); m.metal_fragment_library = nullptr; }
        if (m.metal_command_queue)    { m.metal_command_queue->release();    m.metal_command_queue = nullptr; }

        m.rhi_device.reset();

        m.raw_metal_device->release();
        m.raw_metal_device = nullptr;
    }
}
