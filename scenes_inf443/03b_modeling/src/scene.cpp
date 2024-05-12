#include <cstdlib>
#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"
#include "water.hpp"
#include "fish.hpp"
#include "interpolation.hpp"
# include "rock.hpp"

using namespace cgp;

static void deform_terrain(mesh &m);

void scene_structure::initialize()
{
	timer.start();
	timer.scale = 1.5f;

	camera_projection.depth_min = 0.0001f;
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	//camera_control.look_at({15.0f, 6.0f, 6.0f}, {0, 0, 0}); 
	//camera_control.look_at({1.0f, 0.0f, 0.0f}, {0, 0, 0}); pour le sol
	camera_control.look_at({0.0f, 30.0f, 2.0f }, { 0,0,0 }, { 0,0,1 });


	// General information
	display_info();

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Load skybox
	// ***************************************** //
	/*image_structure image_skybox_template = image_load_file("assets/skybox/hdr_01.png"); // hdr_01.png OR skybox_01.jpg
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
		project::path + "shaders/skybox/skybox.frag.glsl");*/

	// Creating an island for testing purposes
	float L = 50.0f;
	float elevation = -30.0f; // Adjust this value to set the elevation
	mesh terrain_mesh = mesh_primitive_grid({-L, -L, elevation}, {L, -L, elevation}, {L, L, elevation}, {-L, L, elevation}, 100, 100);
	//terrain_mesh.uv = { {0,0}, {1,0}, {1,1}, {0,1} }; // Associate Texture-Coordinates to the vertices of the quadrangle
	deform_terrain(terrain_mesh);
	terrain.initialize_data_on_gpu(terrain_mesh);
	terrain.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/sand.jpg", GL_REPEAT, GL_REPEAT);
	terrain.material.alpha = 0.3f;

	// Load water terrain
	int N_terrain_samples = 400;
	float terrain_length = 100;
	mesh const water_mesh = create_water_mesh(N_terrain_samples, terrain_length);
	water.initialize_data_on_gpu(water_mesh);
	opengl_shader_structure water_shader;
	water_shader.load(
		project::path + "shaders/water/water.vert.glsl",
		project::path + "shaders/water/water.frag.glsl");
	water.shader = water_shader;
	//water.material.color = {0.0f, 0.5f, 1.0f}; // blue color for water
	//water.material.color = { 179 / 255, 229 / 255, 252 / 255 }; // blue color for water
	water.material.phong.specular = 0.0f;	   // non-specular terrain material

	
	// Sending the skybox texture to the water shader as a uniform
	glUseProgram(water.shader.id);
	opengl_check;
	glActiveTexture(GL_TEXTURE1);
	opengl_check;
	/*skybox.texture.bind();
	opengl_uniform(water.shader, "image_skybox", 1);
	opengl_check;

	
	// Load boat
	// Open source file https://sketchfab.com/3d-models/chinese-junk-ship-35b340bce9fb4e0680bc0116cebc35c9
	mesh boat_mesh = mesh_load_file_obj(project::path + "assets/junk_low.obj");
	boat2.initialize_data_on_gpu(boat_mesh);
	boat2.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/boat.png");
	opengl_shader_structure boat_shader;
	boat_shader.load(
		project::path + "shaders/mesh/mesh.vert.glsl",
		project::path + "shaders/mesh/mesh.frag.glsl");
	// boat2.model.scaling = 0.0001f;
	boat2.model.translation = {-0.0f, 0.0f, 0.0f};
	initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, Pi) * rotation_transform::from_axis_angle({1, 0, 0}, Pi / 2);
	boat2.model.rotation = initial_position_rotation;

	// Load fish
	mesh fish_mesh = mesh_load_file_obj(project::path + "assets/fish/20230116_Tobiuo.obj");
	fish.initialize_data_on_gpu(fish_mesh);
	fish.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/fish/Body_Normal.png");
	fish.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/fish/Winf3_Normal.png");
	fish.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/fish/Wing_Normal.png");
	fish.model.rotation = rotation_transform::from_axis_angle({0, 0, 1}, Pi) * rotation_transform::from_axis_angle({1, 0, 0}, Pi / 2);
	fish.model.scaling = 0.1f;

	// Definition of the initial data
	//--------------------------------------//
	numarray<vec3> key_positions = {{0, 0, 0}, {5, 0, 0}, {5, 5, 0}, {0, 5, 0}, {0, 0, 0}};
	for (int i = 0; i < key_positions.size(); ++i)
		key_positions[i] = boat2.model.rotation * key_positions[i] + boat2.model.translation;

	// Key times (time at which the position must pass in the corresponding position)
	numarray<float> key_times =
		{0.0f, 2.0f, 4.0f, 6.0f, 8.0f};

	// Initialize the helping structure to display/interact with these positions
	keyframe.initialize(key_positions, key_times);

	// Set timer bounds
	// The timer must span a time interval on which the interpolation can be conducted
	// By default, set the minimal time to be key_times[1], and the maximal time to be key_time[N-2] (enables cubic interpolation)
	int N = key_times.size();
	timer_interpolation.t_min = key_times[1];
	timer_interpolation.t_max = key_times[N - 2];
	timer_interpolation.t = timer_interpolation.t_min;

	interpolation_update = timer.update();*/

	// Load Rocks
	/*rock_mesh = mesh_primitive_ellipsoid(vec3{3, 10, 15});
	rock_drawable.initialize_data_on_gpu(rock_mesh);
	update_rock(rock_mesh, rock_drawable, parameters);

	rock_mesh2 = mesh_primitive_ellipsoid(vec3{3, 10, 15});
	rock_drawable2.initialize_data_on_gpu(rock_mesh2);
	update_rock2(rock_mesh2, rock_drawable2, parameters);*/

	//rock_drawable2.material.color = vec3 { 0.8f, 0.5f, 0.7f };
	//hierarchy.add(rock_drawable, "Rock1");
	//hierarchy.add(rock_drawable2, "Rock2", "Rock1", { 0, -5, 0 });
	
	rock_mesh1 = mesh_load_file_obj(project::path + "assets/rocks/rock_2.obj");
	resize_rock1(rock_mesh1, 0.5f);
	//update_rock(rock_mesh1, rock1, parameters);
	rock1.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rocks/rock_o.png", GL_REPEAT, GL_REPEAT);
	
	rock1.initialize_data_on_gpu(rock_mesh1);

	//rock.model.rotation = rotation_transform::from_axis_angle({ 1, 0, 0 }, Pi/2);
	rock1.model.scaling = 5.0f;
	//rock1.model.translation = { 0, 15, 0 }; 
	rock1.material.phong.specular = 0.0f;	   // non-specular rock material
	//for (int i = 0; i < rock_mesh1.position.size(); i++) {
	//	std::cout << "pos x:" << rock_mesh1.position[i].x << " pos y: " << rock_mesh1.position[i].y << " pos z: " << rock_mesh1.position[i].z << std::endl;
	//}

	cgp::numarray<cgp::vec3>& originalVertices = rock_mesh1.position;

	// Get the scaling factor applied to rock1
	float scalingFactor = rock1.model.scaling;

	// Create a vector to store the transformed vertices

	// Apply the scaling transformation to each vertex position
	for (const vec3& vertex : originalVertices) {
		// Apply scaling transformation to each vertex position
		vec3 transformedVertex = scalingFactor * vertex;
		rock1_position.push_back(transformedVertex);
	}

	// Print or use the transformed vertices
	//for (const vec3& vertex : transformedVertices) {
	//	std::cout << "Transformed vertex position: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
	//}

	mesh rock_mesh2 = mesh_load_file_obj(project::path + "assets/rocks/rock2.obj");
	resize_rock2(rock_mesh2, 0.5f);
	rock2.initialize_data_on_gpu(rock_mesh2);

	//rock.model.rotation = rotation_transform::from_axis_angle({ 1, 0, 0 }, Pi/2);
	rock2.model.scaling = 5.0f;

	mesh rock_mesh3 = mesh_load_file_obj(project::path + "assets/rocks/rock3.obj");
	resize_rock3(rock_mesh3, 0.5f);
	rock3.initialize_data_on_gpu(rock_mesh3);

	//rock.model.rotation = rotation_transform::from_axis_angle({ 1, 0, 0 }, Pi/2);
	rock3.model.scaling = 5.0f;
	rock3.model.translation = { 0, -15, 0 }; // translations de la shpere

	mesh rock_mesh4 = mesh_load_file_obj(project::path + "assets/rocks/rock4.obj");
	resize_rock4(rock_mesh4, 0.5f);
	rock4.initialize_data_on_gpu(rock_mesh4);

	//rock.model.rotation = rotation_transform::from_axis_angle({ 1, 0, 0 }, Pi/2);
	rock4.model.scaling = 5.0f;
	rock4.model.translation = { 0, -30, 0 }; // translations de la shpere

	// Load grass

	//grass_position = generate_grass_positions_on_terrain(50, terrain_length);

	mesh quad_mesh = mesh_primitive_quadrangle({ -0.25f,0,0 }, { 0.25f,0,0 }, { 0.25f,0,0.5f }, { -0.25f,0,0.5f });
	grass.initialize_data_on_gpu(quad_mesh);
	grass.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/grass.png");
	grass.material.phong = { 0.4f, 0.6f,0,1 };

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
	timer_interpolation.update();

	if (timer.update() - interpolation_update > 5.0f)
	{
		interpolation_update = timer.update();
		for (int i = 0; i < keyframe.key_positions.size(); ++i)
			keyframe.key_positions[i] = boat2.model.rotation * keyframe.key_positions[i] + boat2.model.translation;
	}

	vec3 camera_position = environment.get_camera_position();

	environment.uniform_generic.uniform_float["time"] = timer.t;

	environment.light_position = camera_control.camera_model.position();


	if (gui.display_frame) {
		draw(global_frame, environment);
		draw(rock1, environment);
		//draw(rock2, environment);
		//draw(rock3, environment);
		//draw(rock4, environment);
	}

	if (gui.display_wireframe) {
		draw_wireframe(rock1, environment);
	}

	//draw(rock_drawable, environment);
	//draw(rock_drawable2, environment);
	//draw(hierarchy, environment);

	//if (gui.display_wireframe)
		//draw_wireframe(rock_drawable, environment);

	/*glDepthMask(GL_FALSE); // disable depth-buffer writing
	draw(skybox, environment);
	glDepthMask(GL_TRUE); // re-activate depth-buffer write*/

	display_semiTransparent(); // Display water and terrain as semi transparent for underwater effect

	//draw(water, environment);
	//draw(terrain, environment);

	// Draw fish
	// draw(hierarchy, environment);
	// boat2.model.translation = {camera_position.x, camera_position.y - 10.0f, camera_position.z - 10.0f};
	/*boat2.model.rotation = rotation_transform::from_axis_angle({0, 1, 0}, 0.2f * sin(timer.t)) * rotation_transform::from_axis_angle({1, 0, 0}, 0.2f * sin(timer.t)) * initial_position_rotation;
	boat2.model.scaling = 0.01f; // Ne marche plus correctement;
	draw(boat2, environment);

	// clear trajectory when the timer restart
	if (timer_interpolation.t < timer_interpolation.t_min + 0.1f)
		keyframe.trajectory.clear();

	// Display the key positions and lines b/w positions
	keyframe.display_key_positions(environment);

	// Compute the interpolated position
	//  This is this function that you need to complete
	vec3 p = interpolation(timer_interpolation.t, keyframe.key_positions, keyframe.key_times);

	// Display the interpolated position (and its trajectory)
	keyframe.display_current_position(p, environment);

	fish.model.translation = p;
	draw(fish, environment);*/

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

	//draw(water, environment);

	glDepthMask(false);

	auto const& camera = camera_control.camera_model;

	// Re-orient the grass shape to always face the camera direction
	vec3 const right = camera.right();
	// Rotation such that the grass follows the right-vector of the camera, while pointing toward the z-direction
	rotation_transform R = rotation_transform::from_frame_transform({ 1,0,0 }, { 0,0,1 }, right, { 0,0,1 });
	grass.model.rotation = R;


	// Sort transparent shapes by depth to camera
	//   This step can be skipped, but it will be associated to visual artifacts

	// Transform matrix (the same matrix which is applied in the vertices in the shader: T = Projection x View)
	mat4 T = camera_projection.matrix() * camera.matrix_view();

	srand(42);
	// Create a vector to store the random integers
	std::vector<int> randomNumbers;

	// Generate 50 random integers between 0 and 500
	for (int i = 0; i < 1000; ++i) {
		int randomNumber = rand() % (rock1_position.size()+1); // Generates a number between 0 and 500
		randomNumbers.push_back(randomNumber);
	}

	for (int val : randomNumbers) {
			grass.model.scaling = 2;
			grass.model.translation = rock1_position[val];
			draw(grass, environment);
	}
	

	// Don't forget to re-activate the depth-buffer write
	glDepthMask(true);
	glDisable(GL_BLEND);
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);


	bool update = false;
	update |= ImGui::SliderFloat("Persistance", &parameters.persistency, 0.1f, 0.6f);
	update |= ImGui::SliderFloat("Frequency gain", &parameters.frequency_gain, 0.5f, 1.5f);
	update |= ImGui::SliderInt("Octave", &parameters.octave, 1, 8);
	update |= ImGui::SliderFloat("Height", &parameters.terrain_height, 0.1f, 1.5f);

	if (update) {// if any slider has been changed - then update the terrain
		update_rock(rock_mesh1, rock1, parameters);
		//update_rock2(rock_mesh2, rock_drawable2, parameters);
	}
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
	/*vec3 camera_position_on_boat = {5.0f, 10.0f, 15.0f};
	vec3 camera_position_world = boat2.model.rotation * camera_position_on_boat + boat2.model.translation;
	// camera_control.camera_model.position_camera = camera_position_world;
	camera_control.camera_model.look_at(camera_position_world, boat2.model.translation);

	camera_control.action_keyboard(environment.camera_view);
	if (inputs.keyboard.is_pressed(GLFW_KEY_A))
	{
		vec3 translation_in_boat_coords = {-0.2f, 0.0f, 0.0f};
		// Changing to world coordinates by multiplying by the rotation matrix
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat2.model.translation = {boat2.model.translation.x + translation.x, boat2.model.translation.y + translation.y, boat2.model.translation.z + translation.z};
		initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, Pi / 100.0) * initial_position_rotation;
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_S))
	{
		vec3 translation_in_boat_coords = {0.0f, 0.0f, 0.2f};
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat2.model.translation = {boat2.model.translation.x + translation.x, boat2.model.translation.y + translation.y, boat2.model.translation.z + translation.z};
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_W))
	{
		vec3 translation_in_boat_coords = {0.0f, 0.0f, -0.2f};
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat2.model.translation = {boat2.model.translation.x + translation.x, boat2.model.translation.y + translation.y, boat2.model.translation.z + translation.z};
	}
	if (inputs.keyboard.is_pressed(GLFW_KEY_D))
	{
		vec3 translation_in_boat_coords = {0.2f, 0.0f, 0.0f};
		vec3 translation = initial_position_rotation * translation_in_boat_coords;
		boat2.model.translation = {boat2.model.translation.x + translation.x, boat2.model.translation.y + translation.y, boat2.model.translation.z + translation.z};
		initial_position_rotation = rotation_transform::from_axis_angle({0, 0, 1}, -Pi / 100.0) * initial_position_rotation;
	}*/

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
