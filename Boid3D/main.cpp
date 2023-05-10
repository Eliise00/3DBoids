#include <filesystem>
#include <vector>
#include "Boid3D.hpp"
#include "Gui.hpp"
#include "Model.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "glimac/FreeflyCamera.hpp"
#include "glimac/common.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"
#include "tiny_obj_loader.h"

int const window_width  = 1920;
int const window_height = 1080;

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

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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

    const int NbBoid = 30;

    // BEGINNING OF MY INIT CODE//

    // create the programs
    PenguinProgram penguin{};

    // BEGINNING OF MY INIT CODE//

    //////Texture - loading of the texture -> must be done at the begining of the code
    // Needs Texture.hpp
    Texture penguinTexture("assets/models/texture_penguin.jpg", 0);

    Model penguinModel("assets/models/penguin.obj");

    penguinModel.setVertices();

    penguinModel.setIndices();

    penguinModel.glCode();

    // Depth option
    //////// don't know what this do exactly but it is very important for transparency and depth
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // MVP
    FreeflyCamera ViewMatrix = FreeflyCamera();
    glm::mat4     ProjMatrix = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);

    // For the boids
    glm::mat4 MVMatrix_penguin;
    glm::mat4 NormalMatrix_penguin;

    // light parameter
    std::vector<glm::vec3> Ka;
    std::vector<glm::vec3> Kd;
    std::vector<glm::vec3> Ks;
    std::vector<float>     Shininess;

    /////// loop that generate random parameter for the light color - maybe it's not necessary for the project
    for (int i = 0; i < NbBoid; i++)
    {
        Ka.emplace_back(glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f), glm::linearRand(0.f, 0.05f));
        Kd.emplace_back(glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f), glm::linearRand(0.f, 0.5f));
        Ks.emplace_back(glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f));
        Shininess.emplace_back(glm::linearRand(0.f, 1.f));
    }

    //////// keyboard
    bool Z = false;
    bool Q = false;
    bool S = false;
    bool D = false;

    // END OF MY INIT CODE//
    // Checkpoint
    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;

    // Initialize boids with desired parameters
    std::vector<Boid3D> boids;
    glm::vec3           random_pos = {p6::random::number(-ctx.aspect_ratio(), ctx.aspect_ratio()), p6::random::number(-1, 1), p6::random::number(-1, 1)};

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
        if (D)
        {
            ViewMatrix.moveLeft(0.1);
        }
        if (S)
        {
            ViewMatrix.moveFront(-0.1);
        }
        if (Q)
        {
            ViewMatrix.moveLeft(-0.1);
        }

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // boids
        penguin.m_Program.use();

        ///////// bind the texture of the boids
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, penguinTexture.getTextureID());
        glUniform1i(penguin.uTexture, 0);

        // BEGIN OF MY DRAW CODE//

        penguinModel.bindVertexArray();

        /////// boid deplacement here
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
            MVMatrix_penguin     = glm::rotate(MVMatrix_penguin, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            MVMatrix_penguin     = glm::translate(MVMatrix_penguin, position);                                                                                           // Translate to the position of the boid
            MVMatrix_penguin     = glm::scale(MVMatrix_penguin, glm::vec3(small_boid_params.draw_radius, small_boid_params.draw_radius, small_boid_params.draw_radius)); // Scale to the appropriate radius for your boids
            NormalMatrix_penguin = glm::transpose(glm::inverse(MVMatrix_penguin));

            // penguin
            drawPenguin(i, penguin, penguinModel.getIndices(), ViewMatrix, ProjMatrix, MVMatrix_penguin, NormalMatrix_penguin, Ka, Kd, Ks, Shininess);
            i++;
        }

        penguinModel.debindVertexArray();

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
    penguinModel.clearBuffers();

    return 0;
}