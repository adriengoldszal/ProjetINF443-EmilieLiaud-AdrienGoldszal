
#include "water.hpp"
#include <list>
#include <random>

using namespace cgp;

mesh create_water_mesh(int N, float terrain_length)
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
			// float z = evaluate_terrain_height(x, y);

			// Store vertex coordinates
			terrain.position[kv + N * ku] = {x, y, 0.0f};
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
