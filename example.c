/*
THIS IS AN EXAMPLE PROGRAM TO MAKE SURE THAT THE PLATFORM CODE IS GOOD
*/
#define PLATFORM_USE_OPENGL
#define GL_SILENCE_DEPRECATION // hides macos openGL deprecation messages. We
                               // know already, macos...
#define TIGR_IMPLEMENTATION
#include "platform/platform.h"
#include "platform/utils.h"

// #include "example_includes/linmath.h" // linear math lib for gl stuff

#include "example_includes/gl.c" // next files rely on this
#include "example_includes/simple_render.c"
#include "example_includes/text.c"
#include "example_includes/ui.c"

static int frame_time;

int main() { // int argc, char *argv[]
  platform_api *p = p_init();
  #ifdef _WIN32
    p_log("running in Windows!\n");
  #elif __APPLE__
    p_log("running in MacOS!\n");
    signal(SIGSEGV, p_handler); // install our error handler
  #endif

  int init_window_width = 1024;
  int init_window_height = 700;
  p->window.clear_color[0] = 0.0;
  p->window.clear_color[1] = 0.18;
  p->window.clear_color[2] = 0.26;

  printf("hello platform!\n");
  p_stacktrace();

  // start a new timer with this platform version, and the '0' index.
  p_timer_start(p, 0);
  // create new window with width, height, title, and 1.0 default pixel density
  p_create_window(p, init_window_width, init_window_height,
                  "Technically A Game", 1.0);
  int ns = p_timer_end(p, 0);
  printf("creating the window took %fms.\n", ns / (1000.0 * 1000.0));

  printf("window w, h is: %i,%i\n", p->window.size[0], p->window.size[1]);
  printf("render target w, h is: %i,%i\n", p->draw.size[0], p->draw.size[1]);
  printf("lock size: %i\n", p->draw.lock_size);
  printf("aspect is %f\n", p->window.aspect);

  // x,y,z camera starting location
  p->camera.location[0] = 0.0;
  p->camera.location[1] = 0.0;
  p->camera.location[2] = -5.0;
  // camera move speed, mouse sensitivity
  p->camera.move_speed = 0.00000001;
  p->camera.sensitivity = 0.0005;

  // set up the shader for displaying text:
  text_setup_shader();

  // load up a new font
  text_font *assistant_font = malloc(sizeof(text_font));
  text_init_font("./assets/Assistant-Regular.ttf", assistant_font);

  // set up the shader for simple vertex-colored 3D meshes
  simple_render_setup_shader();

  int forward = 'W';
  int back = 'S';
  int right = 'D';
  int left = 'A';

  // define the simple mesh we'll be rendering:
  int vertex_count = 3 * 12;
  float simple_render_vertices[3 * 3 * 12] = {
      -1.0, -1.0, -1.0, -1.0, -1.0, 1.0,  -1.0, 1.0,  1.0,  //
      1.0,  1.0,  -1.0, -1.0, -1.0, -1.0, -1.0, 1.0,  -1.0, //
      1.0,  -1.0, 1.0,  -1.0, -1.0, -1.0, 1.0,  -1.0, -1.0, //
      1.0,  1.0,  -1.0, 1.0,  -1.0, -1.0, -1.0, -1.0, -1.0, //
      -1.0, -1.0, -1.0, -1.0, 1.0,  1.0,  -1.0, 1.0,  -1.0,
      1.0,  -1.0, 1.0,  -1.0, -1.0, 1.0,  -1.0, -1.0, -1.0,
      -1.0, 1.0,  1.0,  -1.0, -1.0, 1.0,  1.0,  -1.0, 1.0,
      -1.0, 1.0,  1.0,  1.0,  -1.0, 1.0,  1.0,  1.0,  1.0, // added
      1.0,  1.0,  1.0,  1.0,  -1.0, -1.0, 1.0,  1.0,  -1.0,
      1.0,  -1.0, -1.0, 1.0,  1.0,  1.0,  1.0,  -1.0, 1.0,
      1.0,  1.0,  1.0,  1.0,  1.0,  -1.0, -1.0, 1.0,  -1.0,
      1.0,  1.0,  1.0,  -1.0, 1.0,  -1.0, -1.0, 1.0,  1.0,
  };
  float simple_render_colors[3 * 4 * 12] = {
      0.583, 0.771, 0.014, 1.0,   0.609, 0.115, 0.436, 1.0,   0.327, 0.483,
      0.844, 1.0,   0.822, 0.569, 0.201, 1.0,   0.435, 0.602, 0.223, 1.0,
      0.310, 0.747, 0.185, 1.0,   0.597, 0.770, 0.761, 1.0,   0.559, 0.436,
      0.730, 1.0,   0.359, 0.583, 0.152, 1.0,   0.483, 0.596, 0.789, 1.0,
      0.559, 0.861, 0.639, 1.0,   0.195, 0.548, 0.859, 1.0,   0.014, 0.184,
      0.576, 1.0,   0.771, 0.328, 0.970, 1.0,   0.406, 0.615, 0.116, 1.0,
      0.676, 0.977, 0.133, 1.0,   0.971, 0.572, 0.833, 1.0,   0.140, 0.616,
      0.489, 1.0,   0.997, 0.513, 0.064, 1.0,   0.945, 0.719, 0.592, 1.0,
      0.543, 0.021, 0.978, 1.0,   0.279, 0.317, 0.505, 1.0,   0.167, 0.620,
      0.077, 1.0,   0.347, 0.857, 0.137, 1.0,   0.055, 0.953, 0.042, 1.0,
      0.714, 0.505, 0.345, 1.0,   0.783, 0.290, 0.734, 1.0,   0.722, 0.645,
      0.174, 1.0,   0.302, 0.455, 0.848, 1.0,   0.225, 0.587, 0.040, 1.0,
      0.517, 0.713, 0.338, 1.0,   0.053, 0.959, 0.120, 1.0,   0.393, 0.621,
      0.362, 1.0,   0.673, 0.211, 0.457, 1.0,   0.820, 0.883, 0.371, 1.0,
      0.982, 0.099, 0.879, 1.0};

  ui_init();// init our UI library for internal management stuff

  while (!p->quit) {
    // start a timer with index 1 for timing the frames
    p_timer_start(p, 1);
    // run the update loop to detect input, update vars, etc
    p_update(p);

    ui_start_frame(); // start our UI frame management

    // toggle mouselock if we hit tab
    p_handle_mouselock_toggle(p, PK_TAB);

    // change the background clear color if we resize the window,
    // mostly just for testing
    if (p->window.size[0] != init_window_width ||
        p->window.size[1] != init_window_height) {
      printf("noticed an adjustment!\n");
      init_window_width = p->window.size[0];
      init_window_height = p->window.size[1];
      p->window.clear_color[0] += 0.05;
      p->window.clear_color[1] += 0.05;
      p->window.clear_color[2] += 0.05;
    }

    // if we hit alt, switch to ortho view:
    if (p_key_down(p, PK_ALT)) {
      p->camera.ortho = !p->camera.ortho;
    }

    // if we have mouselock on, update the camera location/view
    if (p->window.mouse_lock) {
      p_update_camera_3d(p, forward, back, right, left);
    }

    // this resets all the opengl "defaults" for every frame,
    // in case one of the custom render passes updated these values
    reset_gl_state(p);

    // draw that cube mesh we defined above
    draw_simple_mesh(p, simple_render_vertices, simple_render_colors,
                     vertex_count);

    /**********************************************************************************************************************
          UI STUFF:
    **********************************************************************************************************************/

    // print the FPS in the upper left corner of the window:
    float fps_color[4] = {1, 1, 1, 0.7};
    char fps_buffer[24];
    float fps =
        (1000.0 / (frame_time <= 0 ? 1 : (frame_time / (1000.0 * 1000.0))));
    float fps_location[2] = { 0.005, 0.02 };
    sprintf(fps_buffer, "%f fps", fps);
    ui_do_text(fps_buffer, fps_location, 0.3, fps_color, assistant_font, p);
    p_check_gl_error("printing_fps_text");

    // show a small tip on switching to mouselock
    char *tip_text = "Press Tab to toggle mouselock camera";
    float tip_location[2] = {0.01, 0.06};
    ui_do_text(tip_text, tip_location, 0.4, fps_color, assistant_font, p);
    p_check_gl_error("printing_ui_tip_text");

    // show a "Quit" button for the upper right corner of the window:
    float quit_button_location[2] = { 0.87, 0.025 };
    float quit_button_size[2] = { 0.1, 0.05 };
    float quit_button_color[4] = { 0.6, 0.6, 0.6, 0.7 };
    float quit_button_text_location[2] = { 0.9, 0.06 };
    float quit_button_text_size = 0.4;
    float quit_button_text_color[4] = { 0.1, 0.1, 0.1, 0.7 };

    if(ui_do_button(quit_button_location,
        quit_button_size,
        quit_button_color,
        quit_button_text_color,
        "Quit",
        quit_button_text_location,
        quit_button_text_size,
        assistant_font,
        p)){

      // if someone clicks the "quit" button, we should quit...
      p->quit = true;
    }
    p_check_gl_error("drawing button text");

    // exit the program on escape:
    if (p_key_down(p, PK_ESCAPE)) {
      p->quit = true;
    }

    // @NOTE for the key value - use single quotes here, which indicates a
    // single character, vs null-terminated array:
    if (p_key_held(p, 'Q') && p_key_held(p, PK_CONTROL)) {
      p->quit = true;
    }

    // actually swap the window render buffer so we see our changes:
    p_swap_buffers(p);

    // store the time that this last frame took:
    frame_time = p_timer_end(p, 1);
  }

  printf("**********************\n");
  printf("QUITTING\n");
  printf("**********************\n");

  ui_close(); // clean up any UI stuff
  free(assistant_font);
  p_close(p);
  return 0;
}
