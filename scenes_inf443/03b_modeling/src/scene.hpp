#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "rock.hpp"
#include "terrain.hpp"

// This definitions allow to use the structures: mesh, mesh_drawable, etc. without mentionning explicitly cgp::
using cgp::mesh;
using cgp::mesh_drawable;
using cgp::numarray;
using cgp::timer_basic;
using cgp::vec3;

// Variables associated to the GUI
struct gui_parameters
{
	bool display_frame = true;
	bool display_wireframe = false;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic
{

	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control; // Camera controller figé
	// camera_controller_2d_displacement camera_control; // Adapted to 2D displacement
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;		   // The standard global frame
	environment_structure environment; // Standard environment controler
	cgp::skybox_drawable skybox;

	input_devices inputs; // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;	  // Standard GUI element storage

	// *********************************** //
	// Elements and shapes of the scene
	// *********************************** //

	timer_basic timer;

	vec3 background_color; // Background color

	// *********************************** //
	// Water elements
	// *********************************** //
	cgp::mesh_drawable water_array[3][3];
	float water_length;
	int N_water_samples;
	int nb_hollow;

	// ***********************************//
	// Terrain elements
	// *********************************** //
	TerrainData terrain_array[3][3];
	int Cini;
	int Rini;

	// *********************************** //
	// Fish elements
	// *********************************** //
	cgp::mesh_drawable fish[2];

	// *********************************** //
	// Boat elements
	// *********************************** //
	mesh_drawable boat;
	cgp::rotation_transform initial_position_rotation; // Boat position for rotations

	// *********************************** //
	// House
	// *********************************** //
	cgp::mesh_drawable house;
	cgp::rotation_transform house_initial_rotation;
	int house_number;
	std::vector<cgp::vec3> house_position;

	// *********************************** //
	// Rock elements
	// *********************************** //
	std::vector<int> rocks_type;
	mesh rock_mesh[4];
	RockData rock_array[4];
	// cgp::vec3 resize_ratios[4] = {{2.0f, 1.0f, 3.4f}, {2.0f, 1.0f, 4.2f}, {2.0f, 1.0f, 4.2f}, {2.0f, 1.0f, 3.2f}};
	cgp::vec3 resize_ratios[4] = {{12.0f, 12.0f, 16.0f}, {12.0f, 12.0f, 16.0f}, {12.0f, 12.0f, 18.0f}, {12.0f, 12.0f, 18.0f}};

	// ***********************************//
	// Fishes
	// ***********************************//
	float fish_timer; // Timer used for the interpolation of the position
	cgp ::timer_interval fish_interval;
	numarray<vec3> initial_fish_positions;
	numarray<vec3> fish_positions;
	numarray<vec3> fish_positions2;
	numarray<float> fish_times;

	// ****************************** //
	// Functions
	// ****************************** //
	void
	initialize();		  // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();	  // The display of the GUI, also called within the animation loop
	void scene_structure::display_semiTransparent();
	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void display_info();
};
