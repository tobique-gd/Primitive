#version 450

struct VertexData
{
    vec4 position;
    vec3 normal;
    vec2 texture_coordinate;
};

layout(std430, binding = 0) readonly buffer VertexBuffer
{
    VertexData vertexData[];
};

layout(std140, binding = 1) uniform TransformationData
{
    mat4 model_matrix;
    mat4 view_matrix;
    mat4 perspective_matrix;
};

layout(location = 0) out vec2 o_tex_coord;
layout(location = 1) out vec3 o_normal;

void main()
{
    VertexData vertex = vertexData[gl_VertexIndex];

    gl_Position = perspective_matrix * view_matrix * model_matrix * vertex.position;

    o_normal = normalize(transpose(inverse(mat3(model_matrix))) * vertex.normal);
    o_tex_coord = vertex.texture_coordinate;
}
