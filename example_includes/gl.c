/*******************************************************************************
 *
 * GL_UTILS
 *
 *******************************************************************************/

/*******************************************************************************
 * create_shader_program
 *******************************************************************************/
GLuint create_shader_program(
    char *vertex_source, char *fragment_source,
    char *vertexsource,   // assigning to these creates "undefined behavior"
    char *fragmentsource, // assigning to these creates "undefined behavior"
    GLuint vertexshader, GLuint fragmentshader) {
  // printf("creating a shader program.\n");

  GLuint shaderprogram = 0;
  int IsCompiled_VS, IsCompiled_FS;
  int maxLength;
  char *vertexInfoLog;
  char *fragmentInfoLog;
  /* Read our shaders into the appropriate buffers */
  vertexsource = p_filetobuf(vertex_source); //"shaders/main_vertex_shader.glsl"
  // vertexsource = opengl_tutorial_vertex;

  fragmentsource =
      p_filetobuf(fragment_source); //"shaders/main_fragment_shader.glsl"

  /* Create an empty vertex shader handle */
  vertexshader = glCreateShader(GL_VERTEX_SHADER);

  /* Send the vertex shader source code to GL */
  /* Note that the source code is NULL character terminated. */
  /* GL will automatically detect that therefore the length info can be 0 in
   * this case (the last parameter) */
  glShaderSource(vertexshader, 1, (const GLchar **)&vertexsource, 0);

  /* Compile the vertex shader */
  glCompileShader(vertexshader);

  glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS);

  if (IsCompiled_VS == GL_FALSE) {
    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR) {
      switch (err) {
      case GL_INVALID_ENUM:
        fprintf(stderr, "invalid enum error.\n");
        break;
      case GL_INVALID_VALUE:
        fprintf(stderr, "invalid value error.\n");
        break;
      case GL_INVALID_OPERATION:
        fprintf(stderr, "invalid operation error.\n");
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        fprintf(stderr, "invalid framebuffer error.\n");
        break;
      case GL_OUT_OF_MEMORY:
        fprintf(stderr, "out of memory error.\n");
        break;
      }
    }

    glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxLength);

    /* The maxLength includes the NULL character */
    vertexInfoLog = (char *)malloc(maxLength);

    glGetShaderInfoLog(vertexshader, maxLength, &maxLength, vertexInfoLog);
    fprintf(stderr, "Error With Vertex Shader: '%s'\n", vertexInfoLog);
    /* Handle the error in an appropriate way such as displaying a message or
     * writing to a log file. */
    /* In this simple program, we'll just leave */
    free(vertexInfoLog);
    return shaderprogram;
  }
  /* Create an empty fragment shader handle */
  fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);

  /* Send the fragment shader source code to GL */
  /* Note that the source code is NULL character terminated. */
  /* GL will automatically detect that therefore the length info can be 0 in
   * this case (the last parameter) */
  glShaderSource(fragmentshader, 1, (const GLchar **)&fragmentsource, 0);

  /* Compile the fragment shader */
  glCompileShader(fragmentshader);

  glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS);
  if (IsCompiled_FS == GL_FALSE) {
    GLint maxLength = 4098;
    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR) {
      switch (err) {
      case GL_INVALID_ENUM:
        fprintf(stderr, "invalid enum error.\n");
        break;
      case GL_INVALID_VALUE:
        fprintf(stderr, "invalid value error.\n");
        break;
      case GL_INVALID_OPERATION:
        fprintf(stderr, "invalid operation error.\n");
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        fprintf(stderr, "invalid framebuffer error.\n");
        break;
      case GL_OUT_OF_MEMORY:
        fprintf(stderr, "out of memory error.\n");
        break;
      }
    }

    glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);

    /* The maxLength includes the NULL character */
    fragmentInfoLog = (char *)malloc(maxLength);

    glGetShaderInfoLog(fragmentshader, maxLength, &maxLength, fragmentInfoLog);
    fprintf(stderr, "Error With Fragment Shader: %s\n", fragmentInfoLog);
    /* Handle the error in an appropriate way such as displaying a message or
     * writing to a log file. */
    /* In this simple program, we'll just leave */
    free(fragmentInfoLog);
    return shaderprogram;
  }

  /* If we reached this point it means the vertex and fragment shaders compiled
   * and are syntax error free. */
  /* We must link them together to make a GL shader program */
  /* GL shader programs are monolithic. It is a single piece made of 1 vertex
   * shader and 1 fragment shader. */
  /* Assign our program handle a "name" */
  shaderprogram = glCreateProgram();

  /* Attach our shaders to our program */
  glAttachShader(shaderprogram, vertexshader);
  glAttachShader(shaderprogram, fragmentshader);

  return shaderprogram;
};

