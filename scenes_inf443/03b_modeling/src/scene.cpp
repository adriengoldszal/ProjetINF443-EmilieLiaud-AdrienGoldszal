#include <cstdlib>
#include <random>

#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"
#include "water.hpp"
#include "fish.hpp"
#include "interpolation.hpp"
#include "rock.hpp"

using namespace cgp;

void scene_structure::initialize()
{
	timer.start();
	timer.scale = 1.5f;

	camera_projection.depth_min = 0.0001f;
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 0.0f, 30.0f, 2.0f }, { 0, 0, 0 }, { 0, 0, 1 });

	// General information
	display_info();

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Load terrains
	//
	N_water_samples = 400;
	water_length = 50;
	nb_hollow = 4;
	float depth = -10.0f;

	opengl_shader_structure terrain_shader;
	terrain_shader.load(
		project::path + "shaders/mesh/mesh.vert.glsl",
		project::path + "shaders/mesh/mesh.frag.glsl");

	std::srand(static_cast<unsigned int>(std::time(0)));                    // Seed for randomness

	Cini = 1;
	Rini = 1;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			// Create reference terrain
			terrain_array[i][j].create_terrain_mesh(N_water_samples, water_length, nb_hollow);
			terrain_array[i][j].generate_type_rock(nb_hollow);
			terrain_array[i][j].generate_rock_rotation(nb_hollow);
			terrain_array[i][j].mesh.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg", GL_REPEAT, GL_REPEAT);
			terrain_array[i][j].mesh.shader = terrain_shader;

			// Set the intial terrain layout centered on (Cini, Rini)
			terrain_array[i][j].mesh.model.translation = { water_length * (i - Cini), water_length * (j - Rini), depth };
			for (int k = 0; k < nb_hollow; k++) {
				terrain_array[i][j].hollowCenters[k].x += water_length * (i - Cini);
				terrain_array[i][j].hollowCenters[k].y += water_length * (j - Rini);
			}
		}
	/**/
	std::cout << "Boat position (x,y) : ( " << boat2.model.translation.x << " , " << boat2.model.translation.y << " )" << std::endl; /**/

	// Load boat
	// Open source file https://sketchfab.com/3d-models/chinese-junk-ship-35b340bce9fb4e0680bc0116cebc35c9
	//
	initial_position_rotation = rotation_transform::from_axis_angle({ 0, 0, 1 }, Pi) * rotation_transform::from_axis_angle({ 1, 0, 0 }, Pi / 2);

	opengl_shader_structure boat_shader;
	boat_shader.load(
		project::path + "shaders/mesh/mesh.vert.glsl",
		project::path + "shaders/mesh/mesh.frag.glsl");

	mesh boat_mesh = mesh_load_file_obj(project::path + "assets/junk_low.obj");
	boat2.initialize_data_on_gpu(boat_mesh);

	// boat2.model.scaling = 0.0001f;
	boat2.model.rotation = initial_position_rotation;
	boat2.model.translation = { 0.0f, 0.0f, 1.0f };
	boat2.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/boat.png");
	boat2.shader = boat_shader;
	boat2.material.phong.diffuse = 5.0f;

	/**/
	std::cout << "Boat position (x,y) : ( " << boat2.model.translation.x << " , " << boat2.model.translation.y << " )" << std::endl; /**/

	// Load rocks
	//
	for (int i = 0; i < 4; i++) {
		rock_mesh[i] = mesh_load_file_obj(project::path + "assets/rocks/rock" + str(i + 1) + "_2.obj");
		rock_array[i].resize(rock_mesh[i], resize_ratios[i]);
		rock_array[i].mesh.initialize_data_on_gpu(rock_mesh[i]);
		rock_array[i].mesh.model.scaling = 5.0f;
		rock_array[i].mesh.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rocks/rock_o.png", GL_REPEAT, GL_REPEAT);
		rock_array[i].mesh.shader = terrain_shader;
		rock_array[i].mesh.material.phong.specular = 0.0f; // non-specular rock material
	}

	/*
	rock_mesh2 = mesh_load_file_obj(project::path + "assets/rocks/rock2_2.obj");
	resize_rock2(rock_mesh2, 0.5f);
	rock2.initialize_data_on_gpu(rock_mesh2);

	rock2.model.scaling = 5.0f;
	rock2.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rocks/rock_o.png", GL_REPEAT, GL_REPEAT);
	rock2.shader = terrain_shader;
	rock2.material.phong.specular = 0.0f; // non-specular rock material

	rock_mesh3 = mesh_load_file_obj(project::path + "assets/rocks/rock3_2.obj");
	resize_rock3(rock_mesh3, 0.5f);
	rock3.initialize_data_on_gpu(rock_mesh3);

	rock3.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rocks/rock_o.png", GL_REPEAT, GL_REPEAT);
	rock3.model.scaling = 5.0f;
	rock3.shader = terrain_shader;
	rock3.material.phong.specular = 0.0f;

	rock_mesh4 = mesh_load_file_obj(project::path + "assets/rocks/rock4_2.obj");
	resize_rock4(rock_mesh4, 0.5f);
	rock4.initialize_data_on_gpu(rock_mesh4);

	rock4.model.scaling = 5.0f;
	rock4.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rocks/rock_o.png", GL_REPEAT, GL_REPEAT);
	rock4.shader = terrain_shader;
	rock4.material.phong.specular = 0.0f;

	// generate_rocks_type(rocks_type);
	cgp::numarray<cgp::vec3>& originalVertices = rock_mesh1.position;

	// Get the scaling factor applied to rock1
	float scalingFactor = rock1.model.scaling;

	// Create a vector to store the transformed vertices

	// Apply the scaling transformation to each vertex position
	for (const vec3& vertex : originalVertices) {
		vec3 transformedVertex = scalingFactor * vertex;
		rock1_position.push_back(transformedVertex);
	}
	*/

	// Print or use the transformed vertices
	// for (const vec3& vertex : transformedVertices) {
	//	std::cout << "Transformed vertex position: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
	//}

	// Load grass
	// grass_position = generate_grass_positions_on_terrain(50, terrain_length);
	//
	mesh quad_mesh = mesh_primitive_quadrangle({ -0.25f, 0, 0 }, { 0.25f, 0, 0 }, { 0.25f, 0, 0.5f }, { -0.25f, 0, 0.5f });
	grass.initialize_data_on_gpu(quad_mesh);

	grass.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/grass.png");
	grass.material.phong = { 0.4f, 0.6f, 0, 1 };
}


