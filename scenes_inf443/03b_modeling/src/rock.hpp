#pragma once

#include "cgp/cgp.hpp"

struct RockData
{
public:
	cgp::mesh_drawable mesh;

	void RockData::resize(cgp::mesh& obj, cgp::vec3 ratio);
};
