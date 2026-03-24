#version 330 core
out vec4 color;

uniform sampler2D texID;
in vec2 texCoords;

void main()
{    
    color = texture(texID, texCoords);
}