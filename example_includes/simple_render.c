
typedef struct {
  char *vertex_file_location;
  char *fragment_file_location;
  GLuint program_id;
  GLchar *vertex_source;
  GLchar *fragment_source;
  GLuint vertex_shader;
  GLuint fragment_shader;

  GLuint vertex_loc;
  GLuint color_loc;
  GLuint model_mat_loc;
  GLuint view_mat_loc;
  GLuint projection_mat_loc;

} basic_shader;

basic_shader Basic_Shader = {
    .vertex_file_location = "./assets/basic_vertex.glsl",
    .fragment_file_location = "./assets/basic_fragment.glsl"};

void simple_render_setup_shader() {
  printf("Setting up Basic_Shader program:\n");
  Basic_Shader.program_id = create_shader_program(
      Basic_Shader.vertex_file_location, Basic_Shader.fragment_file_location,
      Basic_Shader.vertex_source, Basic_Shader.fragment_source,
      Basic_Shader.vertex_shader, Basic_Shader.fragment_shader);
  link_shader_program(Basic_Shader.program_id);

  Basic_Shader.vertex_loc =
      glGetAttribLocation(Basic_Shader.program_id, "vertex_position");
  Basic_Shader.color_loc =
      glGetAttribLocation(Basic_Shader.program_id, "in_color");
  Basic_Shader.model_mat_loc =
      glGetUniformLocation(Basic_Shader.program_id, "model_matrix");
  Basic_Shader.view_mat_loc =
      glGetUniformLocation(Basic_Shader.program_id, "view_matrix");
  Basic_Shader.projection_mat_loc =
      glGetUniformLocation(Basic_Shader.program_id, "projection_matrix");
  p_check_gl_error("Basic_Shader_Program");
}

// @NOTE Global!!!
GLuint simple_shader_VAO;
GLuint simple_shader_vertex_buffer;
GLuint simple_shader_color_buffer;

void draw_simple_mesh(
    platform_api *api,
    float vertices[],      // expects a flat array of floats, representing vec3s
    float vertex_colors[], // expects a flat array of floats, representing vec4s
    float vertex_count // total count of vertices (vec3) and also colors (vec4)
) {

  static int buffersGenerated = 0;
  if (buffersGenerated == 0) {
    // LEAK!!!!
    // LEAK!!!!
    // LEAK!!!!
    // Generate 1 VBO buffer, put the resulting identifier in
    // simple_shader_vertex_buffer
    glGenBuffers(1, &simple_shader_vertex_buffer);
    p_check_gl_error("simple_shader_vertex_buffer");

    // LEAK!!!!
    // LEAK!!!!
    // LEAK!!!!
    // Generate 1 VBO buffer, put the resulting identifier in
    // simple_shader_color_buffer
    glGenBuffers(1, &simple_shader_color_buffer);
    p_check_gl_error("simple_shader_color_buffer");

    buffersGenerated = 1;
  }

  // LEAK! Make sure to clear at bottom.
  glGenVertexArrays(1, &simple_shader_VAO);
  glBindVertexArray(simple_shader_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, simple_shader_vertex_buffer);
  // Give our vertices to OpenGL.
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertex_count, vertices,
               GL_STATIC_DRAW);
  p_check_gl_error("glBufferData on vertices");
  glBindBuffer(GL_ARRAY_BUFFER, simple_shader_color_buffer);
  // Give our colors to OpenGL.
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * vertex_count, vertex_colors,
               GL_STATIC_DRAW);
  p_check_gl_error("glBufferData on colors");
  // The following commands will talk about our 'drawPointsVertexBuffer'
  // buffer

  // Use our shader
  glUseProgram(Basic_Shader.program_id);

  glEnableVertexAttribArray(Basic_Shader.vertex_loc);
  glBindBuffer(GL_ARRAY_BUFFER, simple_shader_vertex_buffer);
  glVertexAttribPointer(
      Basic_Shader.vertex_loc, // attribute location. No particular reason for
                               // 0, but must match the layout in the shader.
      3,                       // size (3 floats per vertex)
      GL_FLOAT,                // type
      GL_FALSE,                // normalized?
      0,                       // stride ( 3 floats per vertex )
      (void *)0                // array buffer offset
  );

  glEnableVertexAttribArray(Basic_Shader.color_loc);
  glBindBuffer(GL_ARRAY_BUFFER, simple_shader_color_buffer);
  glVertexAttribPointer(
      Basic_Shader.color_loc, // attribute location. No particular reason for
                              // 0, but must match the layout in the shader.
      4,                      // size
      GL_FLOAT,               // type
      GL_FALSE,               // normalized?
      4 * sizeof(float),      // stride
      (void *)0               // array buffer offset
  );

  // model-related:
  // mat4x4 scale_matrix;
  // mat4x4 translate_matrix;
  // mat4x4 rotate_matrix;
  mat4x4 model_matrix;

  /*
    model transforms:
  */
  mat4x4_identity(model_matrix); // start with identity
  // mat4x4_identity(scale_matrix);    // start with identity
  // mat4x4_identity(translate_matrix);// start with identity
  // mat4x4_identity(rotate_matrix);   // start with identity
  //
  // // always scale first.
  // mat4x4_scale_aniso(scale_matrix, scale_matrix, 1.0, 1.0, 1.0);
  // // always move second:
  // // don't need to translate, since the location should already be in world
  // // space:
  // mat4x4_translate(translate_matrix, 0.0, 0.0, 0.0);
  //
  // mat4x4_rotate(rotate_matrix, rotate_matrix, 0.0, 0.0, 0.0,
  //               1.0);// always rotate last.
  //
  // mat4x4_mul(model_matrix, model_matrix, translate_matrix);
  // mat4x4_mul(model_matrix, model_matrix, rotate_matrix);
  // mat4x4_mul(model_matrix, model_matrix, scale_matrix);

  glUniformMatrix4fv(Basic_Shader.projection_mat_loc, 1, GL_FALSE,
                     (const GLfloat *)api->camera.projection_matrix);
  glUniformMatrix4fv(Basic_Shader.view_mat_loc, 1, GL_FALSE,
                     (const GLfloat *)api->camera.view_matrix);
  glUniformMatrix4fv(Basic_Shader.model_mat_loc, 1, GL_FALSE,
                     (const GLfloat *)model_matrix);

  // Draw the line !
  glBindVertexArray(simple_shader_VAO);
  // float size = (point_size > 0) ? point_size : 10;
  // glPointSize(size);
  glDrawArrays(GL_TRIANGLES, 0, vertex_count);
  glDisableVertexAttribArray(Basic_Shader.vertex_loc);

  /*
   * END ENTITY LOOP
   */

  /*
   * END RENDER COMPLETELY:
   */
  glDeleteVertexArrays(1, &simple_shader_VAO);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // default
}
