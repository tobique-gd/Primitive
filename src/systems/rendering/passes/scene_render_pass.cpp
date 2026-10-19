#include "systems/rendering/passes/scene_render_pass.h"
#include "systems/rendering/rhi/render_context.h"

#include "systems/rendering/rhi/command_list.h"
#include <iostream>

namespace Primitive {

    SceneRenderPass::SceneRenderPass(
        std::shared_ptr<IBuffer> mesh_buffer,
        std::shared_ptr<IBuffer> transform_buffer,
        std::shared_ptr<IBuffer> meshlet_buffer,
        std::shared_ptr<IBuffer> meshlet_vertex_buffer,
        std::shared_ptr<IBuffer> meshlet_triangle_buffer,
        std::shared_ptr<IPipelineState> pipeline,
        uint32_t meshlet_count
    )
        : m_mesh_buffer(std::move(mesh_buffer))
        , m_transform_buffer(std::move(transform_buffer))
        , m_meshlet_buffer(std::move(meshlet_buffer))
        , m_meshlet_vertex_buffer(std::move(meshlet_vertex_buffer))
        , m_meshlet_triangle_buffer(std::move(meshlet_triangle_buffer))
        , m_pipeline(std::move(pipeline))
        , m_meshlet_count(meshlet_count)
    {
    }

    void SceneRenderPass::execute(RenderContext& context)
    {
        auto& command_list = context.get_command_list();

        command_list.set_pipeline_state(m_pipeline);

        command_list.set_mesh_buffer(m_meshlet_buffer, 0);
        command_list.set_mesh_buffer(m_transform_buffer, 1);
        command_list.set_mesh_buffer(m_meshlet_vertex_buffer, 2);
        command_list.set_mesh_buffer(m_mesh_buffer, 3);
        command_list.set_mesh_buffer(m_meshlet_triangle_buffer, 4);

        MTL::Size threadgroups_per_grid(m_meshlet_count, 1, 1);
        MTL::Size threads_per_threadgroup(32, 1, 1);

        command_list.draw_mesh_threadgroups(
            threadgroups_per_grid,
            threads_per_threadgroup,
            threads_per_threadgroup
        );
    }

    void SceneRenderPass::resize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }

}
