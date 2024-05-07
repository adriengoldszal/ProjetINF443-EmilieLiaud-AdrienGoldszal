#include "environment.hpp"

// Change these global values to modify the default behavior
// ************************************************************* //
// The initial zoom factor on the GUI
float project::gui_scale = 1.0f;
// Is FPS limited automatically
bool project::fps_limiting = true;
// Maximal default FPS (used only of fps_max is true)
float project::fps_max = 60.0f;
// Automatic synchronization of GLFW with the vertical-monitor refresh
bool project::vsync = true;
// Initial dimension of the OpenGL window (ratio if in [0,1], and absolute pixel size if > 1)
float project::initial_window_size_width = 0.5f;
float project::initial_window_size_height = 0.5f;
// ************************************************************* //

// This path will be automatically filled when the program starts
std::string project::path = "";

cgp::vec3 environment_structure::get_camera_position() const
{
	vec4 last_col_4 = camera_view * vec4(0.0, 0.0, 0.0, 1.0); // get the last column
	vec3 last_col = {last_col_4.x, last_col_4.y, last_col_4.z};
	return -transpose(mat3(camera_view)) * last_col; // get the orientation matrix * last camera view column
}

void environment_structure::send_opengl_uniform(opengl_shader_structure const &shader, bool expected) const
{
	opengl_uniform(shader, "projection", camera_projection, expected);
	opengl_uniform(shader, "view", camera_view, expected);
	opengl_uniform(shader, "light", light_position, false);

	uniform_generic.send_opengl_uniform(shader, false);
}