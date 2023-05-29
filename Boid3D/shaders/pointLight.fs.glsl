#version 330

//variables d'entrées
in vec3 vPosition_vs; //w0 normalize(-vPos)
in vec3 vNormal_vs;
in vec2 vTexCoords;

out vec4 fFragColor;

uniform sampler2D uTexture;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

/* _vs = travailler dans le view space;
=> la direction de la lumière *  View Matrix */

uniform vec3 uLightPos_vs; //wi (need normalization and pos)
uniform vec3 uLightIntensity; //Li

vec3 blinnPhong() {
    float d = distance(vPosition_vs, uLightPos_vs);
    vec3 Li = (uLightIntensity / (d * d));
    vec3 N = vNormal_vs;
    vec3 w0 = normalize(-vPosition_vs);
    vec3 wi = normalize(uLightPos_vs - vPosition_vs);
    vec3 halfVector = (w0 + wi)/2.f;

    return uKa + Li*(uKd*max(dot(wi, N), 0.) + uKs*pow(max(dot(halfVector, N), 0.), uShininess));
}

void main() {
    vec4 texColor = texture(uTexture, vTexCoords);
    vec3 lighting = blinnPhong();
    vec4 finalColor = vec4(texColor.rgb, texColor.a) * vec4(lighting, 1.0);
    fFragColor = finalColor;
}