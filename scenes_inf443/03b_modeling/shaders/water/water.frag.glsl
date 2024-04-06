#version 330 core

#define PI 3.141592
#define iSteps 16
#define jSteps 8

// Inputs coming from the vertex shader
in struct fragment_data
{
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

in vec4 clip_space;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;

// View matrix
uniform mat4 view;
uniform vec3 camera_position;
uniform vec3 camera_direction;
// Light properties
uniform vec3 light_position;       // Accessible
uniform vec3 light_color;          // Accessible
// Material properties
uniform vec3 material_color;       // Accessible
uniform float material_shininess;  // Accessible
// Texture properties
uniform sampler2D texture_diffuse;  // Not used in the provided code
uniform sampler2D texture_specular; // Not used in the provided code

// Function to compute the fresnel effect
float fresnel(vec3 normal, vec3 view_direction, float F0)  // All arguments accessible
{
    return F0 + (1.0 - F0) * pow(1.0 - dot(normal, view_direction), 5.0);
}

// Function to compute the reflection vector
vec3 reflect(vec3 normal, vec3 view_direction)  // All arguments accessible
{
    return normalize(view_direction - 2.0 * dot(view_direction, normal) * normal);
}

// Function to compute the refraction vector
vec3 refract(vec3 normal, vec3 view_direction, float eta)  // All arguments accessible
{
    float k = 1.0 - eta * eta * (1.0 - dot(normal, view_direction) * dot(normal, view_direction));
    return k < 0.0 ? vec3(0.0) : normalize(eta * view_direction - (eta * dot(normal, view_direction) + sqrt(k)) * normal);
}

// Function to compute the Schlick approximation
vec3 schlick_approximation(vec3 normal, vec3 view_direction, float F0)  // All arguments accessible
{
    return reflect(normal, view_direction) * fresnel(normal, view_direction, F0) + refract(normal, view_direction, 1.0 / 1.5) * (1.0 - fresnel(normal, view_direction, F0));
}

// Function to compute the color of the fragment
vec3 compute_fragment_color(vec3 normal, vec3 view_direction, vec3 light_direction, vec3 light_color, vec3 material_color, float shininess)  // All arguments accessible
{
    float diffuse = max(dot(normal, light_direction), 0.0);
    float specular = pow(max(dot(reflect(-light_direction, normal), view_direction), 0.0), shininess);
    return material_color * (diffuse * light_color + specular);
}

void main()
{
    // Compute the normal
    vec3 normal = normalize(fragment.normal);  // Accessible
    // Compute the view direction
    vec3 view_direction = normalize(camera_position - fragment.position);  // Accessible
    // Compute the light direction
    vec3 light_direction = normalize(light_position - fragment.position);  // Accessible
    // Compute the color
    vec3 color = compute_fragment_color(normal, view_direction, light_direction, light_color, material_color, material_shininess);  // All arguments accessible

    FragColor = vec4(color, 1.0);
}
