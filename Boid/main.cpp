#include <cstdlib>
#include <iostream>
#include <random>
#include "imgui.h"
#include "p6/p6.h"
#define DOCTEST_CONFIG_IMPLEMENT

class Boid {
private:
    glm::vec2 _pos;
    glm::vec2 _speed;

public:
    explicit Boid(glm::vec2 initial_position)
        : _pos(initial_position), //
        _speed(p6::random::number(-.001, .001), p6::random::number(-.001, .001))
    {
    }

    void draw(p6::Context& ctx, float draw_radius) const
    {
        ctx.use_stroke = false;
        ctx.use_fill   = true;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{draw_radius}
        );
    }

    void drawAvoidCircle(p6::Context& ctx, float avoid_radius) const
    {
        ctx.stroke_weight = 0.002f;
        ctx.use_stroke    = true;
        ctx.use_fill      = false;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{avoid_radius}
        );
    }

    void drawAlignCircle(p6::Context& ctx, float align_radius) const
    {
        ctx.stroke_weight = 0.001f;
        ctx.use_stroke    = true;
        ctx.use_fill      = false;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{align_radius}
        );
    }

    void adaptSpeedToBorders(float aspect_ratio, float screen_margin, float turn_factor)
    {
        if (_pos.x > aspect_ratio - screen_margin * aspect_ratio) // Right
            _speed.x -= turn_factor;

        else if (_pos.x < -aspect_ratio + screen_margin * aspect_ratio) // Left
            _speed.x += turn_factor;

        if (_pos.y > 1 - screen_margin) // Top
            _speed.y -= turn_factor;

        else if (_pos.y < -1 + screen_margin) // Bottom
            _speed.y += turn_factor;
    }

    // TODO : struct for the parameters
    void adaptSpeedToBoids(std::vector<Boid>& boids, float avoid_factor, float avoid_radius, float align_factor, float align_radius)
    {
        glm::vec2 avoid_vec = {0, 0};
        glm::vec2 align_vec = {0, 0};
        int       friends   = 0;
        for (auto& other : boids)
        {
            if (_pos != other._pos) // if not itself
            {
                float distance_to_other = glm::distance(_pos, other._pos);
                if (distance_to_other < avoid_radius) // if in avoid radius -> avoid
                {
                    avoid_vec += _pos - other._pos;
                }
                else if (distance_to_other < align_radius) // else if in align radius
                {
                    align_vec += other._speed;
                    friends += 1;
                }
            }
        }
        _speed += avoid_vec * avoid_factor;
        if (friends > 0)
        {
            align_vec /= friends;
            _speed += (align_vec - _speed) * align_factor;
        }
    }

    void clampSpeed(float min_speed, float max_speed)
    {
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

    void updatePosition(float speed_multiplier)
    {
        _pos.x += _speed.x * speed_multiplier;
        _pos.y += _speed.y * speed_multiplier;
    }
};

// TODO : specific functions for parameters, GUI and boid updating in the main
int main()
{
    auto ctx = p6::Context{{1280, 720, "Boid"}};
    ctx.maximize_window();

    /////INITIALISATION
    std::vector<Boid> Boid_array;
    for (size_t i = 0; i < 80; ++i)
    {
        glm::vec2 random_pos = {p6::random::number(-ctx.aspect_ratio(), ctx.aspect_ratio()), p6::random::number(-1, 1)};
        Boid_array.emplace_back(random_pos);
    }

    // Boid parameters
    float draw_radius  = .01;
    float min_speed    = 20.; // to divide by 10000 /!\ not tunable in gui anymore
    float max_speed    = 30.; // to divide by 10000 /!\ not tunable in gui anymore
    float turn_factor  = 2.;  // to divide by 10000
    float avoid_radius = 6.;  // to divide by 100
    float avoid_factor = 2.;  // to divide by 1000
    float align_radius = 20.; // to divide by 100
    float align_factor = 6.;  // to divide by 1000
    // Environment
    float speed_multiplier = 1.; // to accelerate the simulation
    float screen_margin    = .1; // to reduce the limits boids can't cross (default is full window)
    // Helpers
    bool show_avoid_circle = true;
    bool show_align_circle = true;

    // Declare your infinite update loop.
    ctx.update = [&]() {
        ctx.background(p6::NamedColor::Blue);

        ///////RENDER
        ImGui::Begin("Control Panel");

        ImGui::Text("Boid parameters");
        ImGui::SliderFloat("Turn factor", &turn_factor, 1, 6);
        ImGui::SliderFloat("Avoid radius", &avoid_radius, 0, 32);
        ImGui::SliderFloat("Avoid factor", &avoid_factor, 0, 12);
        ImGui::SliderFloat("Align radius", &align_radius, 0, 32);
        ImGui::SliderFloat("Align factor", &align_factor, 0, 12);
        ImGui::Text("Environment");
        ImGui::SliderFloat("Speed multiplier", &speed_multiplier, 1, 10);
        ImGui::SliderFloat("Screen margin", &screen_margin, .1, 1);
        ImGui::Text("Helpers");
        ImGui::Checkbox("Show avoid circles", &show_avoid_circle);
        ImGui::Checkbox("Show align circles", &show_align_circle);
        ImGui::End();

        // Calculate the correct values to use
        const float calc_turn_factor  = (turn_factor * speed_multiplier) / 10000;
        const float calc_avoid_radius = avoid_radius / 100;
        const float calc_avoid_factor = (avoid_factor * speed_multiplier) / 1000;
        const float calc_align_radius = align_radius / 100;
        const float calc_align_factor = (align_factor * speed_multiplier) / 1000;
        const float calc_min_speed    = min_speed / 10000;
        const float calc_max_speed    = max_speed / 10000;

        // For every boid
        for (auto& boid : Boid_array)
        {
            // Display helpers
            if (show_align_circle)
                boid.drawAlignCircle(ctx, calc_align_radius);
            if (show_avoid_circle)
                boid.drawAvoidCircle(ctx, calc_avoid_radius);
            // Update speed
            boid.adaptSpeedToBorders(ctx.aspect_ratio(), screen_margin, calc_turn_factor);
            boid.adaptSpeedToBoids(Boid_array, calc_avoid_factor, calc_avoid_radius, calc_align_factor, calc_align_radius);
            boid.clampSpeed(calc_min_speed, calc_max_speed);
            // Update position
            boid.updatePosition(speed_multiplier);
            // Display
            boid.draw(ctx, draw_radius);
        }
    };

    ctx.start();
}