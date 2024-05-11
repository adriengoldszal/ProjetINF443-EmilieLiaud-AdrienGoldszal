#include "rock.hpp"

using namespace cgp;

void update_rock(cgp::mesh& rock, cgp::mesh_drawable& rock_visual, perlin_noise_parameters const& parameters) {
    //mesh rock_ref = mesh_primitive_grid({ -5, -5, 0 }, { 5, -5, 0 }, { 5, 5, 0 }, { -5, 5, 0 }, 100, 100);
    mesh rock_ref = mesh_primitive_ellipsoid(vec3{ 5, 10, 15 });
    
    for (int k = 0; k < rock.position.size(); k++) {


        // Get the vertex position
        cgp::vec3 p = rock.position[k];

        // Generate Perlin noise in three dimensions
        float noise_x = noise_perlin({ p.x /2 , p.y/2, p.z/2 }, parameters.octave, parameters.persistency, parameters.frequency_gain);
        float noise_y = noise_perlin({ p.y/2, p.z/2, p.x/2 }, parameters.octave, parameters.persistency, parameters.frequency_gain);
        float noise_z = noise_perlin({ p.z / 2, p.x/2, p.y/2 }, parameters.octave, parameters.persistency, parameters.frequency_gain);

        // Adjust the vertex position using Perlin noise
        p.x = rock_ref.position[k].x + parameters.terrain_height * noise_x*0.5f;
        p.y = rock_ref.position[k].y + parameters.terrain_height * noise_y *0.5f;
        //p.z = rock_ref.position[k].z + parameters.terrain_height * noise_z * 0.5f;

        // Update the vertex position
        rock.position[k] = p;
    }


    // Update the normals of the mesh to reflect the changes in vertex positions
    rock.normal_update();

    // Update the vertex buffer objects of the mesh drawable for rendering
    rock_visual.vbo_position.update(rock.position);
    rock_visual.vbo_normal.update(rock.normal);

}

void update_rock2(cgp::mesh& rock, cgp::mesh_drawable& rock_visual, perlin_noise_parameters const& parameters) {
    //mesh rock_ref = mesh_primitive_grid({ -5, -5, 0 }, { 5, -5, 0 }, { 5, 5, 0 }, { -5, 5, 0 }, 100, 100);
    mesh rock_ref = mesh_primitive_ellipsoid(vec3{ 5, 10, 15 });

    for (int k = 0; k < rock.position.size(); k++) {


        // Get the vertex position
        cgp::vec3 p = rock.position[k];

        // Generate Perlin noise in three dimensions
        float noise_x = noise_perlin({ p.x / 2 , p.y / 2, p.z / 2 }, parameters.octave, parameters.persistency, parameters.frequency_gain);
        float noise_y = noise_perlin({ p.y / 2, p.z / 2, p.x / 2 }, parameters.octave, parameters.persistency, parameters.frequency_gain);
        float noise_z = noise_perlin({ p.z / 2, p.x / 2, p.y / 2 }, parameters.octave, parameters.persistency, parameters.frequency_gain);

        // Adjust the vertex position using Perlin noise
        p.x = rock_ref.position[k].x + parameters.terrain_height * noise_x * 0.5f;
        p.y = rock_ref.position[k].y + parameters.terrain_height * noise_y * 0.5f - 3.0f;
        //p.z = rock_ref.position[k].z + parameters.terrain_height * noise_z * 0.5f;

        // Update the vertex position
        rock.position[k] = p;

        // Update the vertex position
        rock.position[k] = p;
        //rock.color[k] = 0.3f * vec3(0, 0.5f, 0);

    }

    // Update the normals of the mesh to reflect the changes in vertex positions
    rock.normal_update();

    // Update the vertex buffer objects of the mesh drawable for rendering
    rock_visual.vbo_position.update(rock.position);
    rock_visual.vbo_normal.update(rock.normal);
    rock_visual.vbo_color.update(rock.color);
}