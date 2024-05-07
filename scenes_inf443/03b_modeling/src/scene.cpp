#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"
#include "water.hpp"
#include "fish.hpp"

using namespace cgp;

static void deform_terrain(mesh &m);

void scene_structure::initialize()
{
	timer.start();
	timer.scale = 1.5f;

	camera_projection.depth_min = 0.0001f;
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({15.0f, 6.0f, 6.0f}, {0, 0, 0}); // camera_control.look_at({1.0f, 0.0f, 0.0f}, {0, 0, 0}); pour le sol

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

	// Creating an island for testing purposes
	float L = 5.0f;
	float elevation = -30.0f; // Adjust this value to set the elevation
	mesh terrain_mesh = mesh_primitive_grid({-L, -L, elevation}, {L, -L, elevation}, {L, L, elevation}, {-L, L, elevation}, 100, 100);
	deform_terrain(terrain_mesh);
	terrain.initialize_data_on_gpu(terrain_mesh);
	terrain.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg");
	terrain.material.alpha = 0.3f;

	// Load water terrain
	int N_terrain_samples = 40;
	float terrain_length = 100;
	mesh const water_mesh = create_water_mesh(N_terrain_samples, terrain_length);
	water.initialize_data_on_gpu(water_mesh);
	opengl_shader_structure water_shader;
	water_shader.load(
		project::path + "shaders/water/water.vert.glsl",
		project::path + "shaders/water/water.frag.glsl");
	water.shader = water_shader;
	water.material.color = {0.0f, 0.5f, 1.0f}; // blue color for water
	water.material.phong.specular = 0.0f;	   // non-specular terrain material

	// Sending the skybox texture to the water shader as a uniform
	glUseProgram(water.shader.id);
	opengl_check;
	glActiveTexture(GL_TEXTURE1);
	opengl_check;
	skybox.texture.bind();
	opengl_uniform(water.shader, "image_skybox", 1);
	opengl_check;

	// Load boat
	mesh boat_mesh = mesh_load_file_obj(project::path + "assets/junk_low.obj");
	boat2.initialize_data_on_gpu(boat_mesh);
	boat2.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/boat.png");
	// boat2.model.scaling = 0.0001f;
	boat2.model.translation = {-1, 1, 0.5f};
	initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, Pi) * rotation_transform::from_axis_angle({1, 0, 0}, Pi / 2);
	boat2.model.rotation = initial_position_rotation;
	// Load fish
	initialize_fish(hierarchy);
}
// deform terrain function for island
static void deform_terrain(mesh &m)
{
	// Set the terrain to have a gaussian shape
	for (int k = 0; k < m.position.size(); ++k)
	{
		vec3 &p = m.position[k];
		float d2 = p.x * p.x + p.y * p.y;
		float z = exp(-d2 / 4) - 1;

		z = z + 0.05f * noise_perlin({p.x, p.y});

		p = {p.x, p.y, z};
	}

	m.normal_update();
}

void scene_structure::display_frame()
{
	timer.update();

	vec3 camera_position = environment.get_camera_position(); // Get camera position for boat moving

	environment.uniform_generic.uniform_float["time"] = timer.t;

	// environment.light_position = camera_control.camera_model.position();

	if (gui.display_frame)
		draw(global_frame, environment);

	glDepthMask(GL_FALSE); // disable depth-buffer writing
	draw(skybox, environment);
	glDepthMask(GL_TRUE); // re-activate depth-buffer write

	draw(terrain, environment);

	display_semiTransparent(); // Display water and terrain as semi transparent for underwater effect

	// draw(water, environment);
	// draw(terrain, environment);

	// Draw fish
	// draw(hierarchy, environment);
	boat2.model.translation = {camera_position.x, camera_position.y - 10.0f, camera_position.z - 10.0f};
	boat2.model.rotation = rotation_transform::from_axis_angle({0, 1, 0}, 0.5f * sin(timer.t)) * rotation_transform::from_axis_angle({1, 0, 0}, 0.2f * sin(timer.t)) * initial_position_rotation;
	boat2.model.scaling = 0.01f; // Ne marche plus correctement;
	draw(boat2, environment);
}

void scene_structure::display_semiTransparent()
{
	// Enable use of alpha component as color blending for transparent elements
	//  alpha = current_color.alpha
	//  new color = previous_color * alpha + current_color * (1-alpha)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);

	draw(water, environment);

	// Don't forget to re-activate the depth-buffer write
	glDepthMask(true);
	glDisable(GL_BLEND);
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
	camera_control.action_keyboard(environment.camera_view);
	if (inputs.keyboard.is_pressed(GLFW_KEY_D))
	{
		initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, Pi / 100.0) * initial_position_rotation;
		boat2.model.translation = {boat2.model.translation.x + 0.1f, boat2.model.translation.y, boat2.model.translation.z};
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_A))
	{
		initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, -Pi / 100.0) * initial_position_rotation;
		boat2.model.translation = {boat2.model.translation.x, boat2.model.translation.y + 0.1f, boat2.model.translation.z};
	}
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
