#version 150 core

in vec3 position;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(position, 1.0);
    TexCoords = position;
}