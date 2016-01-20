#version 150

in vec2 position;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 p = view * model * vec4(position.xy, 0.0, 1.0);
    gl_Position = p;
}
