#version 150 core

in vec2 TexCoords;

out vec4 finalColor;

uniform sampler2D texture1;

void main() {
    finalColor = vec4(vec3(gl_FragCoord.z), 1.0f);
}
