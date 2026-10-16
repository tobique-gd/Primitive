#version 450
#extension GL_EXT_mesh_shader : require

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = 3, max_primitives = 1) out;


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

layout(location = 1) out vec3 v_normal[];

void main() {
    vec4 dummy_pos = vertexData[0].position;
    mat4 dummy_mat = perspectiveMatrix;

    uint vertex_count = 3;
    uint triangle_count = 1;
    SetMeshOutputsEXT(vertex_count, triangle_count);
    gl_MeshVerticesEXT[0].gl_Position = vec4(0.5,-0.5, 0, 1);
    gl_MeshVerticesEXT[1].gl_Position = vec4(0.5, 0.5, 0, 1);
    gl_MeshVerticesEXT[2].gl_Position = vec4(-0.5, 0.5, 0, 1);
    v_normal[0] = vec3(1.0, 0.0, 0.0);
    gl_PrimitiveTriangleIndicesEXT[0] =  uvec3(2, 1, 0);
}
