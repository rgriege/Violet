#version 150

in vec2 TexCoord;
uniform vec4 color;
uniform sampler2D tex;

void main()
{
    gl_FragColor = mix(color, texture(tex, TexCoord), 1);
}
