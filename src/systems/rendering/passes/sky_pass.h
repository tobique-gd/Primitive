#pragma once

#include "render_pass.h"

#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/render_pipeline_state.h"

#include <cstdint>
#include <memory>

namespace Primitive {

    class SkyPass final : public IRenderPass {
    public:
        SkyPass(
            std::shared_ptr<IBuffer> vertex_buffer,
            std::shared_ptr<IBuffer> index_buffer,
            std::shared_ptr<IBuffer> transform_buffer,
            std::shared_ptr<IRenderPipelineState> pipeline
        );

        void execute(RenderContext& context) override;
        void resize(uint32_t width, uint32_t height) override;

    private:
        std::shared_ptr<IBuffer> m_vertex_buffer;
        std::shared_ptr<IBuffer> m_index_buffer;
        std::shared_ptr<IBuffer> m_transform_buffer;
        std::shared_ptr<IRenderPipelineState> m_pipeline;

        uint32_t m_width = 1;
        uint32_t m_height = 1;
    };

}
