#version 450

layout(location = 0) out vec4 fragmentColor;

layout(location = 0) in vec2 tex_coord;
layout(location = 1) in vec3 v_normal;

void main()
{

    fragmentColor = vec4(vec3(0.5, 0.0, 1.0), 1.0);
}
