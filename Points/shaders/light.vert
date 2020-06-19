#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 colour;
layout (location = 2) in vec3 normal;

out vec3 Position;
out vec3 Colour;
out vec3 Normal;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
    Position = pos;
    Colour = colour;
    Normal = normal;
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0f);
}