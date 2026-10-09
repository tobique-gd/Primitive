#pragma once

#include <cstdint>
#include <cstddef>

namespace Engine::RHI {

    enum class BufferUsage {
        Vertex,
        Index,
        Uniform,
        Storage
    };

    enum class MemoryUsage {
        CPUToGPU,
        GPUOnly
    };

    struct BufferDesc {
        size_t size = 0;
        BufferUsage buffer_usage = BufferUsage::Vertex;
        MemoryUsage memory_usage = MemoryUsage::CPUToGPU;
        const void* initial_data = nullptr;
        const char* debug_name = nullptr;
    };

    class IRHIBuffer {
        public:
            virtual ~IRHIBuffer() = default;

            virtual void* map() = 0;
            virtual void unmap() = 0;
            virtual void update_data(const void* data, size_t size, size_t offset) = 0;

            virtual size_t get_size() const = 0;
            virtual BufferUsage get_usage() const = 0;
    };


}
