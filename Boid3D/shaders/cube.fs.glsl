#version 330 core
in vec2 vTexCoords;

out vec4 fFragColor;

uniform sampler2D uTextureCube;

void main()
{

    fFragColor = vec4(texture(uTextureCube, vTexCoords).xyz, 0.5);

}