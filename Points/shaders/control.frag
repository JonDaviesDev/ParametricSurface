#version 450 core
in vec4 outColour;
out vec4 FragColour;

void main()
{
	FragColour = outColour;
}