void scene_structure::display_frame()
{
	timer.update();

	vec3 camera_position = environment.get_camera_position();

	environment.uniform_generic.uniform_float["time"] = timer.t;
	environment.uniform_generic.uniform_float["water_length"] = water_length;

	draw(global_frame, environment);

	int Cmov = (int)(boat2.model.translation.x / water_length + 1.5 + 999) - Cini - 999;
	int Cshift = (999 + Cini - Cmov) % 3;

	int Rmov = (int)(boat2.model.translation.y / water_length + 1.5 + 999) - Rini - 999;
	int Rshift = (999 + Rini - Rmov) % 3;

	// Recycle terrains whenever it is required
	if (Cmov) {
		std::cout << std::endl;
		std::cout << "Cini Cmov " << Cini << " " << Cmov << std::endl;
		std::cout << "Boat X: " << boat2.model.translation.x << " Y: " << boat2.model.translation.y << std::endl;
		for (int j = 0; j < 3; j++) {
			//std::cout << "Terrain (Cshift, j): (" << Cshift << "," << j << ")" << std::endl;
			//std::cout << "     BEF X: " << terrain_array[Cshift][j].mesh.model.translation.x << " Y: " << terrain_array[Cshift][j].mesh.model.translation.y << std::endl;
			terrain_array[Cshift][j].mesh.model.translation.x += 3 * water_length * Cmov;
			//std::cout << "     AFT X: " << terrain_array[Cshift][j].mesh.model.translation.x << " Y: " << terrain_array[Cshift][j].mesh.model.translation.y << std::endl;
			for (int k = 0; k < nb_hollow; k++) terrain_array[Cshift][j].hollowCenters[k].x += 3 * water_length * Cmov;
		}
		Cini += Cmov;
		std::cout << "NEW Cini " << Cini << std::endl;
	}
	if (Rmov) {
		std::cout << std::endl;
		std::cout << "Rini Rmov " << Rini << " " << Rmov << std::endl;
		std::cout << "Boat X: " << boat2.model.translation.x << " Y: " << boat2.model.translation.y << std::endl;
		for (int i = 0; i < 3; i++) {
			//std::cout << "Terrain (i, Rshift): (" << i << ", " << Rshift << ")"  << std::endl;
			//std::cout << "     BEF X: " << terrain_array[i][Rshift].mesh.model.translation.x << " Y: " << terrain_array[i][Rshift].mesh.model.translation.y << std::endl;
			terrain_array[i][Rshift].mesh.model.translation.y += 3 * water_length * Rmov;
			//std::cout << "     AFT X: " << terrain_array[i][Rshift].mesh.model.translation.x << " Y: " << terrain_array[i][Rshift].mesh.model.translation.y << std::endl;
			for (int k = 0; k < nb_hollow; k++) terrain_array[i][Rshift].hollowCenters[k].y += 3 * water_length * Rmov;
		}
		Rini += Rmov;
		std::cout << "NEW Rini " << Rini << std::endl;
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			draw(terrain_array[i][j].mesh, environment);
			for (int k = 0; k < nb_hollow; k++) {
				int rock_type = terrain_array[i][j].type_rock[k];
				rock_array[rock_type].mesh.model.translation = vec3{ terrain_array[i][j].hollowCenters[k].x, terrain_array[i][j].hollowCenters[k].y, -2.0f };
				rock_array[rock_type].mesh.model.rotation = rotation_transform::from_axis_angle({ 0, 0, 1 }, terrain_array[i][j].rock_rotation[k]);
				draw(rock_array[rock_type].mesh, environment);
			}
		}

	// draw(rock_drawable, environment);
	// draw(rock_drawable2, environment);
	// draw(hierarchy, environment);

	// if (gui.display_wireframe)
	// draw_wireframe(rock_drawable, environment);

	draw(boat2, environment);
	display_semiTransparent(); // Display water and terrain as semi transparent for underwater effect

	boat2.model.rotation = rotation_transform::from_axis_angle({ 0, 1, 0 }, 0.08f * sin(timer.t)) * rotation_transform::from_axis_angle({ 1, 0, 0 }, 0.2f * sin(timer.t)) * initial_position_rotation;
	boat2.model.scaling = 0.01f; // Ne marche plus correctement;


	// Detect collisions
	const float collisionThreshold = 5.0f;
	const float moveback = 1.0f;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < nb_hollow; k++) {
				float rockX = terrain_array[i][j].hollowCenters[k].x;
				float rockY = terrain_array[i][j].hollowCenters[k].y;

				// std::cout << "Boat Position: X" << boat2.model.translation.x << " Y: " << boat2.model.translation.y << " Z: " << boat2.model.translation.z << std::endl;

				// Calculate the distance between the boat and the rock
				float distX = boat2.model.translation.x - rockX;
				float distY = boat2.model.translation.y - rockY;
				float distanceSquared = distX * distX + distY * distY;

				if (distanceSquared < collisionThreshold * collisionThreshold)
				{
					// std::cout << "Collision detected" << std::endl;

					// Apply a simple correction by moving the boat away from the rock
					if (std::abs(distX) > std::abs(distY))
					{
						if (distX > 0)
						{
							boat2.model.translation.x += moveback;
						}
						else
						{
							boat2.model.translation.x -= moveback;
						}
					}
					else
					{
						if (distY > 0)
						{
							boat2.model.translation.y += moveback;
						}
						else
						{
							boat2.model.translation.y -= moveback;
						}
					}
				}
			}
		}
	}

}

