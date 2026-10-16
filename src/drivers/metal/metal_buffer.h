#pragma once

#include "Foundation/NSString.hpp"
#include "Metal/MTLBuffer.hpp"
#include "Metal/MTLResource.hpp"
#include "systems/rendering/rhi/buffer.h"
#include <Metal/Metal.hpp>
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace Primitive {

    class MetalBuffer : public IBuffer{
        public:
            MetalBuffer(MTL::Device* device, const BufferDesc& desc) : size_(desc.size), buffer_usage_(desc.buffer_usage) {
                MTL::ResourceOptions options = 0;
                if (desc.memory_usage == MemoryUsage::CPUToGPU) {
                    options |= MTL::ResourceStorageModeShared;
                } else {
                    options |= MTL::ResourceStorageModePrivate;
                }

                if (desc.initial_data && desc.memory_usage == MemoryUsage::CPUToGPU) {
                    metal_buffer_ = device->newBuffer(desc.initial_data, desc.size, options);
                } else {
                    metal_buffer_ = device->newBuffer(desc.size, options);
                }

                if (desc.debug_name && metal_buffer_) {
                    metal_buffer_->setLabel(NS::String::string(desc.debug_name, NS::UTF8StringEncoding));
                }
            }

            ~MetalBuffer() override {
                if (metal_buffer_) {
                    metal_buffer_->release();
                    metal_buffer_ = nullptr;
                }
            }

            void* map() override {
                return metal_buffer_->contents();
            }

            void unmap() override {

            }

            void update_data(const void* data, size_t size, size_t offset) override {
                void* dst = static_cast<uint8_t*>(map()) + offset;
                std::memcpy(dst, data, size);
            }

            size_t get_size() const override { return size_; };
            BufferUsage get_usage() const override { return buffer_usage_; }

            MTL::Buffer* get_native_handle() const { return metal_buffer_; }

        private:
            MTL::Buffer* metal_buffer_ = nullptr;
            size_t size_ = 0;
            BufferUsage buffer_usage_;
    };
}
