#pragma once

#include "Metal/MTLTexture.hpp"
#include "rendering/itexture_2d.h"

#include <Foundation/NSSharedPtr.hpp>
#include <Metal/Metal.hpp>

namespace Engine {
    class MetalTexture2D : public ITexture2D{
        public:
                MetalTexture2D(NS::SharedPtr<MTL::Texture> texture)
                    : texture(texture)
                {
                }
            MTL::Texture* get_texture() const {
                return texture.get();
            }

            void* get_native_handle() const override {
                return texture.get();
            }

        public:
            NS::SharedPtr<MTL::Texture> texture;
    };
}
