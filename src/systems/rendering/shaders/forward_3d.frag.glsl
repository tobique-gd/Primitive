#version 450

layout(location = 0) out vec4 fragmentColor;

layout(location = 0) in vec3 meshlet_color;
layout(location = 1) in vec3 v_normal;

void main()
{
    float lambert_term = dot(normalize(v_normal), normalize(vec3(0.0, 1.0, 1.0)));

    float diff =  lambert_term;

    //vec3 diffuse = diff * vec3(0.773, 0.773, 0.773);
    vec3 diffuse = meshlet_color;

    fragmentColor = vec4(diffuse, 1.0);
}
