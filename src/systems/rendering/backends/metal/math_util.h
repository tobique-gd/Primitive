#pragma once

#include <simd/simd.h>

matrix_float4x4 matrix4x4_translation(float x, float y, float z)
{
    matrix_float4x4 matrix = {};

    matrix.columns[0] = {1, 0, 0, 0};
    matrix.columns[1] = {0, 1, 0, 0};
    matrix.columns[2] = {0, 0, 1, 0};
    matrix.columns[3] = {x, y, z, 1};

    return matrix;
}

matrix_float4x4 matrix4x4_rotation(float radians, float x, float y, float z)
{
    simd::float3 axis = simd::normalize(simd::float3{x, y, z});

    float c = std::cos(radians);
    float s = std::sin(radians);
    float t = 1.0f - c;

    matrix_float4x4 matrix = {};

    matrix.columns[0] = {
        t * axis.x * axis.x + c,
        t * axis.x * axis.y + s * axis.z,
        t * axis.x * axis.z - s * axis.y,
        0
    };

    matrix.columns[1] = {
        t * axis.x * axis.y - s * axis.z,
        t * axis.y * axis.y + c,
        t * axis.y * axis.z + s * axis.x,
        0
    };

    matrix.columns[2] = {
        t * axis.x * axis.z + s * axis.y,
        t * axis.y * axis.z - s * axis.x,
        t * axis.z * axis.z + c,
        0
    };

    matrix.columns[3] = {0, 0, 0, 1};

    return matrix;
}

matrix_float4x4 matrix_make_rows(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33
)
{
    matrix_float4x4 matrix = {};

    matrix.columns[0] = {m00, m10, m20, m30};
    matrix.columns[1] = {m01, m11, m21, m31};
    matrix.columns[2] = {m02, m12, m22, m32};
    matrix.columns[3] = {m03, m13, m23, m33};

    return matrix;
}

matrix_float4x4 matrix_perspective_right_hand(
    float fovY,
    float aspect,
    float nearZ,
    float farZ
)
{
    float yScale = 1.0f / std::tan(fovY * 0.5f);
    float xScale = yScale / aspect;

    matrix_float4x4 matrix = {};

    matrix.columns[0] = {xScale, 0, 0, 0};
    matrix.columns[1] = {0, yScale, 0, 0};
    matrix.columns[2] = {
        0,
        0,
        (farZ + nearZ) / (nearZ - farZ),
        -1
    };
    matrix.columns[3] = {
        0,
        0,
        (2.0f * farZ * nearZ) / (nearZ - farZ),
        0
    };

    return matrix;
}
