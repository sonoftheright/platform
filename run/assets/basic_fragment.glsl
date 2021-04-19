#version 330 core

in vec4 frag_color;
out vec4 color;

void main(){
    color = frag_color;
    // color = vec4(1.0, 0.0, 0.0, 1.0);
}
