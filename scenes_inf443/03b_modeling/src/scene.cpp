#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"
#include "water.hpp"

using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 15.0f,6.0f,6.0f }, {0,0,0});
	

	// General information
	display_info();

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	int N_terrain_samples = 100;
	float terrain_length = 20;
	mesh const terrain_mesh = create_terrain_mesh(N_terrain_samples, terrain_length);
	terrain.initialize_data_on_gpu(terrain_mesh);
	terrain.material.color = { 0.6f,0.85f,0.5f };
	terrain.material.phong.specular = 0.0f; // non-specular terrain material
	terrain.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/texture_grass.jpg", GL_REPEAT, GL_REPEAT);

	mesh const tree_mesh = create_tree();
	tree.initialize_data_on_gpu(tree_mesh);
	//tree.model.translation = { 0, 0, evaluate_terrain_height(0, 0) };

	tree_position = generate_positions_on_terrain(50, terrain_length);

	mesh const water_mesh = create_water_mesh(N_terrain_samples,terrain_length);
	water.initialize_data_on_gpu(water_mesh);
	opengl_shader_structure	water_shader;
	water_shader.load(
		project::path + "shaders/water/water.frag.glsl",
		project::path + "shaders/water/water.vert.glsl");
	water.shader = water_shader;

	


}



void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	draw(terrain, environment);
	for (cgp::vec3 pos : tree_position) {
		tree.model.translation = pos;
		draw(tree, environment);
	}
	//draw(tree, environment);
	draw(water, environment);

	if (gui.display_wireframe) {
		draw_wireframe(terrain, environment);
		draw_wireframe(tree, environment);
	}}


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
	std::cout << "-----------------------------------------------\n" << std::endl;
}
