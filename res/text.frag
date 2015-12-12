#version 150

in vec2 TexCoord;
uniform sampler2D tex;
uniform vec4 color;
out vec4 gl_FragColor;

void main()
{
    vec4 alpha = texture(tex, TexCoord);
    gl_FragColor = color * vec4(1.0, 1.0, 1.0, alpha.a);
}
