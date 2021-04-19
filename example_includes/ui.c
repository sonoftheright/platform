const int MAX_UI_ELEMENTS = 512;
size_t UI_POOL_SIZE = Kilobytes(64);

typedef struct {
  int Hot;    // item currently being hovered over, or tab-selected
  int Active; // item currently being used, typed in, etc
} UI_State;

// this is used to determine which single UI items are
// currently active (just clicked), or Hot (just moused over)
UI_State UI_STATE = {.Active = -1, .Hot = -1};

typedef struct {
  char *text;
  bool is_button;

  float location[2];
  float size[2];
  float color[4];

  float text_color[4];
  float text_size;
  float text_location[2];

  int text_buffer_size;
  int text_length;
  void *text_buffer;

  bool initted;
} UI_Element;

UI_Element UI_ELEMENT_LIST[MAX_UI_ELEMENTS];
int UI_INDEX; // index to track distinct elements in a single render run
POOL *UI_POOL;

#ifdef PLATFORM_USE_OPENGL

typedef struct {
  char *vertex_file_location;
  char *fragment_file_location;
  GLuint program_id;
  GLchar *vertex_source;
  GLchar *fragment_source;
  GLuint vertex_shader;
  GLuint fragment_shader;

  GLuint ui_color_loc;
  GLuint ui_vertices_loc;
  GLuint ui_projection_loc;

} ui_shader;

ui_shader UI_Shader = {
    .vertex_file_location = "./assets/ui_vertex.glsl",
    .fragment_file_location = "./assets/ui_fragment.glsl"};

void ui_setup_shader() {
  UI_Shader.program_id = create_shader_program(
      UI_Shader.vertex_file_location, UI_Shader.fragment_file_location,
      UI_Shader.vertex_source, UI_Shader.fragment_source,
      UI_Shader.vertex_shader, UI_Shader.fragment_shader);
  p_check_gl_error("Creating UI_Shader_Program");
  link_shader_program(UI_Shader.program_id);
  p_check_gl_error("Linking UI_Shader_Program");
  UI_Shader.ui_color_loc =
      glGetUniformLocation(UI_Shader.program_id, "uiColor");
  // UI_Shader.ui_projection_loc =
  //     glGetUniformLocation(UI_Shader.program_id, "projection");
  UI_Shader.ui_vertices_loc =
      glGetUniformLocation(UI_Shader.program_id, "vertices");
  p_check_gl_error("UI_Shader_Program");
}

// @NOTE Global!!!
GLfloat ui_vert_array[18];
GLuint ui_quad_vert_buffer;
GLuint ui_VAO;

// @TODO make sure that mesh draw calls use glDrawElements, so we can pass in
// the indices defined by the triangle lists. glDrawArrays only handles the
// "count" of indices, not the indices array itself. This might mean that we
// only need to load each mesh VBO once per frame, not once per draw call...

