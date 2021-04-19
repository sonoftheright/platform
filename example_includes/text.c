// #include <stdbool.h>
#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #ifdef _WIN32
// #include "./win_dirent.h"
// #ifdef _WIN64
// #endif
// #elif __APPLE__
// #include <dirent.h>
// #endif

// #define STB_DEFINE
// #include "stb/stb.h"
#define STB_TRUETYPE_IMPLEMENTATION // force following
#include "stb_truetype.h"
// #define STB_RECT_PACK_IMPLEMENTATION
// #include "stb_rect_pack.h"

#define FONT_BITMAP_WIDTH (int)1024
#define FONT_BITMAP_HEIGHT (int)1024
#define FONT_RENDER_SIZE (float)64.0

#ifdef PLATFORM_USE_OPENGL

typedef struct {
  unsigned char ttf_buffer[1 << 20];
  unsigned char temp_bitmap[FONT_BITMAP_WIDTH * FONT_BITMAP_HEIGHT];

  stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
  GLuint ftex;

  GLuint stb_tt_VAO;
  GLuint stb_tt_VBO;
} text_font;

typedef struct {
  char *vertex_file_location;
  char *fragment_file_location;
  GLuint program_id;
  GLchar *vertex_source;
  GLchar *fragment_source;
  GLuint vertex_shader;
  GLuint fragment_shader;

  GLuint font_color_loc;
  GLuint projection_loc;
  GLuint vertex_loc;
  GLuint text_loc;

} text_shader;

text_shader Font_Shader = {
    .vertex_file_location = "./assets/font_vertex.glsl",
    .fragment_file_location = "./assets/font_fragment.glsl"};

void text_setup_shader() {
  Font_Shader.program_id = create_shader_program(
      Font_Shader.vertex_file_location, Font_Shader.fragment_file_location,
      Font_Shader.vertex_source, Font_Shader.fragment_source,
      Font_Shader.vertex_shader, Font_Shader.fragment_shader);
  link_shader_program(Font_Shader.program_id);
  Font_Shader.font_color_loc =
      glGetUniformLocation(Font_Shader.program_id, "fontColor");
  Font_Shader.vertex_loc =
      glGetUniformLocation(Font_Shader.program_id, "vertex");
  Font_Shader.projection_loc =
      glGetUniformLocation(Font_Shader.program_id, "projection");
  Font_Shader.text_loc = glGetUniformLocation(Font_Shader.program_id, "text");
  p_check_gl_error("Font_Shaders_Program");
}

// @TODO remember to create this function for other render APIs - not just
// OpenGL
// @TODO migrate over to use the better packing methods recommended in
// stb_truetype:
//           stbtt_PackBegin()
//           stbtt_PackBegin()
//           stbtt_PackSetOversampling()
//           stbtt_PackFontRanges()
//           stbtt_PackEnd()
//           stbtt_GetPackedQuad()

void text_init_font(char *filename, text_font *font) {
  FILE *infile;
  infile = p_fopen(filename, "rb");
  // FILE *infile = stb_p_fopen("/Library/Fonts/Arial.ttf", "rb");
  if (!infile) {
    fprintf(stderr, "\nError opening font file %s\n", filename);
    return;
    // fprintf(stderr, "\nError opening font file /Library/Fonts/Arial.ttf\n");
  }
  fread(font->ttf_buffer, 1, 1 << 20, infile);
  stbtt_BakeFontBitmap(font->ttf_buffer, 0, FONT_RENDER_SIZE, font->temp_bitmap,
                       FONT_BITMAP_WIDTH, FONT_BITMAP_HEIGHT, 32, 96,
                       font->cdata); // no guarantee this fits!
  // can free ttf_buffer at this point
  glGenTextures(1, &font->ftex);
  glBindTexture(GL_TEXTURE_2D, font->ftex);
  glTexImage2D(
      GL_TEXTURE_2D, // target texture
      0,      // level-of-detail number (0 is base image, n is the nth mipmap
              // reduction image)
      GL_RED, // internalFormat (specifies # of color components in texture)
              // (stb creates a monochrome bitmap, so we use GL_RED)
      FONT_BITMAP_WIDTH,  // width of texture
      FONT_BITMAP_HEIGHT, // height of texture
      0,                  // border - value must be 0.
      GL_RED, // format of the pixel data (stb creates a monochrome bitmap, so
              // we use GL_RED)
      GL_UNSIGNED_BYTE, // type of pixel data
      font->temp_bitmap // data to fill texture with
  );
  // can free temp_bitmap at this point
  // free(temp_bitmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  fclose(infile);
}

