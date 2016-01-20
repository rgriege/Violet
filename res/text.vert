#version 150

in vec2 position;
in vec2 texCoord;
uniform mat4 view;
uniform mat4 model;
uniform vec2 offset;
out vec2 TexCoord;

void main()
{
    vec4 pos = vec4(position.x + offset.x, position.y + offset.y, 0.0, 1.0);
    pos = view * model * pos;
    gl_Position = pos;
    TexCoord = texCoord;
}
