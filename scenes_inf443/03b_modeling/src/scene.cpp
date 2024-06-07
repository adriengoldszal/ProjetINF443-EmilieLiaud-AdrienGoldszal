#include <cstdlib>
#include <random>

#include "scene.hpp"

#include "terrain.hpp"
#include "water.hpp"
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
	camera_control.look_at({0.0f, 30.0f, 2.0f}, {0, 0, 0}, {0, 0, 1});

	// General information
	display_info();

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Load skybox
	// ***************************************** //
	image_structure image_skybox_template = image_load_file("assets/skybox/hdr_01.png"); // hdr_01.png OR skybox_01.jpg
	std::vector<image_structure> image_grid = image_split_grid(image_skybox_template, 4, 3);
	skybox.initialize_data_on_gpu();
	skybox.texture.initialize_cubemap_on_gpu(
		image_grid[1].mirror_vertical().rotate_90_degrees_counterclockwise(),
		image_grid[7].mirror_vertical().rotate_90_degrees_clockwise(),
		image_grid[10].mirror_horizontal(),
		image_grid[4].mirror_vertical(),
		image_grid[5].mirror_horizontal(),
		image_grid[3].mirror_vertical());
	skybox.shader.load(
		project::path + "shaders/skybox/skybox.vert.glsl",
		project::path + "shaders/skybox/skybox.frag.glsl");

	environment.background_color = {0.0f, 1.0f, 1.0f};

	// Load Terrain & Water Terrain
	// ***************************************** //
	N_water_samples = 500;
	water_length = 100;
	nb_hollow = 3;
	float depth = -10.0f;

	opengl_shader_structure terrain_shader;
	terrain_shader.load(
		project::path + "shaders/mesh/mesh.vert.glsl",
		project::path + "shaders/mesh/mesh.frag.glsl");

	opengl_shader_structure water_shader;
	water_shader.load(
		project::path + "shaders/water/water.vert.glsl",
		project::path + "shaders/water/water.frag.glsl");

	std::srand(static_cast<unsigned int>(std::time(0))); // Seed for randomness

	Cini = 1;
	Rini = 1;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// Create reference terrain
			terrain_array[i][j].create_terrain_mesh(N_water_samples, water_length, nb_hollow);
			terrain_array[i][j].generate_type_rock(nb_hollow);
			terrain_array[i][j].generate_rock_rotation(nb_hollow);
			terrain_array[i][j].generate_houses(nb_hollow);
			terrain_array[i][j].mesh.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg", GL_REPEAT, GL_REPEAT);
			terrain_array[i][j].mesh.shader = terrain_shader;

			water_array[i][j].initialize_data_on_gpu(create_water_mesh(N_water_samples, water_length));
			water_array[i][j].shader = water_shader;
			water_array[i][j].material.color = {0.0f, 0.5f, 1.0f}; // blue color for water
			water_array[i][j].material.phong.specular = 0.0f;		// non-specular terrain material

			// Sending the skybox texture to the water shader as a uniform
			glUseProgram(water_array[i][j].shader.id);
			opengl_check;
			glActiveTexture(GL_TEXTURE1);
			opengl_check;
			skybox.texture.bind();
			opengl_uniform(water_array[i][j].shader, "image_skybox", 1);
			opengl_check;

			// Set the intial terrain layout centered on (Cini, Rini)
			terrain_array[i][j].mesh.model.translation = {water_length * (i - Cini), water_length * (j - Rini), depth};
			water_array[i][j].model.translation = {water_length * (i - Cini), water_length * (j - Rini), 0};

			for (int k = 0; k < nb_hollow; k++)
			{
				terrain_array[i][j].hollowCenters[k].x += water_length * (i - Cini);
				terrain_array[i][j].hollowCenters[k].y += water_length * (j - Rini);
			}
		}
	}

	// Load boat
	// ***************************************** //
	// Open source file https://sketchfab.com/3d-models/chinese-junk-ship-35b340bce9fb4e0680bc0116cebc35c9
	mesh boat_mesh = mesh_load_file_obj(project::path + "assets/boat.obj");
	boat.initialize_data_on_gpu(boat_mesh);
	boat.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/boat.png");
	opengl_shader_structure boat_shader;
	boat_shader.load(
		project::path + "shaders/mesh/mesh.vert.glsl",
		project::path + "shaders/mesh/mesh.frag.glsl");

	// boat.model.scaling = 0.0001f;
	boat.model.translation = {0.0f, 0.0f, 1.0f};
	initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, Pi) * rotation_transform::from_axis_angle({1, 0, 0}, Pi / 2);
	boat.model.rotation = initial_position_rotation;
	boat.shader = boat_shader;
	boat.material.phong.diffuse = 1.0f;

	// Load fish
	// ***************************************** //
	// Open source file https://sketchfab.com/3d-models/flying-fish-tobiuo-77e1a00a725148a1b4601b7482e60e30

	mesh fish_mesh = mesh_load_file_obj(project::path + "assets/fish/20230116_Tobiuo.obj");
	for (int i = 0; i < 2; i++) {
		fish[i].initialize_data_on_gpu(fish_mesh);
		fish[i].texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/fish/Body_Normal.png");
		fish[i].texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/fish/Winf3_Normal.png");
		fish[i].texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/fish/Wing_Normal.png");
		fish[i].model.rotation = rotation_transform::from_axis_angle({0, 0, 1}, Pi) * rotation_transform::from_axis_angle({1, 0, 0}, Pi / 2);
		fish[i].model.scaling = 0.1f;
	}

	// Definition of the initial data
	fish_timer = 0.0f;

	// Update the current time
	// Adjusted fish positions to the boat referential, is getting multiplied by rotation matrix later on for correction and updated positions
	initial_fish_positions =
		{{0, 0.3, 20.0},
		 {0, 0.3, 15.0},
		 {0, 0.3, 5.0},
		 {0, 0.3, -5.0},
		 {0, 0.3, -10.0},
		 {0, -10.0, -15.0},
		 {0, -10.0, -20.0}};

	fish_positions = initial_fish_positions;
	fish_positions2 = initial_fish_positions;

	// Key times (time at which the position must pass in the corresponding position)
	fish_times =
		{0.0f,
		 1.0f,
		 2.0f,
		 3.0f,
		 4.0f,
		 5.0f,
		 8.0f};

	int N = fish_positions.size();
	fish_interval.t_min = fish_times[1];
	fish_interval.t_max = fish_times[N - 2];
	fish_interval.t = fish_interval.t_min;

	// Load rocks
	//  ***************************************** //
	for (int i = 0; i < 4; i++)
	{
		rock_mesh[i] = mesh_load_file_obj(project::path + "assets/rocks/rock" + str(i + 1) + "_3.obj");
		rock_array[i].resize(rock_mesh[i], resize_ratios[i]);
		rock_array[i].mesh.initialize_data_on_gpu(rock_mesh[i]);
		//rock_array[i].mesh.model.scaling = 5.0f;
		rock_array[i].mesh.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rocks/rock" + str(i + 1) +".png", GL_REPEAT, GL_REPEAT);
		rock_array[i].mesh.shader = terrain_shader;
		rock_array[i].mesh.material.phong.specular = 0.0f; // non-specular rock material
	}

	// Load house
	// Link to open source file : https://www.cgtrader.com/items/4637728/download-page
	// ***************************************** //
	mesh house_mesh = mesh_load_file_obj(project::path + "assets/thaihouse/thaihouse.obj");
	house.initialize_data_on_gpu(house_mesh);
	house.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/thaihouse/Texture/Door_Window/Door_Window_BaseColor.png");
	house.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/thaihouse/Texture/stairs/stairs_BaseColor.png");
	house.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/thaihouse/Texture/structure/structure_BaseColor.png");
	house.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/thaihouse/Texture/wooden_planks/wooden planks_BaseColor.png");
	
	house.model.scaling = 0.1f;
	house.model.translation = {0, 0, 5.0f};
	house_initial_rotation = rotation_transform::from_axis_angle({ 0, 0, 1 }, Pi) * rotation_transform::from_axis_angle({ 1, 0, 0 }, Pi / 2);
	house.model.rotation = house_initial_rotation;
	house_position.clear();
}

