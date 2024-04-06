#version 330 core

#define PI 3.141592
#define iSteps 16
#define jSteps 8

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)
layout (location = 2) in vec3 vertex_color;    // vertex color      (r,g,b)
layout (location = 3) in vec2 vertex_uv;       // vertex uv-texture (u,v)

// Output variables sent to the fragment shader
out struct fragment_data
{
    vec3 position; // vertex position in world space
    vec3 normal;   // normal position in world space
    vec3 color;    // vertex color
    vec2 uv;       // vertex uv
} fragment;

// View matrix
uniform mat4 view;
uniform vec3 camera_position;
uniform vec3 camera_direction;
// Light properties
uniform vec3 light_position;
uniform vec3 light_color;
// Material properties
uniform vec3 material_color;
uniform float material_shininess;
// Texture properties
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

// Perlin noise parameters
uniform float time;  // Time variable for animating the Perlin noise
uniform float noise_scale;  // Scale of the noise
uniform float noise_strength;  // Strength of the noise

// Function to compute Perlin noise
float snoise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = x0 > x0.yy ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod(i, 289.0);
    vec3 p = vec3(permute( permute( i.y + vec3(0.0, i1.y, 1.0 )) + i.x + vec3(0.0, i1.x, 1.0 )) );
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

int permute(int x) {
    return x * 34 + 1 * x * 34;
}

// High octave is required for small bumps/ridges.
const int OCTAVES = 6;
float noise(vec2 position)
{
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for(int i = 0; i < OCTAVES; i++)
    {
        value += amplitude * snoise(position * frequency + time);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return value;
}

void main()
{
    // Compute the position using Perlin noise
    vec2 noise_offset = vec2(noise_scale * vertex_position.xz);
    vec3 offset = vec3(0.0, noise_strength * noise(noise_offset), 0.0);
    vec3 new_position = vertex_position + offset;

    // Transform vertex position to world space
    vec4 world_position = view * vec4(new_position, 1.0);

    // Compute the normal
    vec3 normal = normalize(mat3(view) * vertex_normal);

    // Fill the parameters sent to the fragment shader
    fragment.position = world_position.xyz;
    fragment.normal   = normal;
    fragment.color    = vertex_color;
    fragment.uv       = vertex_uv;

    // Output the final vertex position
    gl_Position = world_position;
}
