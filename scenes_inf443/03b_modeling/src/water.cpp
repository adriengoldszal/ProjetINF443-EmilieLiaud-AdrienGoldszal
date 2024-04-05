#include "water.hpp"
#include <list>
#include <random>

using namespace cgp;

mesh create_water_mesh(int N, float length)
{
	float height = 0.3f;
	mesh water = mesh_primitive_grid(vec3{-length/2,-length/2,height},vec3{length/2,-length/2,height}, vec3{length/2, length/2,height}, vec3{-length/2,length/2,height}, N, N);
	return water;

}