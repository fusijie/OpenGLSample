#version 150 core

in vec2 TexCoords;

out vec4 finalColor;

uniform sampler2D texture_diffuse1;

void main()
{
    finalColor = texture(texture_diffuse1, TexCoords);
}