#version 150 core

in vec3 TexCoords;

out vec4 finalColor;

uniform samplerCube skybox;

void main() {
    finalColor = texture(skybox, TexCoords);
}
