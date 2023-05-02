#ifndef _BOID_3D_HPP
#define _BOID_3D_HPP

#include <cstdlib>
#include <iostream>
#include <random>
#include "p6/p6.h"

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
        _speed(glm::vec3(p6::random::number(-.001, .001), p6::random::number(-.001, .001), p6::random::number(-.001, .001)))
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

    glm::vec3 getPosition()
    {
        return _pos;
    }
};

#endif