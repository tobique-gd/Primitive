#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <simd/simd.h>

#define GLM_FORCE_ALIGNED_GENTYPES

using namespace simd;

struct VertexData {
    alignas(16) glm::vec4 position;
    alignas(16) glm::vec3 normal;
    alignas(8) glm::vec2 texture_coordinate;
};

struct TransformationData {
    alignas(64) glm::mat4x4 model_matrix;
    alignas(64) glm::mat4x4 view_matrix;
    alignas(64) glm::mat4x4 perspective_matrix;
};

struct GPUMeshlet {
    uint32_t vertex_offset;
    uint32_t triangle_offset;
    uint32_t vertex_count;
    uint32_t triangle_count;
};

static_assert(sizeof(GPUMeshlet) == 16);