// @TODO Migrate parameters here to x, y, width, height
// because currently, we allow for possible parameters that would
// draw back-facing triangles.
void draw_immediate_quad(
    vec3 TopLeftPercent, vec3 BottomRightPercent, vec4 Color,
    platform_api *api) { // UI_State UI, int elementID, char UIText, Vec3
                         // TopLeft, Vec3 BottomRight, Vec4 Color

  static int buffersGenerated = 0;
  if (buffersGenerated == 0) {

    // LEAK!!!!
    // LEAK!!!!
    // LEAK!!!!
    glGenVertexArrays(1, &ui_VAO);
    p_check_gl_error("immediate_quad glGenVertexArrays");

    // LEAK!!!!
    // LEAK!!!!
    // LEAK!!!!
    glGenBuffers(1, &ui_quad_vert_buffer);
    p_check_gl_error("immediate_quad glGenBuffers");

    buffersGenerated = 1;
  }

  // convert pixel values to actual GL coords:
  vec3 TopLeft = {(TopLeftPercent[0] / 0.5) - 1.0,
                  ((TopLeftPercent[1] / 0.5) - 1.0) * -1.0, // flip y axis
                  TopLeftPercent[2]};

  vec3 BottomRight = {(BottomRightPercent[0] / 0.5) - 1.0,
                      ((BottomRightPercent[1] / 0.5) - 1.0) *
                          -1.0, // flip y axis
                      BottomRightPercent[2]};

  // NW
  ui_vert_array[0] = TopLeft[0];
  ui_vert_array[1] = TopLeft[1];
  ui_vert_array[2] = TopLeft[2];
  // SW
  ui_vert_array[3] = TopLeft[0];
  ui_vert_array[4] = BottomRight[1];
  ui_vert_array[5] = TopLeft[2];
  // NE
  ui_vert_array[6] = BottomRight[0];
  ui_vert_array[7] = TopLeft[1];
  ui_vert_array[8] = TopLeft[2];
  // SW
  ui_vert_array[9] = TopLeft[0];
  ui_vert_array[10] = BottomRight[1];
  ui_vert_array[11] = TopLeft[2];
  // SE
  ui_vert_array[12] = BottomRight[0];
  ui_vert_array[13] = BottomRight[1];
  ui_vert_array[14] = BottomRight[2];
  // NE
  ui_vert_array[15] = BottomRight[0];
  ui_vert_array[16] = TopLeft[1];
  ui_vert_array[17] = TopLeft[2];

  glBindVertexArray(ui_VAO);
  p_check_gl_error("immediate_quad glBindVertexArrays");

  glBindBuffer(GL_ARRAY_BUFFER, ui_quad_vert_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, ui_vert_array,
               GL_STATIC_DRAW);
  p_check_gl_error("immediate_quad binding buffers");
  // don't clear the color buffers here! That would wipe the previous drawn
  // stuff...
  glClear(GL_DEPTH_BUFFER_BIT);

  glUseProgram(UI_Shader.program_id);
  p_check_gl_error("immediate_quad using program");

  // mat4x4 projection;
  // mat4x4 scale_matrix;
  // mat4x4 translate_matrix;
  // mat4x4 out_matrix;
  //
  // mat4x4_identity(out_matrix);
  // mat4x4_identity(scale_matrix);
  // mat4x4_identity(translate_matrix);
  // mat4x4_identity(projection);
  // UNUSED(size);
  // UNUSED(scale_matrix);

  // mat4x4_ortho(projection, 0.0, api->draw.size[0], api->draw.size[1], 0.0,
  // -1.0,
  //              1.0);
  // mat4x4_scale_aniso(scale_matrix, scale_matrix, 1.0, 1.0, 1.0);
  // mat4x4_translate(translate_matrix, 0.0, 0.0, 0.0);

  // mat4x4_mul(out_matrix, out_matrix, scale_matrix);
  // mat4x4_mul(out_matrix, out_matrix, translate_matrix);
  // mat4x4_mul(out_matrix, out_matrix, projection);
  //
  // glUniformMatrix4fv(UI_Shader.ui_projection_loc, 1, GL_FALSE,
  //                    (const GLfloat *)out_matrix);

  (void)api;

  glUniform4f(UI_Shader.ui_color_loc, Color[0], Color[1], Color[2], Color[3]);
  // printf("color loc is %i\n", UI_Shader.ui_color_loc);
  // printf("color is %f, %f, %f, %f\n", Color[0], Color[1], Color[2],
  // Color[3]); p_check_gl_error("immediate_quad setting gluniform4f");
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, ui_quad_vert_buffer);
  glVertexAttribPointer(0,        // attribute location.
                        3,        // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
  );
  p_check_gl_error("immediate_quad attribPointer");
  // Draw the triangles !
  glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
  glDisableVertexAttribArray(0);

  // glDeleteBuffers(1, &ui_quad_vert_buffer);
  // glDeleteVertexArrays(1, &ui_VAO);
}

