#include "fish.hpp"

using namespace cgp;

void initialize_fish(cgp::hierarchy_mesh_drawable &hierarchy)
{
    mesh_drawable body;
    mesh_drawable head;
    mesh_drawable right_fin;
    mesh_drawable left_fin;
    mesh_drawable tail_fin;
    mesh_drawable back_fin;

    body.initialize_data_on_gpu(mesh_primitive_ellipsoid({1.0f, 2.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, 20, 40));
    head.initialize_data_on_gpu(mesh_primitive_cube({0.0f, 0.0f, 0.0f}, 1.0f)); // head
    right_fin.initialize_data_on_gpu(mesh_primitive_quadrangle(
        {1.0f, 0.0f, 0.0f},
        {1.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}

        ));
    left_fin.initialize_data_on_gpu(mesh_primitive_quadrangle(
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}

        ));
    back_fin.initialize_data_on_gpu(mesh_primitive_quadrangle(
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f}

        ));
    tail_fin.initialize_data_on_gpu(mesh_primitive_quadrangle(
        {0.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {0.0f, -1.0f, 0.0f}

        ));

    body.material.color = {1.0f, 0.0f, 0.0f};      // red
    head.material.color = {1.0f, 0.0f, 0.0f};      // red ;
    right_fin.material.color = {1.0f, 0.0f, 0.0f}; // red ;
    left_fin.material.color = {1.0f, 0.0f, 0.0f};  // red ;
    back_fin.material.color = {1.0f, 0.0f, 0.0f};  // red ;

    hierarchy.add(body, "body");
    hierarchy.add(head, "head", "body", {0, 0, 3.0f});
    hierarchy.add(right_fin, "right_fin", "body", {0, 0, 0});
    hierarchy.add(left_fin, "left_fin", "body", {0, 0, 0});
    hierarchy.add(tail_fin, "tail_fin", "body", {2, 0, 0});
    hierarchy.add(back_fin, "back_fin", "body", {-2, 0, 0});
}