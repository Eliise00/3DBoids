#include "glimac/TrackballCamera.hpp"
#include "glimac/sphere_vertices.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "p6/p6.h"

int main()
{
    auto ctx = p6::Context{{1280, 720, "TP7"}};
    ctx.maximize_window();

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/

    p6::Shader program = p6::load_shader(
        "shaders/3D.vs.glsl",
        "shaders/multiTex3D.fs.glsl"
    );
    program.use();

    GLuint programID = program.id();

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

    glActiveTexture(GL_TEXTURE0);
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
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureCloud.width(), textureCloud.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureCloud.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint uniformMVP    = glGetUniformLocation(programID, "uMVPMatrix");
    GLint uniformMV     = glGetUniformLocation(programID, "uMVMatrix");
    GLint uniformNormal = glGetUniformLocation(programID, "uNormalMatrix");
    // GLint uniformTexture = glGetUniformLocation(programID, "uTexture");

    ////////
    GLint uEarthTexture = glGetUniformLocation(programID, "uEarthTexture");
    GLint uCloudTexture = glGetUniformLocation(programID, "uCloudTexture");

    glUniform1i(uEarthTexture, 0);
    glUniform1i(uCloudTexture, 1);

    glEnable(GL_DEPTH_TEST);

    const std::vector<glimac::ShapeVertex> vertices = glimac::sphere_vertices(1., 32, 16);

    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glimac::ShapeVertex), vertices.data(), GL_STATIC_DRAW);
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

    for (int i = 0; i < 32; i++)
    {
        // Generate a random axis of rotation for the moon
        angleRotation.push_back(glm::sphericalRand(2.f));
        axeTranslation.push_back(glm::sphericalRand(2.f));
    }

    /************************
     * CAMERA
     ***********************/

    // Création de la caméra
    TrackballCamera camera;
    camera.m_Distance = 5.0f;

    // Variables pour la gestion de la souris
    float     scrollSensitivity = 1.0f;
    glm::vec2 lastMovePos;
    bool      right = false;
    bool      left  = false;
    bool      up    = false;
    bool      down  = false;

    // Declare your infinite update loop.
    ctx.update = [&]() {
        if (right)
        {
            camera.rotateLeft(-0.25f);
        }
        if (left)
        {
            camera.rotateLeft(0.25f);
        }
        if (up)
        {
            camera.rotateUp(0.1f);
        }
        if (down)
        {
            camera.rotateUp(-0.1f);
        }

        ctx.key_pressed = [&right, &up, &left, &down](p6::Key key) {
            if (key.physical == GLFW_KEY_RIGHT)
            {
                right = true;
            }
            if (key.physical == GLFW_KEY_LEFT)
            {
                left = true;
            }
            if (key.physical == GLFW_KEY_UP)
            {
                up = true;
            }
            if (key.physical == GLFW_KEY_DOWN)
            {
                down = true;
            }
        };

        ctx.key_released = [&right, &up, &left, &down](p6::Key key) {
            if (key.physical == GLFW_KEY_RIGHT)
            {
                right = false;
            }
            if (key.physical == GLFW_KEY_LEFT)
            {
                left = false;
            }
            if (key.physical == GLFW_KEY_UP)
            {
                up = false;
            }
            if (key.physical == GLFW_KEY_DOWN)
            {
                down = false;
            }
        };

        // Fonction pour gérer les événements de la souris
        ctx.mouse_dragged = [&camera](const p6::MouseDrag& button) {
            camera.rotateLeft(button.delta.x * 5);
            camera.rotateUp(-button.delta.y * 5);
        };

        ctx.mouse_scrolled = [&](p6::MouseScroll scroll) {
            camera.moveFront(scroll.dy * scrollSensitivity);
        };

        glm::mat4 viewMatrix = camera.getViewMatrix();

        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();

        glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.f);

        glm::mat4 MVMatrix = glm::translate(glm::mat4(1), glm::vec3(0., 0., -5.)) * viewMatrix;

        glm::mat4 MVPMatrix = ProjMatrix * MVMatrix;

        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(uniformMV, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
        glUniformMatrix4fv(uniformNormal, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earthTextureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cloudTextureID);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        // Get the current time
        float time = ctx.time();

        for (int i = 0; i < 32; i++)
        {
            glm::mat4 moonMVMatrix = glm::rotate(MVMatrix, time, angleRotation[i]);
            moonMVMatrix           = glm::rotate(moonMVMatrix, ctx.time(), angleRotation[i]); // Translation * Rotation
            moonMVMatrix           = glm::translate(moonMVMatrix, axeTranslation[i]);         // Translation * Rotation * Translation
            moonMVMatrix           = glm::scale(moonMVMatrix, glm::vec3{0.2f});               // Translation * Rotation * Translation * Scale
            // moonMVMatrix = viewMatrix*moonMVMatrix;

            glUniformMatrix4fv(uniformMV, 1, GL_FALSE, glm::value_ptr(moonMVMatrix));
            glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(ProjMatrix * moonMVMatrix));
            glUniformMatrix4fv(uniformNormal, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(moonMVMatrix))));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, moonTextureID);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, cloudTextureID);
            // Unbind cloud on the moon
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);

            glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        }
        /*
        // Draw  second sphere
        MVMatrix = glm::rotate(MVMatrix, time, glm::vec3{0.f, 1.f, 0.f}); // Rotate around the y-axis
        MVMatrix = glm::translate(MVMatrix, {-2.f, 0.f, 0.f});
        MVMatrix = glm::scale(MVMatrix, glm::vec3{0.2f});
        */

        glBindVertexArray(0);
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();

    glDeleteTextures(1, &earthTextureID);
    glDeleteTextures(1, &moonTextureID);
    glDeleteTextures(1, &cloudTextureID);
}