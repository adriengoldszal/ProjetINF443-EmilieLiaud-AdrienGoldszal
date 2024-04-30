#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"
#include "water.hpp"

using namespace cgp;

static void deform_terrain(mesh &m);

void scene_structure::initialize()
{
	timer.start();
	timer.scale = 1.5f;

	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({15.0f, 6.0f, 6.0f}, {0, 0, 0});

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

	// Creating an island
	float L = 5.0f;
	float elevation = 0.0f; // Adjust this value to set the elevation
	mesh terrain_mesh = mesh_primitive_grid({-L, -L, elevation}, {L, -L, elevation}, {L, L, elevation}, {-L, L, elevation}, 100, 100);
	deform_terrain(terrain_mesh);
	terrain.initialize_data_on_gpu(terrain_mesh);
	terrain.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg");

	// Load water terrain
	int N_terrain_samples = 100;
	float terrain_length = 20;
	mesh const water_mesh = create_water_mesh(N_terrain_samples, terrain_length);
	water.initialize_data_on_gpu(water_mesh);
	opengl_shader_structure water_shader;
	water_shader.load(
		project::path + "shaders/water/water.vert.glsl",
		project::path + "shaders/water/water.frag.glsl");
	water.shader = water_shader;
	water.material.color = {0.0f, 0.5f, 1.0f}; // blue color for water
	water.material.phong.specular = 0.0f;	   // non-specular terrain material
											   // water.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/texture_grass.jpg", GL_REPEAT, GL_REPEAT);

	// Sending the skybox texture to the water shader as a uniform
	glUseProgram(water.shader.id);
	opengl_check;
	glActiveTexture(GL_TEXTURE1);
	opengl_check;
	skybox.texture.bind();
	opengl_uniform(water.shader, "image_skybox", 1);
	opengl_check;
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
	environment.uniform_generic.uniform_float["time"] = timer.t;

	environment.light_position = camera_control.camera_model.position();

	if (gui.display_frame)
		draw(global_frame, environment);

	glDepthMask(GL_FALSE); // disable depth-buffer writing
	draw(skybox, environment);
	glDepthMask(GL_TRUE); // re-activate depth-buffer write

	draw(water, environment);
	draw(terrain, environment);
	// draw(sphere_light, environment);
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
