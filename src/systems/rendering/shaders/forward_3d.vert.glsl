#version 450

struct VertexData
{
    vec4 position;
    vec3 normal;
    vec2 textureCoordinate;
};

layout(std430, binding = 0) readonly buffer VertexBuffer
{
    VertexData vertexData[];
};

layout(std140, binding = 1) uniform TransformationData
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 perspectiveMatrix;
};

layout(location = 0) out vec2 o_tex_coord;
layout(location = 1) out vec3 o_normal;

void main()
{
    VertexData vertex = vertexData[gl_VertexIndex];

    gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vertex.position;

    o_normal = normalize(transpose(inverse(mat3(modelMatrix))) * vertex.normal);
    o_tex_coord = vertex.textureCoordinate;
}
