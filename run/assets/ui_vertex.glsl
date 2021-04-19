#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices;

// Output data ; will be interpolated for each fragment.
// out vec2 texCoord;
// uniform mat4 projection;

void main(){
  // gl_Position = projection * vec4(vertices.xy, 1.0, 1.0);
	// gl_Position =  vec4(vertices.xy, 0.1, 1.0);
  gl_Position =  vec4(vertices.xyz, 1.0);
	// texCoord = (vertices.xy + vec2(1, 1)) / 2.0;
}
