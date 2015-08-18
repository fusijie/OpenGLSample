#version 150 core

in vec2 position;
in vec3 color;
in vec2 offset;

out vec3 Color;

uniform vec2 offsets[100];

void main()
{
    vec2 pos = position * (gl_InstanceID / 100.0f);
    gl_Position = vec4(pos + offset, 0.0f, 1.0f);
    Color = color;
}