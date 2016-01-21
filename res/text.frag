#version 150

in vec2 TexCoord;
uniform sampler2D tex;
uniform vec4 color;
out vec4 gl_FragColor;

void main()
{
    vec4 alpha = texture(tex, TexCoord);
    
    if (alpha.a == 0)
      discard;
    
    gl_FragColor = vec4(1.0, 1.0, 1.0, alpha.a) * color;
}
