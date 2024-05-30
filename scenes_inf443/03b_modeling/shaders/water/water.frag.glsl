#version 330 core

// Fragment shader - this code is executed for every pixel/fragment that belongs to a displayed shape

// Inputs coming from the vertex shader
in struct fragment_data {
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

// Coefficients of phong illumination model
struct phong_structure {
    float ambient;
    float diffuse;
    float specular;
    float specular_exponent;
};

// Output of the fragment shader - output color
layout(location = 0) out vec4 FragColor;

// Uniform values that must be send from the C++ code
// ***************************************************** //

uniform samplerCube image_skybox;   // Texture image identifier
uniform float time;                 // Time value
uniform vec3 light;
uniform float water_length;
uniform mat4 view;       // View matrix (rigid transform) of the camera - to compute the camera position

void main() {

    //  Base water color
    vec3 water_color = vec3(0.50, 0.87, 0.92);

    // Compute the position of the center of the camera
    mat3 O = transpose(mat3(view));                   // get the orientation matrix
    vec3 last_col = vec3(view * vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
    vec3 camera_position = -O * last_col;

    // Compute the view direction
    float distance_to_water = length(fragment.position - camera_position);
    vec3 I = (fragment.position - camera_position) / distance_to_water;
    vec3 minusI = I;
    minusI.z = -I.z;

    // Normalize the normal
    vec3 N = normalize(fragment.normal);

    // Calculate reflection and refraction vectors
    vec3 reflected = reflect(I, N);
    vec3 refracted = refract(I, N, 1.0 / 1.33);

    // Fetch colors from the skybox
    vec3 reflectedColor = texture(image_skybox, reflected).rgb;
    vec3 refractedColor = texture(image_skybox, refracted).rgb;

    // Fresnel effect for partial reflection
    float transparency = min(0.2, pow(max(0, dot(N, -I)), 2));

    // Compute Phong illumination model
    vec3 ambient_color = vec3(0.29, 0.58, 0.66) * water_color; // ambient component
    vec3 L = normalize(light - fragment.position); // Light direction
    float diffuse_component = max(dot(N, L), 0.0);
    vec3 diffuse_color = vec3(0.8) * diffuse_component * water_color; // diffuse component

    // Specular component
    vec3 V = normalize(camera_position - fragment.position); // View direction
    vec3 R = reflect(-L, N); // Reflection direction
    float spec_angle = max(dot(R, V), 0.0);
    vec3 specular_color = vec3(0.5) * pow(spec_angle, 32.0); // Specular component with shininess factor

    vec3 phong_color = ambient_color + diffuse_color + specular_color;

    // Blend the Phong color with reflection and refraction
    vec3 color = mix(reflectedColor, water_color, 0.5);
    color = mix(color, refractedColor, transparency);
    color = mix(color, phong_color, 0.2);

    vec3 morning_sunlight = vec3(1.0, 0.8, 0.6);
    float alpha = min(0.5 * sin(time / 10.0) + 0.5, 0.7);
    float beta = min(0.5 * sin(time / 10.0 + 3.1415 / 2.0) + 0.5, 0.4);

    // Apply fog effect
    vec3 fogcolor = vec3(0.15);
    //float alpha = max(0.5 * sin(time / 10.0) + 0.5, 0.2);
    color = mix(color, fogcolor, alpha);
    color = mix(color, morning_sunlight, beta);

    float dmax = 2 * water_length;
    float d = distance(fragment.position, camera_position);
    float Kfog = min(d / dmax, 1.0);

    vec3 backgroundcolor = vec3(0.5, 0.59, 0.59);

    backgroundcolor = (backgroundcolor * (1.0 - alpha)) + (fogcolor * alpha);
    backgroundcolor = backgroundcolor * (1.0 - beta) + (morning_sunlight * beta);
    color = (1.0 - Kfog) * color + Kfog * backgroundcolor;

    FragColor = vec4(color, 0.8); // Alpha value for transparent water

}
