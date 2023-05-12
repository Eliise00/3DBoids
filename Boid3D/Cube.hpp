#ifndef _CUBE_DRAWING_HPP
#define _CUBE_DRAWING_HPP

#include <cstdlib>
#include <glimac/common.hpp>
#include <iostream>
#include <vector>
#include "glm/gtc/type_ptr.hpp"
#include "p6/p6.h"
#include <GLFW/glfw3.h>

void drawCube(float size_x, float size_y, float size_z) {
    // Define vertices of a cube
    float vertices[] = {
        -size_x, -size_y, size_z, 0.1f, // vertex 0
        size_x, -size_y,  size_z, 0.1f, // vertex 1
        size_x, size_y,  size_z, 0.1f, // vertex 2
        -size_x, size_y,  size_z, 0.1f, // vertex 3
        -size_x, -size_y, - size_z, 0.1f, // vertex 4
        size_x, -size_y, - size_z, 0.1f, // vertex 5
        size_x, size_y, - size_z, 0.1f, // vertex 6
        -size_x, size_y, - size_z, 0.1f, // vertex 7
    };

    // Define indices of the vertices that form each face of the cube
    unsigned int indices[] = {
        0, 1, 2, // front face
        2, 3, 0,
        1, 5, 6, // right face
        6, 2, 1,
        4, 0, 3, // left face
        3, 7, 4,
        5, 4, 7, // back face
        7, 6, 5,
        3, 2, 6, // top face
        6, 7, 3,
        4, 5, 1, // bottom face
        1, 0, 4
    };

    // Set up the VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set up the VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set up the IBO
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Draw the cube
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    // Clean up
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &IBO);
}

#endif
