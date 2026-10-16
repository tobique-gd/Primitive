#version 450

layout(location = 0) out vec4 fragmentColor;

//layout(location = 0) in vec2 tex_coord;
layout(location = 1) in vec3 v_normal;

void main()
{
    float lambert_term = dot(normalize(v_normal), normalize(vec3(1.0, -0.5, 1.0)));
    float l_dot_h = lambert_term * 0.5 + 0.5;
    float diff_half =  lambert_term * lambert_term;

    float diff =  lambert_term;

    vec3 diffuse = diff_half * vec3(0.733, 0.733, 0.733);

    fragmentColor = vec4(diffuse, 1.0);
}