void scene_structure::display_semiTransparent()
{
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	bool update = false;
	/*update |= ImGui::SliderFloat("Persistance", &parameters.persistency, 0.1f, 0.6f);
	update |= ImGui::SliderFloat("Frequency gain", &parameters.frequency_gain, 0.5f, 1.5f);
	update |= ImGui::SliderInt("Octave", &parameters.octave, 1, 8);
	update |= ImGui::SliderFloat("Height", &parameters.terrain_height, 0.1f, 1.5f);

	if (update) {// if any slider has been changed - then update the terrain
		//update_rock(rock_mesh1, rock1, parameters);
		//update_rock2(rock_mesh2, rock_drawable2, parameters);
	*/
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	// Fixing camera to move with the boat if wanted
	// Attention {y, z, x}
	vec3 camera_position_on_boat = { 0.0f, 5.0f, 20.0f };
	vec3 camera_position_world = boat2.model.rotation * camera_position_on_boat + boat2.model.translation;
	// camera_control.camera_model.position_camera = camera_position_world;
	vec3 point_to_see = { boat2.model.translation.x, boat2.model.translation.y, boat2.model.translation.z + 5.0f };
	camera_control.camera_model.look_at(camera_position_world, point_to_see);

	camera_control.action_keyboard(environment.camera_view);
	if (inputs.keyboard.is_pressed(GLFW_KEY_A))
	{
		vec3 translation_in_boat_coords = { -0.2f, 0.0f, 0.0f };
		// Changing to world coordinates by multiplying by the rotation matrix
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat2.model.translation = { boat2.model.translation.x + translation.x, boat2.model.translation.y + translation.y, boat2.model.translation.z + translation.z };
		initial_position_rotation = rotation_transform::from_axis_angle({ 0, 0, 1 }, Pi / 100.0) * initial_position_rotation;
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_S))
	{
		vec3 translation_in_boat_coords = { 0.0f, 0.0f, 0.2f };
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat2.model.translation = { boat2.model.translation.x + translation.x, boat2.model.translation.y + translation.y, boat2.model.translation.z + translation.z };
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_W))
	{
		vec3 translation_in_boat_coords = { 0.0f, 0.0f, -0.2f };
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat2.model.translation = { boat2.model.translation.x + translation.x, boat2.model.translation.y + translation.y, boat2.model.translation.z + translation.z };
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_D))
	{
		vec3 translation_in_boat_coords = { 0.2f, 0.0f, 0.0f };
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat2.model.translation = { boat2.model.translation.x + translation.x, boat2.model.translation.y + translation.y, boat2.model.translation.z + translation.z };
		initial_position_rotation = rotation_transform::from_axis_angle({ 0, 0, 1 }, -Pi / 100.0) * initial_position_rotation;
	}

	camera_control.action_keyboard(environment.camera_view);
}

void scene_structure::idle_frame()
{

	camera_control.idle_frame(environment.camera_view);
}

void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}
