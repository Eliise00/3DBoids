#include <filesystem>
#include <vector>
#include "Boid3D.hpp"
#include "Gui.hpp"
#include "Program.hpp"
#include "glimac/FreeflyCamera.hpp"
#include "glimac/common.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"
#include "tiny_obj_loader.h"

int const window_width  = 1920;
int const window_height = 1080;

////////// F

void loadTexture(const std::filesystem::path& filePath, GLuint& textureID)
{
    const auto textureCube = p6::load_image_buffer(filePath);

    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureCube.width(), textureCube.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureCube.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

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
    GLuint textureID = 0;
    loadTexture("assets/models/texture_penguin.jpg", textureID);

    // GLuint     textureID   = 0;
    // const auto textureCube = p6::load_image_buffer("assets/models/texture_penguin.jpg");

    // glGenTextures(1, &textureID);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, textureID);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureCube.width(), textureCube.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureCube.data());
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glBindTexture(GL_TEXTURE_2D, 0);

    /////// end of the texture loading -> put it in a texture class ???

    /////// TinyObj library : use only #include "tiny_obj_loader.h"
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, &warn, &err, "assets/models/penguin.obj");

    if (!warn.empty())
    {
        std::cout << "Warning: " << warn << std::endl;
    }
    if (!err.empty())
    {
        std::cerr << "Error: " << err << std::endl;
    }
    if (!ret)
    {
        exit(1);
    }

    /////// vertices is a vector if glimac (cf. #include "glimac/common.hpp") ->take verices/normals/texCoords
    std::vector<glimac::ShapeVertex> vertices;
    ////// 3561 IS THE NUMBER OF VERTICES //////
    ////// TOD0 : THIS NUMBER COULD BE A VARIABLE -> ctrl + F "v " in .obj file give you the amount of vertices
    for (int i = 0; i < 3561; i++)
    {
        glimac::ShapeVertex newVertex = glimac::ShapeVertex(

            // POSITION
            glm::vec3(
                tinyobj::real_t(attrib.vertices[i * 3]),
                tinyobj::real_t(attrib.vertices[i * 3 + 1]),
                tinyobj::real_t(attrib.vertices[i * 3 + 2])
            ),

            // NORMAL
            glm::vec3(
                tinyobj::real_t(attrib.normals[i * 3 + 0]), // nx
                tinyobj::real_t(attrib.normals[i * 3 + 1]), // ny
                tinyobj::real_t(attrib.normals[i * 3 + 2])  // nz
            ),

            // TEXTURE_COORDINATES
            glm::vec2(
                tinyobj::real_t(attrib.texcoords[i * 2 + 0]), // tx
                tinyobj::real_t(attrib.texcoords[i * 2 + 1])  // ty
            )
        );
        vertices.push_back(newVertex);
    }

    /////// loop extracting the indices of vertex for the ibo
    // Extract the indices
    std::vector<unsigned int> indices;
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            indices.push_back(index.vertex_index);
        }
    }

    /////// GL code
    // Create a VBO for the model
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glimac::ShapeVertex), vertices.data(), GL_STATIC_DRAW);

    // Create an IBO for the model
    GLuint ibo = 0;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create a VAO for the model
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    ////// the order is important
    const GLuint VERTEX_ATTR_POSITION  = 0;
    const GLuint VERTEX_ATTR_NORMAL    = 1;
    const GLuint VERTEX_ATTR_TEXCOORDS = 2;

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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

        ///////// bind the texture of the boids
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(penguin.uTexture, 0);

        // BEGIN OF MY DRAW CODE//

        glBindVertexArray(vao);

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