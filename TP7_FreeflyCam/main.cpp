#include <iostream>
#include <vector>
#include "glimac/FreeflyCamera.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

int const window_width  = 1920;
int const window_height = 1080;

struct EarthProgram {
    p6::Shader m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uEarthTexture;
    GLint uCloudTexture;

    EarthProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/multiTex3D.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uEarthTexture = glGetUniformLocation(m_Program.id(), "uTexture1");
        uCloudTexture = glGetUniformLocation(m_Program.id(), "uTexture2");
    }
};

struct MoonProgram {
    p6::Shader m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    MoonProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/tex3D.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uTexture      = glGetUniformLocation(m_Program.id(), "uTexture");
    }
};

int main()
{
    auto ctx = p6::Context{{window_width, window_height, "TP7"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    //img::Image earthTex = p6::load_image_buffer("assets/textures/EarthMap.jpg");
    const auto earthTex = p6::load_image_buffer("assets/models/EarthMap.jpg");
    //img::Image cloudTex = p6::load_image_buffer("assets/textures/CloudMap.jpg");
    const auto cloudTex = p6::load_image_buffer("assets/models/CloudMap.jpg");
    //img::Image moonTex  = p6::load_image_buffer("assets/textures/MoonMap.jpg");
    const auto moonTex = p6::load_image_buffer("assets/models/MoonMap.jpg");

    // create the programs
    EarthProgram earth{};
    MoonProgram  moon{};

    // init ONE vbo with coord data
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    // array_buffer is for vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // sphere
    const std::vector<glimac::ShapeVertex> sphere1 = glimac::sphere_vertices(1.f, 32, 16);

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ARRAY_BUFFER, sphere1.size() * sizeof(glimac::ShapeVertex), sphere1.data(), GL_STATIC_DRAW);

    // Depth option
    glEnable(GL_DEPTH_TEST);
    // a little transparency...
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // init ONE texture
    GLuint uvtex1 = 0;
    glGenTextures(1, &uvtex1);
    // activate a texture for multi texturing
    glActiveTexture(GL_TEXTURE0);
    // texture_2D is for simple 2D
    glBindTexture(GL_TEXTURE_2D, uvtex1);

    // specify the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earthTex.width(), earthTex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, earthTex.data());
    // needed for pixel overlapping (I guess)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // debind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // init ONE texture
    GLuint uvtex2 = 0;
    glGenTextures(1, &uvtex2);
    // activate again
    glActiveTexture(GL_TEXTURE1);
    // texture_2D is for simple 2D
    glBindTexture(GL_TEXTURE_2D, uvtex2);

    // specify the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cloudTex.width(), cloudTex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, cloudTex.data());
    // needed for pixel overlapping (I guess)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // debind the texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    // init ONE texture
    GLuint uvtex3 = 0;
    glGenTextures(1, &uvtex3);
    // texture_2D is for simple 2D
    glBindTexture(GL_TEXTURE_2D, uvtex3);

    // specify the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, moonTex.width(), moonTex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, moonTex.data());
    // needed for pixel overlapping (I guess)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // debind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // init ONE vao with info data
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

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

    // MVP
    FreeflyCamera ViewMatrix = FreeflyCamera();
    //ViewMatrix.moveFront(-5);

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;
    // For the moons
    glm::mat4              MVMatrix_moon;
    glm::mat4              NormalMatrix_moon;
    std::vector<glm::vec3> RotAxes;
    std::vector<glm::vec3> RotDir;
    for (int i = 0; i < 32; i++)
    {
        RotAxes.emplace_back(glm::ballRand(2.f));
        RotDir.emplace_back(glm::linearRand(0, 1), glm::linearRand(0, 1), glm::linearRand(0, 1));
    }

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

        // BEGIN OF MY DRAW CODE//

        glBindVertexArray(vao);

        earth.m_Program.use();
        glUniform1i(earth.uEarthTexture, 0);
        glUniform1i(earth.uCloudTexture, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uvtex1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, uvtex2);
        // Terre
        MVMatrix     = ViewMatrix.getViewMatrix();
        MVMatrix     = glm::rotate(MVMatrix, -ctx.time(), glm::vec3(0, 1, 0));
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(earth.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(earth.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(earth.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        glDrawArrays(GL_TRIANGLES, 0, sphere1.size());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Lune
        moon.m_Program.use();
        glUniform1i(moon.uTexture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uvtex3);
        // T * R * T * S
        for (int i = 0; i < 32; i++)
        {
            MVMatrix_moon     = ViewMatrix.getViewMatrix();
            MVMatrix_moon     = glm::rotate(MVMatrix_moon, ctx.time(), glm::vec3(RotDir[i][0], RotDir[i][1], RotDir[i][2]));
            MVMatrix_moon     = glm::translate(MVMatrix_moon, RotAxes[i]);
            MVMatrix_moon     = glm::scale(MVMatrix_moon, glm::vec3(0.2, 0.2, 0.2));
            NormalMatrix_moon = glm::transpose(glm::inverse(MVMatrix_moon));

            glUniformMatrix4fv(moon.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix_moon));
            glUniformMatrix4fv(moon.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix_moon));
            glUniformMatrix4fv(moon.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix_moon));
            glDrawArrays(GL_TRIANGLES, 0, sphere1.size());
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

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

    // Clear vbo & vao & texture
    glDeleteTextures(1, &uvtex1);
    glDeleteTextures(1, &uvtex2);
    glDeleteTextures(1, &uvtex3);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return 0;
}