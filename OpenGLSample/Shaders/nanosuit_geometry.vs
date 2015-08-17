#version 150 core

in vec3 position;
in vec3 normal;

out VS_OUT{
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vs_out.normal = vec3(projection * vec4(normalMatrix * normal, 1.0));
}