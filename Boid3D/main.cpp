#include <vector>
#include "glimac/FreeflyCamera.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"

#include "Program.h"
#include "Gui.h"
#include "tiny_obj_loader.h"

int const window_width  = 1920;
int const window_height = 1080;

//sphere
/*
void drawSphere(int i, const PenguinProgram& penguinProgram, const std::vector<glimac::ShapeVertex>& sphereVec, FreeflyCamera ViewMatrix, glm::mat4 ProjMatrix, glm::mat4 MVMatrix, glm::mat4 NormalMatrix, std::vector<glm::vec3> Ka, std::vector<glm::vec3> Kd, std::vector<glm::vec3> Ks, std::vector<float> Shininess)
{
    // Set uniform variables
    glUniformMatrix4fv(penguinProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
    glUniformMatrix4fv(penguinProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(penguinProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

    glUniform3fv(penguinProgram.uKa, 1, glm::value_ptr(Ka[i]));

    glUniform3fv(penguinProgram.uKd, 1, glm::value_ptr(Kd[i]));
    glUniform3fv(penguinProgram.uKs, 1, glm::value_ptr(Ks[i]));
    glUniform1f(penguinProgram.uShininess, Shininess[i]);
    glUniform3fv(penguinProgram.uLightPos_vs, 1, glm::value_ptr(glm::vec3(-1, -1, -1)));
    glUniform3fv(penguinProgram.uLightIntensity, 1, glm::value_ptr(glm::vec3(1, 1, 1)));

    // Draw the sphere using glDrawArrays
    glDrawArrays(GL_TRIANGLES, 0, sphereVec.size());
}*/

