#version 330 core

// Fragment shader - this code is executed for every pixel/fragment that belongs to a displayed shape

// Inputs coming from the vertex shader
in struct fragment_data {
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

// Output of the fragment shader - output color
layout(location = 0) out vec4 FragColor;

// Uniform values that must be send from the C++ code
// ***************************************************** //

uniform samplerCube image_skybox;   // Texture image identifier

uniform mat4 view;       // View matrix (rigid transform) of the camera - to compute the camera position

void main() {
    // Compute the position of the center of the camera
    mat3 O = transpose(mat3(view));                   // get the orientation matrix
    vec3 last_col = vec3(view * vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
    vec3 camera_position = -O * last_col;

    // Normalize the normal
    vec3 normal = normalize(fragment.normal);

    // Calculate reflection vector
    vec3 R = reflect(normalize(fragment.position - camera_position), normal);

    // Sample color from the skybox
    FragColor = vec4(texture(image_skybox, R).rgb, 1.0);
}