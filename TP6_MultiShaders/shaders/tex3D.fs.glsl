#version 330


in vec2 vTexCoords;

out vec4 fFragColor;


uniform sampler2D uTexture;

void main()
{
    fFragColor = texture(uTexture, vTexCoords);
}