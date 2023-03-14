#include "p6/p6.h"
#include <cstdlib>
#include <random>
#define DOCTEST_CONFIG_IMPLEMENT


class Boid {
private:

    glm::vec2 _pos;
    glm::vec2 _speed;
    p6::Angle _dir;
    float     _radius;
    float _aspect_ratio;

public:


    explicit Boid(float aspect_ratio)
        :
        _pos(glm::vec2(p6::random::number(-aspect_ratio, aspect_ratio), p6::random::number(-1, 1))), // remplace seed avec la doc de p6
        _speed(p6::random::number() / 2000, p6::random::number() / 2000),
        _dir(p6::Angle(p6::Radians(p6::random::number(p6::PI * 2)))),
        _radius(0.01),
        _aspect_ratio(aspect_ratio)
    {
    }


    void draw(p6::Context& ctx) const
    {
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{_radius}
        );

        ctx.use_stroke = false;

    }

    //// collision
    bool collides_with(const Boid& other) const
    {
        return glm::distance(_pos, other._pos) < (_radius + other._radius); //sqrt((_pos.x - other._pos.x)^2 + (_pos.y - other._pos.y)^2)
    }

    //// cohésion
    glm::vec2 cohesion(const std::vector<Boid>& boids, float cohesion_radius, float max_speed)
    {
        glm::vec2 center_of_mass(0.0f, 0.0f);
        int count = 0;
        for (const auto& other : boids) {
            if (&other != this && glm::distance(_pos, other._pos) < cohesion_radius) {
                center_of_mass += other._pos;
                count++;
            }
        }
        if (count > 0) {
            center_of_mass /= static_cast<float>(count);
            auto desired_velocity = glm::normalize(center_of_mass - _pos) * max_speed;
            return (desired_velocity - _speed);
        }
        return {0.0f, 0.0f};
    }


    //// update
    void update(std::vector<Boid>& boids)
    {

        auto move = rotated_by(_dir, _speed);
        _pos += move;
        if(abs(_pos.x) > _aspect_ratio){
            _pos.x = _pos.x * -1;
        }
        if(abs(_pos.y) > 1){
            _pos.y = _pos.y * -1;
        }


        // Calculate cohesion steering force
        auto cohesion_force = cohesion(boids, 0.5, 0.001);
        _speed += cohesion_force;
        if (glm::length(_speed) > 0.01) { //calculate the norm of the vector speed
            _dir = p6::Angle(static_cast<p6::Radians>(glm::atan(_speed.y, _speed.x))); //inverse the argument ???
        }

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
    for (size_t i = 0; i < 100; ++i)
    {
        Boid boidTemp(ctx.aspect_ratio());
        Boid_array.push_back(boidTemp);
    }

    auto square_radius = 0.5f;

    // Declare your infinite update loop.
    ctx.update = [&]() {


        ctx.background(p6::NamedColor::Blue);

        ///////RENDER
        // Show a simple window
        ImGui::Begin("Test");
        ImGui::SliderFloat("Square size", &square_radius, 0.f, 1.f);
        ImGui::End();
        ctx.square(p6::Center{}, p6::Radius{square_radius});

        // Draw array
        for (auto& i : Boid_array)
        {

            i.draw(ctx);
            i.update(Boid_array);
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}