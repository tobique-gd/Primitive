#pragma once

#include <cstdint>

#include "systems/rendering/rhi/render_context.h"

namespace Primitive {

    class IRenderPass {
        public:
            virtual ~IRenderPass() = default;

            virtual void execute(RenderContext& context) = 0;
            virtual void resize(uint32_t width, uint32_t height) {}
        };

}
