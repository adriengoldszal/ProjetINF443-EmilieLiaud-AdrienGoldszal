#version 330 core

in struct fragment_data {
    vec3 position;
} fragment;

layout(location = 0) out vec4 FragColor;

uniform samplerCube image_skybox;
uniform float time;

// Main Shader
/***************************************************************************************************/

void main() {
    // Texture color
    vec3 current_color = vec3(texture(image_skybox, fragment.position));

    vec3 backgroundcolor = vec3(0.5, 0.59, 0.59);

    vec3 fogcolor = vec3(0.15);
    vec3 morning_sunlight = vec3(1.0, 0.8, 0.6);
    vec3 red = vec3(1.0, 0.0, 0.0);
    float alpha = min(0.5 * sin(time / 10.0) + 0.5, 0.7);
    float beta = min(0.5 * sin(time / 10.0 + 3.1415 / 2.0) + 0.5, 0.4);
    //float gamma = min(0.5 * sin(time / 10.0 - 3.1415 / 2.0) + 0.5, 0.7);
    current_color = (current_color * (1.0 - alpha)) + (fogcolor * alpha);
    current_color = current_color * (1.0 - beta) + (morning_sunlight * beta);

    backgroundcolor = (backgroundcolor * (1.0 - alpha)) + (fogcolor * alpha);
    backgroundcolor = backgroundcolor * (1.0 - beta) + (morning_sunlight * beta);

    if(fragment.position.z < 1 && fragment.position.z > -0.2) {
        float gamma = min(fragment.position.z + 0.2, 1.0);
        current_color = mix(backgroundcolor, current_color, gamma);
    }

    //current_color = (current_color * (1.0 - gamma)) + (red * gamma);
	// Texture outputs
    /************************************************************/
    FragColor = vec4(current_color, 1.0);

}