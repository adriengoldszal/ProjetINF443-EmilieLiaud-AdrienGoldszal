#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"

// extern std::vector<cgp::vec2> hollowCenters;
// extern std::vector<cgp::vec3> hollowPos;

struct TerrainData
{
public:
    cgp::mesh_drawable mesh;
    std::vector<cgp::vec2> hollowCenters;
    std::vector<int> type_rock;
    std::vector<float> rock_rotation;

    float gaussian(float x, float y, float a, float b, float sigma);
    float terrainFunction(float x, float y, std::vector<cgp::vec2> centers);
    std::vector<cgp::vec2> generateRandomCenters(int N, int terrain_length, int nb_hollow);
    bool nocolision(std::vector<cgp::vec2> centers, float taille, cgp::vec2 new_pos);
    void TerrainData::create_terrain_mesh(int N, int terrain_length, int nb_hollow);
    void TerrainData::generate_type_rock(int nb_hollow);
    void TerrainData::generate_rock_rotation(int nb_hollow);
};

;
