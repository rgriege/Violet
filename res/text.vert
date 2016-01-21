#version 150

in vec2 position;
in vec2 texCoord;
uniform mat4 view;
uniform mat4 model;
uniform vec2 offset;
out vec2 TexCoord;

void main()
{
    vec2 offsetPosition = position + offset;
    gl_Position = view * model * vec4(offsetPosition, 0, 1);
    TexCoord = texCoord;
}
