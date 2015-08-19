#version 150 core

in vec3 position;
in vec2 texCoords;
in mat4 instanceMatrix;

out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(position, 1.0f);
    TexCoords = texCoords;
}