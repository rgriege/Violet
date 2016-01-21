#version 150

in vec2 position;
in vec2 texCoord;
uniform mat4 view;
uniform mat4 model;
out vec2 TexCoord;

void main()
{
    gl_Position = view * model * vec4(position, 0, 1);
    TexCoord = texCoord;
}
