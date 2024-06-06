#include "rock.hpp"
#include "environment.hpp"
#include "scene.hpp"
#include <random>

using namespace cgp;

/*
void update_rock(cgp::mesh &rock, cgp::mesh_drawable &rock_visual, perlin_noise_parameters const &parameters)
{
    // mesh rock_ref = mesh_primitive_grid({ -5, -5, 0 }, { 5, -5, 0 }, { 5, 5, 0 }, { -5, 5, 0 }, 100, 100);
    // mesh rock_ref = mesh_primitive_ellipsoid(vec3{ 5, 10, 15 });
    mesh rock_ref = mesh_load_file_obj(project::path + "assets/rocks/rock1.obj");
    resize_rock1(rock_ref, 0.5f);

    for (int k = 0; k < rock.position.size(); k++)
    {

        // Get the vertex position
        cgp::vec3 p = rock.position[k];

        // Generate Perlin noise in three dimensions
        float noise_x = noise_perlin({p.x / 2, p.y / 2, p.z / 2}, parameters.octave, parameters.persistency, parameters.frequency_gain);
        float noise_y = noise_perlin({p.y / 2, p.z / 2, p.x / 2}, parameters.octave, parameters.persistency, parameters.frequency_gain);
        float noise_z = noise_perlin({p.z / 2, p.x / 2, p.y / 2}, parameters.octave, parameters.persistency, parameters.frequency_gain);

        // Adjust the vertex position using Perlin noise
        // p.x = rock_ref.position[k].x + parameters.terrain_height * noise_x * 0.5f;
        // p.y = rock_ref.position[k].y + parameters.terrain_height * noise_y *0.5f;
        p.z = rock_ref.position[k].z + parameters.terrain_height * noise_z * 0.5f;

        // Update the vertex position
        rock.position[k] = p;
    }

    // Update the normals of the mesh to reflect the changes in vertex positions
    rock.normal_update();

    // Update the vertex buffer objects of the mesh drawable for rendering
    rock_visual.vbo_position.update(rock.position);
    rock_visual.vbo_normal.update(rock.normal);
}

void update_rock2(cgp::mesh &rock, cgp::mesh_drawable &rock_visual, perlin_noise_parameters const &parameters)
{
    // mesh rock_ref = mesh_primitive_grid({ -5, -5, 0 }, { 5, -5, 0 }, { 5, 5, 0 }, { -5, 5, 0 }, 100, 100);
    mesh rock_ref = mesh_primitive_ellipsoid(vec3{5, 10, 15});

    for (int k = 0; k < rock.position.size(); k++)
    {

        // Get the vertex position
        cgp::vec3 p = rock.position[k];

        // Generate Perlin noise in three dimensions
        float noise_x = noise_perlin({p.x / 2, p.y / 2, p.z / 2}, parameters.octave, parameters.persistency, parameters.frequency_gain);
        float noise_y = noise_perlin({p.y / 2, p.z / 2, p.x / 2}, parameters.octave, parameters.persistency, parameters.frequency_gain);
        float noise_z = noise_perlin({p.z / 2, p.x / 2, p.y / 2}, parameters.octave, parameters.persistency, parameters.frequency_gain);

        // Adjust the vertex position using Perlin noise
        p.x = rock_ref.position[k].x + parameters.terrain_height * noise_x * 0.5f;
        p.y = rock_ref.position[k].y + parameters.terrain_height * noise_y * 0.5f - 3.0f;
        // p.z = rock_ref.position[k].z + parameters.terrain_height * noise_z * 0.5f;

        // Update the vertex position
        rock.position[k] = p;

        // Update the vertex position
        rock.position[k] = p;
        // rock.color[k] = 0.3f * vec3(0, 0.5f, 0);
    }

    // Update the normals of the mesh to reflect the changes in vertex positions
    rock.normal_update();

    // Update the vertex buffer objects of the mesh drawable for rendering
    rock_visual.vbo_position.update(rock.position);
    rock_visual.vbo_normal.update(rock.normal);
    rock_visual.vbo_color.update(rock.color);
}
*/

void RockData::resize(cgp::mesh& obj, vec3 ratio)
{
    for (int k = 0; k < obj.position.size(); ++k)
    {
        vec3& p = obj.position[k];  // Get a reference to the current vertex position

        p.x *= ratio.x;              // Scale the Y-coordinate of the vertex position
        p.y *= ratio.y;
        p.z *= ratio.z;

        obj.position[k] = p;        // Update the position of the vertex in the mesh (UTILE ...????)
    }
}

void RockData::generate_grass_pos(cgp::mesh& obj, cgp::mesh_drawable& obj_draw) {
    grass_position.clear();
    cgp::numarray<cgp::vec3>& originalVertices = obj.position;

    // Get the scaling factor applied to rock1
    float scalingFactor = obj_draw.model.scaling;

    // Create a vector to store the transformed vertices

    // Apply the scaling transformation to each vertex position
    for (const vec3& vertex : originalVertices)
    {
        // Apply scaling transformation to each vertex position
        vec3 transformedVertex = 5.0f * vertex;

        grass_position.push_back(transformedVertex);
    }
}


/*
void resize_rock1(cgp::mesh &rock, float facteur)
{
    for (int k = 0; k < rock.position.size(); ++k)
    {
        vec3 &p = rock.position[k]; // Get a reference to the current vertex position

        // Scale the Y-coordinate of the vertex position
        p.y *= facteur;
        p.z *= 1.6f;

        // Update the position of the vertex in the mesh
        rock.position[k] = p;
    }
}

void resize_rock2(cgp::mesh &rock, float facteur)
{
    for (int k = 0; k < rock.position.size(); ++k)
    {
        vec3 &p = rock.position[k]; // Get a reference to the current vertex position

        // Scale the Y-coordinate of the vertex position
        p.y *= facteur;
        p.z *= 2.0f;

        // Update the position of the vertex in the mesh
        rock.position[k] = p;
    }
}

void resize_rock3(cgp::mesh &rock, float facteur)
{
    for (int k = 0; k < rock.position.size(); ++k)
    {
        vec3 &p = rock.position[k]; // Get a reference to the current vertex position

        // Scale the Y-coordinate of the vertex position
        p.y *= facteur;
        p.z *= 2.0f;

        // Update the position of the vertex in the mesh
        rock.position[k] = p;
    }
}

void resize_rock4(cgp::mesh &rock, float facteur)
{
    for (int k = 0; k < rock.position.size(); ++k)
    {
        vec3 &p = rock.position[k]; // Get a reference to the current vertex position

        // Scale the Y-coordinate of the vertex position
        p.y *= facteur;
        p.x *= 1.5f;

        // Update the position of the vertex in the mesh
        rock.position[k] = p;
    }
}

void genrate_rocks_type(std::vector<int> &rocks_type)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3); // Utilisez 0 et 3 pour choisir entre 4 options
    for (int i = 0; i < 9; i++)
    {
        for (int k = 0; k < terrain_array[i].size(); k++)
        {
            int choice = dis(gen);
            rocks_type.push_back(choice);
        }
    }
}
*/
