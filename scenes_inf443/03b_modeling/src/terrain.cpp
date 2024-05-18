
#include "scene.hpp"

#include "terrain.hpp"
#include <list>
#include <random>
#include <iostream>

using namespace cgp;

std::vector<cgp::vec2> hollowCenters;
std::vector<cgp::vec3> hollowPos;


//sigma: width of hollow
float gaussian(float x, float y, float a, float b, float sigma) {
    float dx = x - a;
    float dy = y - b;
    return -exp(-((dx * dx) + (dy * dy)) / (2 * sigma * sigma));
}

float terrainFunction(float x, float y) {
    float result = 0.0f;
    for (vec2 center : hollowCenters) {
        result += gaussian(x, y, center.x, center.y, 2);
    }
    //std::cout << result << std::endl;
    return result-1;
}

void generateRandomCenters(int N, int terrain_length) {
    std::srand(static_cast<unsigned int>(std::time(0)));  // Seed for randomness
    int nb = 0;
    while (nb < 50) {
        vec2 center;
        center.x = (std::rand() % (terrain_length)) - terrain_length / 2;  // Random x within terrain bounds
        center.y = (std::rand() % (terrain_length)) - terrain_length / 2;  // Random y within terrain bounds

        if (nocolision(hollowCenters, 2.5, center)) {
            hollowCenters.push_back(center);
            nb++;
        }
    }
}

bool nocolision(std::vector<cgp::vec2> center, float taille, cgp::vec2 new_pos) {
    for (cgp::vec2 pos : center) {
        if ((std::abs(pos.x - new_pos.x) <= taille) || (std::abs(pos.y - new_pos.y) <= taille)) {
            return false;
        }
    }
    return true;
}

cgp::mesh create_terrain_mesh(int N, int terrain_length){
    std::cout << "start" << std::endl;

    generateRandomCenters(N, terrain_length);

    for (int i = 0; i < hollowCenters.size(); i++) {
        std::cout << "x:" << hollowCenters[i].x << " y:" << hollowCenters[i].y;
    }

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N * N);
    terrain.uv.resize(N * N);

    // Fill terrain geometry
    for (int ku = 0; ku < N; ++ku)
    {
        for (int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku / (N - 1.0f);
            float v = kv / (N - 1.0f);

            // Compute the real coordinates (x,y) of the terrain in [-terrain_length/2, +terrain_length/2]
            float x = (u - 0.5f) * terrain_length;
            float y = (v - 0.5f) * terrain_length;

            // Compute the surface height function at the given sampled coordinate
            float z = terrainFunction(x, y);

            // Store vertex coordinates
            terrain.position[kv + N * ku] = { x,y,z };
            terrain.uv[kv + N * ku] = { u * N / 30, v * N / 30 };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for (int ku = 0; ku < N - 1; ++ku)
    {
        for (int kv = 0; kv < N - 1; ++kv)
        {
            unsigned int idx = kv + N * ku; // current vertex offset

            uint3 triangle_1 = { idx, idx + 1 + N, idx + 1 };
            uint3 triangle_2 = { idx, idx + N, idx + 1 + N };

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
    terrain.fill_empty_field();

    return terrain;
}