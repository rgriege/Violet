#version 150

in vec2 position;
in vec2 texCoord;
uniform mat3 view;
uniform mat3 model;
uniform vec2 offset;
out vec2 TexCoord;

void main()
{
    vec3 pos = vec3(position.x + offset.x, position.y + offset.y, 1.0);
    pos = view * model * pos;
    gl_Position = vec4(pos.xy, 0.0, 1.0);
    TexCoord = texCoord;
}
