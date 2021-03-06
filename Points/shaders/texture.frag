#version 450 core
out vec4 FragColour;
  
in vec3 Colour;
in vec2 TexCoord;

uniform sampler2D Texture;

void main()
{
    FragColour = texture(Texture, TexCoord);
}