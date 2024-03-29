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

struct Environment_params {
    float speed_multiplier;
    float aspect_ratio;
    float screen_margin;
    bool  show_align_circle;
    bool  show_avoid_circle;
};

class Boid {
private:
    glm::vec2 _pos;
    glm::vec2 _speed;
    float     _radius;

public:
    explicit Boid(const glm::vec2& initial_position, const float& radius)
        : _pos(initial_position), //
        _speed(p6::random::number(-.001, .001), p6::random::number(-.001, .001))
        , _radius(radius)
    {
    }

    void drawBody(p6::Context& ctx, const float& draw_radius) const
    {
        ctx.use_stroke = false;
        ctx.use_fill   = true;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{draw_radius}
        );
    }

    void drawHelper(p6::Context& ctx, const float& radius, const float& stroke_weight) const
    {
        ctx.stroke_weight = stroke_weight;
        ctx.use_stroke    = true;
        ctx.use_fill      = false;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{radius}
        );
    }

    void adaptSpeedToBorders(const Environment_params& environment, const Boid_behavior_params& params)
    {
        float turn_factor = (params.avoid_factor * environment.speed_multiplier) / 10000;
        if (_pos.x + (_radius / 2) > environment.aspect_ratio - environment.screen_margin * environment.aspect_ratio) // Right
            _speed.x -= turn_factor;

        else if (_pos.x - (_radius / 2) < -environment.aspect_ratio + environment.screen_margin * environment.aspect_ratio) // Left
            _speed.x += turn_factor;

        if (_pos.y + (_radius / 2) > 1 - environment.screen_margin) // Top
            _speed.y -= turn_factor;

        else if (_pos.y - (_radius / 2) < -1 + environment.screen_margin) // Bottom
            _speed.y += turn_factor;
    }

    void adaptSpeedToBoids(const std::vector<Boid>& boids, const Environment_params& environment, const Boid_behavior_params& params)
    {
        glm::vec2 avoid_vec = {0, 0};
        glm::vec2 align_vec = {0, 0};
        int       friends   = 0;
        for (const auto& other : boids)
        {
            if (_pos != other._pos) // if not itself
            {
                float distance_to_other = glm::distance(_pos, other._pos) - other._radius;
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

    void clampSpeed(const Boid_behavior_params& params)
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

    void updatePosition(const Environment_params& environment)
    {
        _pos.x += _speed.x * environment.speed_multiplier;
        _pos.y += _speed.y * environment.speed_multiplier;
    }

    void fullUpdateChain(p6::Context& ctx, const std::vector<Boid>& boids, const Environment_params& environment, const Boid_behavior_params& params)
    {
        adaptSpeedToBorders(environment, params);
        adaptSpeedToBoids(boids, environment, params);
        clampSpeed(params);
        updatePosition(environment);
        drawBody(ctx, params.draw_radius);
        // Display helpers
        if (environment.show_align_circle)
            drawHelper(ctx, params.align_radius / 100, .001f);
        if (environment.show_avoid_circle)
            drawHelper(ctx, params.avoid_radius / 100, .0015f);
    }
};

void createGuiFromParams(Boid_behavior_params* params, const char* window_name)
{
    ImGui::SetNextWindowBgAlpha(0.);
    ImGui::Begin(window_name);
    ImGui::Text("Boid parameters");
    ImGui::SliderFloat("Avoid radius", &params->avoid_radius, 0, 64);
    ImGui::SliderFloat("Avoid factor", &params->avoid_factor, 0, 24);
    ImGui::SliderFloat("Align radius", &params->align_radius, 0, 64);
    ImGui::SliderFloat("Align factor", &params->align_factor, 0, 24);
    ImGui::End();
};

void createMainGui(Environment_params* params)
{
    ImGui::SetNextWindowBgAlpha(0.);
    ImGui::Begin("Environment settings");
    ImGui::Text("Speed");
    ImGui::SliderFloat("Speed multiplier", &params->speed_multiplier, 1, 50);
    ImGui::Text("Borders");
    ImGui::SliderFloat("Screen margin", &params->screen_margin, .1, 1);
    ImGui::Text("Helpers");
    ImGui::Checkbox("Show avoid circles", &params->show_avoid_circle);
    ImGui::Checkbox("Show align circles", &params->show_align_circle);
    ImGui::End();
}

int main()
{
    // Window
    auto ctx = p6::Context{{1280, 720, "Boid"}};
    ctx.maximize_window();

    // Boids
    size_t            nb_of_boids = 80;
    std::vector<Boid> Boid_array;
    for (size_t i = 0; i < nb_of_boids; ++i)
    {
        glm::vec2 random_pos = {p6::random::number(-ctx.aspect_ratio(), ctx.aspect_ratio()), p6::random::number(-1, 1)};
        if (i < 70)
            Boid_array.emplace_back(random_pos, .01);
        else if (i == 74)
            Boid_array.emplace_back(random_pos, .2);
        else
            Boid_array.emplace_back(random_pos, .08);
    }

    Boid_behavior_params small_boid_params{
        .align_factor = 15.,
        .align_radius = 20.,
        .avoid_factor = 3.,
        .avoid_radius = 5.,
        .draw_radius  = .01,
        .max_speed    = 30,
        .min_speed    = 15};

    Boid_behavior_params big_boid_params{
        .align_factor = 2.,
        .align_radius = 40.,
        .avoid_factor = .5,
        .avoid_radius = 12.,
        .draw_radius  = .08,
        .max_speed    = 8,
        .min_speed    = 4};

    Boid_behavior_params maxi_boid_params{
        .align_factor = 1.,
        .align_radius = 40.,
        .avoid_factor = .2,
        .avoid_radius = 20.,
        .draw_radius  = .2,
        .max_speed    = 4,
        .min_speed    = 2};

    Environment_params environment_params{
        .speed_multiplier  = 1.,
        .aspect_ratio      = ctx.aspect_ratio(),
        .screen_margin     = .1,
        .show_align_circle = true,
        .show_avoid_circle = true};

    // Infinite loop
    ctx.update = [&]() {
        ctx.background(p6::NamedColor::DarkCyan);

        createGuiFromParams(&small_boid_params, "Small Boids");
        createGuiFromParams(&big_boid_params, "Big Boids");
        createMainGui(&environment_params);

        // For every boid
        size_t boid_index = 0;
        for (auto& boid : Boid_array)
        {
            if (boid_index < 70) // many small boids
            {
                boid.fullUpdateChain(ctx, Boid_array, environment_params, small_boid_params);
            }
            else if (boid_index == 74) // one huge boid
            {
                boid.fullUpdateChain(ctx, Boid_array, environment_params, maxi_boid_params);
            }
            else // some big boids
            {
                boid.fullUpdateChain(ctx, Boid_array, environment_params, big_boid_params);
            }
            boid_index++;
        }
    };

    ctx.start();
}