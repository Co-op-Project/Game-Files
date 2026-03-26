#version 460 core
out vec4 color;

uniform sampler2D texID;
in vec2 texCoords;
const float unit = 0.03125;
uniform vec2 texMins;

void main()
{
    vec2 texCoordsOutput = texCoords;
    texCoordsOutput.x = mod(texCoords.x, unit) + texMins.x;
    if(texMins.x == texCoords.x) texCoordsOutput.x += unit;

    texCoordsOutput.y = mod(texCoords.y, unit) + texMins.y;
    if(texMins.y == texCoords.y) texCoordsOutput.y += unit;

    color = texture(texID, texCoordsOutput);
}