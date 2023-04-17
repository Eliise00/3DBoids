#ifndef OPENGL_TPS_TEMPLATE_GUI_H
#define OPENGL_TPS_TEMPLATE_GUI_H

#include "imgui.h"
#include "Boid3D.h"

void createGuiFromParams(Boid_behavior_params* params, const char* window_name)
{
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
#endif // OPENGL_TPS_TEMPLATE_GUI_H
