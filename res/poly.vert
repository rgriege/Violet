#version 150

in vec2 position;
uniform mat3 view;
uniform mat3 model;

void main()
{
    vec3 p = view * model * vec3(position, 1.0);
    gl_Position = vec4(p.xy, 0.0, 1.0);
}
