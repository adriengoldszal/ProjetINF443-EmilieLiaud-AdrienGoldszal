#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "key_positions_structure.hpp"
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
	//camera_controller_2d_displacement camera_control; // Adapted to 2D displacement
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;		   // The standard global frame
	environment_structure environment; // Standard environment controler
	cgp::skybox_drawable skybox;

	input_devices inputs; // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;	  // Standard GUI element storage

	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	timer_basic timer;

	mesh_drawable sphere_light; // Light source

	cgp::mesh_drawable water;
	cgp::mesh_drawable water2;
	cgp::mesh_drawable water3;
	cgp::mesh_drawable water4;
	cgp::mesh_drawable water5;
	cgp::mesh_drawable water6;
	cgp::mesh_drawable water7;
	cgp::mesh_drawable water8;
	cgp::mesh_drawable water9;
	cgp::mesh_drawable water_array[9] = {water, water2, water3, water4, water5, water6, water7, water8, water9};
	int center ;
	float terrain_length;

	cgp::mesh_drawable fish;

	cgp::mesh_drawable fish2;

	// Boat elements
	/************************************/
	mesh_drawable boat2;

	cgp::rotation_transform initial_position_rotation; // Boat position for rotations

	// Rock elements
	/************************************/
	cgp::mesh rock_mesh;
	cgp::mesh_drawable rock_drawable;
	cgp::mesh_drawable rock_drawable2;
	cgp::hierarchy_mesh_drawable hierarchy;

	cgp::mesh rock_mesh1;
	cgp::mesh_drawable rock1;
	std::vector<vec3> rock1_position;

	cgp::mesh rock_mesh2;
	cgp::mesh_drawable rock2;
	cgp::mesh rock_mesh3;
	cgp::mesh_drawable rock3;
	cgp::mesh rock_mesh4;
	cgp::mesh_drawable rock4;

	std::vector<int> rocks_type;


	// Grass elements
	/************************************/
	cgp::mesh_drawable grass;
	std::vector<cgp::vec3> grass_position;
	//perlin_noise_parameters parameters;

	//Terrain elements
	/************************************/
	cgp::mesh terrain_mesh;
	cgp::mesh_drawable terrain;

	// Fishes
	/****************************************/
	// A helper structure used to store and display the key positions/time
	keyframe_structure keyframe;

	// Timer used for the interpolation of the position
	float fish_timer;
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
