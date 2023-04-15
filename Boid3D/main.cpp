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

struct PenguinProgram {
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

    PenguinProgram()
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

struct Boid_behavior_params {
    float align_factor;
    float align_radius;
    float avoid_factor;
    float avoid_radius;
    float draw_radius;
    float max_speed;
    float min_speed;
};

struct Environment_params {
    float speed_multiplier;
    float aspect_ratio;
    float screen_margin;
    bool  show_align_circle;
    bool  show_avoid_circle;
    float z_limit;
};

class Boid3D {
private:
    glm::vec3 _pos;
    glm::vec3 _speed;

public:
    explicit Boid3D(glm::vec3 initial_position)
        : _pos(initial_position), //
        _speed(glm::vec3(p6::random::number(-.001, .001),
                         p6::random::number(-.001, .001),
                         p6::random::number(-.001, .001)))
    {
    }

    void adaptSpeedToBorders(Environment_params environment, Boid_behavior_params params)
    {
        float turn_factor = (params.avoid_factor * environment.speed_multiplier) / 10000;
        if (_pos.x > environment.aspect_ratio - environment.screen_margin * environment.aspect_ratio) // Right
            _speed.x -= turn_factor;

        else if (_pos.x < -environment.aspect_ratio + environment.screen_margin * environment.aspect_ratio) // Left
            _speed.x += turn_factor;

        if (_pos.y > 1 - environment.screen_margin) // Top
            _speed.y -= turn_factor;

        else if (_pos.y < -1 + environment.screen_margin) // Bottom
            _speed.y += turn_factor;

        if (_pos.z > environment.z_limit) // Front
            _speed.z -= turn_factor;

        else if (_pos.z < -environment.z_limit) // Back
            _speed.z += turn_factor;
    }

    void adaptSpeedToBoids(std::vector<Boid3D>& boids, Environment_params environment, Boid_behavior_params params)
    {
        glm::vec3 avoid_vec = glm::vec3(0, 0, 0);
        glm::vec3 align_vec = glm::vec3(0, 0, 0);
        int friends = 0;
        for (auto& other : boids)
        {
            if (_pos != other._pos) // if not itself
            {
                float distance_to_other = glm::distance(_pos, other._pos);
                if (distance_to_other < (params.avoid_radius / 100)) // if in avoid radius -> avoid
                {
                    avoid_vec += _pos - other._pos;
                }
                else if (distance_to_other < (params.align_radius / 100)) // else if in align radius
                {
                    align_vec += other._speed;
                    friends += 1;
                }
            }
        }
        _speed += avoid_vec * ((params.avoid_factor * environment.speed_multiplier) / 1000);
        if (friends > 0)
        {
            align_vec /= friends;
            _speed += (align_vec - _speed) * ((params.align_factor * environment.speed_multiplier) / 1000);
        }
    }

    void clampSpeed(Boid_behavior_params params)
    {
        float max_speed = params.max_speed / 10000;
        float min_speed = params.min_speed / 10000;
        float actual_speed = glm::length(_speed);
        if (actual_speed > max_speed)
        {
            _speed = glm::normalize(_speed) * max_speed;
        }
        else if (actual_speed < min_speed)
        {
            _speed = glm::normalize(_speed) * min_speed;
        }
    }

    void updatePosition(Environment_params environment)
    {
        _pos += _speed * environment.speed_multiplier;
    }
};


void drawSphere(int i, const PenguinProgram& penguinProgram, const std::vector<glimac::ShapeVertex>& sphereVec,
                FreeflyCamera ViewMatrix, glm::mat4 ProjMatrix, glm::mat4 MVMatrix, glm::mat4 NormalMatrix,
                std::vector<glm::vec3> Ka, std::vector<glm::vec3> Kd, std::vector<glm::vec3> Ks, std::vector<float> Shininess)
{

    // Set the MVP matrices
    MVMatrix     = ViewMatrix.getViewMatrix();
    MVMatrix    = glm::scale(MVMatrix, glm::vec3(0.2, 0.2, 0.2));
    NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

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
}


int main()
{
    auto ctx = p6::Context{{window_width, window_height, "TP8_DirectionalLight"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    // create the programs
    PenguinProgram  penguin{};

    // init ONE vbo with coord data
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    // array_buffer is for vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // sphere
    const std::vector<glimac::ShapeVertex> sphereVec = glimac::sphere_vertices(1.f, 32, 16);

    // fill those coords in the vbo / Static is for constant variables
    glBufferData(GL_ARRAY_BUFFER, sphereVec.size() * sizeof(glimac::ShapeVertex), sphereVec.data(), GL_STATIC_DRAW);

    // Depth option
    glEnable(GL_DEPTH_TEST);
    // a little transparency...
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.f), window_width / static_cast<float>(window_height), 0.1f, 100.f);

    // For the boids
    glm::mat4 MVMatrix_penguin;
    glm::mat4 NormalMatrix_penguin;

    std::vector<glm::vec3> Ka;
    std::vector<glm::vec3> Kd;
    std::vector<glm::vec3> Ks;
    std::vector<float> Shininess;

    for (int i = 0; i < 32; i++)
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

        // boids
        penguin.m_Program.use();

        for (int i = 0; i < 32; i++){
            drawSphere(i, penguin, sphereVec, ViewMatrix, ProjMatrix, MVMatrix_penguin, NormalMatrix_penguin, Ka, Kd, Ks, Shininess);
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