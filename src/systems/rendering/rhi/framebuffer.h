#pragma once

#include "systems/rendering/rhi/texture_2d.h"
#include <memory>
#include <glm/vec4.hpp>

namespace Primitive {

    enum class FramebufferType {
        Offscreen,
        Window
    };

    struct FramebufferAttachment {
        uint32_t format = 80;
        bool is_depth = false;
        bool clear_on_load = true;
        glm::vec4 clear_color = {0.25, 0.25, 0.25, 1.0};
        float clear_depth = 1.0f;
    };

    struct FramebufferDesc {
        uint32_t width = 0;
        uint32_t height = 0;
        int sample_count = 1;
        FramebufferType type = FramebufferType::Offscreen;
        FramebufferAttachment color_attachment;
        FramebufferAttachment depth_attachment;
    };

    class IFramebuffer {
        public:
            virtual ~IFramebuffer() = default;

            virtual void resize(uint32_t width, uint32_t height) = 0;
            virtual std::shared_ptr<Texture2D> get_color_attachment() const = 0;
            virtual void* get_native_render_pass_descriptor() const = 0;
    };

}