void scene_structure::display_frame()
{
	timer.update();

	skybox.model.rotation = rotation_transform::from_axis_angle({0, 0, 1}, 0.01f * timer.t);

	vec3 camera_position = environment.get_camera_position();

	glDepthMask(GL_FALSE); // disable depth-buffer writing
	draw(skybox, environment);
	glDepthMask(GL_TRUE); // re-activate depth-buffer write

	environment.uniform_generic.uniform_float["time"] = timer.t;
	environment.uniform_generic.uniform_float["water_length"] = water_length;

	// Update light position & fog parameters
	environment.light_position = {boat.model.translation.x, boat.model.translation.y, 10.0f};

	draw(global_frame, environment);

	// Draw Terrains & Rocks & Houses
	//  ***************************************** //
	int Cmov = (int)(boat.model.translation.x / water_length + 1.5 + 999) - Cini - 999;
	int Cshift = (999 + Cini - Cmov) % 3; //making sure that (999 + Cini - Cmov) > 0

	int Rmov = (int)(boat.model.translation.y / water_length + 1.5 + 999) - Rini - 999;
	int Rshift = (999 + Rini - Rmov) % 3;

	// Recycle terrains whenever it is required
	if (Cmov)
	{
		for (int j = 0; j < 3; j++)
		{
			terrain_array[Cshift][j].mesh.model.translation.x += 3 * water_length * Cmov;
			water_array[Cshift][j].model.translation.x += 3 * water_length * Cmov;
			for (int k = 0; k < nb_hollow; k++)
				terrain_array[Cshift][j].hollowCenters[k].x += 3 * water_length * Cmov;
		}
		Cini += Cmov;
	}
	if (Rmov)
	{
		for (int i = 0; i < 3; i++)
		{
			terrain_array[i][Rshift].mesh.model.translation.y += 3 * water_length * Rmov;
			water_array[i][Rshift].model.translation.y += 3 * water_length * Rmov;
			for (int k = 0; k < nb_hollow; k++)
				terrain_array[i][Rshift].hollowCenters[k].y += 3 * water_length * Rmov;
		}
		Rini += Rmov;
	}

	house_position.clear();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{

			draw(terrain_array[i][j].mesh, environment);
			draw(water_array[i][j], environment);
			for (int k = 0; k < nb_hollow; k++)
			{
				int rock_type = terrain_array[i][j].type_rock[k];
				rock_array[rock_type].mesh.model.translation = vec3{terrain_array[i][j].hollowCenters[k].x, terrain_array[i][j].hollowCenters[k].y, 5.0f};
				rock_array[rock_type].mesh.model.rotation = rotation_transform::from_axis_angle({0, 0, 1}, terrain_array[i][j].rock_rotation[k]);
				draw(rock_array[rock_type].mesh, environment);

				for (int l = 0; l < terrain_array[i][j].nb_houses[k]; l++)
				{
					vec3 new_pos = vec3{ terrain_array[i][j].hollowCenters[k].x + (l + 1) * 10.0f, terrain_array[i][j].hollowCenters[k].y + (l + 1) * 10.0f, -1.0f };
					house.model.translation = new_pos;
					house_position.push_back(new_pos);
					house.model.rotation = rotation_transform::from_axis_angle({0, 0, 1}, l * 15.0f) * house_initial_rotation;
					draw(house, environment);
					house.model.rotation = house_initial_rotation;
				}
			}
		}
	}

	// Draw Boat
	//  ***************************************** //
	draw(boat, environment);
	display_semiTransparent(); // Display water and terrain as semi transparent for underwater effect

	boat.model.rotation = rotation_transform::from_axis_angle({0, 1, 0}, 0.05f * sin(timer.t)) * rotation_transform::from_axis_angle({1, 0, 0}, 0.2f * sin(timer.t)) * initial_position_rotation;
	boat.model.scaling = 0.01f; // Ne marche plus correctement;


	// Draw Fish
	//  ***************************************** //
	// Update the fish time
	fish_interval.update();
	float t = fish_interval.t;

	float elapsed_time = timer.t - fish_timer;

	if (elapsed_time > 10.0f)
	{
		// std::cout << "Updating fish positions" << std::endl;

		for (int i = 0; i < fish_positions.size(); i++)
		{
			vec3 translation = {boat.model.translation.x + 3.0f, boat.model.translation.y, boat.model.translation.z};
			vec3 translation2 = {boat.model.translation.x + 5.0f, boat.model.translation.y, boat.model.translation.z};
			fish_positions[i] = boat.model.rotation * initial_fish_positions[i] + translation;
			fish_positions2[i] = boat.model.rotation * initial_fish_positions[i] + translation2;

			fish[0].model.rotation = boat.model.rotation;
			fish[1].model.rotation = boat.model.rotation;
		}

		// Update fish_timer after updating positions
		fish_timer = timer.t;
		// std::cout << "Fish Timer updated : " << fish_timer << std::endl;
	}

	// Compute the interpolated position
	vec3 p = interpolation(t, fish_positions, fish_times);
	vec3 p2 = interpolation(t, fish_positions2, fish_times);

	fish[0].model.translation = p;
	fish[1].model.translation = p2;

	draw(fish[0], environment);
	draw(fish[1], environment);


	// Detect collisions
	//  ***************************************** //
	const float collisionThreshold = 15.5f;
	const float HousecollisionThreshold = 7.0f;
	const float moveback = 1.0f;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < nb_hollow; k++)
			{
				float rockX = terrain_array[i][j].hollowCenters[k].x;
				float rockY = terrain_array[i][j].hollowCenters[k].y;

				// Calculate the distance between the boat and the rock
				float distX = boat.model.translation.x - rockX;
				float distY = boat.model.translation.y - rockY;
				float distanceSquared = distX * distX + distY * distY;

				if (distanceSquared < collisionThreshold * collisionThreshold)
				{
					// Apply a simple correction by moving the boat away from the rock
					if (std::abs(distX) > std::abs(distY))
					{
						if (distX > 0)
						{
							boat.model.translation.x += moveback;
						}
						else
						{
							boat.model.translation.x -= moveback;
						}
					}
					else
					{
						if (distY > 0)
						{
							boat.model.translation.y += moveback;
						}
						else
						{
							boat.model.translation.y -= moveback;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < house_position.size(); i++)
	{
		float houseX = house_position[i].x;
		float houseY = house_position[i].y;

		// Calculate the distance between the boat and the rock
		float distX = boat.model.translation.x - houseX;
		float distY = boat.model.translation.y - houseY;
		float distanceSquared = distX * distX + distY * distY;

		if (distanceSquared < HousecollisionThreshold * HousecollisionThreshold)
		{
			// Apply a simple correction by moving the boat away from the rock
			if (std::abs(distX) > std::abs(distY))
			{
				if (distX > 0)
				{
					boat.model.translation.x += moveback;
				}
				else
				{
					boat.model.translation.x -= moveback;
				}
			}
			else
			{
				if (distY > 0)
				{
					boat.model.translation.y += moveback;
				}
				else
				{
					boat.model.translation.y -= moveback;
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
	vec3 camera_position_on_boat = {0.0f, 5.0f, 20.0f};
	vec3 camera_position_world = boat.model.rotation * camera_position_on_boat + boat.model.translation;
	// camera_control.camera_model.position_camera = camera_position_world;
	vec3 point_to_see = {boat.model.translation.x, boat.model.translation.y, boat.model.translation.z + 5.0f};
	camera_control.camera_model.look_at(camera_position_world, point_to_see);

	camera_control.action_keyboard(environment.camera_view);
	if (inputs.keyboard.is_pressed(GLFW_KEY_A))
	{

		vec3 translation_in_boat_coords = {-0.2f, 0.0f, 0.0f};
		// Changing to world coordinates by multiplying by the rotation matrix
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat.model.translation = {boat.model.translation.x + translation.x, boat.model.translation.y + translation.y, boat.model.translation.z + translation.z};
		initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, Pi / 100.0) * initial_position_rotation;
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_S))
	{
		vec3 translation_in_boat_coords = {0.0f, 0.0f, 0.2f};
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat.model.translation = {boat.model.translation.x + translation.x, boat.model.translation.y + translation.y, boat.model.translation.z + translation.z};
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_W))
	{
		vec3 translation_in_boat_coords = {0.0f, 0.0f, -0.2f};
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat.model.translation = {boat.model.translation.x + translation.x, boat.model.translation.y + translation.y, boat.model.translation.z + translation.z};
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_D))
	{
		vec3 translation_in_boat_coords = {0.2f, 0.0f, 0.0f};
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat.model.translation = {boat.model.translation.x + translation.x, boat.model.translation.y + translation.y, boat.model.translation.z + translation.z};
		initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, -Pi / 100.0) * initial_position_rotation;
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
	std::cout << "-----------------------------------------------\n"
			  << std::endl;
}
