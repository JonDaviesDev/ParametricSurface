#version 450 core
layout (location = 0) in vec3 curvePosition;	  
layout (location = 1) in vec4 curveColour; 
layout (location = 2) in vec3 curveNormal;
layout (location = 3) in vec2 curveTex;

out vec4 theColour;

out vec3 Colour;
out vec2 TexCoord;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
   	gl_Position = projection * view * model * vec4(curvePosition, 1.0f);
	theColour = curveColour;
	TexCoord = curveTex;
}