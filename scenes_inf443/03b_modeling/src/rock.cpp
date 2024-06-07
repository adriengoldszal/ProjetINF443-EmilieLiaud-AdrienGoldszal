#include "rock.hpp"
#include "environment.hpp"
#include "scene.hpp"
#include <random>

using namespace cgp;

void RockData::resize(cgp::mesh& obj, vec3 ratio)
{
    for (int k = 0; k < obj.position.size(); ++k)
    {
        vec3& p = obj.position[k];  // Get a reference to the current vertex position

        p.x *= ratio.x;              // Scale the Y-coordinate of the vertex position
        p.y *= ratio.y;
        p.z *= ratio.z;

        obj.position[k] = p;        // Update the position of the vertex in the mesh
    }
}