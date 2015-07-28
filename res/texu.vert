#version 150

in vec2 position;
in vec2 texCoord;
uniform mat3 view;
uniform mat3 model;
out vec2 TexCoord;

void main()
{
    vec3 p = view * model * vec3(position, 1.0);
    gl_Position = vec4(p.xy, 0.0, 1.0);
    TexCoord = texCoord;
}
