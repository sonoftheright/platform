#version 330 core

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

in vec3 vertex_position;
in vec4 in_color;

out vec4 frag_color;

void main(){
    gl_Position = projection_matrix * view_matrix * (model_matrix * vec4(vertex_position, 1.0));
    frag_color = in_color; // interpolated sample
}
