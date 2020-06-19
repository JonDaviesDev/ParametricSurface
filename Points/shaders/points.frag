#version 450 core
in vec4 theColour;	
out vec4 FragColour;

in vec2 TexCoord;

uniform bool textureCheck;
uniform sampler2D Texture;

void main()
{
	if (textureCheck)
	{
		FragColour = texture(Texture, TexCoord);
	}
	else
	{
		FragColour = theColour;
	}
}
