#include "p6/p6.h"
#include <cstdlib>
#include <random>
#define DOCTEST_CONFIG_IMPLEMENT


class Boid {
private:

    glm::vec2 _pos;
    p6::Angle _dir;
    float     _speed;
    float     _radius;
    float _aspect_ratio;



public:

    std::vector<Boid> _everyone;

    explicit Boid(float aspect_ratio)
        :
        _pos(glm::vec2(p6::random::number(-aspect_ratio, aspect_ratio), p6::random::number(-1, 1))), // remplace seed avec la doc de p6
        _dir(p6::Angle(p6::Radians(p6::random::number(p6::PI * 2)))),
        _speed(p6::random::number() / 500),
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

    void update()
    {
        auto move = rotated_by(_dir, glm::vec2(_speed, 0.f));
        _pos += move;
        if(abs(_pos.x) > _aspect_ratio){
            _pos.x = _pos.x * -1;
        }
        if(abs(_pos.y) > 1){
            _pos.y = _pos.y * -1;
        }
    }

    /*
    void check_neighbours(){
        const float zone_radius = 0.05;
        for (auto& i: _everyone){
            if(abs(i._pos.x) < abs(_pos.x + zone_radius)){
                i._dir = _dir;
            }
            if(abs(i._pos.y) < abs(_pos.y + zone_radius)){
                i._dir = _dir;
            }
        }
    }*/
};


int main()
{
    auto ctx = p6::Context{{1280, 720, "Boid"}};
    ctx.maximize_window();

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    std::vector<Boid> Boid_array;
    for (size_t i = 0; i < 200; ++i)
    {
        Boid boidTemp(ctx.aspect_ratio());
        Boid_array.push_back(boidTemp);
    }
    auto square_radius = 0.5f;


    // Declare your infinite update loop.
    ctx.update = [&]() {
        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/
        ctx.background(p6::NamedColor::Blue);
        // Show a simple window
        ImGui::Begin("Test");
        ImGui::SliderFloat("Square size", &square_radius, 0.f, 1.f);
        ImGui::End();

        // Show the official ImGui demo window
        // It is very useful to discover all the widgets available in ImGui
        // ImGui::ShowDemoWindow();

        ctx.square(p6::Center{}, p6::Radius{square_radius});

        // Draw array
        for (auto& i : Boid_array)
        {
            //i.check_neighbours();
            i.draw(ctx);
            i.update();
        }
    };

    // Should be done last. It starts the infinite loop.
    ctx.start();
}