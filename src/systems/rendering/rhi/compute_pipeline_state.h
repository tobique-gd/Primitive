#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <Metal/Metal.hpp>

namespace Primitive
{

    struct ComputePipelineDesc {
        void* compute_shader;

        // https://developer.apple.com/documentation/metal/mtlcomputepipelinedescriptor/maxtotalthreadsperthreadgroup
        // By default, this property’s value is 0, which instructs Metal to calculate the maximum number of threads per
        // threadgroup based on the device’s capabilities and the compute shader’s memory usage.
        uint32_t max_total_threads_per_threadgroup = 0;

        std::string debug_name;
    };

    class IComputePipelineState {
        public:
            virtual ~IComputePipelineState() = default;

            virtual const std::string& get_debug_name() const = 0;
    };

}
