
#include "scene.hpp"

#include "terrain.hpp"
#include <list>
#include <random>
#include <iostream>

using namespace cgp;

// sigma: width of hollow
float TerrainData::gaussian(float x, float y, float a, float b, float sigma)
{
    float dx = x - a;
    float dy = y - b;
    return -exp(-((dx * dx) + (dy * dy)) / (2 * sigma * sigma));
}

float TerrainData::terrainFunction(float x, float y, std::vector<cgp::vec2> centers)
{
    float result = 0.0f;
    for (vec2 center : centers)
    {
        result += gaussian(x, y, center.x, center.y, 2);
    }
    // std::cout << result << std::endl;
    int offset = 4;
    return result + offset;
}

bool TerrainData::nocolision(std::vector<cgp::vec2> center, float taille, cgp::vec2 new_pos)
{
    for (cgp::vec2 pos : center)
    {
        if ((std::abs(pos.x - new_pos.x) <= taille) || (std::abs(pos.y - new_pos.y) <= taille))
        {
            return false;
        }
    }
    return true;
}

std::vector<cgp::vec2> TerrainData::generateRandomCenters(int N, int terrain_length, int nb_hollow)
{
    std::srand(static_cast<unsigned int>(std::time(0))); // Seed for randomness
    std::vector<cgp::vec2> centers;
    int nb = 0;
    while (nb < nb_hollow)
    {
        vec2 center;
        center.x = (std::rand() % (terrain_length)) - terrain_length / 2; // Random x within terrain bounds
        center.y = (std::rand() % (terrain_length)) - terrain_length / 2; // Random y within terrain bounds

        if (nocolision(centers, 2.5, center))
        {
            centers.push_back(center);
            nb++;
        }
    }
    return centers;
}

void TerrainData::create_terrain_mesh(int N, int terrain_length, int nb_hollow)
{
    std::cout << "start" << std::endl;

    hollowCenters = generateRandomCenters(N, terrain_length, nb_hollow);

    for (int i = 0; i < hollowCenters.size(); i++)
    {
        std::cout << "x:" << hollowCenters[i].x << " y:" << hollowCenters[i].y;
    };

    cgp::mesh terrain; // temporary terrain storage (CPU only)
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
            float z = terrainFunction(x, y, hollowCenters);

            // Store vertex coordinates
            terrain.position[kv + N * ku] = {x, y, z};
            terrain.uv[kv + N * ku] = {u * 2.0f, v * 2.0f};
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for (int ku = 0; ku < N - 1; ++ku)
    {
        for (int kv = 0; kv < N - 1; ++kv)
        {
            unsigned int idx = kv + N * ku; // current vertex offset

            uint3 triangle_1 = {idx, idx + 1 + N, idx + 1};
            uint3 triangle_2 = {idx, idx + N, idx + 1 + N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
    terrain.fill_empty_field();

    mesh.initialize_data_on_gpu(terrain);
}

void TerrainData::generate_type_rock(int nb_hollow) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);

    type_rock.clear();  // Ensure the vector is empty before filling it with new values

    for (int i = 0; i < nb_hollow; ++i)
    {
        int random_value = dis(gen);
        type_rock.push_back(random_value);
    }
}

void TerrainData::generate_rock_rotation(int nb_hollow) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);


    rock_rotation.clear();  // Ensure the vector is empty before filling it with new values

    for (int i = 0; i < nb_hollow; ++i)
    {
        int random_value = dis(gen);
        int random_value2 = dis(gen);
        std::cout << 2*(float)random_value / 100 << std::endl;
        rock_rotation.push_back(2* (float) random_value/100);
    }
}