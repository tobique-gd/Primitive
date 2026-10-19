#pragma once

#include "render_pass.h"

#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/pipeline_state.h"

#include <cstdint>
#include <memory>

namespace Primitive {

    class SceneRenderPass final : public IRenderPass {
    public:
        SceneRenderPass(
            std::shared_ptr<IBuffer> mesh_buffer,
            std::shared_ptr<IBuffer> transform_buffer,
            std::shared_ptr<IBuffer> meshlet_buffer,
            std::shared_ptr<IBuffer> meshlet_vertex_buffer,
            std::shared_ptr<IBuffer> meshlet_triangle_buffer,
            std::shared_ptr<IPipelineState> pipeline,
            uint32_t meshlet_count
        );

        void execute(RenderContext& context) override;
        void resize(uint32_t width, uint32_t height) override;

    private:
        std::shared_ptr<IBuffer> m_mesh_buffer;
        std::shared_ptr<IBuffer> m_transform_buffer;
        std::shared_ptr<IBuffer> m_meshlet_buffer;
        std::shared_ptr<IBuffer> m_meshlet_vertex_buffer;
        std::shared_ptr<IBuffer> m_meshlet_triangle_buffer;
        std::shared_ptr<IPipelineState> m_pipeline;
        uint32_t m_meshlet_count;

        uint32_t m_width = 1;
        uint32_t m_height = 1;
    };

}