bool do_immediate_button(int button_index, float x, float y, float w, float h, vec4 color,
              platform_api *api) {
  if (button_index > MAX_UI_ELEMENTS) {
    printf("Cannot add any more buttons! Increase the UI Element list buffer "
           "or delete some.\n");
    return false;
  }

  bool results = false;

  if (api->window.mouse_lock == false) {

    if (UI_STATE.Active == button_index) {
      if (!api->mouse.left.is_down) {
        if (UI_STATE.Hot ==
            button_index) { // we clicked, and are still hovering
                            // over this item. Do the action.
          results = true;
        }
        // done. set this to no longer be the active item.
        // NOTE: This could void a previously set hot item, if
        // we have a lot of UI elements...
        UI_STATE.Active = -1;
      }
    } else if (UI_STATE.Hot == button_index) {
      if (api->mouse.left.is_down == true) { // we hovered over this item and
                                             // clicked down, we're active now!
        UI_STATE.Active = button_index;
      }
    }

    float mouse_x_relative =
        (api->mouse.position[0] * api->window.scale) / api->draw.size[0];
    float mouse_y_relative =
        (api->mouse.position[1] * api->window.scale) / api->draw.size[1];

    bool hoveringInside = mouse_x_relative >= x &&
                          mouse_x_relative <= (x + w) &&
                          mouse_y_relative >= y && mouse_y_relative <= (y + h);

    if (hoveringInside == true) {
      // check that there aren't any active items before setting this button to
      // hot. if there is an active item, it won't set this as hot, because
      // we're still interacting with the active item... for example, if you
      // move the mouse while still typing into an input box. otherwise, set
      // this item as hot.
      if (UI_STATE.Active < 0) {
        UI_STATE.Hot = button_index;
      }
    }
    // make sure we're the hot item before resetting.
    else if (UI_STATE.Hot == button_index) {
      UI_STATE.Hot = -1;
    }

    if (UI_STATE.Active == button_index) {
      color[0] += 0.3;
      color[1] += 0.3;
      color[2] += 0.3;
    }
    if (UI_STATE.Hot == button_index) {
      color[0] += 0.2;
      color[1] += 0.2;
      color[2] += 0.2;
    }
  }

  // now, run the draw stuff for the button to show on the screen...
  vec3 UpperLeft = {x, y, 0.99};
  vec3 BottomRight = {(x + w), (y + h), 0.99};

  draw_immediate_quad(UpperLeft, BottomRight, color, api);
  return results;
}

void ui_init(){
  ui_setup_shader();// set up the shader for displaying simple UI elements
  UI_POOL = pool_create(UI_POOL_SIZE);
}

void ui_start_frame(){
  UI_INDEX = 0;
}

// when we switch views, we want to reset all our current
// elements so we are forced to re-create the text buffers for each element.
// This way we avoid potential memory issues.
void ui_clear_view(){
  for(int i = 0; i < MAX_UI_ELEMENTS; i++){
    UI_ELEMENT_LIST[i].initted = false;
  }

  free(UI_POOL);
  UI_POOL = pool_create(UI_POOL_SIZE);
}

void ui_close(){
  free(UI_POOL);
}

