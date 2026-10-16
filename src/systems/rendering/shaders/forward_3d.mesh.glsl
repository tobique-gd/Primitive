#version 450
#extension GL_EXT_mesh_shader : require

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = 24, max_primitives = 12) out;


struct VertexData
{
    vec4 position;
    vec3 normal;
    vec2 textureCoordinate;
};

layout(std430, binding = 0) readonly buffer VertexBuffer
{
    VertexData vertex_Data[];
};

layout(std430, binding = 1) readonly buffer IndexBuffer
{
    uint index_Data[];
};

layout(std140, binding = 2) uniform TransformationData
{
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 perspectiveMatrix;
};

layout(location = 1) out vec3 v_normal[];

void main() {
    uint g_ID = gl_GlobalInvocationID.x;

    uint vertex_count = 24;
    uint triangle_count = 12;

    SetMeshOutputsEXT(vertex_count, triangle_count);

    mat4 mvp = perspectiveMatrix * viewMatrix * modelMatrix;

    if (g_ID < 24) {

        gl_MeshVerticesEXT[g_ID].gl_Position = mvp * vertex_Data[g_ID].position;
        v_normal[g_ID] = normalize(transpose(inverse(mat3(modelMatrix))) * vertex_Data[g_ID].normal);

    }

    if (g_ID < 12) {
        uint index0 = index_Data[g_ID * 3 + 0];
        uint index1 = index_Data[g_ID * 3 + 1];
        uint index2 = index_Data[g_ID * 3 + 2];

        gl_PrimitiveTriangleIndicesEXT[g_ID] = uvec3(index0, index1, index2);
    }








}
