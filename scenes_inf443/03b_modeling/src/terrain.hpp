#pragma once

#include "cgp/cgp.hpp"

extern std::vector<cgp::vec2> hollowCenters;
extern std::vector<cgp::vec3> hollowPos;

cgp::mesh create_terrain_mesh(int N, int terrain_length);

float gaussian(float x, float y, float a, float b, float sigma);
float terrainFunction(float x, float y);
void generateRandomCenters(int N, int terrain_length);
bool nocolision(std::vector<cgp::vec2> center, float taille, cgp::vec2 new_pos);
