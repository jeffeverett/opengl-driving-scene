// fragment shader
#version 330 core
out vec4 FragColor;
in  vec2 TexCoord;
  
uniform sampler2D fboAttachment;
  
void main()
{
    FragColor = texture(fboAttachment, TexCoord);
}