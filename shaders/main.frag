
#version 330 core

out vec4 frag_color;

in vec2 tex_coord;
in vec3 nrm;
in vec3 frag_pos;

uniform float shiny;
uniform vec3 light_col;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform vec3 obj_col;

uniform sampler2D tex_alb;
uniform sampler2D tex_nrm;

void main()
{
    // ambeint
    float ambient_strength = 0.1f; 
    // vec3 ambient = ambient_strength * light_col * vec3(texture(tex_alb, tex_coord));
    vec3 ambient = ambient_strength * light_col *obj_col;
    
    //diffuse
    vec3 norm = normalize(nrm);
    
    vec3 light_dir = normalize(light_pos - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    // vec3 diffuse = diff * light_col vec3(texture(tex_alb, tex_coord));
    vec3 diffuse = diff * light_col * obj_col;

    // specular
    float spec_strength = 0.5;
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shiny);
    vec3 specular  = spec_strength * spec * light_col;

    vec3 result = ambient + diffuse + specular;
    frag_color = vec4(result, 1.0);
};