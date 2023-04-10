#include <cstdlib>
#include <iostream>
#include <random>
#include "imgui.h"
#include "p6/p6.h"
#define DOCTEST_CONFIG_IMPLEMENT

struct Boid_behavior_params {
    float align_factor;
    float align_radius;
    float avoid_factor;
    float avoid_radius;
    float draw_radius;
    float max_speed;
    float min_speed;
};

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

    void adaptSpeedToBorders(float aspect_ratio, float screen_margin, Boid_behavior_params params, float speed_multiplier)
    {
        float turn_factor = (params.avoid_factor * speed_multiplier) / 10000;
        if (_pos.x > aspect_ratio - screen_margin * aspect_ratio) // Right
            _speed.x -= turn_factor;

        else if (_pos.x < -aspect_ratio + screen_margin * aspect_ratio) // Left
            _speed.x += turn_factor;

        if (_pos.y > 1 - screen_margin) // Top
            _speed.y -= turn_factor;

        else if (_pos.y < -1 + screen_margin) // Bottom
            _speed.y += turn_factor;
    }

    void adaptSpeedToBoids(std::vector<Boid>& boids, Boid_behavior_params params, float speed_multiplier)
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
        _speed += avoid_vec * ((params.avoid_factor * speed_multiplier) / 1000);
        if (friends > 0)
        {
            align_vec /= friends;
            _speed += (align_vec - _speed) * ((params.align_factor * speed_multiplier) / 1000);
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

    void updatePosition(float speed_multiplier)
    {
        _pos.x += _speed.x * speed_multiplier;
        _pos.y += _speed.y * speed_multiplier;
    }
};

// TODO : full params struct, specific function for GUI
int main()
{
    // Window
    auto ctx = p6::Context{{1280, 720, "Boid"}};
    ctx.maximize_window();

    // Boids
    std::vector<Boid> Boid_array;
    for (size_t i = 0; i < 80; ++i)
    {
        glm::vec2 random_pos = {p6::random::number(-ctx.aspect_ratio(), ctx.aspect_ratio()), p6::random::number(-1, 1)};
        Boid_array.emplace_back(random_pos);
    }

    Boid_behavior_params boid_behavior_params{
        .align_factor = 6.,
        .align_radius = 20.,
        .avoid_factor = 2.,
        .avoid_radius = 6.,
        .draw_radius  = .01,
        .max_speed    = 30,
        .min_speed    = 20};

    // Environment parameters
    float speed_multiplier = 1.; // used to accelerate the simulation
    float screen_margin    = .1; // used to reduce the limits boids can't cross (default is full window)
    // Helper booleans
    bool show_avoid_circle = true;
    bool show_align_circle = true;

    std::vector<Boid> Boid_array2;
    for (size_t i = 0; i < 5; ++i)
    {
        glm::vec2 random_pos = {p6::random::number(-ctx.aspect_ratio(), ctx.aspect_ratio()), p6::random::number(-1, 1)};
        Boid_array2.emplace_back(random_pos);
    }
    Boid_behavior_params boid_behavior_params2{
        .align_factor = 12.,
        .align_radius = 40.,
        .avoid_factor = 4.,
        .avoid_radius = 12.,
        .draw_radius  = .08,
        .max_speed    = 8,
        .min_speed    = 6};

    // Infinite loop
    ctx.update = [&]() {
        ctx.background(p6::NamedColor::DarkCyan);

        // FAIRE UNE FONCTION GUI À QUI ON PASSE LES OBJETS PARAMÈTRES
        ImGui::Begin("Control Panel");
        ImGui::Text("Boid parameters");
        ImGui::SliderFloat("Avoid radius", &boid_behavior_params.avoid_radius, 0, 32);
        ImGui::SliderFloat("Avoid factor", &boid_behavior_params.avoid_factor, 0, 12);
        ImGui::SliderFloat("Align radius", &boid_behavior_params.align_radius, 0, 32);
        ImGui::SliderFloat("Align factor", &boid_behavior_params.align_factor, 0, 12);
        ImGui::Text("Environment");
        ImGui::SliderFloat("Speed multiplier", &speed_multiplier, 1, 50);
        ImGui::SliderFloat("Screen margin", &screen_margin, .1, 1);
        ImGui::Text("Helpers");
        ImGui::Checkbox("Show avoid circles", &show_avoid_circle);
        ImGui::Checkbox("Show align circles", &show_align_circle);
        ImGui::End();

        // For every boid
        for (auto& boid : Boid_array)
        {
            // Update speed
            boid.adaptSpeedToBorders(ctx.aspect_ratio(), screen_margin, boid_behavior_params, speed_multiplier);
            boid.adaptSpeedToBoids(Boid_array, boid_behavior_params, speed_multiplier);
            boid.adaptSpeedToBoids(Boid_array2, boid_behavior_params2, speed_multiplier);
            boid.clampSpeed(boid_behavior_params);
            // Update position
            boid.updatePosition(speed_multiplier);
            // Display
            boid.drawBody(ctx, boid_behavior_params.draw_radius);
            // Display helpers
            if (show_align_circle)
                boid.drawHelper(ctx, boid_behavior_params.align_radius / 100, .001f);
            if (show_avoid_circle)
                boid.drawHelper(ctx, boid_behavior_params.avoid_radius / 100, .0015f);
        }

        // For every boid
        for (auto& boid : Boid_array2)
        {
            // Update speed
            boid.adaptSpeedToBorders(ctx.aspect_ratio(), screen_margin, boid_behavior_params2, speed_multiplier);
            boid.adaptSpeedToBoids(Boid_array2, boid_behavior_params2, speed_multiplier);
            boid.adaptSpeedToBoids(Boid_array, boid_behavior_params, speed_multiplier);
            boid.clampSpeed(boid_behavior_params2);
            // Update position
            boid.updatePosition(speed_multiplier);
            // Display
            boid.drawBody(ctx, boid_behavior_params2.draw_radius);
            // Display helpers
            if (show_align_circle)
                boid.drawHelper(ctx, boid_behavior_params2.align_radius / 100, .001f);
            if (show_avoid_circle)
                boid.drawHelper(ctx, boid_behavior_params2.avoid_radius / 100, .0015f);
        }
    };

    ctx.start();
}