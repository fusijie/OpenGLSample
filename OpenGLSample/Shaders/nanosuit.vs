#version 150 core

in vec3 position;
in vec3 normal;

out vec3 Position;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    Normal = mat3(transpose(inverse(model))) * normal;
    Position = vec3(model * vec4(position, 1.0f));
}