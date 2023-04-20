#include <iostream>
#include <vector>
#include <unordered_set>
#include "glimac/FreeflyCamera.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "img/src/Image.h"
#include "p6/p6.h"

#include "tiny_obj_loader.h"
#include "glimac/common.hpp"

int const window_width  = 1920;
int const window_height = 1080;

struct EarthProgram {
    p6::Shader m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;

    GLint uTexture;

    GLint uKa;
    GLint uKd;
    GLint uKs;
    GLint uShininess;
    GLint uLightPos_vs;
    GLint uLightIntensity;

    EarthProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/directionalLight.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");

        uTexture = glGetUniformLocation(m_Program.id(), "uTexture");

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

    //////Texture

    GLuint textureID = 0;
    const auto textureCube = p6::load_image_buffer("assets/models/texture_cube_test.jpg");

    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureCube.width(), textureCube.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureCube.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);


    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;


    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, &warn, &err, "assets/models/cube_test.obj");
    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }
    if (!ret) {
        exit(1);
    }


    // New loop
    std::vector<glimac::ShapeVertex> vertices;
    ////// 8 IS THE NUMBER OF VERTICES //////
    ////// TOD0 : CHANGE THIS NUMBER FOR PENGUIN
    for (int i=0; i<8; i++){
        glimac::ShapeVertex newVertex = glimac::ShapeVertex(

            // POSITION
            glm::vec3(
                tinyobj::real_t(attrib.vertices[i*3]),
                tinyobj::real_t(attrib.vertices[i*3+1]),
                tinyobj::real_t(attrib.vertices[i*3+2])
            ),

            // NORMAL
            glm::vec3(
                tinyobj::real_t(attrib.normals[i*3+0]),  // nx
                tinyobj::real_t(attrib.normals[i*3+1]),  // ny
                tinyobj::real_t(attrib.normals[i*3+2])   // nz
            ),

            // TEXTURE_COORDINATES
            glm::vec2(
                tinyobj::real_t(attrib.texcoords[i*2+0]),  //tx
                tinyobj::real_t(attrib.texcoords[i*2+1])   //ty
            )
        );
        vertices.push_back(newVertex);
    }

    for(int i = 0; i < vertices.size(); i++){
        std::cout << glm::to_string(vertices[i].position) << std::endl;
        std::cout << glm::to_string(vertices[i].normal) << std::endl;
        std::cout << glm::to_string(vertices[i].texCoords) << std::endl;
    }
    std::cout <<vertices.size()<< std::endl;


    // Extract the indices
    std::vector<unsigned int> indices;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            indices.push_back(index.vertex_index);
        }
    }

    for(int i = 0; i < indices.size(); i++){
        std::cout << indices[i] << std::endl;
    }
    std::cout <<indices.size()<< std::endl;

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

    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORMAL = 1;
    const GLuint VERTEX_ATTR_TEXCOORDS = 2;

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid *)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid *)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid *)offsetof(glimac::ShapeVertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    glEnable(GL_DEPTH_TEST);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        earth.m_Program.use();

        glUniform1i(earth.uTexture, 0);


        // BEGIN OF MY DRAW CODE//

        MVMatrix     = ViewMatrix.getViewMatrix();
        MVMatrix     = glm::rotate(MVMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        MVMatrix     = glm::translate(MVMatrix, glm::vec3(0.0f, -2.0f, 0.0f));
        MVMatrix     = glm::rotate(MVMatrix, -ctx.time(), glm::vec3(0, 1, 0));
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glBindVertexArray(vao);

        glUniformMatrix4fv(earth.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(earth.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(earth.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glUniform3fv(earth.uKa, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
        glUniform3fv(earth.uKd, 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
        glUniform3fv(earth.uKs, 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
        glUniform1f(earth.uShininess, 0.6);

        glUniform3fv(earth.uLightPos_vs, 1, glm::value_ptr(glm::vec3(-3, -3, -3)));
        glUniform3fv(earth.uLightIntensity, 1, glm::value_ptr(glm::vec3(1, 1, 1)));

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


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