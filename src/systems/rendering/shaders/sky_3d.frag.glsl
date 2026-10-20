#version 450

layout(location = 0) out vec4 fragmentColor;

layout(location = 0) in vec2 tex_coord;
layout(location = 1) in vec3 v_normal;

void main()
{
    fragmentColor = vec4(vec3(8.0/255.0, 10.0/255.0, 11.5/255.0), 1.0);
}
