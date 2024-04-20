#version 330 core

layout(location = 0) in vec3 position;

out struct fragment_data {
    vec3 position;
} fragment;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragment.position = position.xyz;

    mat4 modelView = mat4(mat3(view * model)); // remove the translational part
    gl_Position = projection * modelView * vec4(position, 1.0);
}