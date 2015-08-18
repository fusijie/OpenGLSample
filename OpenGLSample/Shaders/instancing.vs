#version 150 core

in vec2 position;
in vec3 color;

out vec3 Color;

uniform vec2 offsets[100];

void main()
{
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(position + offset, 0.0f, 1.0f);
    Color = color;
}