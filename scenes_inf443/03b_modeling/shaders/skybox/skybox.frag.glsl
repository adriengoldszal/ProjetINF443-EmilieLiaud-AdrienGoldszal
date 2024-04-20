#version 330 core

in struct fragment_data {
    vec3 position;
} fragment;

layout(location = 0) out vec4 FragColor;

uniform samplerCube image_skybox;

// Main Shader
/***************************************************************************************************/

void main() {
    // Texture color
    vec3 current_color = vec3(texture(image_skybox, fragment.position));

	// Texture outputs
    /************************************************************/
    FragColor = vec4(current_color, 1.0);

}