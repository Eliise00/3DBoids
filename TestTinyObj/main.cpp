#include <iostream>
#include <vector>
#include "glimac/FreeflyCamera.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

#include "tiny_obj_loader.h"

int const window_width  = 1920;
int const window_height = 1080;

struct EarthProgram {
    p6::Shader m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uKa;

    GLint uKd;
    GLint uKs;
    GLint uShininess;

    GLint uLightPos_vs;
    GLint uLightIntensity;

    EarthProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/pointLight.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");

        uKa             = glGetUniformLocation(m_Program.id(), "uKa");

        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");

    }
};

int main()
{
    auto ctx = p6::Context{{window_width, window_height, "TestObj"}};
    ctx.maximize_window();

    EarthProgram earth{};

    // BEGINNING OF MY INIT CODE//


    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;

    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, &warn, &err, "assets/models/penguin.obj");
    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }
    if (!ret) {
        exit(1);
    }

    // Extract the vertices
    std::vector<float> vertices;
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        vertices.push_back(attrib.vertices[i]);
        vertices.push_back(attrib.vertices[i + 1]);
        vertices.push_back(attrib.vertices[i + 2]);
    }

    // Extract the indices
    std::vector<unsigned int> indices;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            indices.push_back(index.vertex_index);
        }
    }


    // Create a VBO for the model
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);


    // Create a VAO for the model
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);

    // MVP
    FreeflyCamera ViewMatrix = FreeflyCamera();

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;

    bool Z = false;
    bool Q = false;
    bool S = false;
    bool D = false;


    // END OF MY INIT CODE//

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    /* Loop until the user closes the window */
    ctx.update = [&]() {

        if (Z)
        {
            ViewMatrix.moveFront(0.1);
        }
        if (Q)
        {
            ViewMatrix.moveLeft(0.1);
        }
        if (S)
        {
            ViewMatrix.moveFront(-0.1);
        }
        if (D)
        {
            ViewMatrix.moveLeft(-0.1);
        }

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        earth.m_Program.use();

        // BEGIN OF MY DRAW CODE//


        MVMatrix     = ViewMatrix.getViewMatrix();
        MVMatrix     = glm::rotate(MVMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        MVMatrix     = glm::translate(MVMatrix, glm::vec3(0.0f, -2.0f, 0.0f));
        //MVMatrix = glm::scale(MVMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
        MVMatrix     = glm::rotate(MVMatrix, -ctx.time(), glm::vec3(0, 1, 0));

        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));


        glBindVertexArray(vao);


        glUniformMatrix4fv(earth.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(earth.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(earth.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glUniform3fv(earth.uKa, 1, glm::value_ptr(glm::vec3(0.25, 0, 0.25)));
        glUniform3fv(earth.uKd, 1, glm::value_ptr(glm::vec3(0.07568, 0.61424, 0.07568)));
        glUniform3fv(earth.uKs, 1, glm::value_ptr(glm::vec3(0.633, 0.727811, 0.633)));
        glUniform1f(earth.uShininess, 1);

        glUniform3fv(earth.uLightPos_vs, 1, glm::value_ptr(glm::vec3(-3, -3, -3)));
        glUniform3fv(earth.uLightIntensity, 1, glm::value_ptr(glm::vec3(2, 2, 2)));


        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());


        glBindVertexArray(0);

        // END OF MY DRAW CODE//

    };

    ctx.key_pressed = [&Z, &Q, &S, &D](const p6::Key& key) {
        if (key.physical == GLFW_KEY_W)
        {
            Z = true;
        }
        if (key.physical == GLFW_KEY_A)
        {
            Q = true;
        }
        if (key.physical == GLFW_KEY_S)
        {
            S = true;
        }
        if (key.physical == GLFW_KEY_D)
        {
            D = true;
        }
    };

    ctx.key_released = [&Z, &Q, &S, &D](const p6::Key& key) {
        if (key.physical == GLFW_KEY_W)
        {
            Z = false;
        }
        if (key.physical == GLFW_KEY_A)
        {
            Q = false;
        }
        if (key.physical == GLFW_KEY_S)
        {
            S = false;
        }
        if (key.physical == GLFW_KEY_D)
        {
            D = false;
        }
    };

    ctx.mouse_dragged = [&ViewMatrix](const p6::MouseDrag& button) {
        ViewMatrix.rotateLeft(button.delta.x * 50);
        ViewMatrix.rotateUp(-button.delta.y * 50);
    };

    ctx.start();

    // Clear vbo & vao
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}