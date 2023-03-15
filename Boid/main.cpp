#include <cstdlib>
#include <random>
#include "p6/p6.h"

#define DOCTEST_CONFIG_IMPLEMENT

class Boid {
private:
    glm::vec2 _pos;
    glm::vec2 _speed;
    float     _turn_factor;
    // p6::Angle _dir;
    float _radius;

    float _cohesion_radius;
    float _max_speed;

public:
    explicit Boid(float aspect_ratio)
        : _pos(glm::vec2(p6::random::number(-aspect_ratio, aspect_ratio), p6::random::number(-1, 1))), // remplace seed avec la doc de p6
        _speed(p6::random::number(-.001, .001), p6::random::number(-.001, .001))
        , _turn_factor(0.0002)
        //, _dir(p6::Angle(p6::Radians(p6::random::number(p6::PI * 2))))
        , _radius(.01)
        , _cohesion_radius(.04)
        , _max_speed(.001)
    {
    }

    void draw(p6::Context& ctx) const
    {
        ctx.stroke_weight = 0.005f;
        ctx.use_stroke    = false;
        ctx.use_fill      = true;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{_radius}
        );
        ctx.use_stroke = true;
        ctx.use_fill   = false;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{_cohesion_radius}
        );
    }

    //// collision
    bool collides_with(const Boid& other) const
    {
        return glm::distance(_pos, other._pos) < (_radius + other._radius); // sqrt((_pos.x - other._pos.x)^2 + (_pos.y - other._pos.y)^2)
    }

    //// cohesion
    // glm::vec2 cohesion(const std::vector<Boid>& boids, float cohesion_radius, float max_speed)
    // {
    //     glm::vec2 center_of_mass(0.0f, 0.0f);
    //     int       count = 0;
    //     for (const auto& other : boids)
    //     {
    //         if (&other != this && glm::distance(_pos, other._pos) < cohesion_radius)
    //         {
    //             center_of_mass += other._pos;
    //             count++;
    //         }
    //     }
    //     if (count > 0)
    //     {
    //         center_of_mass /= static_cast<float>(count);
    //         auto desired_velocity = glm::normalize(center_of_mass - _pos) * max_speed;
    //         return (desired_velocity - _speed);
    //     }
    //     return {0.0f, 0.0f};
    // }

    //// update
    void updatePosition(std::vector<Boid>& boids, float speed_multiplicator, float min_speed, float max_speed, float screen_margin, float aspect_ratio)
    {
        //////////////////////////////////////////////////
        // LIMITS OF THE SCREEN
        //////////////////////////////////////////////////

        // 1) Plain teleportation
        // if (abs(_pos.x) > _aspect_ratio)
        //     _pos.x = _pos.x * -1;
        // if (abs(_pos.y) > 1)
        //     _pos.y = _pos.y * -1;

        // 2) Make them turn away !
        float calculated_turn_factor = _turn_factor * speed_multiplicator;
        // Right
        if (_pos.x > aspect_ratio - screen_margin * aspect_ratio)
        {
            _speed.x -= calculated_turn_factor;
        }
        // Left
        else if (_pos.x < -aspect_ratio + screen_margin * aspect_ratio)
        {
            _speed.x += calculated_turn_factor;
        }
        // Top
        if (_pos.y > 1 - screen_margin)
        {
            _speed.y -= calculated_turn_factor;
        }
        // Bottom
        else if (_pos.y < -1 + screen_margin)
        {
            _speed.y += calculated_turn_factor;
        }

        //////////////////////////////////////////////////
        // MIN AND MAX SPEED
        //////////////////////////////////////////////////
        // updating the speed to have a min and max speed
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

        //////////////////////////////////////////////////
        // POSITION UPDATE
        //////////////////////////////////////////////////
        // last thing : updating the position from the speed
        _pos.x += _speed.x * speed_multiplicator;
        _pos.y += _speed.y * speed_multiplicator;

        // Calculate cohesion steering force
        // auto cohesion_force = cohesion(boids, _cohesion_radius, _max_speed);
        // _speed += cohesion_force;
        // if (glm::length(_speed) > 0.01)
        // {                                                                              // calculate the norm of the vector speed
        //     _dir = p6::Angle(static_cast<p6::Radians>(glm::atan(_speed.y, _speed.x))); // inverse the argument ???
        // }

        /*
        // Check for collisions with other boids
        for (auto& other : boids) {
            if (&other != this && collides_with(other)) {
                // Steer away from the other boid
                auto diff = _pos - other._pos;
                _dir += p6::Angle(static_cast<p6::Radians>(glm::atan(diff.y, diff.x)));
            }
        }*/
    }
};

int main()
{
    auto ctx = p6::Context{{1280, 720, "Boid"}};
    ctx.maximize_window();

    /////INITIALISATION

    std::vector<Boid> Boid_array;
    for (size_t i = 0; i < 80; ++i)
    {
        /*
        Pour plus tard : c'est lunaire de leur donner l'aspect ratio et
        qu'ensuite eux ils calculent leur position aléatoire.
        Faudrait calculer une position random et leur donner.
        */
        Boid boidTemp(ctx.aspect_ratio());
        Boid_array.push_back(boidTemp);
    }

    float speed_multiplicator = 1;
    float min_speed           = 30;
    float max_speed           = 60;
    float screen_margin       = .1;

    // Declare your infinite update loop.
    ctx.update = [&]() {
        ctx.background(p6::NamedColor::Blue);

        ///////RENDER
        // Show a simple window
        ImGui::Begin("Test");
        ImGui::SliderFloat("Speed Multiplicator", &speed_multiplicator, 1, 10);
        // ImGui::SliderFloat("Min Speed", &min_speed, 0, 30);
        // ImGui::SliderFloat("Max Speed", &max_speed, 40, 70);
        ImGui::SliderFloat("Screen Margin", &screen_margin, .1, 1);
        ImGui::End();

        // Draw array
        for (auto& i : Boid_array)
        {
            i.draw(ctx);
            i.updatePosition(Boid_array, speed_multiplicator, min_speed / 10000, max_speed / 10000, screen_margin, ctx.aspect_ratio());
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}