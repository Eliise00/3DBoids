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

struct BoidProgram {
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

    BoidProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/pointLight.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");

        uKa = glGetUniformLocation(m_Program.id(), "uKa");

        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
    }
};

class Boid3D {
private:
    glm::vec3 _pos;
    glm::vec3 _speed;

public:
    explicit Boid3D(glm::vec3 initial_position)
        : _pos(initial_position), //
        _speed(p6::random::number(-.001, .001), p6::random::number(-.001, .001), p6::random::number(-.001, .001))
    {
    }

    /*
    void drawBody(p6::Context& ctx, float draw_radius) const
    {
        ctx.use_stroke = false;
        ctx.use_fill   = true;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{draw_radius}
        );
    }

    void drawHelper(p6::Context& ctx, float radius, float stroke_weight) const
    {
        ctx.stroke_weight = stroke_weight;
        ctx.use_stroke    = true;
        ctx.use_fill      = false;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{radius}
        );
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
    }

    void adaptSpeedToBoids(std::vector<Boid3D>& boids, Environment_params environment, Boid_behavior_params params)
    {
        glm::vec2 avoid_vec = {0, 0};
        glm::vec2 align_vec = {0, 0};
        int       friends   = 0;
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
        float max_speed    = params.max_speed / 10000;
        float min_speed    = params.min_speed / 10000;
        float actual_speed = sqrt(_speed.x * _speed.x + _speed.y * _speed.y);
        if (actual_speed > max_speed)
        {
            _speed.x = (_speed.x / actual_speed) * max_speed;
            _speed.y = (_speed.y / actual_speed) * max_speed;
        }
        else if (actual_speed < min_speed)
        {
            _speed.x = (_speed.x / actual_speed) * min_speed;
            _speed.y = (_speed.y / actual_speed) * min_speed;
        }
    }

    void updatePosition(Environment_params environment)
    {
        _pos.x += _speed.x * environment.speed_multiplier;
        _pos.y += _speed.y * environment.speed_multiplier;
    }
    */
};

int main()
{
    int nbOfBalls = 1; // futur nombre de boid à définir en fonction du nombre de boids

    auto ctx = p6::Context{{window_width, window_height, "TP8_DirectionalLight"}};
    ctx.maximize_window();

    // BEGINNING OF MY INIT CODE//

    // create the programs
    BoidProgram boid{};

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
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;

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

        boid.m_Program.use();

        // Terre
        MVMatrix     = ViewMatrix.getViewMatrix();
        MVMatrix     = glm::rotate(MVMatrix, -ctx.time(), glm::vec3(0, 1, 0));
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(boid.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(boid.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(boid.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glUniform3fv(boid.uKa, 1, glm::value_ptr(glm::vec3(0.0215, 0.1745, 0.0215)));

        glUniform3fv(boid.uKd, 1, glm::value_ptr(glm::vec3(0.07568, 0.61424, 0.07568)));
        glUniform3fv(boid.uKs, 1, glm::value_ptr(glm::vec3(0.633, 0.727811, 0.633)));
        glUniform1f(boid.uShininess, 0.6);

        glUniform3fv(boid.uLightPos_vs, 1, glm::value_ptr(glm::vec3(glm::rotate(ViewMatrix.getViewMatrix(), ctx.time(), glm::vec3(0, 1, 0)) * glm::vec4(1, 1, 0, 1))));
        glUniform3fv(boid.uLightIntensity, 1, glm::value_ptr(glm::vec3(1, 1, 1)));

        glDrawArrays(GL_TRIANGLES, 0, sphere1.size());

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