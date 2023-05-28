#include <filesystem>
#include <vector>
#include "Boid3D.hpp"
#include "Cube.hpp"
#include "Drawing.hpp"
#include "Gui.hpp"
#include "Model.hpp"
#include "Program.hpp"
#include "Texture.hpp"
#include "glimac/FreeflyCamera.hpp"
#include "glimac/ThirdPersonCamera.hpp"
#include "glimac/common.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"
#include "tiny_obj_loader.h"

int const window_width  = 1920;
int const window_height = 1080;

int main()
{
    // P6 CONTEXT //
    auto ctx = p6::Context{{window_width, window_height, "Boid3D"}};
    ctx.maximize_window();

    // BOID PARAMETERS //
    Boid_behavior_params small_boid_params{
        .align_factor = 12.,
        .align_radius = 24.,
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

    // BEGINNING OF INIT CODE //
    // PROGRAMS
    PenguinProgram penguin{};
    CubeProgram    cube{};
    ModelProgram   iceField{};
    ModelProgram   surveyor{};
    // GUI VARIABLES
    bool showBoidsbox           = true;
    bool freeflyCameraActivated = true;

    // LOADING OF THE TEXTURES & MODELS //
    // Boidsbox
    Texture cubeTexture("assets/models/texture_ice.jpg", 0);

    // Penguin
    Texture penguinTexture("assets/models/texture_penguin.jpg", 1);

    Model penguinModel("assets/models/penguin.obj");
    penguinModel.setVertices();
    penguinModel.setIndices();
    penguinModel.glCode();

    // Icefield
    Texture IceFieldTexture("assets/models/texture_ice.jpg", 2);

    Model iceFieldModel("assets/models/Icefield.obj");
    iceFieldModel.setVertices();
    iceFieldModel.setIndices();
    iceFieldModel.glCode();

    // Surveyor
    Texture SurveyorTexture("assets/models/surveyor_orange.png", 3);

    Model surveyorModel("assets/models/surveyor.obj");
    surveyorModel.setVertices();
    surveyorModel.setIndices();
    surveyorModel.glCode();

    glm::vec3 surveyorPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    // DEPTH OPTIONS //
    //////// don't know what this do exactly but it is very important for transparency and depth
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // MVP
    FreeflyCamera freeViewMatrix  = FreeflyCamera();
    auto          thirdViewMatrix = ThirdPersonCamera(surveyorPosition);
    glm::mat4     ProjMatrix      = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);

    // For the surveyor
    glm::mat4 MVMatrix_surveyor;

    // For the cube
    glm::mat4 MVMatrix_cube;

    // For the boids
    glm::mat4 MVMatrix_penguin;
    glm::mat4 NormalMatrix_penguin;

    // For the Ice Field
    glm::mat4 MVMatrix_ice;

    // light parameter
    std::vector<glm::vec3> Ka;
    std::vector<glm::vec3> Kd;
    std::vector<glm::vec3> Ks;
    std::vector<float>     Shininess;

    /////// loop that generate random parameter for the light color - not necessary for the project
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

    // END OF INIT CODE//
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

    // Loop until the user closes the window //
    ctx.update = [&]() {
        createGuiFromParams(&small_boid_params, "Small Boids");
        createMainGui(&environment_params);
        ImGui::Begin("Other parameters");
        ImGui::Checkbox("Display the cube", &showBoidsbox); // freeflyCameraActivated
        ImGui::Checkbox("Freefly camera", &freeflyCameraActivated);
        ImGui::End();

        // Surveryor control
        float     camSpeed = 0.01f;
        glm::vec3 moveOffset(0.0f, 0.0f, 0.0f);
        if (Z)
        {
            freeViewMatrix.moveFront(camSpeed);
            moveOffset.z -= camSpeed;
        }
        if (D)
        {
            freeViewMatrix.moveLeft(camSpeed);
            moveOffset.x -= camSpeed;
        }
        if (S)
        {
            freeViewMatrix.moveFront(-camSpeed);
            moveOffset.z += camSpeed;
        }
        if (Q)
        {
            freeViewMatrix.moveLeft(-camSpeed);
            moveOffset.x += camSpeed;
        }

        surveyorPosition += moveOffset;
        thirdViewMatrix.update(surveyorPosition);

        // CAMERAS MVMATRIXES //
        glm::mat4 freeMVMatrix  = freeViewMatrix.getViewMatrix();
        glm::mat4 thirdMVMatrix = thirdViewMatrix.getViewMatrix();

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the cube program and set the uniforms
        cube.m_Program.use();

        ///////// bind the texture of the cube
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture.getTextureID());
        glUniform1i(cube.uTextureCube, 0);

        MVMatrix_cube               = freeflyCameraActivated ? freeMVMatrix : thirdMVMatrix;
        glm::mat4 NormalMatrix_cube = glm::transpose(glm::inverse(MVMatrix_cube));

        glUniformMatrix4fv(cube.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix_cube));
        glUniformMatrix4fv(cube.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix_cube));
        glUniformMatrix4fv(cube.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix_cube));

        // Cube
        if (showBoidsbox)
            drawCube(1.5 + environment_params.screen_margin, 1.5 + environment_params.screen_margin, 2.);

        // Ice Field
        iceField.m_Program.use();

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, IceFieldTexture.getTextureID());
        glUniform1i(iceField.uTexture, 2);

        iceFieldModel.bindVertexArray();

        MVMatrix_ice               = freeflyCameraActivated ? freeMVMatrix : thirdMVMatrix;
        MVMatrix_ice               = glm::translate(MVMatrix_ice, glm::vec3(0.0f, 1.6f, 0.0f));
        MVMatrix_ice               = glm::scale(MVMatrix_ice, glm::vec3(1.0f / 2.0f));
        glm::mat4 NormalMatrix_ice = glm::transpose(glm::inverse(MVMatrix_ice));
        drawModel(iceField, iceFieldModel.getIndices(), ProjMatrix, MVMatrix_ice, NormalMatrix_ice);

        iceFieldModel.debindVertexArray();

        // Drawing of the surveryor (only in third person)
        if (!freeflyCameraActivated)
        {
            // surveyor
            surveyor.m_Program.use();

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, SurveyorTexture.getTextureID());
            glUniform1i(surveyor.uTexture, 3);

            surveyorModel.bindVertexArray();
            MVMatrix_surveyor = thirdMVMatrix;
            MVMatrix_surveyor = glm::translate(glm::mat4(1.0f), surveyorPosition);
            MVMatrix_surveyor = glm::translate(MVMatrix_surveyor, surveyorPosition);

            // MVMatrix_surveyor               = glm::rotate(MVMatrix_surveyor, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            MVMatrix_surveyor               = glm::scale(MVMatrix_surveyor, glm::vec3(1.0f / 20.0f));
            glm::mat4 NormalMatrix_surveyor = glm::transpose(glm::inverse(MVMatrix_surveyor));
            drawModel(surveyor, surveyorModel.getIndices(), ProjMatrix, MVMatrix_surveyor, NormalMatrix_surveyor);

            surveyorModel.debindVertexArray();
        }

        // boids
        penguin.m_Program.use();

        // bind the texture of the boids
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, penguinTexture.getTextureID());
        glUniform1i(penguin.uTexture, 1);

        penguinModel.bindVertexArray();

        // boid movement
        int i = 0;
        for (auto& boid : boids)
        {
            // Update boid positions and velocities based on behavior rules
            boid.adaptSpeedToBorders(environment_params, small_boid_params);
            boid.adaptSpeedToBoids(boids, environment_params, small_boid_params);
            boid.clampSpeed(small_boid_params);
            boid.updatePosition(environment_params);

            // Get the position of the boid
            glm::vec3 position = boid.getPosition();

            // Set the MVP matrices
            MVMatrix_penguin     = freeflyCameraActivated ? freeMVMatrix : thirdMVMatrix;
            MVMatrix_penguin     = glm::rotate(MVMatrix_penguin, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            MVMatrix_penguin     = glm::rotate(MVMatrix_penguin, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            MVMatrix_penguin     = glm::translate(MVMatrix_penguin, position);                                                                                           // Translate to the position of the boid
            MVMatrix_penguin     = glm::scale(MVMatrix_penguin, glm::vec3(small_boid_params.draw_radius, small_boid_params.draw_radius, small_boid_params.draw_radius)); // Scale to the appropriate radius for your boids
            NormalMatrix_penguin = glm::transpose(glm::inverse(MVMatrix_penguin));

            // draw the penguins !
            drawPenguin(i, penguin, penguinModel.getIndices(), ProjMatrix, MVMatrix_penguin, NormalMatrix_penguin, Ka, Kd, Ks, Shininess);
            i++;
        }

        penguinModel.debindVertexArray();

        // END OF DRAW CODE//
    };

    // INPUT MANAGEMENT //
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

    ctx.mouse_dragged = [&freeViewMatrix, &thirdViewMatrix](const p6::MouseDrag& button) {
        freeViewMatrix.rotateLeft(button.delta.x * 50);
        freeViewMatrix.rotateUp(button.delta.y * 50);

        thirdViewMatrix.rotateLeft(button.delta.x * 50);
        thirdViewMatrix.rotateUp(button.delta.y * 50);
    };

    ctx.start();

    // Clear vbo & vao
    penguinModel.clearBuffers();
    iceFieldModel.clearBuffers();
    surveyorModel.clearBuffers();

    return 0;
}