void drawPenguin(int i, const PenguinProgram& penguinProgram, std::vector<unsigned int> indices, FreeflyCamera ViewMatrix, glm::mat4 ProjMatrix, glm::mat4 MVMatrix, glm::mat4 NormalMatrix, std::vector<glm::vec3> Ka, std::vector<glm::vec3> Kd, std::vector<glm::vec3> Ks, std::vector<float> Shininess)
{
    // Set uniform variables
    glUniformMatrix4fv(penguinProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
    glUniformMatrix4fv(penguinProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(penguinProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

    glUniform3fv(penguinProgram.uKa, 1, glm::value_ptr(Ka[i]));

    glUniform3fv(penguinProgram.uKd, 1, glm::value_ptr(Kd[i]));
    glUniform3fv(penguinProgram.uKs, 1, glm::value_ptr(Ks[i]));
    glUniform1f(penguinProgram.uShininess, Shininess[i]);
    glUniform3fv(penguinProgram.uLightPos_vs, 1, glm::value_ptr(glm::vec3(-1, -1, -1)));
    glUniform3fv(penguinProgram.uLightIntensity, 1, glm::value_ptr(glm::vec3(1, 1, 1)));

    // Draw the sphere using glDrawArrays
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

}

int main()
{
    auto ctx = p6::Context{{window_width, window_height, "Boid3D"}};
    ctx.maximize_window();

    // BOID PARAMETERS //

    Boid_behavior_params small_boid_params{
        .align_factor = 6.,
        .align_radius = 20.,
        .avoid_factor = 2.,
        .avoid_radius = 6.,
        .draw_radius  = .05,
        .max_speed    = 30,
        .min_speed    = 20};

    Environment_params environment_params{
        .speed_multiplier  = 1.,
        .aspect_ratio      = ctx.aspect_ratio(),
        .screen_margin     = .1,
        .show_align_circle = true,
        .show_avoid_circle = true,
        .z_limit           = 1.};

    const int NbBoid = 50;

    // BEGINNING OF MY INIT CODE//

    // create the programs
    PenguinProgram penguin{};

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

    // sphere
    //const std::vector<glimac::ShapeVertex> sphereVec = glimac::sphere_vertices(1.f, 32, 16);
    //glBufferData(GL_ARRAY_BUFFER, sphereVec.size() * sizeof(glimac::ShapeVertex), sphereVec.data(), GL_STATIC_DRAW);

    //penguin
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Depth option
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create a VAO for the model
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //sphere
    /*
    // enable the INDEX attribut we want / POSITION is index 0 by default
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORM     = 1;
    const GLuint VERTEX_ATTR_UV       = 2;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORM);
    glEnableVertexAttribArray(VERTEX_ATTR_UV);
    // rebind the vbo to specify vao attribute
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, position)));
    glVertexAttribPointer(VERTEX_ATTR_NORM, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, normal)));
    glVertexAttribPointer(VERTEX_ATTR_UV, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)(offsetof(glimac::ShapeVertex, texCoords)));
    // debind the vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // debind the vao
    glBindVertexArray(0);
     */

    //penguin
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);

    // MVP
    FreeflyCamera ViewMatrix = FreeflyCamera();
    glm::mat4     ProjMatrix = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);

    // For the boids
    glm::mat4 MVMatrix_penguin;
    glm::mat4 NormalMatrix_penguin;

    std::vector<glm::vec3> Ka;
    std::vector<glm::vec3> Kd;
    std::vector<glm::vec3> Ks;
    std::vector<float>     Shininess;

    for (int i = 0; i < NbBoid; i++)
    {
        Ka.emplace_back(glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f));
        Kd.emplace_back(glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f));
        Ks.emplace_back(glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f));
        Shininess.emplace_back(glm::linearRand(0.f, 1.f));
    }

    bool Z = false;
    bool Q = false;
    bool S = false;
    bool D = false;

    // END OF MY INIT CODE//

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    // Initialize boids with desired parameters
    std::vector<Boid3D> boids;
    // glm::vec3 initial_position(1.0f, 2.0f, 3.0f);
    glm::vec3 random_pos = {p6::random::number(-ctx.aspect_ratio(), ctx.aspect_ratio()), p6::random::number(-1, 1), p6::random::number(-1, 1)};

    for (int i = 0; i < NbBoid; ++i)
    {
        Boid3D boid(random_pos);
        boids.push_back(boid);
    }

    /* Loop until the user closes the window */
    ctx.update = [&]() {

        createGuiFromParams(&small_boid_params, "Small Boids");
        createMainGui(&environment_params);

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

        // boids
        penguin.m_Program.use();

        // BEGIN OF MY DRAW CODE//

        glBindVertexArray(vao);

        for (auto& boid : boids)
        {
            int i = 0;

            // Update boid positions and velocities based on behavior rules
            boid.adaptSpeedToBorders(environment_params, small_boid_params);
            boid.adaptSpeedToBoids(boids, environment_params, small_boid_params);
            boid.clampSpeed(small_boid_params);
            boid.updatePosition(environment_params);

            // Get the position of the boid
            glm::vec3 position = boid.getPosition();

            // Set the MVP matrices
            MVMatrix_penguin     = ViewMatrix.getViewMatrix();
            MVMatrix_penguin     = glm::rotate(MVMatrix_penguin, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            //MVMatrix_penguin     = glm::rotate(MVMatrix_penguin, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            MVMatrix_penguin     = glm::rotate(MVMatrix_penguin, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            MVMatrix_penguin     = glm::translate(MVMatrix_penguin, position);                                                                                           // Translate to the position of the boid
            MVMatrix_penguin     = glm::scale(MVMatrix_penguin, glm::vec3(small_boid_params.draw_radius, small_boid_params.draw_radius, small_boid_params.draw_radius)); // Scale to the appropriate radius for your boids
            NormalMatrix_penguin = glm::transpose(glm::inverse(MVMatrix_penguin));

            //sphere
            //drawSphere(i, penguin, sphereVec, ViewMatrix, ProjMatrix, MVMatrix_penguin, NormalMatrix_penguin, Ka, Kd, Ks, Shininess);

            //penguin
            drawPenguin(i, penguin, indices, ViewMatrix, ProjMatrix, MVMatrix_penguin, NormalMatrix_penguin, Ka, Kd, Ks, Shininess);

            i++;


        }

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