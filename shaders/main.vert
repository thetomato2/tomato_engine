#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;
layout (location = 2) in vec3 a_nrm;

out vec2 tex_coord;
out vec3 frag_pos;
out vec3 nrm;

uniform mat4 model;
uniform mat4 wvp;

void main()
{
   frag_pos = vec3(model * vec4(a_pos, 1.0f));
   nrm = mat3(transpose(inverse(model))) * a_nrm;
   tex_coord = a_tex_coord;

   gl_Position = wvp * model *  vec4(frag_pos, 1.0f);

};