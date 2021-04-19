#version 330 core

precision highp float;
// in vec2 texCoord;

uniform vec4 uiColor;
out vec4 color;

void main(){
  color = vec4(uiColor);
}
