#pragma once

#include "cgp/cgp.hpp"


struct perlin_noise_parameters
{
	float persistency = 0.443;
	float frequency_gain = 1.237f;
	int octave = 8;
	float terrain_height = 1.413f;
};

// Recompute the vertices of the terrain everytime a parameter is modified
//  and update the mesh_drawable accordingly
void update_rock(cgp::mesh& rock, cgp::mesh_drawable& rock_visual, perlin_noise_parameters const& parameters);

void update_rock2(cgp::mesh& rock, cgp::mesh_drawable& rock_visual, perlin_noise_parameters const& parameters);

