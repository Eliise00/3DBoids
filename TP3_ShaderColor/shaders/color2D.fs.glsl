#version 330 core

in vec3 vFragColor;
in vec2 vFragPosition;
out vec3 fFragColor;

float attenuation(float alpha, float beta){
    float distance = distance(vFragPosition, vec2(0, 0));
    return alpha * exp(-beta * pow(distance, 2));
}

// Uniforms
uniform vec2 u_point = vec2(0.5, 0.5); // Point to calculate distance from
uniform float u_radius = 0.5; // Radius of gradient

void main() {

    // Calculate the distance from the point
    float distance = distance(vFragPosition, u_point);

    // Calculate the alpha value using smoothstep
    float a = smoothstep(u_radius - 0.1, u_radius, distance);

    //float a = attenuation(3, -35);

    fFragColor = a * vFragColor;
}