void text_print(text_font *font, float rel_x, float rel_y, float size,
                char *text, vec4 color, platform_api *api) {

  static int buffersGenerated = 0;
  if (buffersGenerated == 0) {
    glGenVertexArrays(1, &font->stb_tt_VAO);
    glGenBuffers(1, &font->stb_tt_VBO);
    buffersGenerated = 1;
  }

  // assume orthographic projection with units = screen pixels, origin at top
  // left
  // glEnable(GL_TEXTURE_2D); // fires invalid enum error atm...

  // float scale = 1.0; // maybe use this for retina displays?

  // float xscale, yscale;
  // glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
  // size = size * xscale;

  float screen_x = rel_x * api->draw.size[0];
  float screen_y = rel_y * api->draw.size[1];

  // printf("x,y is %f, %f\n", x, y);
  // api->quit = true;

  float text_render_size = size * api->window.scale;

  // @TODO: In order to make this faster, we should do a loop through the string
  // first to find how many characters there are, then fill an array of that
  // size with the quad data, and drawArrays once per string, instead of per
  // quad:

  // set initial positions to 0,0
  // so we can scale properly, then position things where they need to go.
  float x = 0;
  float y = 0;

  while (*text) {
    if (*text >= 32) { //  && *text < 128
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(font->cdata, FONT_BITMAP_WIDTH, FONT_BITMAP_HEIGHT,
                         *text - 32, &x, &y, &q,
                         1); // 1=opengl & d3d10+,0=d3d9

      glBindVertexArray(font->stb_tt_VAO);
      glBindBuffer(GL_ARRAY_BUFFER, font->stb_tt_VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glUseProgram(Font_Shader.program_id);

      // mat4x4 projection;
      // mat4x4 scale_matrix;
      mat4x4 out_matrix;
      // mat4x4 translate_matrix;

      p_mat4x4_identity(out_matrix);
      // mat4x4_identity(scale_matrix);
      // mat4x4_identity(translate_matrix);
      // mat4x4_identity(projection);
      // (void)(size);
      // UNUSED(scale_matrix);

      // mat4x4_ortho(projection, 0.0f, api->draw.size[0], api->draw.size[1],
      // 0.0f,
      //              -1.0f, 1.0f);
      // mat4x4_scale_aniso(scale_matrix, scale_matrix, text_render_size,
      //                    text_render_size, text_render_size);
      // mat4x4_translate(translate_matrix, gl_coord_x, gl_coord_y, 0.0);
      // (void)gl_coord_x;
      // (void)gl_coord_y;

      // mat4x4_mul(out_matrix, out_matrix, projection);

      // mat4x4_mul(out_matrix, out_matrix, scale_matrix);
      // mat4x4_mul(out_matrix, out_matrix, translate_matrix);
      glUniformMatrix4fv(Font_Shader.projection_loc, 1, GL_FALSE, (const GLfloat *)out_matrix);

      glUniform4f(Font_Shader.font_color_loc, color[0], color[1], color[2], color[3]);

      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(font->stb_tt_VAO);

      // Update VBO for each character
      // formatted as x,y locations, then u,v texture coordinates
      // values first scaled, then positioned, then
      // converted to gl coords:
      GLfloat vertices[6][4] = {
          {((q.x0 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0,
           (((q.y0 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0,
           q.s0, q.t0}, // upper left
          {((q.x0 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0,
           (((q.y1 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0,
           q.s0, q.t1}, // lower left
          {((q.x1 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0,
           (((q.y1 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0,
           q.s1, q.t1}, // lower right

          {((q.x0 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0,
           (((q.y0 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0,
           q.s0, q.t0}, // upper left
          {((q.x1 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0,
           (((q.y1 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0,
           q.s1, q.t1}, // lower right
          {((q.x1 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0,
           (((q.y0 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0,
           q.s1, q.t0}, // upper right
      };

      // // Original:
      // GLfloat vertices[6][4] = {
      //     {q.x0, q.y0, q.s0, q.t0}, // upper left
      //     {q.x0, q.y1, q.s0, q.t1}, // lower left
      //     {q.x1, q.y1, q.s1, q.t1}, // lower right
      //     {q.x0, q.y0, q.s0, q.t0}, // upper left
      //     {q.x1, q.y1, q.s1, q.t1}, // lower right
      //     {q.x1, q.y0, q.s1, q.t0}, // upper right
      // };

      glBindTexture(GL_TEXTURE_2D, font->ftex);

      // Update content of VBO memory
      glBindBuffer(GL_ARRAY_BUFFER, font->stb_tt_VBO);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // Render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);

      // glDeleteVertexArrays(1, &font->stb_tt_VAO);
      // glDeleteBuffers(1, &font->stb_tt_VBO);

      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);

      ++text;
    }
  }
}

void text_create_buffer(POOL *pool, char *text, void *out_buffer, int *out_size, int *out_length){
  int text_length = p_strlen(text);
  int buffer_size = sizeof(float) * 6 * 4 * text_length;
  *out_length = text_length;
  *out_size = buffer_size;

  out_buffer = pool_alloc(pool, buffer_size);
}

void text_print_from_cache(text_font *font, float rel_x, float rel_y, float size, char *text, vec4 color, float *text_buffer, platform_api *api) {

  static int buffersGenerated = 0;
  if (buffersGenerated == 0) {
    glGenVertexArrays(1, &font->stb_tt_VAO);
    glGenBuffers(1, &font->stb_tt_VBO);
    buffersGenerated = 1;
  }

  // assume orthographic projection with units = screen pixels, origin at top
  // left
  // glEnable(GL_TEXTURE_2D); // fires invalid enum error atm...

  // float scale = 1.0; // maybe use this for retina displays?

  // float xscale, yscale;
  // glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
  // size = size * xscale;

  float screen_x = rel_x * api->draw.size[0];
  float screen_y = rel_y * api->draw.size[1];

  // printf("x,y is %f, %f\n", x, y);
  // api->quit = true;

  float text_render_size = size * api->window.scale;

  // @TODO: In order to make this faster, we should do a loop through the string
  // first to find how many characters there are, then fill an array of that
  // size with the quad data, and drawArrays once per string, instead of per
  // quad:

  // set initial positions to 0,0
  // so we can scale properly, then position things where they need to go.
  float x = 0;
  float y = 0;
  int i = 0;

  while (*text) {
    if (*text >= 32) { //  && *text < 128
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(font->cdata, FONT_BITMAP_WIDTH, FONT_BITMAP_HEIGHT,
                         *text - 32, &x, &y, &q,
                         1); // 1=opengl & d3d10+,0=d3d9

      glBindVertexArray(font->stb_tt_VAO);
      glBindBuffer(GL_ARRAY_BUFFER, font->stb_tt_VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glUseProgram(Font_Shader.program_id);

      // mat4x4 projection;
      // mat4x4 scale_matrix;
      mat4x4 out_matrix;
      // mat4x4 translate_matrix;

      p_mat4x4_identity(out_matrix);
      // mat4x4_identity(scale_matrix);
      // mat4x4_identity(translate_matrix);
      // mat4x4_identity(projection);
      // (void)(size);
      // UNUSED(scale_matrix);

      // mat4x4_ortho(projection, 0.0f, api->draw.size[0], api->draw.size[1],
      // 0.0f,
      //              -1.0f, 1.0f);
      // mat4x4_scale_aniso(scale_matrix, scale_matrix, text_render_size,
      //                    text_render_size, text_render_size);
      // mat4x4_translate(translate_matrix, gl_coord_x, gl_coord_y, 0.0);
      // (void)gl_coord_x;
      // (void)gl_coord_y;

      // mat4x4_mul(out_matrix, out_matrix, projection);

      // mat4x4_mul(out_matrix, out_matrix, scale_matrix);
      // mat4x4_mul(out_matrix, out_matrix, translate_matrix);
      glUniformMatrix4fv(Font_Shader.projection_loc, 1, GL_FALSE, (const GLfloat *)out_matrix);

      glUniform4f(Font_Shader.font_color_loc, color[0], color[1], color[2], color[3]);

      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(font->stb_tt_VAO);

      // Update VBO for each character
      // formatted as x,y locations, then u,v texture coordinates
      // values first scaled, then positioned, then
      // converted to gl coords:
      // upper left:
      text_buffer[i * 0] = ((q.x0 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0;
      text_buffer[i * 1] = (((q.y0 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0;
      text_buffer[i * 2] = q.s0;
      text_buffer[i * 3] = q.t0;

      text_buffer[i * 4] = ((q.x0 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0;
      text_buffer[i * 5] = (((q.y1 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0;
      text_buffer[i * 6] = q.s0;
      text_buffer[i * 7] = q.t1;

      text_buffer[i * 8] = ((q.x1 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0;
      text_buffer[i * 9] = (((q.y1 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0;
      text_buffer[i * 10] = q.s1;
      text_buffer[i * 11] = q.t1;

      text_buffer[i * 12] = ((q.x0 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0;
      text_buffer[i * 13] = (((q.y0 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0;
      text_buffer[i * 14] = q.s0;
      text_buffer[i * 15] = q.t0;

      text_buffer[i * 16] = ((q.x1 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0;
      text_buffer[i * 17] = (((q.y1 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0;
      text_buffer[i * 18] = q.s1;
      text_buffer[i * 19] = q.t1;

      text_buffer[i * 20] = ((q.x1 * text_render_size + screen_x) / api->draw.size[0]) / 0.5 - 1.0;
      text_buffer[i * 21] = (((q.y0 * text_render_size + screen_y) / api->draw.size[1]) / 0.5 - 1.0) * -1.0;
      text_buffer[i * 22] = q.s1;
      text_buffer[i * 23] = q.t0;

      // // Original:
      // GLfloat vertices[6][4] = {
      //     {q.x0, q.y0, q.s0, q.t0}, // upper left
      //     {q.x0, q.y1, q.s0, q.t1}, // lower left
      //     {q.x1, q.y1, q.s1, q.t1}, // lower right
      //     {q.x0, q.y0, q.s0, q.t0}, // upper left
      //     {q.x1, q.y1, q.s1, q.t1}, // lower right
      //     {q.x1, q.y0, q.s1, q.t0}, // upper right
      // };

      glBindTexture(GL_TEXTURE_2D, font->ftex);

      // Update content of VBO memory
      glBindBuffer(GL_ARRAY_BUFFER, font->stb_tt_VBO);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(text_buffer), text_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // Render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);

      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);

      ++text;
      i++;
    }
  }
}



void text_view_atlas(text_font *font, float rel_x, float rel_y, float rel_w,
                     float rel_h, vec4 color, platform_api *api) {

  glGenVertexArrays(1, &font->stb_tt_VAO);
  glGenBuffers(1, &font->stb_tt_VBO);
  glBindVertexArray(font->stb_tt_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, font->stb_tt_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUseProgram(Font_Shader.program_id);

  mat4x4 projection;
  p_mat4x4_ortho(projection, 0.0f, api->draw.size[0], api->draw.size[1], 0.0f,
                 -1.0f, 1.0f);
  glUniformMatrix4fv(Font_Shader.projection_loc, 1, GL_FALSE,
                     (const GLfloat *)projection);

  glUniform4f(Font_Shader.font_color_loc, color[0], color[1], color[2],
              color[3]);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(font->stb_tt_VAO);

  // GLfloat scale = 1.0; // maybe use this for retina displays?
  float x = rel_x * api->draw.size[0];
  float y = rel_y * api->draw.size[1];

  float w = rel_w * api->draw.size[0];
  float h = rel_h * api->draw.size[1];

  // Update VBO for each character
  // formatted as x,y locations, then x,y texture coordinates
  GLfloat vertices[6][4] = {
      {x, y, 0.0, 0.0},         // upper left
      {x, y + h, 0.0, 1.0},     // lower left
      {x + w, y + h, 1.0, 1.0}, // lower right

      {x, y, 0.0, 0.0},         // upper left
      {x + w, y + h, 1.0, 1.0}, // lower right
      {x + w, y, 1.0, 0.0}      // upper right

  };

  glBindTexture(GL_TEXTURE_2D, font->ftex);

  // Update content of VBO memory
  glBindBuffer(GL_ARRAY_BUFFER, font->stb_tt_VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Render quad
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDeleteVertexArrays(1, &font->stb_tt_VAO);
  glDeleteBuffers(1, &font->stb_tt_VBO);

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

#endif

#ifdef PLATFORM_USE_METAL

typedef struct {
  unsigned char ttf_buffer[1 << 20];
  unsigned char temp_bitmap[FONT_BITMAP_WIDTH * FONT_BITMAP_HEIGHT];

  stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
  uint32_t ftex;

  uint32_t stb_tt_VAO;
  uint32_t stb_tt_VBO;
} text_font;

void text_setup_shader() {}
void text_init_font(char *filename, text_font *font) {}
void text_print(text_font *font, float rel_x, float rel_y, float size,
                char *text, vec4 color, platform_api *api) {}
void text_view_atlas(text_font *font, float rel_x, float rel_y, float rel_w,
                     float rel_h, vec4 color, platform_api *api) {}

#endif