bool ui_do_button(
  float location[2], float size[2],
  float color[4], float text_color[4],
  char * text,
  float text_location[2],
  float text_size,
  text_font *font,
  platform_api *api){
    if (UI_INDEX > MAX_UI_ELEMENTS) {
      printf("Cannot add any more elements! Increase the UI Element list buffer "
             "or delete some.\n");
      return false;
    }
    UI_Element *current_element = &UI_ELEMENT_LIST[UI_INDEX];
    float x,y,w,h;

    if(!current_element->initted){
      text_create_buffer(UI_POOL, text, current_element->text_buffer, &current_element->text_buffer_size, &current_element->text_length);

      current_element->is_button = true;
      current_element->initted = true;
    }

    current_element->color[0] = color[0];
    current_element->color[1] = color[1];
    current_element->color[2] = color[2];
    current_element->color[3] = color[3];
    current_element->text_color[0] = text_color[0];
    current_element->text_color[1] = text_color[1];
    current_element->text_color[2] = text_color[2];
    current_element->text_color[3] = text_color[3];
    current_element->location[0] = x = location[0];
    current_element->location[1] = y = location[1];
    current_element->size[0] = w = size[0];
    current_element->size[1] = h = size[1];
    current_element->text_size = text_size;
    current_element->text_location[0] = text_location[0];
    current_element->text_location[1] = text_location[1];

    bool results = false;

    if (api->window.mouse_lock == false) {

      if (UI_STATE.Active == UI_INDEX) {
        if (!api->mouse.left.is_down) {
          if (UI_STATE.Hot ==
              UI_INDEX) { // we clicked, and are still hovering
                              // over this item. Do the action.
            results = true;
          }
          // done. set this to no longer be the active item.
          // NOTE: This could void a previously set hot item, if
          // we have a lot of UI elements...
          UI_STATE.Active = -1;
        }
      } else if (UI_STATE.Hot == UI_INDEX) {
        if (api->mouse.left.is_down == true) { // we hovered over this item and
                                               // clicked down, we're active now!
          UI_STATE.Active = UI_INDEX;
        }
      }

      float mouse_x_relative =
          (api->mouse.position[0] * api->window.scale) / api->draw.size[0];
      float mouse_y_relative =
          (api->mouse.position[1] * api->window.scale) / api->draw.size[1];

      bool hoveringInside = mouse_x_relative >= x &&
                            mouse_x_relative <= (x + w) &&
                            mouse_y_relative >= y && mouse_y_relative <= (y + h);

      if (hoveringInside == true) {
        // check that there aren't any active items before setting this button to
        // hot. if there is an active item, it won't set this as hot, because
        // we're still interacting with the active item... for example, if you
        // move the mouse while still typing into an input box. otherwise, set
        // this item as hot.
        if (UI_STATE.Active < 0) {
          UI_STATE.Hot = UI_INDEX;
        }
      }
      // make sure we're the hot item before resetting.
      else if (UI_STATE.Hot == UI_INDEX) {
        UI_STATE.Hot = -1;
      }

      if (UI_STATE.Active == UI_INDEX) {
        color[0] += 0.3;
        color[1] += 0.3;
        color[2] += 0.3;
      }
      if (UI_STATE.Hot == UI_INDEX) {
        color[0] += 0.2;
        color[1] += 0.2;
        color[2] += 0.2;
      }
    }

    // now, run the draw stuff for the button to show on the screen...
    vec3 UpperLeft = {x, y, 0.99};
    vec3 BottomRight = {(x + w), (y + h), 0.99};

    draw_immediate_quad(UpperLeft, BottomRight, color, api);
    text_print(font, text_location[0], text_location[1], text_size, text, current_element->text_color, api);
    UI_INDEX += 1;
    return results;
}

void ui_do_text(
  char * text,
  float location[2],
  float size,
  float color[4],
  text_font *font,
  platform_api *api){
    if (UI_INDEX > MAX_UI_ELEMENTS) {
      printf("Cannot add any more elements! Increase the UI Element list buffer "
             "or delete some.\n");
      return;
    }

    UI_Element *current_element = &UI_ELEMENT_LIST[UI_INDEX];

    // @TODO for now, we'll call the main element properties the same as the text properties:
    current_element->text_color[0] = current_element->color[0] = color[0];
    current_element->text_color[1] = current_element->color[1] = color[1];
    current_element->text_color[2] = current_element->color[2] = color[2];
    current_element->text_color[3] = current_element->color[3] = color[3];
    current_element->location[0] = location[0];
    current_element->location[1] = location[1];
    current_element->size[0] = size;
    current_element->size[1] = size;

    if(!current_element->initted){
      text_create_buffer(UI_POOL, text, current_element->text_buffer, &current_element->text_buffer_size, &current_element->text_length);
      current_element->initted = true;
    }

    text_print(font, location[0], location[1], size, text, color, api);
    UI_INDEX += 1;
}

#endif

#ifdef PLATFORM_USE_METAL

void ui_setup_shader() {}
void draw_immediate_quad(vec3 TopLeftPercent, vec3 BottomRightPercent,
                         vec4 Color, platform_api *api) {
  return;
} // UI_State UI, int elementID, char UIText, Vec3
  // TopLeft, Vec3 BottomRight, Vec4 Color
bool do_immediate_button(int button_index, float x, float y, float w, float h, vec4 color,
              platform_api *api) {
  return false;
}

#endif