/*******************************************************************************
 * link_shader_program
 *******************************************************************************/

GLuint link_shader_program(GLuint program_id) {
  // printf("linking a shader program.\n");
  int is_linked;
  int max_length;
  char *shader_program_info_log;

  /* Link our program */
  /* At this stage, the vertex and fragment programs are inspected, optimized
   * and a binary code is generated for the shader. */
  /* The binary code is uploaded to the GPU, if there is no error. */
  glLinkProgram(program_id);

  /* Again, we must check and make sure that it linked. If it fails, it would
   * mean either there is a mismatch between the vertex */
  /* and fragment shaders. It might be that you have surpassed your GPU's
   * abilities. Perhaps too many ALU operations or */
  /* too many texel fetch instructions or too many interpolators or dynamic
   * loops. */

  glGetProgramiv(program_id, GL_LINK_STATUS, (int *)&is_linked);

  if (is_linked == GL_FALSE) {
    /* Noticed that glGetProgramiv is used to get the length for a shader
     * program, not glGetShaderiv. */
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &max_length);

    /* The max_length includes the NULL character */
    shader_program_info_log = (char *)malloc(max_length);

    /* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
    glGetProgramInfoLog(program_id, max_length, &max_length,
                        shader_program_info_log);

    /* Handle the error in an appropriate way such as displaying a message or
     * writing to a log file. */
    fprintf(stderr, "ERROR LINKING PROGRAM: %s\n", shader_program_info_log);
    /* In this simple program, we'll just leave */
    free(shader_program_info_log);
    return program_id;
  }

  return program_id;
};

void check_framebuffer_status() {
  printf("checking framebuffer status.\n");
  GLenum status;
  status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  switch (status) {
  case GL_FRAMEBUFFER_UNDEFINED:
    fputs("Framebuffer Error: GL_FRAMEBUFFER_UNDEFINED\n", stderr);
    break;

  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    fputs("Framebuffer Error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n", stderr);
    break;

  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    fputs("Framebuffer Error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n",
          stderr);
    break;

  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    fputs("Framebuffer Error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n", stderr);
    break;

  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    fputs("Framebuffer Error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n", stderr);
    break;

  case GL_FRAMEBUFFER_UNSUPPORTED:
    fputs("Framebuffer Error: GL_FRAMEBUFFER_UNSUPPORTED\n", stderr);
    break;

  case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
    fputs("Framebuffer Error: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n", stderr);
    break;

  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
    fputs("Framebuffer Error: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS\n",
          stderr);
    break;

  case GL_FRAMEBUFFER_COMPLETE:
    printf("Frame buffer complete & ready.\n");
    break;

  default:
    /* programming error; will fail on all hardware */
    fputs("Framebuffer Error: %s\n", stderr);
    exit(-1);
  }
}

/*******************************************************************************
 * _canvas_create_fbo
 *******************************************************************************/

void _create_fbo(GLuint *FBO_Handle) {
  printf("creating a fbo.\n");
  glGenFramebuffers(1, FBO_Handle);
  glBindFramebuffer(GL_FRAMEBUFFER, *FBO_Handle);
  p_check_gl_error("create_fbo");
}

void _create_color_texture_attachment(GLuint *renderedTexture,
                                      GLuint attachment, int width,
                                      int height) {
  printf("creating a color texture attachment.\n");
  glGenTextures(1, renderedTexture);
  // "Bind" the newly created texture : all future texture functions will modify
  // this texture
  glBindTexture(GL_TEXTURE_2D, *renderedTexture);

  // Poor filtering. Needed !
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Give an empty image to OpenGL ( the last "NULL" )
  // @NOTE that this will need to be updated when the window resizes.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, 0);
  // Set "renderedTexture" as our colour attachement #0 (ie,
  // GL_COLOR_ATTACHMENT0 is tied to layout(location = 0) in shader)
  glFramebufferTexture(GL_FRAMEBUFFER, attachment, *renderedTexture,
                       0); // last 0 is which mipmap level to use - not using
                           // mipmaps, so set to 0.
}

