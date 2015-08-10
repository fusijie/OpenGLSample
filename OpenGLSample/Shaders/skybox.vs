#version 150 core

in vec3 position;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = position;
}