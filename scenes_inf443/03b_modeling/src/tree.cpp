#include "tree.hpp"

using namespace cgp;


mesh create_cylinder_mesh(float radius, float height)
{
    mesh m; 
    // To do: fill this mesh ...
    // ...
    // To add a position: 
    //   m.position.push_back(vec3{x,y,z})
    // Or in pre-allocating the buffer:
    //   m.position.resize(maximalSize);
    //   m.position[index] = vec3{x,y,z}; (with 0<= index < maximalSize)
    // 
    // Similar with the triangle connectivity:
    //  m.connectivity.push_back(uint3{index_1, index_2, index_3});


    // Need to call fill_empty_field() before returning the mesh 
    //  this function fill all empty buffer with default values (ex. normals, colors, etc).

    int N = 18;
    float p_x;
    float p_y;

    for (int k = 0; k < N; k++) {
        float ang = k / float(N);
        p_x = radius * std::cos(2 * 3.14f * ang);
        p_y = radius * std::sin(2 * 3.14f * ang);

        m.position.push_back(vec3 {p_x, p_y, 0.0f });
        m.position.push_back(vec3 {p_x, p_y , height });
    }

    for (int k = 0; k < N ; k++) {
        int d0 = 2 * k;
        int u0 = (d0 + 1) % (2 * N);
        int d1 = (d0 + 2) % (2 * N);
        int u1 = (d0 + 3) % (2 * N);
        m.connectivity.push_back(uint3 { d0, u0, d1 });
        m.connectivity.push_back(uint3{ d1, u1, u0 });
     }

    m.fill_empty_field();

    return m;
}


mesh create_cone_mesh(float radius, float height, float z_offset)
{
    mesh m; 
    // To do: fill this mesh ...
    // ...

    int N = 20;
    float p_x;
    float p_y;

    m.position.push_back(vec3{ 0, 0, z_offset });
    m.position.push_back(vec3{ 0, 0, z_offset + height });

    for (int k = 0; k < N; k++) {
        float ang = k / float(N);
        p_x = radius * std::cos(2 * 3.14f * ang);
        p_y = radius * std::sin(2 * 3.14f * ang);

        m.position.push_back(vec3{ p_x, p_y, z_offset });
    }

    for (int k = 2; k < N + 2; k++) {
        int p0 = k;
        int p1 = 0;
        if (k == N + 1) {
            p1 = 2;
        }
        else {
            p1 = k + 1;
        }
        m.connectivity.push_back(uint3 { 0, p0, p1 });
        m.connectivity.push_back(uint3{ 1, p0, p1 });
    }

    m.fill_empty_field();
    return m;
}



mesh create_tree()
{
    float h = 0.7f; // trunk height
    float r = 0.1f; // trunk radius
    // Create a brown trunk
    mesh trunk = create_cylinder_mesh(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });
    // Create a green foliage from 3 cones
    mesh foliage = create_cone_mesh(4 * r, 6 * r, 0.0f); // base-cone
    foliage.push_back(create_cone_mesh(4 * r, 6 * r, 2 * r)); // middlecone
    foliage.push_back(create_cone_mesh(4 * r, 6 * r, 4 * r)); // top-cone
    foliage.translate({ 0,0,h }); // place foliage at the top of the trunk
        foliage.color.fill({ 0.4f, 0.6f, 0.3f });

    // The tree is composed of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);
    return tree;
}

