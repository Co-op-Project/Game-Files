#version 460 core
layout (location = 0) in vec4 inPos;

uniform mat4 projection;
uniform mat4 model;
out vec2 texCoords;

void main()
{
    texCoords = inPos.zw;
    gl_Position = projection * model * vec4(inPos.xy, 0.0, 1.0);
}