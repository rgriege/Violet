#version 150

in vec2 position;
in vec2 texCoord;
uniform mat4 view;
uniform mat4 model;
out vec2 TexCoord;

void main()
{
    vec3 p = view * model * vec4(position, 0.0, 1.0);
    gl_Position = p;
    TexCoord = texCoord;
}
