#version 150

in vec2 TexCoord;
uniform sampler2D tex;
out vec4 gl_FragColor;

void main()
{
    gl_FragColor = texture(tex, TexCoord);
}
