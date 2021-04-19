#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 fontColor;

void main(){
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
  color = fontColor * sampled;
  // color = texture( text, TexCoords ).rgba;
  // color = vec4(1,0,0,1);
}
