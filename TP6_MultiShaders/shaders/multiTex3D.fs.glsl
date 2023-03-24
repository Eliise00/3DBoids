#version 330
in vec2 vTexCoords;

out vec4 fFragColor;


uniform sampler2D uEarthTextur;
uniform sampler2D uCloudTexture;

void main()
{
    vec4 color1 = texture(uEarthTextur, vTexCoords); // couleur de la première texture
    vec4 color2 = texture(uCloudTexture, vTexCoords); // couleur de la deuxième texture

    fFragColor = color1 + color2; // assigner le résultat à la couleur de sortie
}