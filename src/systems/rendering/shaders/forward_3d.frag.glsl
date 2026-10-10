#version 450

layout(location = 0) out vec4 fragmentColor;

layout(location = 0) in vec2 tex_coord;
layout(location = 1) in vec3 v_normal;

void main()
{
    float lambert_term = dot(normalize(v_normal), normalize(vec3(1.0, -0.5, 1.0)));
    float diff =  lambert_term * lambert_term;
    vec3 diffuse = diff * vec3(1.0) * vec3(0.1, 0.8, 0.6);

    fragmentColor = vec4(diffuse, 1.0);
}
