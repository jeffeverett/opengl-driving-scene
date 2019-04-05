// fragment shader
#version 400 core
out vec4 FragColor;
in  vec2 TexCoord;
  
uniform sampler2D fboAttachment;
  
void main()
{
    FragColor = texture(fboAttachment, TexCoord);
}