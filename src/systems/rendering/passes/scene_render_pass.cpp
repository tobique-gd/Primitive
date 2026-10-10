#include "systems/rendering/passes/scene_render_pass.h"
#include "systems/rendering/rhi/render_context.h"

#include "systems/rendering/rhi/command_list.h"

namespace Engine {

    SceneRenderPass::SceneRenderPass(
        std::shared_ptr<RHI::IBuffer> vertex_buffer,
        std::shared_ptr<RHI::IBuffer> index_buffer,
        std::shared_ptr<RHI::IBuffer> transform_buffer,
        std::shared_ptr<RHI::IPipelineState> pipeline
    )
        : m_vertex_buffer(std::move(vertex_buffer))
        , m_index_buffer(std::move(index_buffer))
        , m_transform_buffer(std::move(transform_buffer))
        , m_pipeline(std::move(pipeline))
    {
    }

    void SceneRenderPass::execute(RHI::RenderContext& context)
    {
        auto& command_list = context.get_command_list();

        command_list.set_pipeline_state(m_pipeline);
        command_list.set_vertex_buffer(m_vertex_buffer, 0);
        command_list.set_vertex_buffer(m_transform_buffer, 1);
        command_list.set_index_buffer(m_index_buffer);
        command_list.draw_indexed(36, 0);
    }

    void SceneRenderPass::resize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }

}
