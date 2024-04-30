
#include "terrain.hpp"
#include <list>
#include <random>

using namespace cgp;

// Evaluate 3D position of the terrain for any (x,y)
float evaluate_terrain_height(float x, float y)
{
    /*vec2 p_0 = {0, 0};
    float h_0 = 2.0f;
    float sigma_0 = 3.0f;

    float d = norm(vec2(x, y) - p_0) / sigma_0;

    float z = h_0 * std::exp(-d * d);*/

    // vec2 p_i[4] = { {-10,-10}, {5,5}, {-3,4}, {6,4} };
    // float h_i[4] = { 3.0f, -1.5f, 1.0f, 2.0f };
    // float sigma_i[4] = { 10.0f, 3.0f, 4.0f, 4.0f };

    std::array<vec2, 4> p_i = {vec2{-10, -10}, vec2{5, 5}, vec2{-3, 4}, vec2{6, 4}};
    std::array<float, 4> h_i = {3.0f, -1.5f, 1.0f, 2.0f};
    std::array<float, 4> sigma_i = {10.0f, 3.0f, 4.0f, 4.0f};

    float z = 0.0f;

    for (int k = 0; k < p_i.size(); k++)
    {
        float d = norm(vec2(x, y) - p_i[k]) / sigma_i[k];
        z += h_i[k] * std::exp(-d * d);
    }

    return z;
}

bool nocolision(std::vector<cgp::vec3> tree_position, float taille, cgp::vec3 new_pos)
{
    for (cgp::vec3 pos : tree_position)
    {
        if ((std::abs(pos.x - new_pos.x) <= taille) || (std::abs(pos.y - new_pos.y) <= taille))
        {
            return false;
        }
    }
    return true;
}

std::vector<cgp::vec3> generate_positions_on_terrain(int N, float terrain_length)
{
    std::vector<cgp::vec3> list_p;
    float x = 0;
    float y = 0;
    int k = 0;
    while (k < N)
    {
        x = rand_uniform(-terrain_length / 2, terrain_length / 2);
        y = rand_uniform(-terrain_length / 2, terrain_length / 2);
        if (nocolision(list_p, 0.2f, vec3{x, y, 0}) == true)
        {
            list_p.push_back({x, y, evaluate_terrain_height(x, y)});
            k++;
        }
    }
    return list_p;
}

mesh create_terrain_mesh(int N, float terrain_length)
{

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
            float z = evaluate_terrain_height(x, y);

            // Store vertex coordinates
            terrain.position[kv + N * ku] = {x, y, z};
            terrain.uv[kv + N * ku] = {u * N / 30, v * N / 30};
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

    return terrain;
}