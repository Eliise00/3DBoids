#include "glimac/sphere_vertices.hpp"
#include "p6/p6.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/random.hpp"


int main(int argc, char* argv[])
{
    auto ctx = p6::Context{{1280, 720, "TP3 EX1"}};
    ctx.maximize_window();

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/


    struct EarthProgram {
        p6::Shader m_Program;

        GLint uniformMVPMatrix;
        GLint uniformMVMatrix;
        GLint uniformNormalMatrix;
        GLint uEarthTexture;
        GLint uCloudTexture;

        EarthProgram()
            : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/multiTex3D.fs.glsl")}
        {
            uniformMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
            uniformMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
            uniformNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
            uEarthTexture = glGetUniformLocation(m_Program.id(), "uEarthTexture");
            uCloudTexture = glGetUniformLocation(m_Program.id(), "uCloudTexture");
        }
    };



    struct MoonProgram {
        p6::Shader m_Program;

        GLint uniformMVPMatrix;
        GLint uniformMVMatrix;
        GLint uniformNormalMatrix;
        GLint uniformTexture;

        MoonProgram()
            : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/tex3D.fs.glsl")}
        {
            uniformMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
            uniformMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
            uniformNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
            uniformTexture      = glGetUniformLocation(m_Program.id(), "uTexture");
        }
    };


    /*************************
     * TEXTURE
     *************************/

    ////Earth Texture
    GLuint earthTextureID = 0;

    const auto textureEarth = p6::load_image_buffer("assets/models/EarthMap.jpg");

    glGenTextures(1, &earthTextureID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, earthTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureEarth.width(), textureEarth.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureEarth.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Unbind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);


    ////Moon texture
    GLuint moonTextureID = 0;

    const auto textureMoon = p6::load_image_buffer("assets/models/MoonMap.jpg");

    glGenTextures(1, &moonTextureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, moonTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureMoon.width(), textureMoon.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureMoon.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Unbind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);



    ////Cloud Texture
    GLuint cloudTextureID = 0;

    const auto textureCloud = p6::load_image_buffer("assets/models/CloudMap.jpg");

    glGenTextures(1, &cloudTextureID);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,cloudTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureCloud.width(), textureCloud.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureCloud.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Unbind the texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    
    EarthProgram earthProgram{};
    MoonProgram moonProgram{};



    glEnable(GL_DEPTH_TEST);

    const std::vector<glimac::ShapeVertex> vertices = glimac::sphere_vertices(1., 32, 16);

    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glimac::ShapeVertex) , vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const GLuint VERTEX_ATTR_POSITION = 0;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);

    const GLuint VERTEX_ATTR_NORMAL = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);

    const GLuint VERTEX_ATTR_TEXCOORDS = 2;
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, position));

    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, normal));

    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    std::vector<glm::vec3> angleRotation;
    std::vector<glm::vec3> axeTranslation;

    for (int i = 0; i < 32; i++) {
        // Generate a random axis of rotation for the moon
        angleRotation.push_back(glm::sphericalRand(2.f));
        axeTranslation.push_back(glm::sphericalRand(2.f));
    }


    // Declare your infinite update loop.
    ctx.update = [&]() {
        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        earthProgram.m_Program.use();
        moonProgram.m_Program.use();

        glUniform1i(earthProgram.uEarthTexture, 0);
        glUniform1i(earthProgram.uCloudTexture, 1);

        const glm::mat4 projMatrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);

        const glm::mat4 globalMVMatrix = glm::translate(glm::mat4{1.f}, {0.f, 0.f, -5.f});

        const glm::mat4 earthMVMatrix = glm::rotate(globalMVMatrix, ctx.time(), {0.f, 1.f, 0.f});
        glUniformMatrix4fv(earthProgram.uniformMVMatrix, 1, GL_FALSE, glm::value_ptr(earthMVMatrix));
        glUniformMatrix4fv(earthProgram.uniformNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
        glUniformMatrix4fv(earthProgram.uniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix * earthMVMatrix));


        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earthTextureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cloudTextureID);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());


        // Get the current time
        float time = ctx.time();

        for(int i = 0; i < 32; i++){

            glUniform1i(moonProgram.uniformTexture, 0);


            glm::mat4 moonMVMatrix = glm::rotate(globalMVMatrix, time, angleRotation[i]);
            moonMVMatrix = glm::rotate(moonMVMatrix, ctx.time(), angleRotation[i]); // Translation * Rotation
            moonMVMatrix = glm::translate(moonMVMatrix, axeTranslation[i]); // Translation * Rotation * Translation
            moonMVMatrix = glm::scale(moonMVMatrix, glm::vec3{0.2f}); // Translation * Rotation * Translation * Scale


            glUniformMatrix4fv(moonProgram.uniformMVMatrix, 1, GL_FALSE, glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(moonProgram.uniformNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(moonMVMatrix))));
            glUniformMatrix4fv(moonProgram.uniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix * moonMVMatrix));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, moonTextureID);

            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        }


        glBindVertexArray(0);
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();

    glDeleteTextures(1, &earthTextureID);
    glDeleteTextures(1, &moonTextureID);
    glDeleteTextures(1, &cloudTextureID);
}