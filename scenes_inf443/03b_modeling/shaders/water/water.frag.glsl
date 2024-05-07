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

    //  Base water color
    vec3 water_color = vec3(0.0, 0.3, 0.5);
    // Compute the position of the center of the camera
    mat3 O = transpose(mat3(view));                   // get the orientation matrix
    vec3 last_col = vec3(view * vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
    vec3 camera_position = -O * last_col;

    float distance_to_water = length(fragment.position - camera_position);
    vec3 I = (fragment.position - camera_position) / distance_to_water;

    // Normalize the normal
    vec3 normal = normalize(fragment.normal);

    // Calculate reflection and refraction vectors
    vec3 reflected = reflect(I, normal);
    vec3 refracted = refract(I, normal, 1.0 / 1.33);

    // reflected skybox color
    vec3 reflectedColor = texture(image_skybox, reflected).rgb;
    vec3 refractedColor = texture(image_skybox, refracted).rgb;

    // Partial reflection - Water reflects at normal angles and refracts more at steep angles
            // Reflectiveness also corresponds to the angle steepness
            // Fresnel effect. Source: https://www.youtube.com/watch?v=vTMEdHcKgM4&t=874s
    float transparency = min(0.2, pow(max(0, dot(normal, -I)), 2));

    //current_color = mix(reflexion_texture, refraction_texture, transparency); // Blend reflection and refraction

    //Blend colors
    vec3 color = mix(reflectedColor, refractedColor, transparency);

    FragColor = vec4(color, 0.9); //Alpha value for transparent water
}
