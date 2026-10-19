#version 450
#extension GL_EXT_mesh_shader : require
#extension GL_EXT_shader_8bit_storage : require
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : require


layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = 64, max_primitives = 64) out;

struct VertexData {
    vec4 position;
    vec3 normal;
    vec2 textureCoordinate;
};

struct Meshlet {
    uint vertex_offset;
    uint triangle_offset;
    uint vertex_count;
    uint triangle_count;
};

layout(std430, binding = 0) readonly buffer VertexBuffer {
    VertexData vertex_data[];
};

layout(std140, binding = 1) uniform TransformationData {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 perspectiveMatrix;
};

layout(std430, binding = 2) readonly buffer MeshletBuffer {
    Meshlet meshlets[];
};

layout(std430, binding = 3) readonly buffer MeshletVertexBuffer {
    uint meshlet_vertices[];
};

layout(std430, binding = 4) readonly buffer MeshletTriangleBuffer {
    uint8_t meshlet_triangles[];
};

layout(location = 0) perprimitiveEXT out vec3 meshlet_color[];
layout(location = 1) out vec3 v_normal[];

//https://thebookofshaders.com/10/
float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {
    Meshlet meshlet = meshlets[gl_WorkGroupID.x];

    SetMeshOutputsEXT(meshlet.vertex_count, meshlet.triangle_count);

    mat4 mvp = perspectiveMatrix * viewMatrix * modelMatrix;
    mat3 normal_matrix = transpose(inverse(mat3(modelMatrix)));

    for (uint i = gl_LocalInvocationIndex; i < meshlet.vertex_count; i += gl_WorkGroupSize.x) {
        uint vertex_index = meshlet_vertices[meshlet.vertex_offset + i];
        VertexData vertex = vertex_data[vertex_index];

        gl_MeshVerticesEXT[i].gl_Position = mvp * vertex.position;
        v_normal[i] = normalize(normal_matrix * vertex.normal);
    }

    for (uint i = gl_LocalInvocationIndex; i < meshlet.triangle_count; i += gl_WorkGroupSize.x) {
        meshlet_color[i] = vec3(
            random(vec2(gl_WorkGroupID.x * 2.8, gl_WorkGroupID.x * 3.1)),
            random(vec2(gl_WorkGroupID.x * 8.72, gl_WorkGroupID.x * 4.32)),
            random(vec2(gl_WorkGroupID.x * 3.2, gl_WorkGroupID.x * 5.9))
        );

        uint offset = meshlet.triangle_offset + i * 3u;

        gl_PrimitiveTriangleIndicesEXT[i] = uvec3(
            uint(meshlet_triangles[offset]),
            uint(meshlet_triangles[offset + 1u]),
            uint(meshlet_triangles[offset + 2u])
        );
    }




}
