#version 450 core
layout (location = 4) in vec3 controlPositionLocation;	  
layout (location = 5) in vec4 controlColourLocation;

out vec4 outColour;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
   	gl_Position = projection * view * model * vec4(controlPositionLocation, 1.0f);
	outColour = controlColourLocation;
}