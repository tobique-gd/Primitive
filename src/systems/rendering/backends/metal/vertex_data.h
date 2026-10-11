#pragma once
#include <glm/glm.hpp>
#include <simd/simd.h>

using namespace simd;

struct VertexData {
    float4 position;
    float3 normal;
    float2 textureCoordinate;
};

struct TransformationData {
    glm::mat4x4 modelMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 perspectiveMatrix;
};
