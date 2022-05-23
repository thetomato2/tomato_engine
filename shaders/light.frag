#version 330 core

out vec4 frag_color;

uniform vec3 light_col;

void main()
{
    frag_color = vec4(light_col, 1.0f);
};