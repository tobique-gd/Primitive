#pragma once

#include "Metal/MTLTexture.hpp"
#include "systems/rendering/rhi/texture_2d.h"

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/Metal.hpp>

namespace Engine::RHI::Metal {
    class MetalTexture2D : public ITexture2D{
        public:
            MetalTexture2D(NS::SharedPtr<MTL::Texture> texture)
                : texture(texture)
            {
                width = texture->width();
                height = texture->height();
            }
            MTL::Texture* get_texture() const {
                return texture.get();
            }

            void* get_native_handle() const override {
                return texture.get();
            }

        private:
            NS::SharedPtr<MTL::Texture> texture;
    };
}
