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
    float     _turn_factor;
    float     _avoid_radius;
    float     _avoid_factor;
    float     _align_radius;
    float     _align_factor;
    float     _draw_radius;

public:
    explicit Boid(float aspect_ratio)
        : _pos(glm::vec2(p6::random::number(-aspect_ratio, aspect_ratio), p6::random::number(-1, 1))), // remplace seed avec la doc de p6
        _speed(p6::random::number(-.001, .001), p6::random::number(-.001, .001))
        , _turn_factor(.0002)
        , _avoid_radius(.06)
        , _avoid_factor(.002)
        , _align_radius(.2)
        , _align_factor(.006)
        , _draw_radius(.01)
    {
    }

    void draw(p6::Context& ctx, bool show_avoid_circle, bool show_align_circle) const
    {
        // boid circle
        ctx.use_stroke = false;
        ctx.use_fill   = true;
        ctx.circle(
            p6::Center{_pos.x, _pos.y},
            p6::Radius{_draw_radius}
        );
        // avoid circle
        if (show_avoid_circle)
        {
            ctx.stroke_weight = 0.002f;
            ctx.use_stroke    = true;
            ctx.use_fill      = false;
            ctx.circle(
                p6::Center{_pos.x, _pos.y},
                p6::Radius{_avoid_radius}
            );
        }
        // cohesion circle
        if (show_align_circle)
        {
            ctx.stroke_weight = 0.0025f;
            ctx.use_stroke    = true;
            ctx.use_fill      = false;
            ctx.circle(
                p6::Center{_pos.x, _pos.y},
                p6::Radius{_align_radius}
            );
        }
    }

    //// update
    void updatePosition(float aspect_ratio, std::vector<Boid>& boids, float min_speed, float max_speed, float turn_factor, float avoid_radius, float avoid_factor, float align_radius, float align_factor, float speed_multiplier, float screen_margin)
    {
        _turn_factor  = turn_factor;
        _avoid_radius = avoid_radius;
        _avoid_factor = avoid_factor;
        _align_radius = align_radius;
        _align_factor = align_factor;
        //////////////////////////////////////////////////
        // LIMITS OF THE SCREEN
        //////////////////////////////////////////////////

        // 1) Plain teleportation
        // if (abs(_pos.x) > _aspect_ratio)
        //     _pos.x = _pos.x * -1;
        // if (abs(_pos.y) > 1)
        //     _pos.y = _pos.y * -1;

        // 2) Make them turn away !
        float calculated_turn_factor  = _turn_factor * speed_multiplier;
        float calculated_avoid_factor = _avoid_factor * speed_multiplier;
        float calculated_align_factor = _align_factor * speed_multiplier;
        //  Right
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
        // AVOIDANCE & ALIGNMENT
        //////////////////////////////////////////////////
        glm::vec2 avoid_vec = {0, 0};
        glm::vec2 align_vec = {0, 0};
        int       friends   = 0;
        for (auto& other : boids)
        {
            if (_pos != other._pos) // if not itself
            {
                float distance_to_other = glm::distance(_pos, other._pos);
                if (distance_to_other < _avoid_radius) // if in avoid radius -> avoid
                {
                    avoid_vec += _pos - other._pos;
                }
                else if (distance_to_other < _align_radius) // else if in align radius
                {
                    align_vec += other._speed;
                    friends += 1;
                }
            }
        }
        _speed += avoid_vec * calculated_avoid_factor;
        if (friends > 0)
        {
            align_vec /= friends;
            _speed += (align_vec - _speed) * calculated_align_factor;
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
        _pos.x += _speed.x * speed_multiplier;
        _pos.y += _speed.y * speed_multiplier;
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

    // Boid parameters
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
        // ImGui::SliderFloat("Min Speed", &min_speed, 0, 30);
        // ImGui::SliderFloat("Max Speed", &max_speed, 40, 70);
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

        // Draw array
        for (auto& i : Boid_array)
        {
            i.draw(ctx, show_avoid_circle, show_align_circle);
            i.updatePosition(ctx.aspect_ratio(), Boid_array, min_speed / 10000, max_speed / 10000, turn_factor / 10000, avoid_radius / 100, avoid_factor / 1000, align_radius / 100, align_factor / 1000, speed_multiplier, screen_margin);
            // il faut améliorer la fonction update position,
            // il faudrait lui donner un objet "parameters" ça serait bcp + propre
            // il faudrait SURTOUT faire une fonction qui update les boids avec les parametres
            //      en gros séparer l'update des parametres des boids via la GUI
            //      ET l'update de la position en fonction de ces paramètres
            //      séparer en différente fonction tout ce qu'il se passe quoi
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}