void _create_depth_texture_attachment(GLuint *renderedTexture,
                                      GLuint attachment, int width,
                                      int height) {
  printf("creating depth texture attachment.\n");
  glGenTextures(1, renderedTexture);
  // "Bind" the newly created texture : all future texture functions will modify
  // this texture
  glBindTexture(GL_TEXTURE_2D, *renderedTexture);

  // Poor filtering. Needed !
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

  // Give an empty image to OpenGL ( the last "NULL" )
  // @NOTE that this will need to be updated when the window resizes.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  // Set "renderedTexture" as our colour attachement #0 (ie,
  // GL_COLOR_ATTACHMENT0 is tied to layout(location = 0) in shader)
  glFramebufferTexture(GL_FRAMEBUFFER, attachment, *renderedTexture,
                       0); // last 0 is which mipmap level to use - not using
                           // mipmaps, so set to 0.
}

void _create_ray_depth_texture_attachment(GLuint *depthTexture,
                                          GLuint attachment, int width,
                                          int height) {
  printf("creating ray depth texture attachment.\n");
  // Alternative : Depth texture
  // NOTE: Right now, this is using an 'out' variable from the shader
  // to define "depth" as the first ray intersection. Ideally, once we
  // have proper synced scene data, we can actually use the depth buffer that
  // openGL provides.
  // When we make this switch, we'll be using the "glGenRenderBuffers()", then
  // "glBindRenderBuffer(GL_RENDERBUFFER, depthBuffer)" and
  // "glRenderBufferStorage(etc)" and "glFramebufferRenderbuffer(etc)" and this
  // is because we're not saving a "texture"
  glGenTextures(1, depthTexture);
  glBindTexture(GL_TEXTURE_2D, *depthTexture);

  // Poor filtering. Needed !
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Give an empty image to OpenGL ( the last "NULL" )
  // @NOTE that this will need to be updated when the window resizes.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  // Set "renderedTexture" as our colour attachement #1 (ie,
  // GL_COLOR_ATTACHMENT1 is tied to layout(location = 1) in shader)
  glFramebufferTexture(GL_FRAMEBUFFER, attachment, *depthTexture, 0); // last 0 is which mipmap level to use - not using mipmaps, so set to 0.
}

// use this to mark that anything rendered after this will use this buffer
void use_fbo(GLuint fbo, int screenWidth, int screenHeight) {
  // printf("using a fbo.\n");
  glBindTexture(GL_TEXTURE_2D, 0); // unbind any  current textures
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, screenWidth, screenHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void _canvas_unuse_fbo(int screenWidth, int screenHeight) {
  // printf("unusing a fbo.\n");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, screenWidth, screenHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*******************************************************************************
 * reset_gl_state
 *******************************************************************************/

void reset_gl_state(platform_api *p) {
  // Set all these values at the beginning of every frame, in case we changed
  // this in the previous frame to some special render setting:
  p_check_gl_error("pre_reset_gl_state");
  // printf("updating render size to: %i, %i\r", p->draw.size[0],
  // p->draw.size[1]); fflush(stdout);
  glViewport(0, 0, p->draw.size[0], p->draw.size[1]);
  // (void)p;
  p_check_gl_error("gl_viewport");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  p_check_gl_error("gl_clear_2");
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  p_check_gl_error("gl_enable_depth_test_3");
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  p_check_gl_error("gl_depth_func_less_4");
  glFrontFace(GL_CCW); // set front-facing triangles to be drawn
                       // counter-clockwise (also the default)
  p_check_gl_error("gl_front_face_ccw_5");
  glEnable(GL_CULL_FACE); // hide back-facing tris
  p_check_gl_error("gl_enable_cull_face_6");
  glEnable(GL_BLEND); // allow for alpha-blending on textures
  p_check_gl_error("gl_enable_blend_7");
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  p_check_gl_error("gl_blend_func_8");
  glClearColor(p->window.clear_color[0], p->window.clear_color[1],
               p->window.clear_color[2],
               1.0); // this is the "background" color of all rendering
  // glClearColor(0.0, 0.0, 0.0, 1.0);
  p_check_gl_error("gl_clear_color_10");
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // default
  p_check_gl_error("gl_polygon_mode_fill");
}
