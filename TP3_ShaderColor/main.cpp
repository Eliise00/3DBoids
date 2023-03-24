
#define GLFW_INCLUDE_NONE

#include "p6/p6.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstddef>
#include <filesystem>
#include <valarray>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"



#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>


int main(int argc, char* argv[])
{
    auto ctx = p6::Context{{1280, 720, "TP3 EX2"}};
    ctx.maximize_window();

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/



    p6::Shader program = p6::load_shader(
         "shaders" / std::filesystem::path{argv[1]},
         "shaders" / std::filesystem::path{argv[2]}
        );
    program.use();


    //STRUCTURE VERTEX2DCOLOR

    struct Vertex2DColor {
        glm::vec2 _position;
        glm::vec3 _color;

        Vertex2DColor(glm::vec2 pos, glm::vec3 color):_position(pos), _color(color){};
    };


    GLuint vbo, vao;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);


    Vertex2DColor vertices[] = {
        Vertex2DColor(glm::vec2(0.5, 0.5), glm::vec3(1, 0, 0)),
        Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(1, 1, 1)),
        Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(0, 0, 1)),
        Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(0, 0, 1)),
        Vertex2DColor(glm::vec2(-0.5, 0.5), glm::vec3(1, 1, 1)),
        Vertex2DColor(glm::vec2(0.5, 0.5), glm::vec3(1, 0, 0)),
    };

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex2DColor) , vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const GLuint VERTEX_ATTR_POSITION = 0;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

    const GLuint VERTEX_ATTR_COLOR = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_COLOR);


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (void*)offsetof(Vertex2DColor, _position) );

    glVertexAttribPointer(VERTEX_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (void*)offsetof(Vertex2DColor, _color));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);



    // Declare your infinite update loop.
    ctx.update = [&]() {
        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/
        program.use();

        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);



    };


    // Should be done last. It starts the infinite loop.
    ctx.start();

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

}

