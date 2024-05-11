#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "key_positions_structure.hpp"

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
	// camera_controller_orbit_euler camera_control; // Camera controller figé
	camera_controller_2d_displacement camera_control; // Adapted to 2D displacement
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

	cgp::mesh_drawable terrain;

	cgp::mesh_drawable fish;

	// Boat elements
	/************************************/
	mesh_drawable boat2;

	cgp::rotation_transform initial_position_rotation; // Boat position for rotations

	// Fishes
	/****************************************/
	// A helper structure used to store and display the key positions/time
	keyframe_structure keyframe;

	// Timer used for the interpolation of the position
	float fish_timer;
	cgp ::timer_interval fish_interval;
	numarray<vec3> initial_fish_positions ;
	numarray<vec3> fish_positions;
	numarray<float> fish_times ;

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
