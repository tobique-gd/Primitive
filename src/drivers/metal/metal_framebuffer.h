#pragma once

#include "systems/rendering/rhi/framebuffer.h"
#include <cstdint>
#include <memory>
#include <Metal/Metal.hpp>

namespace Primitive {
    class MetalFramebuffer : public IFramebuffer {
        public:
            MetalFramebuffer(MTL::Device* device, const FramebufferDesc& desc);
            ~MetalFramebuffer() override;

            void resize(uint32_t width, uint32_t height) override;
            std::shared_ptr<Texture2D> get_color_attachment() const override { return resolved_color_texture_; }
            void* get_native_render_pass_descriptor() const override { return render_pass_descriptor_; }

        private:
            void invalidate();

            MTL::Device* device_ = nullptr;
            FramebufferDesc desc_;

            MTL::Texture* msaa_color_texture_ = nullptr;
            MTL::Texture* depth_texture_ = nullptr;
            std::shared_ptr<Texture2D> resolved_color_texture_;
            MTL::RenderPassDescriptor* render_pass_descriptor_ = nullptr;

    };
}
