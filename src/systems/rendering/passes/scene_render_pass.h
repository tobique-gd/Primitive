#pragma once

#include "render_pass.h"

#include "systems/rendering/rhi/buffer.h"
#include "systems/rendering/rhi/pipeline_state.h"

#include <cstdint>
#include <memory>

namespace Engine {

    class SceneRenderPass final : public IRenderPass {
    public:
        SceneRenderPass(
            std::shared_ptr<RHI::IBuffer> vertex_buffer,
            std::shared_ptr<RHI::IBuffer> index_buffer,
            std::shared_ptr<RHI::IBuffer> transform_buffer,
            std::shared_ptr<RHI::IPipelineState> pipeline
        );

        void execute(RHI::RenderContext& context) override;
        void resize(uint32_t width, uint32_t height) override;

    private:
        std::shared_ptr<RHI::IBuffer> m_vertex_buffer;
        std::shared_ptr<RHI::IBuffer> m_index_buffer;
        std::shared_ptr<RHI::IBuffer> m_transform_buffer;
        std::shared_ptr<RHI::IPipelineState> m_pipeline;

        uint32_t m_width = 1;
        uint32_t m_height = 1;
    };

}
