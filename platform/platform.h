#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
  #define _CRT_SECURE_NO_WARNINGS NOPE
#endif

#define p_log printf
#define p_vsnlog vsnprintf
#define p_snlog snprintf
#define p_flog fprintf
#define p_wlog wprintf
#define bool _Bool // NOTE: C99 and newer only
#define true 1
#define false 0

/*
Renderer options:

#define PLATFORM_USE_OPENGL
#define PLATFORM_USE_METAL
*/

// DEFAULT TO USING OPENGL:
#if !defined(PLATFORM_USE_OPENGL) && !defined(PLATFORM_USE_METAL)
  #define PLATFORM_USE_OPENGL
#endif

#if defined(__APPLE__) && defined(PLATFORM_USE_OPENGL)
  #define GL_SILENCE_DEPRECATION // hides macos openGL deprecation messages. We know already, macos...
#endif

#include <stdio.h>

// @TODO these are set arbitrarily - should they be parameters of init, or
// just figured out and set statically, like it is now?
#define P_NUM_KEYS 512          // ???
#define P_NUM_INPUT_HISTORY 256 // ???
#define P_MAX_CHARS_PER_FRAME 20
#define P_MAX_TIMERS 100
#define P_MAX_FONTS 10

#define PI         3.14159265   // The value of pi
#define Kilobytes(Value) ( (Value) * 1024 )
#define Megabytes(Value) ( Kilobytes(Value) * 1024 )
#define Gigabytes(Value) ( Megabytes(Value) * 1024 )

void p_prevent_output(){ fclose(stdout); }

#ifdef _MSC_VER
typedef unsigned __int64 p_uint64;
typedef __int64 p_int64;
#else
// ??
typedef unsigned long long p_uint64;
typedef long long p_int64;
#endif



// ----------------------------------------------------------
#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h> // @TODO eventually remove, replacing with our own defs?
  #include <shellapi.h>
  #include <winuser.h>
  #include <stdlib.h>
  #include <math.h>
  #include <DSound.h> // may require linking?
  #include <sysinfoapi.h>


  typedef long GLintptr;
  typedef unsigned char uint8_t;
  typedef unsigned short uint16_t;
  typedef unsigned int uint32_t;
  typedef unsigned long long uint64_t;

  typedef signed char int8_t;
  typedef short int16_t;
  typedef int int32_t;
  typedef long long int64_t;

  #define WIDGET_SCALE 3
  #define WIDGET_FADE 16

  int main(int argc, char *argv[]);

  static wchar_t *unicode(const char *str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
    wchar_t *dest = (wchar_t *)malloc(sizeof(wchar_t) * len);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, dest, len);
    return dest;
  }

  static BOOL UnadjustWindowRectEx(LPRECT prc, DWORD dwStyle, BOOL fMenu,
                                   DWORD dwExStyle) {
    BOOL fRc;
    RECT rc;
    SetRectEmpty(&rc);
    fRc = AdjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
    if(fRc) {
      prc->left -= rc.left;
      prc->top -= rc.top;
      prc->right -= rc.right;
      prc->bottom -= rc.bottom;
    }
    return fRc;
  }

  // We supply our own WinMain and just chain through to the user's
  // real entry point.
  #ifdef UNICODE
    int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                          LPWSTR lpCmdLine, int nCmdShow)
  #else
    int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                         LPSTR lpCmdLine, int nCmdShow)
  #endif
  {
    int n, argc;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    char **argv = (char **)calloc(argc + 1, sizeof(int));

    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    for(n = 0; n < argc; n++) {
      int len = WideCharToMultiByte(CP_UTF8, 0, wargv[n], -1, 0, 0, NULL, NULL);
      argv[n] = (char *)malloc(len);
      WideCharToMultiByte(CP_UTF8, 0, wargv[n], -1, argv[n], len, NULL, NULL);
    }
    return main(argc, argv);
  }

#endif

#ifdef PLATFORM_USE_OPENGL
  #ifdef _WIN32
    #include <GL/gl.h>
  #else
    #include <OpenGL/gl3.h>
  #endif
#endif

#ifdef PLATFORM_USE_METAL // using Metal just assumes we're in MacOS already...
  #include <Metal/Metal.h>
#endif

/*
   Modelled after Sean Barrett's platform API code (https://gist.github.com/nothings/ef38135f4aa4799e8f09069a44ded5a2).
   Sean Barrett's code is modelled after Per Vognsen's Mu (https://gist.github.com/pervognsen/6a67966c5dc4247a0021b95c8d0a7b72)
   There are many things here that we're just not using, so we commented some things out.
*/

typedef int p_point[2];
typedef float p_pointf[2];

typedef struct {
  float seconds_delta;
  double seconds;
  p_uint64 ns;
  p_uint64 ns_delta;
  p_uint64 ms;
  p_uint64 ms_delta;
} p_time_info;

typedef struct {
  float seconds_delta; // OUT
  double seconds;      // OUT
  unsigned long long ns;       // OUT
  p_uint64 ns_delta; // OUT
  p_uint64 ms;       // OUT
  p_uint64 ms_delta; // OUT

  p_uint64 ns_delta_0; // OUT
  p_uint64 ns_delta_1; // OUT
  p_uint64 ns_delta_2; // OUT
  p_uint64 ns_smoothed; // OUT

  #ifdef _WIN32
  long long ticks_per_second;
  long long initial_ticks;
  long long delta_ticks;
  long long ticks;
  #endif
  // struct {
  //   float clamped_max_seconds_delta; // IN
  //   p_time_info clamped_time;        // OUT
  //   bool paused;                     // IN
  //   p_time_info pausable_time;       // OUT
  // } eextra;
} p_time_info_toplevel;

typedef struct {
  int startTime;
  int endTime;
  int delta;
  bool already_started;
} p_timer;

typedef struct {
  bool is_down;
  bool was_pressed;
  bool was_released;
} p_button;

typedef struct {
  unsigned char is_down : 1;
  unsigned char was_pressed : 1;
  unsigned char was_released : 1;
} p_button_history;

typedef struct {
  p_point position;             // position in canvas coordinates
  p_point delta_position;       // delta of position in canvas coordinates
  p_point raw_delta_position;   // movement in abstract coordinates (raw mouse
                                // data if possible)
  p_button left, middle, right; // button transitions & state
  float delta_mousewheel;       // wheel movement in units of "lines"
  p_point window_position;      // position in windows coordinates
  p_point console_position; // position in console (textout, printf) coordinates
  int x, y;                 // copy of 'position' for convenience
} p_mouse_state;

typedef struct {
  struct {
    float x, y;           // OUT stick direction & magnitude, each -1..1
    float magnitude;      // OUT stick magnitude  0..1, magnitude of <x,y>
    float deadzone_inner; // IN 0-1 (set during INIT to device default if 0)
    float deadzone_outer; // IN 0-1 (set during INIT to 1 if 0)
    float bias; // IN curve remapping, linear if 0, -1 has larger range for
                // small motion, 1 has larger range for large motion
  } processed;
  struct {
    float x, y; // OUT stick direction
  } raw;
} p_gamepad_stick_state; // 24

typedef struct {
  bool connected;
  struct {
    p_gamepad_stick_state left;
    p_gamepad_stick_state right;
  } sticks;

  struct {
    unsigned char a, b;
    unsigned char x, y;
    unsigned char start, back;
    unsigned char trigger_left, trigger_right;
    unsigned char shoulder_left, shoulder_right;
  } buttons;

  struct {
    unsigned char right, up, left, down;
  } dpad;

  struct {
    float shoulder_left_analog, shoulder_right_analog;
    float trigger_left_analog, trigger_right_analog;
  } extra;
} p_gamepad_state; // ~80

typedef struct {
  p_uint64 id;            // OUT unique identifier for every history element
  double wallclock_seconds; // OUT unique timestamp for every history element
  p_mouse_state mouse;      // OUT mouse state at this timestamp
  p_button_history keys[P_NUM_KEYS]; // OUT keyboard state at this timestamp
  p_gamepad_state gamepad;           // OUT gamepad state at this timestamp
} p_input_history_state;             // 8+8+40+128*3+~64 => ~512

typedef struct {
  float fov;                      // IN field of view
  float fov_rad;                  // INOUT field of view in radians
  float near_clip;                // IN near render clip
  float far_clip;                 // IN far render clip

  bool ortho;                     // IN orthographic projection- default to false

  float sensitivity;              // IN how sensitive the camera should be, given mouse movement
  float move_speed;               // IN how fast to fly when moving

  float camera_right[3];          // OUT camera right vector
  float camera_forward[3];        // OUT camera forward vector
  float camera_up[3];             // OUT camera up vector


  float look_matrix[3];           // OUT camera look matrix
  float scale;

  float location[3];              // INOUT location of camera in 3D space

  float projection_matrix[4][4];  // OUT projection matrix (ie, perspective or ortho)
  float view_matrix[4][4];        // OUT view matrix (ie, lookat matrix, direction we're viewing)

  float camera_matrix[4][4];      // OUT final camera matrix to apply to all meshes
} p_camera;

typedef struct {
  bool quit; // OUT stbp_platform reports user requested exit

  struct {
    p_point
        size;             // INOUT size of client rect in pixels; 0,0 means same as display
    float scale;          // IN scale of window vs render rect size
    bool fullscreen;      // IN fullscreen (preferably borderless windowed
                          // fullscreen)
    char *title;          // IN window title
    p_point position;     // OUT position of top-left corner in screen coordinates
    bool forbid_resize;   // IN don't allow user resizing
    bool focused;         // OUT whether window is frontmost/focused
    float aspect;         // OUT aspect ratio

    bool vsync;           // IN

    bool mouse_lock;      // INOUT whether the mouse is hidden and limited to the screen
    bool prev_mouse_lock; // INOUT whether the mouse was locked in the previous frame
    bool constrain_mouse; // IN whether or not to constrain mouse movement events to within the window

    float clear_color[3];

    // struct {
    //    bool enabled;            // INOUT toggle this to show a magnifying
    //    glass int vkey;                // IN key which when held down sets the
    //    'enabled' flag int num_pixels;          // IN number of pixels in
    //    window space on each axis float zoom_factor;       // IN amount to
    //    zoom contents by
    // } magnify;

    void *window_handle;
    void *gl_context;
    void *metal_context;
    #ifdef __APPLE__
      void *autoreleasepool;
    #elif _WIN32
      int gl_legacy;
      HGLRC hglrc; // handle to the gl render context
      HDC dc; // device context
      DWORD dwStyle;
      RECT oldPos;
      wchar_t *wtitle;
      HKEY regKey;
      bool preserve_window_pos;
    #endif
    int shown;

  } window;

  struct {
    p_point size;   // INOUT size of rendertarget to render to; 0,0 means same as
                    // window
    bool pixelate;  // IN resample to screen with nearest-neighbor?
    bool lock_size; // INOUT resizing the window doesn't change display size; defaults to false.
    int x, y;       // IN copy of size.x, size.y for convenience
  } draw;           // renamed from 'display' so 'display' can refer to monitors

  p_time_info_toplevel time;

  p_mouse_state mouse;            // OUT state of the mouse & mouse buttons
  p_button prev_keys[P_NUM_KEYS]; // OUT previous state of the keyboard (TEMPORARY, until we use the history)
  p_button keys[P_NUM_KEYS];      // OUT state of the keyboard
  p_gamepad_state gamepads[4];    // OUT state of the gamepads
  int typing[P_MAX_CHARS_PER_FRAME + 1]; // OUT UNICODE characters typed since last frame, 0-terminated

  p_camera camera;

  struct {
    int one_event_per_frame;  // IN input events are doled out one per frame so
                              // their ordering is unambigous
    int preserve_all_events;  // IN input events are doled out multiple per
                              // frame, at most one of each type, see docs
    int numlock_changes_vkey; // IN change vkey values depending on numlock
                              // state (this is normal behavior on windows)
    unsigned char key_for_char[P_NUM_KEYS]; // OUT the virtual key for whatever
                                            // key is in the same location as
                                            // given character on a US keyboard
    char keynames[P_NUM_KEYS][30]; // OUT names for virtual keys; only defined
                                   // if user has pressed it at least once
    p_input_history_state history[P_NUM_INPUT_HISTORY]; // ~10KB
    bool shift, control,
        alt; // IN copy of keys[STBVK_SHIFT].is_down etc. for convenience
  } input;

  struct {
    int disable; // INIT disable sound entirely (no perf difference on windows)
    int sample_rate; // INOUT, set to -1 for no audio
    int channels;  // OUT number of channels, always 2 (someday: INOUT, defaults to 2)
    float *buffer; // IN interleaved stereo samples you output to, has minimum latency to output
    int buffer_num_aframes;     // OUT max number of samples you can set, 2x the number of 'audio frames'
    float *continuous_buffer;   // IN interleaved stereo samples that are continuously connected to previous frame, non-minimal latency
    int continuous_num_aframes; // OUT number of samples you output
    int latency_protect_ms;     // INIT controls size of continuous_buffer
    int max_oneshot_ms;         // INIT controls size of oneshot_buffer
    int continuous_latency_aframes; // IN estimated latency of continuous buffer
    int reset; // OUT if true, kills audio and next continuous buffer will start with minimal latency
    struct {
      void (*callback)(float *buffer, int num_floats);
    } advanced;
  } sound;

  struct {
    float version;      // INOUT can use STBP_GL_VERSION() to construct
    bool compatibility; // INIT request compatibility profile (@TODO: becomes *simulated* compatibility)
    bool debug_context; // INIT request a debug context
    int renderbuffer;   // OUT renderbuffer to render to (set by default)
  } opengl;

  struct {
    bool show_frame_time; // IN displays frame times in top-left corner

    struct {
      p_point size; // INOUT size in pixels of console; 0,0 means same as window
      float scale;  // INIT scale of console pixels relative to window pixels
                    // (sets size)
      bool show;    // INOUT if true, console is visible
      bool autoshow; // IN printing to the console sets show to true
      int show_vkey; // IN this key toggles show
    } console;

    struct {
      p_pointf mouselook_degrees; // OUT absolute mouselook in degrees
      float mouselook_scale;      // IN scale from mousemove-units to DEGREES
      p_pointf
          wasd_movement; // OUT desired speed -1 to 1 w/ +x = forward, +y = left
      p_pointf wasd_smoothed;  // OUT automatically smoothed desired speed
      float smooth_start_time; // IN  0 to 1 time in seconds
      float smooth_stop_time;  // IN  0 to 1 time in seconds
    } fps_input;

    struct {
      int absolute_steps; // OUT the total wallclock physics steps
      int step_ns; // IN desired number of nanoseconds per physics step

      int prev_steps_delta; // OUT how many steps in the frame before last
      int steps_delta; // OUT how many steps in the last frame

      int prev_steps_remainder; // OUT number of nanoseconds remaining after the last step in the frame before last
      int steps_remainder; // OUT number of nanoseconds remaining after the last step
    } physics;

    struct {
      void *buffer; // must be enough memory for all audio buffers, OpenGL
                    // compatability mode, etc.
      size_t size_in_bytes;
    } memory;

    p_gamepad_state gamepad; // whichever gamepad most recently changed
  } extra;

  // these allow you to see how much time is being spent in internals of
  // stb_platform startup
  struct {
    struct {
      float phase_0;
      float phase_1;
      float phase_2;
      float phase_3;
      float phase_4;
    } startup_timing;
    struct {
      float phase_0;
      float phase_1;
      float phase_2;
      float phase_3;
      float phase_4;
      float internal;
    } window_timing;

    p_timer timers[P_MAX_TIMERS];
    int started_timers[P_MAX_TIMERS];
    int timer_count;

    struct {
      int64_t physical_memory;
      int64_t cpu_count;
    } hardware;

    bool initted;
  } system;
} platform_api;


// these are console colors, so we can print out errors (and other things) more visibly.
// use like: p_log(RED "this is a big red message!\n");
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

// simple 'here' check to make sure we're reaching a function call
#define p_here() p_log(CYAN "here: " BOLDCYAN "%s(), %s:%i\n" RESET, __func__, __FILE__, __LINE__)

// Reads a single UTF8 codepoint.
const char *_p_decode_utf8(const char *text, int *cp) {
  unsigned char c = *text++;
  int extra = 0, min = 0;
  *cp = 0;
  if(c >= 0xf0) {
    *cp = c & 0x07;
    extra = 3;
    min = 0x10000;
  } else if(c >= 0xe0) {
    *cp = c & 0x0f;
    extra = 2;
    min = 0x800;
  } else if(c >= 0xc0) {
    *cp = c & 0x1f;
    extra = 1;
    min = 0x80;
  } else if(c >= 0x80) {
    *cp = 0xfffd;
  } else
    *cp = c;
  while(extra--) {
    c = *text++;
    if((c & 0xc0) != 0x80) {
      *cp = 0xfffd;
      break;
    }
    (*cp) = ((*cp) << 6) | (c & 0x3f);
  }
  if(*cp < min) *cp = 0xfffd;
  return text;
}

#ifdef PLATFORM_USE_OPENGL

#define GL_2D 0x0600
#define GL_2_BYTES 0x1407
#define GL_3D 0x0601
#define GL_3D_COLOR 0x0602
#define GL_3D_COLOR_TEXTURE 0x0603
#define GL_3_BYTES 0x1408
#define GL_4D_COLOR_TEXTURE 0x0604
#define GL_4_BYTES 0x1409
#define GL_ACCUM 0x0100
#define GL_ACCUM_ALPHA_BITS 0x0D5B
#define GL_ACCUM_BLUE_BITS 0x0D5A
#define GL_ACCUM_BUFFER_BIT 0x00000200
#define GL_ACCUM_CLEAR_VALUE 0x0B80
#define GL_ACCUM_GREEN_BITS 0x0D59
#define GL_ACCUM_RED_BITS 0x0D58
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_ADD 0x0104
#define GL_ADD_SIGNED 0x8574
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_ALIASED_POINT_SIZE_RANGE 0x846D
// #define GL_ALL_ATTRIB_BITS 0xFFFFFFFF // defined in <GL/gl.h>
#define GL_ALPHA 0x1906
#define GL_ALPHA12 0x803D
#define GL_ALPHA16 0x803E
#define GL_ALPHA4 0x803B
#define GL_ALPHA8 0x803C
#define GL_ALPHA_BIAS 0x0D1D
#define GL_ALPHA_BITS 0x0D55
#define GL_ALPHA_INTEGER 0x8D97
#define GL_ALPHA_SCALE 0x0D1C
#define GL_ALPHA_TEST 0x0BC0
#define GL_ALPHA_TEST_FUNC 0x0BC1
#define GL_ALPHA_TEST_REF 0x0BC2
#define GL_ALREADY_SIGNALED 0x911A
#define GL_ALWAYS 0x0207
#define GL_AMBIENT 0x1200
#define GL_AMBIENT_AND_DIFFUSE 0x1602
#define GL_AND 0x1501
#define GL_AND_INVERTED 0x1504
#define GL_AND_REVERSE 0x1502
#define GL_ANY_SAMPLES_PASSED 0x8C2F
#define GL_ARRAY_BUFFER 0x8892
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_ATTRIB_STACK_DEPTH 0x0BB0
#define GL_AUTO_NORMAL 0x0D80
#define GL_AUX0 0x0409
#define GL_AUX1 0x040A
#define GL_AUX2 0x040B
#define GL_AUX3 0x040C
#define GL_AUX_BUFFERS 0x0C00
#define GL_BACK 0x0405
#define GL_BACK_LEFT 0x0402
#define GL_BACK_RIGHT 0x0403
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#define GL_BGRA_INTEGER 0x8D9B
#define GL_BGR_INTEGER 0x8D9A
#define GL_BITMAP 0x1A00
#define GL_BITMAP_TOKEN 0x0704
#define GL_BLEND 0x0BE2
#define GL_BLEND_COLOR 0x8005
#define GL_BLEND_DST 0x0BE0
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_EQUATION 0x8009
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_BLEND_SRC 0x0BE1
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLUE 0x1905
#define GL_BLUE_BIAS 0x0D1B
#define GL_BLUE_BITS 0x0D54
#define GL_BLUE_INTEGER 0x8D96
#define GL_BLUE_SCALE 0x0D1A
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_BUFFER 0x82E0
#define GL_BUFFER_ACCESS 0x88BB
#define GL_BUFFER_ACCESS_FLAGS 0x911F
#define GL_BUFFER_MAPPED 0x88BC
#define GL_BUFFER_MAP_LENGTH 0x9120
#define GL_BUFFER_MAP_OFFSET 0x9121
#define GL_BUFFER_MAP_POINTER 0x88BD
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_BYTE 0x1400
#define GL_C3F_V3F 0x2A24
#define GL_C4F_N3F_V3F 0x2A26
#define GL_C4UB_V2F 0x2A22
#define GL_C4UB_V3F 0x2A23
#define GL_CCW 0x0901
#define GL_CLAMP 0x2900
#define GL_CLAMP_FRAGMENT_COLOR 0x891B
#define GL_CLAMP_READ_COLOR 0x891C
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_CLAMP_VERTEX_COLOR 0x891A
#define GL_CLEAR 0x1500
#define GL_CLIENT_ACTIVE_TEXTURE 0x84E1
// #define GL_CLIENT_ALL_ATTRIB_BITS 0xFFFFFFFF // defined in <GL/gl.h>
#define GL_CLIENT_ATTRIB_STACK_DEPTH 0x0BB1
#define GL_CLIENT_PIXEL_STORE_BIT 0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT 0x00000002
#define GL_CLIP_DISTANCE0 0x3000
#define GL_CLIP_DISTANCE1 0x3001
#define GL_CLIP_DISTANCE2 0x3002
#define GL_CLIP_DISTANCE3 0x3003
#define GL_CLIP_DISTANCE4 0x3004
#define GL_CLIP_DISTANCE5 0x3005
#define GL_CLIP_DISTANCE6 0x3006
#define GL_CLIP_DISTANCE7 0x3007
#define GL_CLIP_PLANE0 0x3000
#define GL_CLIP_PLANE1 0x3001
#define GL_CLIP_PLANE2 0x3002
#define GL_CLIP_PLANE3 0x3003
#define GL_CLIP_PLANE4 0x3004
#define GL_CLIP_PLANE5 0x3005
#define GL_COEFF 0x0A00
#define GL_COLOR 0x1800
#define GL_COLOR_ARRAY 0x8076
#define GL_COLOR_ARRAY_BUFFER_BINDING 0x8898
#define GL_COLOR_ARRAY_POINTER 0x8090
#define GL_COLOR_ARRAY_SIZE 0x8081
#define GL_COLOR_ARRAY_STRIDE 0x8083
#define GL_COLOR_ARRAY_TYPE 0x8082
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_COLOR_ATTACHMENT16 0x8CF0
#define GL_COLOR_ATTACHMENT17 0x8CF1
#define GL_COLOR_ATTACHMENT18 0x8CF2
#define GL_COLOR_ATTACHMENT19 0x8CF3
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT20 0x8CF4
#define GL_COLOR_ATTACHMENT21 0x8CF5
#define GL_COLOR_ATTACHMENT22 0x8CF6
#define GL_COLOR_ATTACHMENT23 0x8CF7
#define GL_COLOR_ATTACHMENT24 0x8CF8
#define GL_COLOR_ATTACHMENT25 0x8CF9
#define GL_COLOR_ATTACHMENT26 0x8CFA
#define GL_COLOR_ATTACHMENT27 0x8CFB
#define GL_COLOR_ATTACHMENT28 0x8CFC
#define GL_COLOR_ATTACHMENT29 0x8CFD
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT30 0x8CFE
#define GL_COLOR_ATTACHMENT31 0x8CFF
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_COLOR_CLEAR_VALUE 0x0C22
#define GL_COLOR_INDEX 0x1900
#define GL_COLOR_INDEXES 0x1603
#define GL_COLOR_LOGIC_OP 0x0BF2
#define GL_COLOR_MATERIAL 0x0B57
#define GL_COLOR_MATERIAL_FACE 0x0B55
#define GL_COLOR_MATERIAL_PARAMETER 0x0B56
#define GL_COLOR_SUM 0x8458
#define GL_COLOR_WRITEMASK 0x0C23
#define GL_COMBINE 0x8570
#define GL_COMBINE_ALPHA 0x8572
#define GL_COMBINE_RGB 0x8571
#define GL_COMPARE_REF_TO_TEXTURE 0x884E
#define GL_COMPARE_R_TO_TEXTURE 0x884E
#define GL_COMPILE 0x1300
#define GL_COMPILE_AND_EXECUTE 0x1301
#define GL_COMPILE_STATUS 0x8B81
#define GL_COMPRESSED_ALPHA 0x84E9
#define GL_COMPRESSED_INTENSITY 0x84EC
#define GL_COMPRESSED_LUMINANCE 0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA 0x84EB
#define GL_COMPRESSED_RED 0x8225
#define GL_COMPRESSED_RED_RGTC1 0x8DBB
#define GL_COMPRESSED_RG 0x8226
#define GL_COMPRESSED_RGB 0x84ED
#define GL_COMPRESSED_RGBA 0x84EE
#define GL_COMPRESSED_RG_RGTC2 0x8DBD
#define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
#define GL_COMPRESSED_SIGNED_RG_RGTC2 0x8DBE
#define GL_COMPRESSED_SLUMINANCE 0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA 0x8C4B
#define GL_COMPRESSED_SRGB 0x8C48
#define GL_COMPRESSED_SRGB_ALPHA 0x8C49
#define GL_COMPRESSED_TEXTURE_FORMATS 0x86A3
#define GL_CONDITION_SATISFIED 0x911C
#define GL_CONSTANT 0x8576
#define GL_CONSTANT_ALPHA 0x8003
#define GL_CONSTANT_ATTENUATION 0x1207
#define GL_CONSTANT_COLOR 0x8001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_CONTEXT_FLAGS 0x821E
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#ifdef _WIN32
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001 // predefined in <OpenGL/gl3.h>
#endif
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB 0x00000004
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_COORD_REPLACE 0x8862
#define GL_COPY 0x1503
#define GL_COPY_INVERTED 0x150C
#define GL_COPY_PIXEL_TOKEN 0x0706
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_CULL_FACE 0x0B44
#define GL_CULL_FACE_MODE 0x0B45
#define GL_CURRENT_BIT 0x00000001
#define GL_CURRENT_COLOR 0x0B00
#define GL_CURRENT_FOG_COORD 0x8453
#define GL_CURRENT_FOG_COORDINATE 0x8453
#define GL_CURRENT_INDEX 0x0B01
#define GL_CURRENT_NORMAL 0x0B02
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_CURRENT_QUERY 0x8865
#define GL_CURRENT_RASTER_COLOR 0x0B04
#define GL_CURRENT_RASTER_DISTANCE 0x0B09
#define GL_CURRENT_RASTER_INDEX 0x0B05
#define GL_CURRENT_RASTER_POSITION 0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID 0x0B08
#define GL_CURRENT_RASTER_SECONDARY_COLOR 0x845F
#define GL_CURRENT_RASTER_TEXTURE_COORDS 0x0B06
#define GL_CURRENT_SECONDARY_COLOR 0x8459
#define GL_CURRENT_TEXTURE_COORDS 0x0B03
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_CW 0x0900
#define GL_DEBUG_CALLBACK_FUNCTION 0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM 0x8245
#define GL_DEBUG_GROUP_STACK_DEPTH 0x826D
#define GL_DEBUG_LOGGED_MESSAGES 0x9145
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_POP_GROUP 0x826A
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PUSH_GROUP 0x8269
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DECAL 0x2101
#define GL_DECR 0x1E03
#define GL_DECR_WRAP 0x8508
#define GL_DELETE_STATUS 0x8B80
#define GL_DEPTH 0x1801
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_DEPTH32F_STENCIL8 0x8CAD
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_DEPTH_BIAS 0x0D1F
#define GL_DEPTH_BITS 0x0D56
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_DEPTH_CLAMP 0x864F
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_COMPONENT 0x1902
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_DEPTH_COMPONENT32 0x81A7
#define GL_DEPTH_COMPONENT32F 0x8CAC
#define GL_DEPTH_FUNC 0x0B74
#define GL_DEPTH_RANGE 0x0B70
#define GL_DEPTH_SCALE 0x0D1E
#define GL_DEPTH_STENCIL 0x84F9
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_DEPTH_TEST 0x0B71
#define GL_DEPTH_TEXTURE_MODE 0x884B
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DIFFUSE 0x1201
#define GL_DISPLAY_LIST 0x82E7
#define GL_DITHER 0x0BD0
#define GL_DOMAIN 0x0A02
#define GL_DONT_CARE 0x1100
#define GL_DOT3_RGB 0x86AE
#define GL_DOT3_RGBA 0x86AF
#define GL_DOUBLE 0x140A
#define GL_DOUBLEBUFFER 0x0C32
#define GL_DRAW_BUFFER 0x0C01
#define GL_DRAW_BUFFER0 0x8825
#define GL_DRAW_BUFFER1 0x8826
#define GL_DRAW_BUFFER10 0x882F
#define GL_DRAW_BUFFER11 0x8830
#define GL_DRAW_BUFFER12 0x8831
#define GL_DRAW_BUFFER13 0x8832
#define GL_DRAW_BUFFER14 0x8833
#define GL_DRAW_BUFFER15 0x8834
#define GL_DRAW_BUFFER2 0x8827
#define GL_DRAW_BUFFER3 0x8828
#define GL_DRAW_BUFFER4 0x8829
#define GL_DRAW_BUFFER5 0x882A
#define GL_DRAW_BUFFER6 0x882B
#define GL_DRAW_BUFFER7 0x882C
#define GL_DRAW_BUFFER8 0x882D
#define GL_DRAW_BUFFER9 0x882E
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#ifdef _WIN32
#define GL_DRAW_FRAMEBUFFER_BINDING 0x8CA6 // defined in <OpenGL/gl3.h>
#endif
#define GL_DRAW_PIXEL_TOKEN 0x0705
#define GL_DST_ALPHA 0x0304
#define GL_DST_COLOR 0x0306
#define GL_DYNAMIC_COPY 0x88EA
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_EDGE_FLAG 0x0B43
#define GL_EDGE_FLAG_ARRAY 0x8079
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING 0x889B
#define GL_EDGE_FLAG_ARRAY_POINTER 0x8093
#define GL_EDGE_FLAG_ARRAY_STRIDE 0x808C
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_EMISSION 0x1600
#define GL_ENABLE_BIT 0x00002000
#define GL_EQUAL 0x0202
#define GL_EQUIV 0x1509
#define GL_EVAL_BIT 0x00010000
#define GL_EXP 0x0800
#define GL_EXP2 0x0801
#define GL_EXTENSIONS 0x1F03
#define GL_EYE_LINEAR 0x2400
#define GL_EYE_PLANE 0x2502
#define GL_FALSE 0
#define GL_FASTEST 0x1101
#define GL_FEEDBACK 0x1C01
#define GL_FEEDBACK_BUFFER_POINTER 0x0DF0
#define GL_FEEDBACK_BUFFER_SIZE 0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE 0x0DF2
#define GL_FILL 0x1B02
#define GL_FIRST_VERTEX_CONVENTION 0x8E4D
#define GL_FIXED_ONLY 0x891D
#define GL_FLAT 0x1D00
#define GL_FLOAT 0x1406
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT2x3 0x8B65
#define GL_FLOAT_MAT2x4 0x8B66
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_FLOAT_MAT3x4 0x8B68
#define GL_FLOAT_MAT4 0x8B5C
#define GL_FLOAT_MAT4x2 0x8B69
#define GL_FLOAT_MAT4x3 0x8B6A
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_FOG 0x0B60
#define GL_FOG_BIT 0x00000080
#define GL_FOG_COLOR 0x0B66
#define GL_FOG_COORD 0x8451
#define GL_FOG_COORDINATE 0x8451
#define GL_FOG_COORDINATE_ARRAY 0x8457
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING 0x889D
#define GL_FOG_COORDINATE_ARRAY_POINTER 0x8456
#define GL_FOG_COORDINATE_ARRAY_STRIDE 0x8455
#define GL_FOG_COORDINATE_ARRAY_TYPE 0x8454
#define GL_FOG_COORDINATE_SOURCE 0x8450
#define GL_FOG_COORD_ARRAY 0x8457
#define GL_FOG_COORD_ARRAY_BUFFER_BINDING 0x889D
#define GL_FOG_COORD_ARRAY_POINTER 0x8456
#define GL_FOG_COORD_ARRAY_STRIDE 0x8455
#define GL_FOG_COORD_ARRAY_TYPE 0x8454
#define GL_FOG_COORD_SRC 0x8450
#define GL_FOG_DENSITY 0x0B62
#define GL_FOG_END 0x0B64
#define GL_FOG_HINT 0x0C54
#define GL_FOG_INDEX 0x0B61
#define GL_FOG_MODE 0x0B65
#define GL_FOG_START 0x0B63
#define GL_FRAGMENT_DEPTH 0x8452
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_FRAMEBUFFER 0x8D40
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_BINDING 0x8CA6
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_DEFAULT 0x8218
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#define GL_FRAMEBUFFER_UNDEFINED 0x8219
#define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
#define GL_FRONT 0x0404
#define GL_FRONT_AND_BACK 0x0408
#define GL_FRONT_FACE 0x0B46
#define GL_FRONT_LEFT 0x0400
#define GL_FRONT_RIGHT 0x0401
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_FUNC_SUBTRACT 0x800A
#define GL_GENERATE_MIPMAP 0x8191
#define GL_GENERATE_MIPMAP_HINT 0x8192
#define GL_GEOMETRY_INPUT_TYPE 0x8917
#define GL_GEOMETRY_OUTPUT_TYPE 0x8918
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_GEOMETRY_VERTICES_OUT 0x8916
#define GL_GEQUAL 0x0206
#define GL_GREATER 0x0204
#define GL_GREEN 0x1904
#define GL_GREEN_BIAS 0x0D19
#define GL_GREEN_BITS 0x0D53
#define GL_GREEN_INTEGER 0x8D95
#define GL_GREEN_SCALE 0x0D18
#define GL_GUILTY_CONTEXT_RESET_ARB 0x8253
#define GL_HALF_FLOAT 0x140B
#define GL_HINT_BIT 0x00008000
#define GL_INCR 0x1E02
#define GL_INCR_WRAP 0x8507
#define GL_INDEX 0x8222
#define GL_INDEX_ARRAY 0x8077
#define GL_INDEX_ARRAY_BUFFER_BINDING 0x8899
#define GL_INDEX_ARRAY_POINTER 0x8091
#define GL_INDEX_ARRAY_STRIDE 0x8086
#define GL_INDEX_ARRAY_TYPE 0x8085
#define GL_INDEX_BITS 0x0D51
#define GL_INDEX_CLEAR_VALUE 0x0C20
#define GL_INDEX_LOGIC_OP 0x0BF1
#define GL_INDEX_MODE 0x0C30
#define GL_INDEX_OFFSET 0x0D13
#define GL_INDEX_SHIFT 0x0D12
#define GL_INDEX_WRITEMASK 0x0C21
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_INNOCENT_CONTEXT_RESET_ARB 0x8254
#define GL_INT 0x1404
#define GL_INTENSITY 0x8049
#define GL_INTENSITY12 0x804C
#define GL_INTENSITY16 0x804D
#define GL_INTENSITY4 0x804A
#define GL_INTENSITY8 0x804B
#define GL_INTERLEAVED_ATTRIBS 0x8C8C
#define GL_INTERPOLATE 0x8575
#define GL_INT_2_10_10_10_REV 0x8D9F
#define GL_INT_SAMPLER_1D 0x8DC9
#define GL_INT_SAMPLER_1D_ARRAY 0x8DCE
#define GL_INT_SAMPLER_2D 0x8DCA
#define GL_INT_SAMPLER_2D_ARRAY 0x8DCF
#define GL_INT_SAMPLER_2D_MULTISAMPLE 0x9109
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_INT_SAMPLER_2D_RECT 0x8DCD
#define GL_INT_SAMPLER_3D 0x8DCB
#define GL_INT_SAMPLER_BUFFER 0x8DD0
#define GL_INT_SAMPLER_CUBE 0x8DCC
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#ifdef _WIN32
#define GL_INVALID_INDEX 0xFFFFFFFF // predefined in <OpenGL/gl3.h>
#endif
#define GL_INVALID_OPERATION 0x0502
#define GL_INVALID_VALUE 0x0501
#define GL_INVERT 0x150A
#define GL_KEEP 0x1E00
#define GL_LAST_VERTEX_CONVENTION 0x8E4E
#define GL_LEFT 0x0406
#define GL_LEQUAL 0x0203
#define GL_LESS 0x0201
#define GL_LIGHT0 0x4000
#define GL_LIGHT1 0x4001
#define GL_LIGHT2 0x4002
#define GL_LIGHT3 0x4003
#define GL_LIGHT4 0x4004
#define GL_LIGHT5 0x4005
#define GL_LIGHT6 0x4006
#define GL_LIGHT7 0x4007
#define GL_LIGHTING 0x0B50
#define GL_LIGHTING_BIT 0x00000040
#define GL_LIGHT_MODEL_AMBIENT 0x0B53
#define GL_LIGHT_MODEL_COLOR_CONTROL 0x81F8
#define GL_LIGHT_MODEL_LOCAL_VIEWER 0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE 0x0B52
#define GL_LINE 0x1B01
#define GL_LINEAR 0x2601
#define GL_LINEAR_ATTENUATION 0x1208
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_LINES 0x0001
#define GL_LINES_ADJACENCY 0x000A
#define GL_LINE_BIT 0x00000004
#define GL_LINE_LOOP 0x0002
#define GL_LINE_RESET_TOKEN 0x0707
#define GL_LINE_SMOOTH 0x0B20
#define GL_LINE_SMOOTH_HINT 0x0C52
#define GL_LINE_STIPPLE 0x0B24
#define GL_LINE_STIPPLE_PATTERN 0x0B25
#define GL_LINE_STIPPLE_REPEAT 0x0B26
#define GL_LINE_STRIP 0x0003
#define GL_LINE_STRIP_ADJACENCY 0x000B
#define GL_LINE_TOKEN 0x0702
#define GL_LINE_WIDTH 0x0B21
#define GL_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_LINE_WIDTH_RANGE 0x0B22
#define GL_LINK_STATUS 0x8B82
#define GL_LIST_BASE 0x0B32
#define GL_LIST_BIT 0x00020000
#define GL_LIST_INDEX 0x0B33
#define GL_LIST_MODE 0x0B30
#define GL_LOAD 0x0101
// #define GL_LOGIC_OP 0x0BF1 // defined in GL/gl.h for v1.0 & v1.1
#define GL_LOGIC_OP_MODE 0x0BF0
#define GL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define GL_LOWER_LEFT 0x8CA1
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE12 0x8041
#define GL_LUMINANCE12_ALPHA12 0x8047
#define GL_LUMINANCE12_ALPHA4 0x8046
#define GL_LUMINANCE16 0x8042
#define GL_LUMINANCE16_ALPHA16 0x8048
#define GL_LUMINANCE4 0x803F
#define GL_LUMINANCE4_ALPHA4 0x8043
#define GL_LUMINANCE6_ALPHA2 0x8044
#define GL_LUMINANCE8 0x8040
#define GL_LUMINANCE8_ALPHA8 0x8045
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_MAJOR_VERSION 0x821B
#define GL_MAP1_COLOR_4 0x0D90
#define GL_MAP1_GRID_DOMAIN 0x0DD0
#define GL_MAP1_GRID_SEGMENTS 0x0DD1
#define GL_MAP1_INDEX 0x0D91
#define GL_MAP1_NORMAL 0x0D92
#define GL_MAP1_TEXTURE_COORD_1 0x0D93
#define GL_MAP1_TEXTURE_COORD_2 0x0D94
#define GL_MAP1_TEXTURE_COORD_3 0x0D95
#define GL_MAP1_TEXTURE_COORD_4 0x0D96
#define GL_MAP1_VERTEX_3 0x0D97
#define GL_MAP1_VERTEX_4 0x0D98
#define GL_MAP2_COLOR_4 0x0DB0
#define GL_MAP2_GRID_DOMAIN 0x0DD2
#define GL_MAP2_GRID_SEGMENTS 0x0DD3
#define GL_MAP2_INDEX 0x0DB1
#define GL_MAP2_NORMAL 0x0DB2
#define GL_MAP2_TEXTURE_COORD_1 0x0DB3
#define GL_MAP2_TEXTURE_COORD_2 0x0DB4
#define GL_MAP2_TEXTURE_COORD_3 0x0DB5
#define GL_MAP2_TEXTURE_COORD_4 0x0DB6
#define GL_MAP2_VERTEX_3 0x0DB7
#define GL_MAP2_VERTEX_4 0x0DB8
#define GL_MAP_COLOR 0x0D10
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_STENCIL 0x0D11
#define GL_MAP_UNSYNCHRONIZED_BIT 0x0020
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MATRIX_MODE 0x0BA0
#define GL_MAX 0x8008
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
#define GL_MAX_ATTRIB_STACK_DEPTH 0x0D35
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH 0x0D3B
#define GL_MAX_CLIP_DISTANCES 0x0D32
#define GL_MAX_CLIP_PLANES 0x0D32
#define GL_MAX_COLOR_ATTACHMENTS 0x8CDF
#define GL_MAX_COLOR_TEXTURE_SAMPLES 0x910E
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS 0x8A2E
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH 0x826C
#define GL_MAX_DEBUG_LOGGED_MESSAGES 0x9144
#define GL_MAX_DEBUG_MESSAGE_LENGTH 0x9143
#define GL_MAX_DEPTH_TEXTURE_SAMPLES 0x910F
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS 0x88FC
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_MAX_EVAL_ORDER 0x0D30
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS 0x9125
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS 0x8A2D
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES 0x8DE0
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS 0x8A2C
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_INTEGER_SAMPLES 0x9110
#define GL_MAX_LABEL_LENGTH 0x82E8
#define GL_MAX_LIGHTS 0x0D31
#define GL_MAX_LIST_NESTING 0x0B31
#define GL_MAX_MODELVIEW_STACK_DEPTH 0x0D36
#define GL_MAX_NAME_STACK_DEPTH 0x0D37
#define GL_MAX_PIXEL_MAP_TABLE 0x0D34
#define GL_MAX_PROGRAM_TEXEL_OFFSET 0x8905
#define GL_MAX_PROJECTION_STACK_DEPTH 0x0D38
#define GL_MAX_RECTANGLE_TEXTURE_SIZE 0x84F8
#define GL_MAX_RENDERBUFFER_SIZE 0x84E8
#define GL_MAX_SAMPLES 0x8D57
#define GL_MAX_SAMPLE_MASK_WORDS 0x8E59
#define GL_MAX_SERVER_WAIT_TIMEOUT 0x9111
#define GL_MAX_TEXTURE_BUFFER_SIZE 0x8C2B
#define GL_MAX_TEXTURE_COORDS 0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_MAX_TEXTURE_LOD_BIAS 0x84FD
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_TEXTURE_STACK_DEPTH 0x0D39
#define GL_MAX_TEXTURE_UNITS 0x84E2
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_MAX_UNIFORM_BLOCK_SIZE 0x8A30
#define GL_MAX_UNIFORM_BUFFER_BINDINGS 0x8A2F
#define GL_MAX_VARYING_COMPONENTS 0x8B4B
#define GL_MAX_VARYING_FLOATS 0x8B4B
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS 0x9122
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_VERTEX_UNIFORM_BLOCKS 0x8A2B
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
#define GL_MAX_VIEWPORT_DIMS 0x0D3A
#define GL_MIN 0x8007
#define GL_MINOR_VERSION 0x821C
#define GL_MIN_PROGRAM_TEXEL_OFFSET 0x8904
#define GL_MIRRORED_REPEAT 0x8370
#define GL_MODELVIEW 0x1700
#define GL_MODELVIEW_MATRIX 0x0BA6
#define GL_MODELVIEW_STACK_DEPTH 0x0BA3
#define GL_MODULATE 0x2100
#define GL_MULT 0x0103
#define GL_MULTISAMPLE 0x809D
#define GL_MULTISAMPLE_ARB 0x809D
#define GL_MULTISAMPLE_BIT 0x20000000
#define GL_MULTISAMPLE_BIT_ARB 0x20000000
#define GL_N3F_V3F 0x2A25
#define GL_NAME_STACK_DEPTH 0x0D70
#define GL_NAND 0x150E
#define GL_NEAREST 0x2600
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_NEVER 0x0200
#define GL_NICEST 0x1102
#define GL_NONE 0
#define GL_NOOP 0x1505
#define GL_NOR 0x1508
#define GL_NORMALIZE 0x0BA1
#define GL_NORMAL_ARRAY 0x8075
#define GL_NORMAL_ARRAY_BUFFER_BINDING 0x8897
#define GL_NORMAL_ARRAY_POINTER 0x808F
#define GL_NORMAL_ARRAY_STRIDE 0x807F
#define GL_NORMAL_ARRAY_TYPE 0x807E
#define GL_NORMAL_MAP 0x8511
#define GL_NOTEQUAL 0x0205
#define GL_NO_ERROR 0
#define GL_NO_RESET_NOTIFICATION_ARB 0x8261
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_NUM_EXTENSIONS 0x821D
#define GL_OBJECT_LINEAR 0x2401
#define GL_OBJECT_PLANE 0x2501
#define GL_OBJECT_TYPE 0x9112
#define GL_ONE 1
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_ONE_MINUS_SRC1_ALPHA 0x88FB
#define GL_ONE_MINUS_SRC1_COLOR 0x88FA
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_OPERAND0_ALPHA 0x8598
#define GL_OPERAND0_RGB 0x8590
#define GL_OPERAND1_ALPHA 0x8599
#define GL_OPERAND1_RGB 0x8591
#define GL_OPERAND2_ALPHA 0x859A
#define GL_OPERAND2_RGB 0x8592
#define GL_OR 0x1507
#define GL_ORDER 0x0A01
#define GL_OR_INVERTED 0x150D
#define GL_OR_REVERSE 0x150B
#define GL_OUT_OF_MEMORY 0x0505
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_PACK_IMAGE_HEIGHT 0x806C
#define GL_PACK_LSB_FIRST 0x0D01
#define GL_PACK_ROW_LENGTH 0x0D02
#define GL_PACK_SKIP_IMAGES 0x806B
#define GL_PACK_SKIP_PIXELS 0x0D04
#define GL_PACK_SKIP_ROWS 0x0D03
#define GL_PACK_SWAP_BYTES 0x0D00
#define GL_PASS_THROUGH_TOKEN 0x0700
#define GL_PERSPECTIVE_CORRECTION_HINT 0x0C50
#define GL_PIXEL_MAP_A_TO_A 0x0C79
#define GL_PIXEL_MAP_A_TO_A_SIZE 0x0CB9
#define GL_PIXEL_MAP_B_TO_B 0x0C78
#define GL_PIXEL_MAP_B_TO_B_SIZE 0x0CB8
#define GL_PIXEL_MAP_G_TO_G 0x0C77
#define GL_PIXEL_MAP_G_TO_G_SIZE 0x0CB7
#define GL_PIXEL_MAP_I_TO_A 0x0C75
#define GL_PIXEL_MAP_I_TO_A_SIZE 0x0CB5
#define GL_PIXEL_MAP_I_TO_B 0x0C74
#define GL_PIXEL_MAP_I_TO_B_SIZE 0x0CB4
#define GL_PIXEL_MAP_I_TO_G 0x0C73
#define GL_PIXEL_MAP_I_TO_G_SIZE 0x0CB3
#define GL_PIXEL_MAP_I_TO_I 0x0C70
#define GL_PIXEL_MAP_I_TO_I_SIZE 0x0CB0
#define GL_PIXEL_MAP_I_TO_R 0x0C72
#define GL_PIXEL_MAP_I_TO_R_SIZE 0x0CB2
#define GL_PIXEL_MAP_R_TO_R 0x0C76
#define GL_PIXEL_MAP_R_TO_R_SIZE 0x0CB6
#define GL_PIXEL_MAP_S_TO_S 0x0C71
#define GL_PIXEL_MAP_S_TO_S_SIZE 0x0CB1
#define GL_PIXEL_MODE_BIT 0x00000020
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PIXEL_PACK_BUFFER_BINDING 0x88ED
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_PIXEL_UNPACK_BUFFER_BINDING 0x88EF
#define GL_POINT 0x1B00
#define GL_POINTS 0x0000
#define GL_POINT_BIT 0x00000002
#define GL_POINT_DISTANCE_ATTENUATION 0x8129
#define GL_POINT_FADE_THRESHOLD_SIZE 0x8128
#define GL_POINT_SIZE 0x0B11
#define GL_POINT_SIZE_GRANULARITY 0x0B13
#define GL_POINT_SIZE_MAX 0x8127
#define GL_POINT_SIZE_MIN 0x8126
#define GL_POINT_SIZE_RANGE 0x0B12
#define GL_POINT_SMOOTH 0x0B10
#define GL_POINT_SMOOTH_HINT 0x0C51
#define GL_POINT_SPRITE 0x8861
#define GL_POINT_SPRITE_COORD_ORIGIN 0x8CA0
#define GL_POINT_TOKEN 0x0701
#define GL_POLYGON 0x0009
#define GL_POLYGON_BIT 0x00000008
#define GL_POLYGON_MODE 0x0B40
#define GL_POLYGON_OFFSET_FACTOR 0x8038
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_POLYGON_OFFSET_LINE 0x2A02
#define GL_POLYGON_OFFSET_POINT 0x2A01
#define GL_POLYGON_OFFSET_UNITS 0x2A00
#define GL_POLYGON_SMOOTH 0x0B41
#define GL_POLYGON_SMOOTH_HINT 0x0C53
#define GL_POLYGON_STIPPLE 0x0B42
#define GL_POLYGON_STIPPLE_BIT 0x00000010
#define GL_POLYGON_TOKEN 0x0703
#define GL_POSITION 0x1203
#define GL_PREVIOUS 0x8578
#define GL_PRIMARY_COLOR 0x8577
#define GL_PRIMITIVES_GENERATED 0x8C87
#define GL_PRIMITIVE_RESTART 0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX 0x8F9E
#define GL_PROGRAM 0x82E2
#define GL_PROGRAM_PIPELINE 0x82E4
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_PROJECTION 0x1701
#define GL_PROJECTION_MATRIX 0x0BA7
#define GL_PROJECTION_STACK_DEPTH 0x0BA4
#define GL_PROVOKING_VERTEX 0x8E4F
#define GL_PROXY_TEXTURE_1D 0x8063
#define GL_PROXY_TEXTURE_1D_ARRAY 0x8C19
#define GL_PROXY_TEXTURE_2D 0x8064
#define GL_PROXY_TEXTURE_2D_ARRAY 0x8C1B
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE 0x9101
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_PROXY_TEXTURE_3D 0x8070
#define GL_PROXY_TEXTURE_CUBE_MAP 0x851B
#define GL_PROXY_TEXTURE_RECTANGLE 0x84F7
#define GL_Q 0x2003
#define GL_QUADRATIC_ATTENUATION 0x1209
#define GL_QUADS 0x0007
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_QUAD_STRIP 0x0008
#define GL_QUERY 0x82E3
#define GL_QUERY_BY_REGION_NO_WAIT 0x8E16
#define GL_QUERY_BY_REGION_WAIT 0x8E15
#define GL_QUERY_COUNTER_BITS 0x8864
#define GL_QUERY_NO_WAIT 0x8E14
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_QUERY_WAIT 0x8E13
#define GL_R 0x2002
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_R16 0x822A
#define GL_R16F 0x822D
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R16_SNORM 0x8F98
#define GL_R32F 0x822E
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_R3_G3_B2 0x2A10
#define GL_R8 0x8229
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R8_SNORM 0x8F94
#define GL_RASTERIZER_DISCARD 0x8C89
#define GL_READ_BUFFER 0x0C02
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_READ_FRAMEBUFFER_BINDING 0x8CAA
#define GL_READ_ONLY 0x88B8
#define GL_READ_WRITE 0x88BA
#define GL_RED 0x1903
#define GL_RED_BIAS 0x0D15
#define GL_RED_BITS 0x0D52
#define GL_RED_INTEGER 0x8D94
#define GL_RED_SCALE 0x0D14
#define GL_REFLECTION_MAP 0x8512
#define GL_RENDER 0x1C00
#define GL_RENDERBUFFER 0x8D41
#define GL_RENDERBUFFER_ALPHA_SIZE 0x8D53
#define GL_RENDERBUFFER_BINDING 0x8CA7
#define GL_RENDERBUFFER_BLUE_SIZE 0x8D52
#define GL_RENDERBUFFER_DEPTH_SIZE 0x8D54
#define GL_RENDERBUFFER_GREEN_SIZE 0x8D51
#define GL_RENDERBUFFER_HEIGHT 0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT 0x8D44
#define GL_RENDERBUFFER_RED_SIZE 0x8D50
#define GL_RENDERBUFFER_SAMPLES 0x8CAB
#define GL_RENDERBUFFER_STENCIL_SIZE 0x8D55
#define GL_RENDERBUFFER_WIDTH 0x8D42
#define GL_RENDERER 0x1F01
#define GL_RENDER_MODE 0x0C40
#define GL_REPEAT 0x2901
#define GL_REPLACE 0x1E01
#define GL_RESCALE_NORMAL 0x803A
#define GL_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GL_RETURN 0x0102
#define GL_RG 0x8227
#define GL_RG16 0x822C
#define GL_RG16F 0x822F
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG16_SNORM 0x8F99
#define GL_RG32F 0x8230
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C
#define GL_RG8 0x822B
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG8_SNORM 0x8F95
#define GL_RGB 0x1907
#define GL_RGB10 0x8052
#define GL_RGB10_A2 0x8059
#define GL_RGB10_A2UI 0x906F
#define GL_RGB12 0x8053
#define GL_RGB16 0x8054
#define GL_RGB16F 0x881B
#define GL_RGB16I 0x8D89
#define GL_RGB16UI 0x8D77
#define GL_RGB16_SNORM 0x8F9A
#define GL_RGB32F 0x8815
#define GL_RGB32I 0x8D83
#define GL_RGB32UI 0x8D71
#define GL_RGB4 0x804F
#define GL_RGB5 0x8050
#define GL_RGB5_A1 0x8057
#define GL_RGB8 0x8051
#define GL_RGB8I 0x8D8F
#define GL_RGB8UI 0x8D7D
#define GL_RGB8_SNORM 0x8F96
#define GL_RGB9_E5 0x8C3D
#define GL_RGBA 0x1908
#define GL_RGBA12 0x805A
#define GL_RGBA16 0x805B
#define GL_RGBA16F 0x881A
#define GL_RGBA16I 0x8D88
#define GL_RGBA16UI 0x8D76
#define GL_RGBA16_SNORM 0x8F9B
#define GL_RGBA2 0x8055
#define GL_RGBA32F 0x8814
#define GL_RGBA32I 0x8D82
#define GL_RGBA32UI 0x8D70
#define GL_RGBA4 0x8056
#define GL_RGBA8 0x8058
#define GL_RGBA8I 0x8D8E
#define GL_RGBA8UI 0x8D7C
#define GL_RGBA8_SNORM 0x8F97
#define GL_RGBA_INTEGER 0x8D99
#define GL_RGBA_MODE 0x0C31
#define GL_RGB_INTEGER 0x8D98
#define GL_RGB_SCALE 0x8573
#define GL_RG_INTEGER 0x8228
#define GL_RIGHT 0x0407
#define GL_S 0x2000
#define GL_SAMPLER 0x82E6
#define GL_SAMPLER_1D 0x8B5D
#define GL_SAMPLER_1D_ARRAY 0x8DC0
#define GL_SAMPLER_1D_ARRAY_SHADOW 0x8DC3
#define GL_SAMPLER_1D_SHADOW 0x8B61
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_2D_ARRAY 0x8DC1
#define GL_SAMPLER_2D_ARRAY_SHADOW 0x8DC4
#define GL_SAMPLER_2D_MULTISAMPLE 0x9108
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910B
#define GL_SAMPLER_2D_RECT 0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW 0x8B64
#define GL_SAMPLER_2D_SHADOW 0x8B62
#define GL_SAMPLER_3D 0x8B5F
#define GL_SAMPLER_BINDING 0x8919
#define GL_SAMPLER_BUFFER 0x8DC2
#define GL_SAMPLER_CUBE 0x8B60
#define GL_SAMPLER_CUBE_SHADOW 0x8DC5
#define GL_SAMPLES 0x80A9
#define GL_SAMPLES_ARB 0x80A9
#define GL_SAMPLES_PASSED 0x8914
#define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB 0x809E
#define GL_SAMPLE_ALPHA_TO_ONE 0x809F
#define GL_SAMPLE_ALPHA_TO_ONE_ARB 0x809F
#define GL_SAMPLE_BUFFERS 0x80A8
#define GL_SAMPLE_BUFFERS_ARB 0x80A8
#define GL_SAMPLE_COVERAGE 0x80A0
#define GL_SAMPLE_COVERAGE_ARB 0x80A0
#define GL_SAMPLE_COVERAGE_INVERT 0x80AB
#define GL_SAMPLE_COVERAGE_INVERT_ARB 0x80AB
#define GL_SAMPLE_COVERAGE_VALUE 0x80AA
#define GL_SAMPLE_COVERAGE_VALUE_ARB 0x80AA
#define GL_SAMPLE_MASK 0x8E51
#define GL_SAMPLE_MASK_VALUE 0x8E52
#define GL_SAMPLE_POSITION 0x8E50
#define GL_SCISSOR_BIT 0x00080000
#define GL_SCISSOR_BOX 0x0C10
#define GL_SCISSOR_TEST 0x0C11
#define GL_SECONDARY_COLOR_ARRAY 0x845E
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING 0x889C
#define GL_SECONDARY_COLOR_ARRAY_POINTER 0x845D
#define GL_SECONDARY_COLOR_ARRAY_SIZE 0x845A
#define GL_SECONDARY_COLOR_ARRAY_STRIDE 0x845C
#define GL_SECONDARY_COLOR_ARRAY_TYPE 0x845B
#define GL_SELECT 0x1C02
#define GL_SELECTION_BUFFER_POINTER 0x0DF3
#define GL_SELECTION_BUFFER_SIZE 0x0DF4
#define GL_SEPARATE_ATTRIBS 0x8C8D
#define GL_SEPARATE_SPECULAR_COLOR 0x81FA
#define GL_SET 0x150F
#define GL_SHADER 0x82E1
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_SHADER_TYPE 0x8B4F
#define GL_SHADE_MODEL 0x0B54
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_SHININESS 0x1601
#define GL_SHORT 0x1402
#define GL_SIGNALED 0x9119
#define GL_SIGNED_NORMALIZED 0x8F9C
#define GL_SINGLE_COLOR 0x81F9
#define GL_SLUMINANCE 0x8C46
#define GL_SLUMINANCE8 0x8C47
#define GL_SLUMINANCE8_ALPHA8 0x8C45
#define GL_SLUMINANCE_ALPHA 0x8C44
#define GL_SMOOTH 0x1D01
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_SMOOTH_LINE_WIDTH_RANGE 0x0B22
#define GL_SMOOTH_POINT_SIZE_GRANULARITY 0x0B13
#define GL_SMOOTH_POINT_SIZE_RANGE 0x0B12
#define GL_SOURCE0_ALPHA 0x8588
#define GL_SOURCE0_RGB 0x8580
#define GL_SOURCE1_ALPHA 0x8589
#define GL_SOURCE1_RGB 0x8581
#define GL_SOURCE2_ALPHA 0x858A
#define GL_SOURCE2_RGB 0x8582
#define GL_SPECULAR 0x1202
#define GL_SPHERE_MAP 0x2402
#define GL_SPOT_CUTOFF 0x1206
#define GL_SPOT_DIRECTION 0x1204
#define GL_SPOT_EXPONENT 0x1205
#define GL_SRC0_ALPHA 0x8588
#define GL_SRC0_RGB 0x8580
#define GL_SRC1_ALPHA 0x8589
#define GL_SRC1_COLOR 0x88F9
#define GL_SRC1_RGB 0x8581
#define GL_SRC2_ALPHA 0x858A
#define GL_SRC2_RGB 0x8582
#define GL_SRC_ALPHA 0x0302
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_SRC_COLOR 0x0300
#define GL_SRGB 0x8C40
#define GL_SRGB8 0x8C41
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_SRGB_ALPHA 0x8C42
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_STATIC_COPY 0x88E6
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STENCIL 0x1802
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_STENCIL_BITS 0x0D57
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_STENCIL_CLEAR_VALUE 0x0B91
#define GL_STENCIL_FAIL 0x0B94
#define GL_STENCIL_FUNC 0x0B92
#define GL_STENCIL_INDEX 0x1901
#define GL_STENCIL_INDEX1 0x8D46
#define GL_STENCIL_INDEX16 0x8D49
#define GL_STENCIL_INDEX4 0x8D47
#define GL_STENCIL_INDEX8 0x8D48
#define GL_STENCIL_PASS_DEPTH_FAIL 0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS 0x0B96
#define GL_STENCIL_REF 0x0B97
#define GL_STENCIL_TEST 0x0B90
#define GL_STENCIL_VALUE_MASK 0x0B93
#define GL_STENCIL_WRITEMASK 0x0B98
#define GL_STEREO 0x0C33
#define GL_STREAM_COPY 0x88E2
#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_SUBPIXEL_BITS 0x0D50
#define GL_SUBTRACT 0x84E7
#define GL_SYNC_CONDITION 0x9113
#define GL_SYNC_FENCE 0x9116
#define GL_SYNC_FLAGS 0x9115
#define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_SYNC_STATUS 0x9114
#define GL_T 0x2001
#define GL_T2F_C3F_V3F 0x2A2A
#define GL_T2F_C4F_N3F_V3F 0x2A2C
#define GL_T2F_C4UB_V3F 0x2A29
#define GL_T2F_N3F_V3F 0x2A2B
#define GL_T2F_V3F 0x2A27
#define GL_T4F_C4F_N3F_V4F 0x2A2D
#define GL_T4F_V4F 0x2A28
#define GL_TEXTURE 0x1702
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_1D_ARRAY 0x8C18
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9102
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_ALPHA_SIZE 0x805F
#define GL_TEXTURE_ALPHA_TYPE 0x8C13
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_BINDING_1D 0x8068
#define GL_TEXTURE_BINDING_1D_ARRAY 0x8C1C
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_TEXTURE_BINDING_2D_ARRAY 0x8C1D
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_TEXTURE_BINDING_BUFFER 0x8C2C
#define GL_TEXTURE_BINDING_CUBE_MAP 0x8514
#define GL_TEXTURE_BINDING_RECTANGLE 0x84F6
#define GL_TEXTURE_BIT 0x00040000
#define GL_TEXTURE_BLUE_SIZE 0x805E
#define GL_TEXTURE_BLUE_TYPE 0x8C12
#define GL_TEXTURE_BORDER 0x1005
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_TEXTURE_BUFFER 0x8C2A
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#define GL_TEXTURE_COMPARE_MODE 0x884C
// #define GL_TEXTURE_COMPONENTS 0x1003 // defined in GL/gl.h for v1.0 & v1.1
#define GL_TEXTURE_COMPRESSED 0x86A1
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE 0x86A0
#define GL_TEXTURE_COMPRESSION_HINT 0x84EF
#define GL_TEXTURE_COORD_ARRAY 0x8078
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A
#define GL_TEXTURE_COORD_ARRAY_POINTER 0x8092
#define GL_TEXTURE_COORD_ARRAY_SIZE 0x8088
#define GL_TEXTURE_COORD_ARRAY_STRIDE 0x808A
#define GL_TEXTURE_COORD_ARRAY_TYPE 0x8089
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
#define GL_TEXTURE_DEPTH 0x8071
#define GL_TEXTURE_DEPTH_SIZE 0x884A
#define GL_TEXTURE_DEPTH_TYPE 0x8C16
#define GL_TEXTURE_ENV 0x2300
#define GL_TEXTURE_ENV_COLOR 0x2201
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_TEXTURE_FILTER_CONTROL 0x8500
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_TEXTURE_GEN_MODE 0x2500
#define GL_TEXTURE_GEN_Q 0x0C63
#define GL_TEXTURE_GEN_R 0x0C62
#define GL_TEXTURE_GEN_S 0x0C60
#define GL_TEXTURE_GEN_T 0x0C61
#define GL_TEXTURE_GREEN_SIZE 0x805D
#define GL_TEXTURE_GREEN_TYPE 0x8C11
#define GL_TEXTURE_HEIGHT 0x1001
#define GL_TEXTURE_INTENSITY_SIZE 0x8061
#define GL_TEXTURE_INTENSITY_TYPE 0x8C15
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#define GL_TEXTURE_LOD_BIAS 0x8501
#define GL_TEXTURE_LUMINANCE_SIZE 0x8060
#define GL_TEXTURE_LUMINANCE_TYPE 0x8C14
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MATRIX 0x0BA8
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_PRIORITY 0x8066
#define GL_TEXTURE_RECTANGLE 0x84F5
#define GL_TEXTURE_RED_SIZE 0x805C
#define GL_TEXTURE_RED_TYPE 0x8C10
#define GL_TEXTURE_RESIDENT 0x8067
#define GL_TEXTURE_SAMPLES 0x9106
#define GL_TEXTURE_SHARED_SIZE 0x8C3F
#define GL_TEXTURE_STACK_DEPTH 0x0BA5
#define GL_TEXTURE_STENCIL_SIZE 0x88F1
#define GL_TEXTURE_SWIZZLE_A 0x8E45
#define GL_TEXTURE_SWIZZLE_B 0x8E44
#define GL_TEXTURE_SWIZZLE_G 0x8E43
#define GL_TEXTURE_SWIZZLE_R 0x8E42
#define GL_TEXTURE_SWIZZLE_RGBA 0x8E46
#define GL_TEXTURE_WIDTH 0x1000
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TIMEOUT_EXPIRED 0x911B
#ifdef _WIN32
#define GL_TIMEOUT_IGNORED 0xFFFFFFFFFFFFFFFF // defined in <OpenGL/gl3.h>
#endif
#define GL_TIMESTAMP 0x8E28
#define GL_TIME_ELAPSED 0x88BF
#define GL_TRANSFORM_BIT 0x00001000
#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_TRANSFORM_FEEDBACK_VARYINGS 0x8C83
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSPOSE_COLOR_MATRIX 0x84E6
#define GL_TRANSPOSE_MODELVIEW_MATRIX 0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX 0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX 0x84E5
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLES_ADJACENCY 0x000C
#define GL_TRIANGLE_FAN 0x0006
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_STRIP_ADJACENCY 0x000D
#define GL_TRUE 1
#define GL_UNIFORM_ARRAY_STRIDE 0x8A3C
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS 0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_BINDING 0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_UNIFORM_BLOCK_INDEX 0x8A3A
#define GL_UNIFORM_BLOCK_NAME_LENGTH 0x8A41
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_UNIFORM_BUFFER_BINDING 0x8A28
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_UNIFORM_BUFFER_SIZE 0x8A2A
#define GL_UNIFORM_BUFFER_START 0x8A29
#define GL_UNIFORM_IS_ROW_MAJOR 0x8A3E
#define GL_UNIFORM_MATRIX_STRIDE 0x8A3D
#define GL_UNIFORM_NAME_LENGTH 0x8A39
#define GL_UNIFORM_OFFSET 0x8A3B
#define GL_UNIFORM_SIZE 0x8A38
#define GL_UNIFORM_TYPE 0x8A37
#define GL_UNKNOWN_CONTEXT_RESET_ARB 0x8255
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_UNPACK_LSB_FIRST 0x0CF1
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_UNPACK_SKIP_IMAGES 0x806D
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_UNPACK_SKIP_ROWS 0x0CF3
#define GL_UNPACK_SWAP_BYTES 0x0CF0
#define GL_UNSIGNALED 0x9118
#define GL_UNSIGNED_BYTE 0x1401
#define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
#define GL_UNSIGNED_BYTE_3_3_2 0x8032
#define GL_UNSIGNED_INT 0x1405
#define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
#define GL_UNSIGNED_INT_10_10_10_2 0x8036
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#define GL_UNSIGNED_INT_5_9_9_9_REV 0x8C3E
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_UNSIGNED_INT_SAMPLER_1D 0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY 0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D 0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY 0x8DD7
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT 0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_3D 0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_BUFFER 0x8DD8
#define GL_UNSIGNED_INT_SAMPLER_CUBE 0x8DD4
#define GL_UNSIGNED_INT_VEC2 0x8DC6
#define GL_UNSIGNED_INT_VEC3 0x8DC7
#define GL_UNSIGNED_INT_VEC4 0x8DC8
#define GL_UNSIGNED_NORMALIZED 0x8C17
#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
#define GL_UPPER_LEFT 0x8CA2
#define GL_V2F 0x2A20
#define GL_V3F 0x2A21
#define GL_VALIDATE_STATUS 0x8B83
#define GL_VENDOR 0x1F00
#define GL_VERSION 0x1F02
#define GL_VERTEX_ARRAY 0x8074
#define GL_VERTEX_ARRAY_BINDING 0x85B5
#define GL_VERTEX_ARRAY_BUFFER_BINDING 0x8896
#define GL_VERTEX_ARRAY_POINTER 0x808E
#define GL_VERTEX_ARRAY_SIZE 0x807A
#define GL_VERTEX_ARRAY_STRIDE 0x807C
#define GL_VERTEX_ARRAY_TYPE 0x807B
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR 0x88FE
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER 0x88FD
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE 0x8643
#define GL_VERTEX_SHADER 0x8B31
#define GL_VIEWPORT 0x0BA2
#define GL_VIEWPORT_BIT 0x00000800
#define GL_WAIT_FAILED 0x911D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING 0x889E
#define GL_WRITE_ONLY 0x88B9
#define GL_XOR 0x1506
#define GL_ZERO 0
#define GL_ZOOM_X 0x0D16
#define GL_ZOOM_Y 0x0D17

  #ifdef _WIN32
    #ifndef APIENTRY
    #define APIENTRY
    #endif
    #ifndef APIENTRYP
    #define APIENTRYP APIENTRY *
    #endif

    typedef ptrdiff_t GLsizeiptr;
    typedef struct __GLsync *GLsync;
    typedef uint64_t GLuint64;
    typedef int64_t GLint64;
    typedef unsigned int GLenum;
    typedef unsigned char GLboolean;
    typedef unsigned int GLbitfield;
    typedef void GLvoid;
    typedef signed char GLbyte;
    typedef unsigned char GLubyte;
    typedef short GLshort;
    typedef unsigned short GLushort;
    typedef int GLint;
    typedef unsigned int GLuint;
    typedef int32_t GLclampx;
    typedef int GLsizei;
    typedef float_t GLfloat;
    typedef float_t GLclampf;
    typedef double GLdouble;
    typedef double GLclampd;
    typedef void *GLeglClientBufferEXT;
    typedef void *GLeglImageOES;
    typedef char GLchar;
    typedef char GLcharARB;
    typedef unsigned int GLhandleARB;

    typedef void ( *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
    typedef void ( *GLDEBUGPROCARB)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
    typedef void ( *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
    typedef void ( *GLDEBUGPROCAMD)(GLuint id,GLenum category,GLenum severity,GLsizei length,const GLchar *message,void *userParam);

    typedef BOOL(WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
    typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);
    // typedef HGLRC(WINAPI *PFNWGLGETCURRENTCONTEXTARBPROC)(void);
    // typedef BOOL(WINAPI *PFNWGLMAKECURRENTPROC)(HDC hDC, HGLRC hShareContext);

    typedef void (APIENTRYP PFNGLACCUMPROC)(GLenum op, GLfloat   value);
    typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum   texture);
    typedef void (APIENTRYP PFNGLALPHAFUNCPROC)(GLenum   func, GLfloat   ref);
    typedef GLboolean (APIENTRYP PFNGLARETEXTURESRESIDENTPROC)(GLsizei   n, const  GLuint  * textures, GLboolean  * residences);
    typedef void (APIENTRYP PFNGLARRAYELEMENTPROC)(GLint   i);
    typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint   program, GLuint   shader);
    typedef void (APIENTRYP PFNGLBEGINPROC)(GLenum   mode);
    typedef void (APIENTRYP PFNGLBEGINCONDITIONALRENDERPROC)(GLuint   id, GLenum   mode);
    typedef void (APIENTRYP PFNGLBEGINQUERYPROC)(GLenum   target, GLuint   id);
    typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKPROC)(GLenum   primitiveMode);
    typedef void (APIENTRYP PFNGLBINDATTRIBLOCATIONPROC)(GLuint   program, GLuint   index, const  GLchar  * name);
    typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum   target, GLuint   buffer);
    typedef void (APIENTRYP PFNGLBINDBUFFERBASEPROC)(GLenum   target, GLuint   index, GLuint   buffer);
    typedef void (APIENTRYP PFNGLBINDBUFFERRANGEPROC)(GLenum   target, GLuint   index, GLuint   buffer, GLintptr   offset, GLsizeiptr   size);
    typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONPROC)(GLuint   program, GLuint   color, const  GLchar  * name);
    typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)(GLuint   program, GLuint   colorNumber, GLuint   index, const  GLchar  * name);
    typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum   target, GLuint   framebuffer);
    typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC)(GLenum   target, GLuint   renderbuffer);
    typedef void (APIENTRYP PFNGLBINDSAMPLERPROC)(GLuint   unit, GLuint   sampler);
    typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum   target, GLuint   texture);
    typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint   array);
    typedef void (APIENTRYP PFNGLBITMAPPROC)(GLsizei   width, GLsizei   height, GLfloat   xorig, GLfloat   yorig, GLfloat   xmove, GLfloat   ymove, const  GLubyte  * bitmap);
    typedef void (APIENTRYP PFNGLBLENDCOLORPROC)(GLfloat   red, GLfloat   green, GLfloat   blue, GLfloat   alpha);
    typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum   mode);
    typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum   modeRGB, GLenum   modeAlpha);
    typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum   sfactor, GLenum   dfactor);
    typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC)(GLenum   sfactorRGB, GLenum   dfactorRGB, GLenum   sfactorAlpha, GLenum   dfactorAlpha);
    typedef void (APIENTRYP PFNGLBLITFRAMEBUFFERPROC)(GLint   srcX0, GLint   srcY0, GLint   srcX1, GLint   srcY1, GLint   dstX0, GLint   dstY0, GLint   dstX1, GLint   dstY1, GLbitfield   mask, GLenum   filter);
    typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum   target, GLsizeiptr   size, const void * data, GLenum   usage);
    typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum   target, GLintptr   offset, GLsizeiptr   size, const void * data);
    typedef void (APIENTRYP PFNGLCALLLISTPROC)(GLuint   list);
    typedef void (APIENTRYP PFNGLCALLLISTSPROC)(GLsizei   n, GLenum   type, const void * lists);
    typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum   target);
    typedef void (APIENTRYP PFNGLCLAMPCOLORPROC)(GLenum   target, GLenum   clamp);
    typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield   mask);
    typedef void (APIENTRYP PFNGLCLEARACCUMPROC)(GLfloat   red, GLfloat   green, GLfloat   blue, GLfloat   alpha);
    typedef void (APIENTRYP PFNGLCLEARBUFFERFIPROC)(GLenum   buffer, GLint   drawbuffer, GLfloat   depth, GLint   stencil);
    typedef void (APIENTRYP PFNGLCLEARBUFFERFVPROC)(GLenum   buffer, GLint   drawbuffer, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLCLEARBUFFERIVPROC)(GLenum   buffer, GLint   drawbuffer, const  GLint  * value);
    typedef void (APIENTRYP PFNGLCLEARBUFFERUIVPROC)(GLenum   buffer, GLint   drawbuffer, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat   red, GLfloat   green, GLfloat   blue, GLfloat   alpha);
    typedef void (APIENTRYP PFNGLCLEARDEPTHPROC)(GLdouble   depth);
    typedef void (APIENTRYP PFNGLCLEARINDEXPROC)(GLfloat   c);
    typedef void (APIENTRYP PFNGLCLEARSTENCILPROC)(GLint   s);
    typedef void (APIENTRYP PFNGLCLIENTACTIVETEXTUREPROC)(GLenum   texture);
    typedef GLenum (APIENTRYP PFNGLCLIENTWAITSYNCPROC)(GLsync   sync, GLbitfield   flags, GLuint64   timeout);
    typedef void (APIENTRYP PFNGLCLIPPLANEPROC)(GLenum   plane, const  GLdouble  * equation);
    typedef void (APIENTRYP PFNGLCOLOR3BPROC)(GLbyte   red, GLbyte   green, GLbyte   blue);
    typedef void (APIENTRYP PFNGLCOLOR3BVPROC)(const  GLbyte  * v);
    typedef void (APIENTRYP PFNGLCOLOR3DPROC)(GLdouble   red, GLdouble   green, GLdouble   blue);
    typedef void (APIENTRYP PFNGLCOLOR3DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLCOLOR3FPROC)(GLfloat   red, GLfloat   green, GLfloat   blue);
    typedef void (APIENTRYP PFNGLCOLOR3FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLCOLOR3IPROC)(GLint   red, GLint   green, GLint   blue);
    typedef void (APIENTRYP PFNGLCOLOR3IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLCOLOR3SPROC)(GLshort   red, GLshort   green, GLshort   blue);
    typedef void (APIENTRYP PFNGLCOLOR3SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLCOLOR3UBPROC)(GLubyte   red, GLubyte   green, GLubyte   blue);
    typedef void (APIENTRYP PFNGLCOLOR3UBVPROC)(const  GLubyte  * v);
    typedef void (APIENTRYP PFNGLCOLOR3UIPROC)(GLuint   red, GLuint   green, GLuint   blue);
    typedef void (APIENTRYP PFNGLCOLOR3UIVPROC)(const  GLuint  * v);
    typedef void (APIENTRYP PFNGLCOLOR3USPROC)(GLushort   red, GLushort   green, GLushort   blue);
    typedef void (APIENTRYP PFNGLCOLOR3USVPROC)(const  GLushort  * v);
    typedef void (APIENTRYP PFNGLCOLOR4BPROC)(GLbyte   red, GLbyte   green, GLbyte   blue, GLbyte   alpha);
    typedef void (APIENTRYP PFNGLCOLOR4BVPROC)(const  GLbyte  * v);
    typedef void (APIENTRYP PFNGLCOLOR4DPROC)(GLdouble   red, GLdouble   green, GLdouble   blue, GLdouble   alpha);
    typedef void (APIENTRYP PFNGLCOLOR4DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLCOLOR4FPROC)(GLfloat   red, GLfloat   green, GLfloat   blue, GLfloat   alpha);
    typedef void (APIENTRYP PFNGLCOLOR4FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLCOLOR4IPROC)(GLint   red, GLint   green, GLint   blue, GLint   alpha);
    typedef void (APIENTRYP PFNGLCOLOR4IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLCOLOR4SPROC)(GLshort   red, GLshort   green, GLshort   blue, GLshort   alpha);
    typedef void (APIENTRYP PFNGLCOLOR4SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLCOLOR4UBPROC)(GLubyte   red, GLubyte   green, GLubyte   blue, GLubyte   alpha);
    typedef void (APIENTRYP PFNGLCOLOR4UBVPROC)(const  GLubyte  * v);
    typedef void (APIENTRYP PFNGLCOLOR4UIPROC)(GLuint   red, GLuint   green, GLuint   blue, GLuint   alpha);
    typedef void (APIENTRYP PFNGLCOLOR4UIVPROC)(const  GLuint  * v);
    typedef void (APIENTRYP PFNGLCOLOR4USPROC)(GLushort   red, GLushort   green, GLushort   blue, GLushort   alpha);
    typedef void (APIENTRYP PFNGLCOLOR4USVPROC)(const  GLushort  * v);
    typedef void (APIENTRYP PFNGLCOLORMASKPROC)(GLboolean   red, GLboolean   green, GLboolean   blue, GLboolean   alpha);
    typedef void (APIENTRYP PFNGLCOLORMASKIPROC)(GLuint   index, GLboolean   r, GLboolean   g, GLboolean   b, GLboolean   a);
    typedef void (APIENTRYP PFNGLCOLORMATERIALPROC)(GLenum   face, GLenum   mode);
    typedef void (APIENTRYP PFNGLCOLORP3UIPROC)(GLenum   type, GLuint   color);
    typedef void (APIENTRYP PFNGLCOLORP3UIVPROC)(GLenum   type, const  GLuint  * color);
    typedef void (APIENTRYP PFNGLCOLORP4UIPROC)(GLenum   type, GLuint   color);
    typedef void (APIENTRYP PFNGLCOLORP4UIVPROC)(GLenum   type, const  GLuint  * color);
    typedef void (APIENTRYP PFNGLCOLORPOINTERPROC)(GLint   size, GLenum   type, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint   shader);
    typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE1DPROC)(GLenum   target, GLint   level, GLenum   internalformat, GLsizei   width, GLint   border, GLsizei   imageSize, const void * data);
    typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE2DPROC)(GLenum   target, GLint   level, GLenum   internalformat, GLsizei   width, GLsizei   height, GLint   border, GLsizei   imageSize, const void * data);
    typedef void (APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DPROC)(GLenum   target, GLint   level, GLenum   internalformat, GLsizei   width, GLsizei   height, GLsizei   depth, GLint   border, GLsizei   imageSize, const void * data);
    typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLsizei   width, GLenum   format, GLsizei   imageSize, const void * data);
    typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLint   yoffset, GLsizei   width, GLsizei   height, GLenum   format, GLsizei   imageSize, const void * data);
    typedef void (APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLint   yoffset, GLint   zoffset, GLsizei   width, GLsizei   height, GLsizei   depth, GLenum   format, GLsizei   imageSize, const void * data);
    typedef void (APIENTRYP PFNGLCOPYBUFFERSUBDATAPROC)(GLenum   readTarget, GLenum   writeTarget, GLintptr   readOffset, GLintptr   writeOffset, GLsizeiptr   size);
    typedef void (APIENTRYP PFNGLCOPYPIXELSPROC)(GLint   x, GLint   y, GLsizei   width, GLsizei   height, GLenum   type);
    typedef void (APIENTRYP PFNGLCOPYTEXIMAGE1DPROC)(GLenum   target, GLint   level, GLenum   internalformat, GLint   x, GLint   y, GLsizei   width, GLint   border);
    typedef void (APIENTRYP PFNGLCOPYTEXIMAGE2DPROC)(GLenum   target, GLint   level, GLenum   internalformat, GLint   x, GLint   y, GLsizei   width, GLsizei   height, GLint   border);
    typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE1DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLint   x, GLint   y, GLsizei   width);
    typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE2DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLint   yoffset, GLint   x, GLint   y, GLsizei   width, GLsizei   height);
    typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLint   yoffset, GLint   zoffset, GLint   x, GLint   y, GLsizei   width, GLsizei   height);
    typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
    typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum   type);
    typedef void (APIENTRYP PFNGLCULLFACEPROC)(GLenum   mode);
    typedef void (APIENTRYP PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC   callback, const void * userParam);
    typedef void (APIENTRYP PFNGLDEBUGMESSAGECONTROLPROC)(GLenum   source, GLenum   type, GLenum   severity, GLsizei   count, const  GLuint  * ids, GLboolean   enabled);
    typedef void (APIENTRYP PFNGLDEBUGMESSAGEINSERTPROC)(GLenum   source, GLenum   type, GLuint   id, GLenum   severity, GLsizei   length, const  GLchar  * buf);
    typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei   n, const  GLuint  * buffers);
    typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei   n, const  GLuint  * framebuffers);
    typedef void (APIENTRYP PFNGLDELETELISTSPROC)(GLuint   list, GLsizei   range);
    typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint   program);
    typedef void (APIENTRYP PFNGLDELETEQUERIESPROC)(GLsizei   n, const  GLuint  * ids);
    typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC)(GLsizei   n, const  GLuint  * renderbuffers);
    typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC)(GLsizei   count, const  GLuint  * samplers);
    typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint   shader);
    typedef void (APIENTRYP PFNGLDELETESYNCPROC)(GLsync   sync);
    typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei   n, const  GLuint  * textures);
    typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei   n, const  GLuint  * arrays);
    typedef void (APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum   func);
    typedef void (APIENTRYP PFNGLDEPTHMASKPROC)(GLboolean   flag);
    typedef void (APIENTRYP PFNGLDEPTHRANGEPROC)(GLdouble   n, GLdouble   f);
    typedef void (APIENTRYP PFNGLDETACHSHADERPROC)(GLuint   program, GLuint   shader);
    typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum   cap);
    typedef void (APIENTRYP PFNGLDISABLECLIENTSTATEPROC)(GLenum   array);
    typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint   index);
    typedef void (APIENTRYP PFNGLDISABLEIPROC)(GLenum   target, GLuint   index);
    typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum   mode, GLint   first, GLsizei   count);
    typedef void (APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum   mode, GLint   first, GLsizei   count, GLsizei   instancecount);
    typedef void (APIENTRYP PFNGLDRAWBUFFERPROC)(GLenum   buf);
    typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC)(GLsizei   n, const  GLenum  * bufs);
    typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum   mode, GLsizei   count, GLenum   type, const void * indices);
    typedef void (APIENTRYP PFNGLDRAWELEMENTSBASEVERTEXPROC)(GLenum   mode, GLsizei   count, GLenum   type, const void * indices, GLint   basevertex);
    typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum   mode, GLsizei   count, GLenum   type, const void * indices, GLsizei   instancecount);
    typedef void (APIENTRYP PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)(GLenum   mode, GLsizei   count, GLenum   type, const void * indices, GLsizei   instancecount, GLint   basevertex);
    typedef void (APIENTRYP PFNGLDRAWPIXELSPROC)(GLsizei   width, GLsizei   height, GLenum   format, GLenum   type, const void * pixels);
    typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSPROC)(GLenum   mode, GLuint   start, GLuint   end, GLsizei   count, GLenum   type, const void * indices);
    typedef void (APIENTRYP PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)(GLenum   mode, GLuint   start, GLuint   end, GLsizei   count, GLenum   type, const void * indices, GLint   basevertex);
    typedef void (APIENTRYP PFNGLEDGEFLAGPROC)(GLboolean   flag);
    typedef void (APIENTRYP PFNGLEDGEFLAGPOINTERPROC)(GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLEDGEFLAGVPROC)(const  GLboolean  * flag);
    typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum   cap);
    typedef void (APIENTRYP PFNGLENABLECLIENTSTATEPROC)(GLenum   array);
    typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint   index);
    typedef void (APIENTRYP PFNGLENABLEIPROC)(GLenum   target, GLuint   index);
    typedef void (APIENTRYP PFNGLENDPROC)(void);
    typedef void (APIENTRYP PFNGLENDCONDITIONALRENDERPROC)(void);
    typedef void (APIENTRYP PFNGLENDLISTPROC)(void);
    typedef void (APIENTRYP PFNGLENDQUERYPROC)(GLenum   target);
    typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKPROC)(void);
    typedef void (APIENTRYP PFNGLEVALCOORD1DPROC)(GLdouble   u);
    typedef void (APIENTRYP PFNGLEVALCOORD1DVPROC)(const  GLdouble  * u);
    typedef void (APIENTRYP PFNGLEVALCOORD1FPROC)(GLfloat   u);
    typedef void (APIENTRYP PFNGLEVALCOORD1FVPROC)(const  GLfloat  * u);
    typedef void (APIENTRYP PFNGLEVALCOORD2DPROC)(GLdouble   u, GLdouble   v);
    typedef void (APIENTRYP PFNGLEVALCOORD2DVPROC)(const  GLdouble  * u);
    typedef void (APIENTRYP PFNGLEVALCOORD2FPROC)(GLfloat   u, GLfloat   v);
    typedef void (APIENTRYP PFNGLEVALCOORD2FVPROC)(const  GLfloat  * u);
    typedef void (APIENTRYP PFNGLEVALMESH1PROC)(GLenum   mode, GLint   i1, GLint   i2);
    typedef void (APIENTRYP PFNGLEVALMESH2PROC)(GLenum   mode, GLint   i1, GLint   i2, GLint   j1, GLint   j2);
    typedef void (APIENTRYP PFNGLEVALPOINT1PROC)(GLint   i);
    typedef void (APIENTRYP PFNGLEVALPOINT2PROC)(GLint   i, GLint   j);
    typedef void (APIENTRYP PFNGLFEEDBACKBUFFERPROC)(GLsizei   size, GLenum   type, GLfloat  * buffer);
    typedef GLsync (APIENTRYP PFNGLFENCESYNCPROC)(GLenum   condition, GLbitfield   flags);
    typedef void (APIENTRYP PFNGLFINISHPROC)(void);
    typedef void (APIENTRYP PFNGLFLUSHPROC)(void);
    typedef void (APIENTRYP PFNGLFLUSHMAPPEDBUFFERRANGEPROC)(GLenum   target, GLintptr   offset, GLsizeiptr   length);
    typedef void (APIENTRYP PFNGLFOGCOORDPOINTERPROC)(GLenum   type, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLFOGCOORDDPROC)(GLdouble   coord);
    typedef void (APIENTRYP PFNGLFOGCOORDDVPROC)(const  GLdouble  * coord);
    typedef void (APIENTRYP PFNGLFOGCOORDFPROC)(GLfloat   coord);
    typedef void (APIENTRYP PFNGLFOGCOORDFVPROC)(const  GLfloat  * coord);
    typedef void (APIENTRYP PFNGLFOGFPROC)(GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLFOGFVPROC)(GLenum   pname, const  GLfloat  * params);
    typedef void (APIENTRYP PFNGLFOGIPROC)(GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLFOGIVPROC)(GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum   target, GLenum   attachment, GLenum   renderbuffertarget, GLuint   renderbuffer);
    typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC)(GLenum   target, GLenum   attachment, GLuint   texture, GLint   level);
    typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE1DPROC)(GLenum   target, GLenum   attachment, GLenum   textarget, GLuint   texture, GLint   level);
    typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum   target, GLenum   attachment, GLenum   textarget, GLuint   texture, GLint   level);
    typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DPROC)(GLenum   target, GLenum   attachment, GLenum   textarget, GLuint   texture, GLint   level, GLint   zoffset);
    typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURELAYERPROC)(GLenum   target, GLenum   attachment, GLuint   texture, GLint   level, GLint   layer);
    typedef void (APIENTRYP PFNGLFRONTFACEPROC)(GLenum   mode);
    typedef void (APIENTRYP PFNGLFRUSTUMPROC)(GLdouble   left, GLdouble   right, GLdouble   bottom, GLdouble   top, GLdouble   zNear, GLdouble   zFar);
    typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei   n, GLuint  * buffers);
    typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei   n, GLuint  * framebuffers);
    typedef GLuint (APIENTRYP PFNGLGENLISTSPROC)(GLsizei   range);
    typedef void (APIENTRYP PFNGLGENQUERIESPROC)(GLsizei   n, GLuint  * ids);
    typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC)(GLsizei   n, GLuint  * renderbuffers);
    typedef void (APIENTRYP PFNGLGENSAMPLERSPROC)(GLsizei   count, GLuint  * samplers);
    typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei   n, GLuint  * textures);
    typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei   n, GLuint  * arrays);
    typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum   target);
    typedef void (APIENTRYP PFNGLGETACTIVEATTRIBPROC)(GLuint   program, GLuint   index, GLsizei   bufSize, GLsizei  * length, GLint  * size, GLenum  * type, GLchar  * name);
    typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMPROC)(GLuint   program, GLuint   index, GLsizei   bufSize, GLsizei  * length, GLint  * size, GLenum  * type, GLchar  * name);
    typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)(GLuint   program, GLuint   uniformBlockIndex, GLsizei   bufSize, GLsizei  * length, GLchar  * uniformBlockName);
    typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMBLOCKIVPROC)(GLuint   program, GLuint   uniformBlockIndex, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMNAMEPROC)(GLuint   program, GLuint   uniformIndex, GLsizei   bufSize, GLsizei  * length, GLchar  * uniformName);
    typedef void (APIENTRYP PFNGLGETACTIVEUNIFORMSIVPROC)(GLuint   program, GLsizei   uniformCount, const  GLuint  * uniformIndices, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETATTACHEDSHADERSPROC)(GLuint   program, GLsizei   maxCount, GLsizei  * count, GLuint  * shaders);
    typedef GLint (APIENTRYP PFNGLGETATTRIBLOCATIONPROC)(GLuint   program, const  GLchar  * name);
    typedef void (APIENTRYP PFNGLGETBOOLEANI_VPROC)(GLenum   target, GLuint   index, GLboolean  * data);
    typedef void (APIENTRYP PFNGLGETBOOLEANVPROC)(GLenum   pname, GLboolean  * data);
    typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERI64VPROC)(GLenum   target, GLenum   pname, GLint64  * params);
    typedef void (APIENTRYP PFNGLGETBUFFERPARAMETERIVPROC)(GLenum   target, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETBUFFERPOINTERVPROC)(GLenum   target, GLenum   pname, void ** params);
    typedef void (APIENTRYP PFNGLGETBUFFERSUBDATAPROC)(GLenum   target, GLintptr   offset, GLsizeiptr   size, void * data);
    typedef void (APIENTRYP PFNGLGETCLIPPLANEPROC)(GLenum   plane, GLdouble  * equation);
    typedef void (APIENTRYP PFNGLGETCOMPRESSEDTEXIMAGEPROC)(GLenum   target, GLint   level, void * img);
    typedef GLuint (APIENTRYP PFNGLGETDEBUGMESSAGELOGPROC)(GLuint   count, GLsizei   bufSize, GLenum  * sources, GLenum  * types, GLuint  * ids, GLenum  * severities, GLsizei  * lengths, GLchar  * messageLog);
    typedef void (APIENTRYP PFNGLGETDOUBLEVPROC)(GLenum   pname, GLdouble  * data);
    typedef GLenum (APIENTRYP PFNGLGETERRORPROC)(void);
    typedef void (APIENTRYP PFNGLGETFLOATVPROC)(GLenum   pname, GLfloat  * data);
    typedef GLint (APIENTRYP PFNGLGETFRAGDATAINDEXPROC)(GLuint   program, const  GLchar  * name);
    typedef GLint (APIENTRYP PFNGLGETFRAGDATALOCATIONPROC)(GLuint   program, const  GLchar  * name);
    typedef void (APIENTRYP PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)(GLenum   target, GLenum   attachment, GLenum   pname, GLint  * params);
    typedef GLenum (APIENTRYP PFNGLGETGRAPHICSRESETSTATUSARBPROC)(void);
    typedef void (APIENTRYP PFNGLGETINTEGER64I_VPROC)(GLenum   target, GLuint   index, GLint64  * data);
    typedef void (APIENTRYP PFNGLGETINTEGER64VPROC)(GLenum   pname, GLint64  * data);
    typedef void (APIENTRYP PFNGLGETINTEGERI_VPROC)(GLenum   target, GLuint   index, GLint  * data);
    typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum   pname, GLint  * data);
    typedef void (APIENTRYP PFNGLGETLIGHTFVPROC)(GLenum   light, GLenum   pname, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETLIGHTIVPROC)(GLenum   light, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETMAPDVPROC)(GLenum   target, GLenum   query, GLdouble  * v);
    typedef void (APIENTRYP PFNGLGETMAPFVPROC)(GLenum   target, GLenum   query, GLfloat  * v);
    typedef void (APIENTRYP PFNGLGETMAPIVPROC)(GLenum   target, GLenum   query, GLint  * v);
    typedef void (APIENTRYP PFNGLGETMATERIALFVPROC)(GLenum   face, GLenum   pname, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETMATERIALIVPROC)(GLenum   face, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETMULTISAMPLEFVPROC)(GLenum   pname, GLuint   index, GLfloat  * val);
    typedef void (APIENTRYP PFNGLGETOBJECTLABELPROC)(GLenum   identifier, GLuint   name, GLsizei   bufSize, GLsizei  * length, GLchar  * label);
    typedef void (APIENTRYP PFNGLGETOBJECTPTRLABELPROC)(const void * ptr, GLsizei   bufSize, GLsizei  * length, GLchar  * label);
    typedef void (APIENTRYP PFNGLGETPIXELMAPFVPROC)(GLenum   map, GLfloat  * values);
    typedef void (APIENTRYP PFNGLGETPIXELMAPUIVPROC)(GLenum   map, GLuint  * values);
    typedef void (APIENTRYP PFNGLGETPIXELMAPUSVPROC)(GLenum   map, GLushort  * values);
    typedef void (APIENTRYP PFNGLGETPOINTERVPROC)(GLenum   pname, void ** params);
    typedef void (APIENTRYP PFNGLGETPOLYGONSTIPPLEPROC)(GLubyte  * mask);
    typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint   program, GLsizei   bufSize, GLsizei  * length, GLchar  * infoLog);
    typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint   program, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETQUERYOBJECTI64VPROC)(GLuint   id, GLenum   pname, GLint64  * params);
    typedef void (APIENTRYP PFNGLGETQUERYOBJECTIVPROC)(GLuint   id, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VPROC)(GLuint   id, GLenum   pname, GLuint64  * params);
    typedef void (APIENTRYP PFNGLGETQUERYOBJECTUIVPROC)(GLuint   id, GLenum   pname, GLuint  * params);
    typedef void (APIENTRYP PFNGLGETQUERYIVPROC)(GLenum   target, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETRENDERBUFFERPARAMETERIVPROC)(GLenum   target, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC)(GLuint   sampler, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVPROC)(GLuint   sampler, GLenum   pname, GLuint  * params);
    typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC)(GLuint   sampler, GLenum   pname, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC)(GLuint   sampler, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint   shader, GLsizei   bufSize, GLsizei  * length, GLchar  * infoLog);
    typedef void (APIENTRYP PFNGLGETSHADERSOURCEPROC)(GLuint   shader, GLsizei   bufSize, GLsizei  * length, GLchar  * source);
    typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint   shader, GLenum   pname, GLint  * params);
    typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGPROC)(GLenum   name);
    typedef const GLubyte * (APIENTRYP PFNGLGETSTRINGIPROC)(GLenum   name, GLuint   index);
    typedef void (APIENTRYP PFNGLGETSYNCIVPROC)(GLsync   sync, GLenum   pname, GLsizei   bufSize, GLsizei  * length, GLint  * values);
    typedef void (APIENTRYP PFNGLGETTEXENVFVPROC)(GLenum   target, GLenum   pname, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETTEXENVIVPROC)(GLenum   target, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETTEXGENDVPROC)(GLenum   coord, GLenum   pname, GLdouble  * params);
    typedef void (APIENTRYP PFNGLGETTEXGENFVPROC)(GLenum   coord, GLenum   pname, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETTEXGENIVPROC)(GLenum   coord, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETTEXIMAGEPROC)(GLenum   target, GLint   level, GLenum   format, GLenum   type, void * pixels);
    typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERFVPROC)(GLenum   target, GLint   level, GLenum   pname, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETTEXLEVELPARAMETERIVPROC)(GLenum   target, GLint   level, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETTEXPARAMETERIIVPROC)(GLenum   target, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETTEXPARAMETERIUIVPROC)(GLenum   target, GLenum   pname, GLuint  * params);
    typedef void (APIENTRYP PFNGLGETTEXPARAMETERFVPROC)(GLenum   target, GLenum   pname, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETTEXPARAMETERIVPROC)(GLenum   target, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)(GLuint   program, GLuint   index, GLsizei   bufSize, GLsizei  * length, GLsizei  * size, GLenum  * type, GLchar  * name);
    typedef GLuint (APIENTRYP PFNGLGETUNIFORMBLOCKINDEXPROC)(GLuint   program, const  GLchar  * uniformBlockName);
    typedef void (APIENTRYP PFNGLGETUNIFORMINDICESPROC)(GLuint   program, GLsizei   uniformCount, const  GLchar  *const* uniformNames, GLuint  * uniformIndices);
    typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint   program, const  GLchar  * name);
    typedef void (APIENTRYP PFNGLGETUNIFORMFVPROC)(GLuint   program, GLint   location, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETUNIFORMIVPROC)(GLuint   program, GLint   location, GLint  * params);
    typedef void (APIENTRYP PFNGLGETUNIFORMUIVPROC)(GLuint   program, GLint   location, GLuint  * params);
    typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIIVPROC)(GLuint   index, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIUIVPROC)(GLuint   index, GLenum   pname, GLuint  * params);
    typedef void (APIENTRYP PFNGLGETVERTEXATTRIBPOINTERVPROC)(GLuint   index, GLenum   pname, void ** pointer);
    typedef void (APIENTRYP PFNGLGETVERTEXATTRIBDVPROC)(GLuint   index, GLenum   pname, GLdouble  * params);
    typedef void (APIENTRYP PFNGLGETVERTEXATTRIBFVPROC)(GLuint   index, GLenum   pname, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETVERTEXATTRIBIVPROC)(GLuint   index, GLenum   pname, GLint  * params);
    typedef void (APIENTRYP PFNGLGETNCOLORTABLEARBPROC)(GLenum   target, GLenum   format, GLenum   type, GLsizei   bufSize, void * table);
    typedef void (APIENTRYP PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC)(GLenum   target, GLint   lod, GLsizei   bufSize, void * img);
    typedef void (APIENTRYP PFNGLGETNCONVOLUTIONFILTERARBPROC)(GLenum   target, GLenum   format, GLenum   type, GLsizei   bufSize, void * image);
    typedef void (APIENTRYP PFNGLGETNHISTOGRAMARBPROC)(GLenum   target, GLboolean   reset, GLenum   format, GLenum   type, GLsizei   bufSize, void * values);
    typedef void (APIENTRYP PFNGLGETNMAPDVARBPROC)(GLenum   target, GLenum   query, GLsizei   bufSize, GLdouble  * v);
    typedef void (APIENTRYP PFNGLGETNMAPFVARBPROC)(GLenum   target, GLenum   query, GLsizei   bufSize, GLfloat  * v);
    typedef void (APIENTRYP PFNGLGETNMAPIVARBPROC)(GLenum   target, GLenum   query, GLsizei   bufSize, GLint  * v);
    typedef void (APIENTRYP PFNGLGETNMINMAXARBPROC)(GLenum   target, GLboolean   reset, GLenum   format, GLenum   type, GLsizei   bufSize, void * values);
    typedef void (APIENTRYP PFNGLGETNPIXELMAPFVARBPROC)(GLenum   map, GLsizei   bufSize, GLfloat  * values);
    typedef void (APIENTRYP PFNGLGETNPIXELMAPUIVARBPROC)(GLenum   map, GLsizei   bufSize, GLuint  * values);
    typedef void (APIENTRYP PFNGLGETNPIXELMAPUSVARBPROC)(GLenum   map, GLsizei   bufSize, GLushort  * values);
    typedef void (APIENTRYP PFNGLGETNPOLYGONSTIPPLEARBPROC)(GLsizei   bufSize, GLubyte  * pattern);
    typedef void (APIENTRYP PFNGLGETNSEPARABLEFILTERARBPROC)(GLenum   target, GLenum   format, GLenum   type, GLsizei   rowBufSize, void * row, GLsizei   columnBufSize, void * column, void * span);
    typedef void (APIENTRYP PFNGLGETNTEXIMAGEARBPROC)(GLenum   target, GLint   level, GLenum   format, GLenum   type, GLsizei   bufSize, void * img);
    typedef void (APIENTRYP PFNGLGETNUNIFORMDVARBPROC)(GLuint   program, GLint   location, GLsizei   bufSize, GLdouble  * params);
    typedef void (APIENTRYP PFNGLGETNUNIFORMFVARBPROC)(GLuint   program, GLint   location, GLsizei   bufSize, GLfloat  * params);
    typedef void (APIENTRYP PFNGLGETNUNIFORMIVARBPROC)(GLuint   program, GLint   location, GLsizei   bufSize, GLint  * params);
    typedef void (APIENTRYP PFNGLGETNUNIFORMUIVARBPROC)(GLuint   program, GLint   location, GLsizei   bufSize, GLuint  * params);
    typedef void (APIENTRYP PFNGLHINTPROC)(GLenum   target, GLenum   mode);
    typedef void (APIENTRYP PFNGLINDEXMASKPROC)(GLuint   mask);
    typedef void (APIENTRYP PFNGLINDEXPOINTERPROC)(GLenum   type, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLINDEXDPROC)(GLdouble   c);
    typedef void (APIENTRYP PFNGLINDEXDVPROC)(const  GLdouble  * c);
    typedef void (APIENTRYP PFNGLINDEXFPROC)(GLfloat   c);
    typedef void (APIENTRYP PFNGLINDEXFVPROC)(const  GLfloat  * c);
    typedef void (APIENTRYP PFNGLINDEXIPROC)(GLint   c);
    typedef void (APIENTRYP PFNGLINDEXIVPROC)(const  GLint  * c);
    typedef void (APIENTRYP PFNGLINDEXSPROC)(GLshort   c);
    typedef void (APIENTRYP PFNGLINDEXSVPROC)(const  GLshort  * c);
    typedef void (APIENTRYP PFNGLINDEXUBPROC)(GLubyte   c);
    typedef void (APIENTRYP PFNGLINDEXUBVPROC)(const  GLubyte  * c);
    typedef void (APIENTRYP PFNGLINITNAMESPROC)(void);
    typedef void (APIENTRYP PFNGLINTERLEAVEDARRAYSPROC)(GLenum   format, GLsizei   stride, const void * pointer);
    typedef GLboolean (APIENTRYP PFNGLISBUFFERPROC)(GLuint   buffer);
    typedef GLboolean (APIENTRYP PFNGLISENABLEDPROC)(GLenum   cap);
    typedef GLboolean (APIENTRYP PFNGLISENABLEDIPROC)(GLenum   target, GLuint   index);
    typedef GLboolean (APIENTRYP PFNGLISFRAMEBUFFERPROC)(GLuint   framebuffer);
    typedef GLboolean (APIENTRYP PFNGLISLISTPROC)(GLuint   list);
    typedef GLboolean (APIENTRYP PFNGLISPROGRAMPROC)(GLuint   program);
    typedef GLboolean (APIENTRYP PFNGLISQUERYPROC)(GLuint   id);
    typedef GLboolean (APIENTRYP PFNGLISRENDERBUFFERPROC)(GLuint   renderbuffer);
    typedef GLboolean (APIENTRYP PFNGLISSAMPLERPROC)(GLuint   sampler);
    typedef GLboolean (APIENTRYP PFNGLISSHADERPROC)(GLuint   shader);
    typedef GLboolean (APIENTRYP PFNGLISSYNCPROC)(GLsync   sync);
    typedef GLboolean (APIENTRYP PFNGLISTEXTUREPROC)(GLuint   texture);
    typedef GLboolean (APIENTRYP PFNGLISVERTEXARRAYPROC)(GLuint   array);
    typedef void (APIENTRYP PFNGLLIGHTMODELFPROC)(GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLLIGHTMODELFVPROC)(GLenum   pname, const  GLfloat  * params);
    typedef void (APIENTRYP PFNGLLIGHTMODELIPROC)(GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLLIGHTMODELIVPROC)(GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLLIGHTFPROC)(GLenum   light, GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLLIGHTFVPROC)(GLenum   light, GLenum   pname, const  GLfloat  * params);
    typedef void (APIENTRYP PFNGLLIGHTIPROC)(GLenum   light, GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLLIGHTIVPROC)(GLenum   light, GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLLINESTIPPLEPROC)(GLint   factor, GLushort   pattern);
    typedef void (APIENTRYP PFNGLLINEWIDTHPROC)(GLfloat   width);
    typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint   program);
    typedef void (APIENTRYP PFNGLLISTBASEPROC)(GLuint   base);
    typedef void (APIENTRYP PFNGLLOADIDENTITYPROC)(void);
    typedef void (APIENTRYP PFNGLLOADMATRIXDPROC)(const  GLdouble  * m);
    typedef void (APIENTRYP PFNGLLOADMATRIXFPROC)(const  GLfloat  * m);
    typedef void (APIENTRYP PFNGLLOADNAMEPROC)(GLuint   name);
    typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXDPROC)(const  GLdouble  * m);
    typedef void (APIENTRYP PFNGLLOADTRANSPOSEMATRIXFPROC)(const  GLfloat  * m);
    typedef void (APIENTRYP PFNGLLOGICOPPROC)(GLenum   opcode);
    typedef void (APIENTRYP PFNGLMAP1DPROC)(GLenum   target, GLdouble   u1, GLdouble   u2, GLint   stride, GLint   order, const  GLdouble  * points);
    typedef void (APIENTRYP PFNGLMAP1FPROC)(GLenum   target, GLfloat   u1, GLfloat   u2, GLint   stride, GLint   order, const  GLfloat  * points);
    typedef void (APIENTRYP PFNGLMAP2DPROC)(GLenum   target, GLdouble   u1, GLdouble   u2, GLint   ustride, GLint   uorder, GLdouble   v1, GLdouble   v2, GLint   vstride, GLint   vorder, const  GLdouble  * points);
    typedef void (APIENTRYP PFNGLMAP2FPROC)(GLenum   target, GLfloat   u1, GLfloat   u2, GLint   ustride, GLint   uorder, GLfloat   v1, GLfloat   v2, GLint   vstride, GLint   vorder, const  GLfloat  * points);
    typedef void * (APIENTRYP PFNGLMAPBUFFERPROC)(GLenum   target, GLenum   access);
    typedef void * (APIENTRYP PFNGLMAPBUFFERRANGEPROC)(GLenum   target, GLintptr   offset, GLsizeiptr   length, GLbitfield   access);
    typedef void (APIENTRYP PFNGLMAPGRID1DPROC)(GLint   un, GLdouble   u1, GLdouble   u2);
    typedef void (APIENTRYP PFNGLMAPGRID1FPROC)(GLint   un, GLfloat   u1, GLfloat   u2);
    typedef void (APIENTRYP PFNGLMAPGRID2DPROC)(GLint   un, GLdouble   u1, GLdouble   u2, GLint   vn, GLdouble   v1, GLdouble   v2);
    typedef void (APIENTRYP PFNGLMAPGRID2FPROC)(GLint   un, GLfloat   u1, GLfloat   u2, GLint   vn, GLfloat   v1, GLfloat   v2);
    typedef void (APIENTRYP PFNGLMATERIALFPROC)(GLenum   face, GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLMATERIALFVPROC)(GLenum   face, GLenum   pname, const  GLfloat  * params);
    typedef void (APIENTRYP PFNGLMATERIALIPROC)(GLenum   face, GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLMATERIALIVPROC)(GLenum   face, GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLMATRIXMODEPROC)(GLenum   mode);
    typedef void (APIENTRYP PFNGLMULTMATRIXDPROC)(const  GLdouble  * m);
    typedef void (APIENTRYP PFNGLMULTMATRIXFPROC)(const  GLfloat  * m);
    typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXDPROC)(const  GLdouble  * m);
    typedef void (APIENTRYP PFNGLMULTTRANSPOSEMATRIXFPROC)(const  GLfloat  * m);
    typedef void (APIENTRYP PFNGLMULTIDRAWARRAYSPROC)(GLenum   mode, const  GLint  * first, const  GLsizei  * count, GLsizei   drawcount);
    typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSPROC)(GLenum   mode, const  GLsizei  * count, GLenum   type, const void *const* indices, GLsizei   drawcount);
    typedef void (APIENTRYP PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)(GLenum   mode, const  GLsizei  * count, GLenum   type, const void *const* indices, GLsizei   drawcount, const  GLint  * basevertex);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD1DPROC)(GLenum   target, GLdouble   s);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD1DVPROC)(GLenum   target, const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD1FPROC)(GLenum   target, GLfloat   s);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD1FVPROC)(GLenum   target, const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD1IPROC)(GLenum   target, GLint   s);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD1IVPROC)(GLenum   target, const  GLint  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD1SPROC)(GLenum   target, GLshort   s);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD1SVPROC)(GLenum   target, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD2DPROC)(GLenum   target, GLdouble   s, GLdouble   t);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD2DVPROC)(GLenum   target, const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD2FPROC)(GLenum   target, GLfloat   s, GLfloat   t);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD2FVPROC)(GLenum   target, const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD2IPROC)(GLenum   target, GLint   s, GLint   t);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD2IVPROC)(GLenum   target, const  GLint  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD2SPROC)(GLenum   target, GLshort   s, GLshort   t);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD2SVPROC)(GLenum   target, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD3DPROC)(GLenum   target, GLdouble   s, GLdouble   t, GLdouble   r);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD3DVPROC)(GLenum   target, const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD3FPROC)(GLenum   target, GLfloat   s, GLfloat   t, GLfloat   r);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD3FVPROC)(GLenum   target, const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD3IPROC)(GLenum   target, GLint   s, GLint   t, GLint   r);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD3IVPROC)(GLenum   target, const  GLint  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD3SPROC)(GLenum   target, GLshort   s, GLshort   t, GLshort   r);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD3SVPROC)(GLenum   target, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD4DPROC)(GLenum   target, GLdouble   s, GLdouble   t, GLdouble   r, GLdouble   q);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD4DVPROC)(GLenum   target, const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD4FPROC)(GLenum   target, GLfloat   s, GLfloat   t, GLfloat   r, GLfloat   q);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD4FVPROC)(GLenum   target, const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD4IPROC)(GLenum   target, GLint   s, GLint   t, GLint   r, GLint   q);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD4IVPROC)(GLenum   target, const  GLint  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD4SPROC)(GLenum   target, GLshort   s, GLshort   t, GLshort   r, GLshort   q);
    typedef void (APIENTRYP PFNGLMULTITEXCOORD4SVPROC)(GLenum   target, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLMULTITEXCOORDP1UIPROC)(GLenum   texture, GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLMULTITEXCOORDP1UIVPROC)(GLenum   texture, GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLMULTITEXCOORDP2UIPROC)(GLenum   texture, GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLMULTITEXCOORDP2UIVPROC)(GLenum   texture, GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLMULTITEXCOORDP3UIPROC)(GLenum   texture, GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLMULTITEXCOORDP3UIVPROC)(GLenum   texture, GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLMULTITEXCOORDP4UIPROC)(GLenum   texture, GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLMULTITEXCOORDP4UIVPROC)(GLenum   texture, GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLNEWLISTPROC)(GLuint   list, GLenum   mode);
    typedef void (APIENTRYP PFNGLNORMAL3BPROC)(GLbyte   nx, GLbyte   ny, GLbyte   nz);
    typedef void (APIENTRYP PFNGLNORMAL3BVPROC)(const  GLbyte  * v);
    typedef void (APIENTRYP PFNGLNORMAL3DPROC)(GLdouble   nx, GLdouble   ny, GLdouble   nz);
    typedef void (APIENTRYP PFNGLNORMAL3DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLNORMAL3FPROC)(GLfloat   nx, GLfloat   ny, GLfloat   nz);
    typedef void (APIENTRYP PFNGLNORMAL3FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLNORMAL3IPROC)(GLint   nx, GLint   ny, GLint   nz);
    typedef void (APIENTRYP PFNGLNORMAL3IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLNORMAL3SPROC)(GLshort   nx, GLshort   ny, GLshort   nz);
    typedef void (APIENTRYP PFNGLNORMAL3SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLNORMALP3UIPROC)(GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLNORMALP3UIVPROC)(GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLNORMALPOINTERPROC)(GLenum   type, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLOBJECTLABELPROC)(GLenum   identifier, GLuint   name, GLsizei   length, const  GLchar  * label);
    typedef void (APIENTRYP PFNGLOBJECTPTRLABELPROC)(const void * ptr, GLsizei   length, const  GLchar  * label);
    typedef void (APIENTRYP PFNGLORTHOPROC)(GLdouble   left, GLdouble   right, GLdouble   bottom, GLdouble   top, GLdouble   zNear, GLdouble   zFar);
    typedef void (APIENTRYP PFNGLPASSTHROUGHPROC)(GLfloat   token);
    typedef void (APIENTRYP PFNGLPIXELMAPFVPROC)(GLenum   map, GLsizei   mapsize, const  GLfloat  * values);
    typedef void (APIENTRYP PFNGLPIXELMAPUIVPROC)(GLenum   map, GLsizei   mapsize, const  GLuint  * values);
    typedef void (APIENTRYP PFNGLPIXELMAPUSVPROC)(GLenum   map, GLsizei   mapsize, const  GLushort  * values);
    typedef void (APIENTRYP PFNGLPIXELSTOREFPROC)(GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLPIXELSTOREIPROC)(GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLPIXELTRANSFERFPROC)(GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLPIXELTRANSFERIPROC)(GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLPIXELZOOMPROC)(GLfloat   xfactor, GLfloat   yfactor);
    typedef void (APIENTRYP PFNGLPOINTPARAMETERFPROC)(GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLPOINTPARAMETERFVPROC)(GLenum   pname, const  GLfloat  * params);
    typedef void (APIENTRYP PFNGLPOINTPARAMETERIPROC)(GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLPOINTPARAMETERIVPROC)(GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLPOINTSIZEPROC)(GLfloat   size);
    typedef void (APIENTRYP PFNGLPOLYGONMODEPROC)(GLenum   face, GLenum   mode);
    typedef void (APIENTRYP PFNGLPOLYGONOFFSETPROC)(GLfloat   factor, GLfloat   units);
    typedef void (APIENTRYP PFNGLPOLYGONSTIPPLEPROC)(const  GLubyte  * mask);
    typedef void (APIENTRYP PFNGLPOPATTRIBPROC)(void);
    typedef void (APIENTRYP PFNGLPOPCLIENTATTRIBPROC)(void);
    typedef void (APIENTRYP PFNGLPOPDEBUGGROUPPROC)(void);
    typedef void (APIENTRYP PFNGLPOPMATRIXPROC)(void);
    typedef void (APIENTRYP PFNGLPOPNAMEPROC)(void);
    typedef void (APIENTRYP PFNGLPRIMITIVERESTARTINDEXPROC)(GLuint   index);
    typedef void (APIENTRYP PFNGLPRIORITIZETEXTURESPROC)(GLsizei   n, const  GLuint  * textures, const  GLfloat  * priorities);
    typedef void (APIENTRYP PFNGLPROVOKINGVERTEXPROC)(GLenum   mode);
    typedef void (APIENTRYP PFNGLPUSHATTRIBPROC)(GLbitfield   mask);
    typedef void (APIENTRYP PFNGLPUSHCLIENTATTRIBPROC)(GLbitfield   mask);
    typedef void (APIENTRYP PFNGLPUSHDEBUGGROUPPROC)(GLenum   source, GLuint   id, GLsizei   length, const  GLchar  * message);
    typedef void (APIENTRYP PFNGLPUSHMATRIXPROC)(void);
    typedef void (APIENTRYP PFNGLPUSHNAMEPROC)(GLuint   name);
    typedef void (APIENTRYP PFNGLQUERYCOUNTERPROC)(GLuint   id, GLenum   target);
    typedef void (APIENTRYP PFNGLRASTERPOS2DPROC)(GLdouble   x, GLdouble   y);
    typedef void (APIENTRYP PFNGLRASTERPOS2DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS2FPROC)(GLfloat   x, GLfloat   y);
    typedef void (APIENTRYP PFNGLRASTERPOS2FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS2IPROC)(GLint   x, GLint   y);
    typedef void (APIENTRYP PFNGLRASTERPOS2IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS2SPROC)(GLshort   x, GLshort   y);
    typedef void (APIENTRYP PFNGLRASTERPOS2SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS3DPROC)(GLdouble   x, GLdouble   y, GLdouble   z);
    typedef void (APIENTRYP PFNGLRASTERPOS3DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS3FPROC)(GLfloat   x, GLfloat   y, GLfloat   z);
    typedef void (APIENTRYP PFNGLRASTERPOS3FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS3IPROC)(GLint   x, GLint   y, GLint   z);
    typedef void (APIENTRYP PFNGLRASTERPOS3IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS3SPROC)(GLshort   x, GLshort   y, GLshort   z);
    typedef void (APIENTRYP PFNGLRASTERPOS3SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS4DPROC)(GLdouble   x, GLdouble   y, GLdouble   z, GLdouble   w);
    typedef void (APIENTRYP PFNGLRASTERPOS4DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS4FPROC)(GLfloat   x, GLfloat   y, GLfloat   z, GLfloat   w);
    typedef void (APIENTRYP PFNGLRASTERPOS4FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS4IPROC)(GLint   x, GLint   y, GLint   z, GLint   w);
    typedef void (APIENTRYP PFNGLRASTERPOS4IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLRASTERPOS4SPROC)(GLshort   x, GLshort   y, GLshort   z, GLshort   w);
    typedef void (APIENTRYP PFNGLRASTERPOS4SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLREADBUFFERPROC)(GLenum   src);
    typedef void (APIENTRYP PFNGLREADPIXELSPROC)(GLint   x, GLint   y, GLsizei   width, GLsizei   height, GLenum   format, GLenum   type, void * pixels);
    typedef void (APIENTRYP PFNGLREADNPIXELSPROC)(GLint   x, GLint   y, GLsizei   width, GLsizei   height, GLenum   format, GLenum   type, GLsizei   bufSize, void * data);
    typedef void (APIENTRYP PFNGLREADNPIXELSARBPROC)(GLint   x, GLint   y, GLsizei   width, GLsizei   height, GLenum   format, GLenum   type, GLsizei   bufSize, void * data);
    typedef void (APIENTRYP PFNGLRECTDPROC)(GLdouble   x1, GLdouble   y1, GLdouble   x2, GLdouble   y2);
    typedef void (APIENTRYP PFNGLRECTDVPROC)(const  GLdouble  * v1, const  GLdouble  * v2);
    typedef void (APIENTRYP PFNGLRECTFPROC)(GLfloat   x1, GLfloat   y1, GLfloat   x2, GLfloat   y2);
    typedef void (APIENTRYP PFNGLRECTFVPROC)(const  GLfloat  * v1, const  GLfloat  * v2);
    typedef void (APIENTRYP PFNGLRECTIPROC)(GLint   x1, GLint   y1, GLint   x2, GLint   y2);
    typedef void (APIENTRYP PFNGLRECTIVPROC)(const  GLint  * v1, const  GLint  * v2);
    typedef void (APIENTRYP PFNGLRECTSPROC)(GLshort   x1, GLshort   y1, GLshort   x2, GLshort   y2);
    typedef void (APIENTRYP PFNGLRECTSVPROC)(const  GLshort  * v1, const  GLshort  * v2);
    typedef GLint (APIENTRYP PFNGLRENDERMODEPROC)(GLenum   mode);
    typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(GLenum   target, GLenum   internalformat, GLsizei   width, GLsizei   height);
    typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)(GLenum   target, GLsizei   samples, GLenum   internalformat, GLsizei   width, GLsizei   height);
    typedef void (APIENTRYP PFNGLROTATEDPROC)(GLdouble   angle, GLdouble   x, GLdouble   y, GLdouble   z);
    typedef void (APIENTRYP PFNGLROTATEFPROC)(GLfloat   angle, GLfloat   x, GLfloat   y, GLfloat   z);
    typedef void (APIENTRYP PFNGLSAMPLECOVERAGEPROC)(GLfloat   value, GLboolean   invert);
    typedef void (APIENTRYP PFNGLSAMPLECOVERAGEARBPROC)(GLfloat   value, GLboolean   invert);
    typedef void (APIENTRYP PFNGLSAMPLEMASKIPROC)(GLuint   maskNumber, GLbitfield   mask);
    typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC)(GLuint   sampler, GLenum   pname, const  GLint  * param);
    typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC)(GLuint   sampler, GLenum   pname, const  GLuint  * param);
    typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC)(GLuint   sampler, GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC)(GLuint   sampler, GLenum   pname, const  GLfloat  * param);
    typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC)(GLuint   sampler, GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC)(GLuint   sampler, GLenum   pname, const  GLint  * param);
    typedef void (APIENTRYP PFNGLSCALEDPROC)(GLdouble   x, GLdouble   y, GLdouble   z);
    typedef void (APIENTRYP PFNGLSCALEFPROC)(GLfloat   x, GLfloat   y, GLfloat   z);
    typedef void (APIENTRYP PFNGLSCISSORPROC)(GLint   x, GLint   y, GLsizei   width, GLsizei   height);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BPROC)(GLbyte   red, GLbyte   green, GLbyte   blue);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3BVPROC)(const  GLbyte  * v);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DPROC)(GLdouble   red, GLdouble   green, GLdouble   blue);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FPROC)(GLfloat   red, GLfloat   green, GLfloat   blue);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IPROC)(GLint   red, GLint   green, GLint   blue);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SPROC)(GLshort   red, GLshort   green, GLshort   blue);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBPROC)(GLubyte   red, GLubyte   green, GLubyte   blue);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UBVPROC)(const  GLubyte  * v);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIPROC)(GLuint   red, GLuint   green, GLuint   blue);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3UIVPROC)(const  GLuint  * v);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USPROC)(GLushort   red, GLushort   green, GLushort   blue);
    typedef void (APIENTRYP PFNGLSECONDARYCOLOR3USVPROC)(const  GLushort  * v);
    typedef void (APIENTRYP PFNGLSECONDARYCOLORP3UIPROC)(GLenum   type, GLuint   color);
    typedef void (APIENTRYP PFNGLSECONDARYCOLORP3UIVPROC)(GLenum   type, const  GLuint  * color);
    typedef void (APIENTRYP PFNGLSECONDARYCOLORPOINTERPROC)(GLint   size, GLenum   type, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLSELECTBUFFERPROC)(GLsizei   size, GLuint  * buffer);
    typedef void (APIENTRYP PFNGLSHADEMODELPROC)(GLenum   mode);
    typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint   shader, GLsizei   count, const  GLchar  *const* string, const  GLint  * length);
    typedef void (APIENTRYP PFNGLSTENCILFUNCPROC)(GLenum   func, GLint   ref, GLuint   mask);
    typedef void (APIENTRYP PFNGLSTENCILFUNCSEPARATEPROC)(GLenum   face, GLenum   func, GLint   ref, GLuint   mask);
    typedef void (APIENTRYP PFNGLSTENCILMASKPROC)(GLuint   mask);
    typedef void (APIENTRYP PFNGLSTENCILMASKSEPARATEPROC)(GLenum   face, GLuint   mask);
    typedef void (APIENTRYP PFNGLSTENCILOPPROC)(GLenum   fail, GLenum   zfail, GLenum   zpass);
    typedef void (APIENTRYP PFNGLSTENCILOPSEPARATEPROC)(GLenum   face, GLenum   sfail, GLenum   dpfail, GLenum   dppass);
    typedef void (APIENTRYP PFNGLTEXBUFFERPROC)(GLenum   target, GLenum   internalformat, GLuint   buffer);
    typedef void (APIENTRYP PFNGLTEXCOORD1DPROC)(GLdouble   s);
    typedef void (APIENTRYP PFNGLTEXCOORD1DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD1FPROC)(GLfloat   s);
    typedef void (APIENTRYP PFNGLTEXCOORD1FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD1IPROC)(GLint   s);
    typedef void (APIENTRYP PFNGLTEXCOORD1IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD1SPROC)(GLshort   s);
    typedef void (APIENTRYP PFNGLTEXCOORD1SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD2DPROC)(GLdouble   s, GLdouble   t);
    typedef void (APIENTRYP PFNGLTEXCOORD2DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD2FPROC)(GLfloat   s, GLfloat   t);
    typedef void (APIENTRYP PFNGLTEXCOORD2FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD2IPROC)(GLint   s, GLint   t);
    typedef void (APIENTRYP PFNGLTEXCOORD2IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD2SPROC)(GLshort   s, GLshort   t);
    typedef void (APIENTRYP PFNGLTEXCOORD2SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD3DPROC)(GLdouble   s, GLdouble   t, GLdouble   r);
    typedef void (APIENTRYP PFNGLTEXCOORD3DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD3FPROC)(GLfloat   s, GLfloat   t, GLfloat   r);
    typedef void (APIENTRYP PFNGLTEXCOORD3FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD3IPROC)(GLint   s, GLint   t, GLint   r);
    typedef void (APIENTRYP PFNGLTEXCOORD3IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD3SPROC)(GLshort   s, GLshort   t, GLshort   r);
    typedef void (APIENTRYP PFNGLTEXCOORD3SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD4DPROC)(GLdouble   s, GLdouble   t, GLdouble   r, GLdouble   q);
    typedef void (APIENTRYP PFNGLTEXCOORD4DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD4FPROC)(GLfloat   s, GLfloat   t, GLfloat   r, GLfloat   q);
    typedef void (APIENTRYP PFNGLTEXCOORD4FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD4IPROC)(GLint   s, GLint   t, GLint   r, GLint   q);
    typedef void (APIENTRYP PFNGLTEXCOORD4IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLTEXCOORD4SPROC)(GLshort   s, GLshort   t, GLshort   r, GLshort   q);
    typedef void (APIENTRYP PFNGLTEXCOORD4SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLTEXCOORDP1UIPROC)(GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLTEXCOORDP1UIVPROC)(GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLTEXCOORDP2UIPROC)(GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLTEXCOORDP2UIVPROC)(GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLTEXCOORDP3UIPROC)(GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLTEXCOORDP3UIVPROC)(GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLTEXCOORDP4UIPROC)(GLenum   type, GLuint   coords);
    typedef void (APIENTRYP PFNGLTEXCOORDP4UIVPROC)(GLenum   type, const  GLuint  * coords);
    typedef void (APIENTRYP PFNGLTEXCOORDPOINTERPROC)(GLint   size, GLenum   type, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLTEXENVFPROC)(GLenum   target, GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLTEXENVFVPROC)(GLenum   target, GLenum   pname, const  GLfloat  * params);
    typedef void (APIENTRYP PFNGLTEXENVIPROC)(GLenum   target, GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLTEXENVIVPROC)(GLenum   target, GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLTEXGENDPROC)(GLenum   coord, GLenum   pname, GLdouble   param);
    typedef void (APIENTRYP PFNGLTEXGENDVPROC)(GLenum   coord, GLenum   pname, const  GLdouble  * params);
    typedef void (APIENTRYP PFNGLTEXGENFPROC)(GLenum   coord, GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLTEXGENFVPROC)(GLenum   coord, GLenum   pname, const  GLfloat  * params);
    typedef void (APIENTRYP PFNGLTEXGENIPROC)(GLenum   coord, GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLTEXGENIVPROC)(GLenum   coord, GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLTEXIMAGE1DPROC)(GLenum   target, GLint   level, GLint   internalformat, GLsizei   width, GLint   border, GLenum   format, GLenum   type, const void * pixels);
    typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum   target, GLint   level, GLint   internalformat, GLsizei   width, GLsizei   height, GLint   border, GLenum   format, GLenum   type, const void * pixels);
    typedef void (APIENTRYP PFNGLTEXIMAGE2DMULTISAMPLEPROC)(GLenum   target, GLsizei   samples, GLenum   internalformat, GLsizei   width, GLsizei   height, GLboolean   fixedsamplelocations);
    typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC)(GLenum   target, GLint   level, GLint   internalformat, GLsizei   width, GLsizei   height, GLsizei   depth, GLint   border, GLenum   format, GLenum   type, const void * pixels);
    typedef void (APIENTRYP PFNGLTEXIMAGE3DMULTISAMPLEPROC)(GLenum   target, GLsizei   samples, GLenum   internalformat, GLsizei   width, GLsizei   height, GLsizei   depth, GLboolean   fixedsamplelocations);
    typedef void (APIENTRYP PFNGLTEXPARAMETERIIVPROC)(GLenum   target, GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLTEXPARAMETERIUIVPROC)(GLenum   target, GLenum   pname, const  GLuint  * params);
    typedef void (APIENTRYP PFNGLTEXPARAMETERFPROC)(GLenum   target, GLenum   pname, GLfloat   param);
    typedef void (APIENTRYP PFNGLTEXPARAMETERFVPROC)(GLenum   target, GLenum   pname, const  GLfloat  * params);
    typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum   target, GLenum   pname, GLint   param);
    typedef void (APIENTRYP PFNGLTEXPARAMETERIVPROC)(GLenum   target, GLenum   pname, const  GLint  * params);
    typedef void (APIENTRYP PFNGLTEXSUBIMAGE1DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLsizei   width, GLenum   format, GLenum   type, const void * pixels);
    typedef void (APIENTRYP PFNGLTEXSUBIMAGE2DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLint   yoffset, GLsizei   width, GLsizei   height, GLenum   format, GLenum   type, const void * pixels);
    typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC)(GLenum   target, GLint   level, GLint   xoffset, GLint   yoffset, GLint   zoffset, GLsizei   width, GLsizei   height, GLsizei   depth, GLenum   format, GLenum   type, const void * pixels);
    typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSPROC)(GLuint   program, GLsizei   count, const  GLchar  *const* varyings, GLenum   bufferMode);
    typedef void (APIENTRYP PFNGLTRANSLATEDPROC)(GLdouble   x, GLdouble   y, GLdouble   z);
    typedef void (APIENTRYP PFNGLTRANSLATEFPROC)(GLfloat   x, GLfloat   y, GLfloat   z);
    typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint   location, GLfloat   v0);
    typedef void (APIENTRYP PFNGLUNIFORM1FVPROC)(GLint   location, GLsizei   count, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint   location, GLint   v0);
    typedef void (APIENTRYP PFNGLUNIFORM1IVPROC)(GLint   location, GLsizei   count, const  GLint  * value);
    typedef void (APIENTRYP PFNGLUNIFORM1UIPROC)(GLint   location, GLuint   v0);
    typedef void (APIENTRYP PFNGLUNIFORM1UIVPROC)(GLint   location, GLsizei   count, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(GLint   location, GLfloat   v0, GLfloat   v1);
    typedef void (APIENTRYP PFNGLUNIFORM2FVPROC)(GLint   location, GLsizei   count, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORM2IPROC)(GLint   location, GLint   v0, GLint   v1);
    typedef void (APIENTRYP PFNGLUNIFORM2IVPROC)(GLint   location, GLsizei   count, const  GLint  * value);
    typedef void (APIENTRYP PFNGLUNIFORM2UIPROC)(GLint   location, GLuint   v0, GLuint   v1);
    typedef void (APIENTRYP PFNGLUNIFORM2UIVPROC)(GLint   location, GLsizei   count, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(GLint   location, GLfloat   v0, GLfloat   v1, GLfloat   v2);
    typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(GLint   location, GLsizei   count, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORM3IPROC)(GLint   location, GLint   v0, GLint   v1, GLint   v2);
    typedef void (APIENTRYP PFNGLUNIFORM3IVPROC)(GLint   location, GLsizei   count, const  GLint  * value);
    typedef void (APIENTRYP PFNGLUNIFORM3UIPROC)(GLint   location, GLuint   v0, GLuint   v1, GLuint   v2);
    typedef void (APIENTRYP PFNGLUNIFORM3UIVPROC)(GLint   location, GLsizei   count, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(GLint   location, GLfloat   v0, GLfloat   v1, GLfloat   v2, GLfloat   v3);
    typedef void (APIENTRYP PFNGLUNIFORM4FVPROC)(GLint   location, GLsizei   count, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORM4IPROC)(GLint   location, GLint   v0, GLint   v1, GLint   v2, GLint   v3);
    typedef void (APIENTRYP PFNGLUNIFORM4IVPROC)(GLint   location, GLsizei   count, const  GLint  * value);
    typedef void (APIENTRYP PFNGLUNIFORM4UIPROC)(GLint   location, GLuint   v0, GLuint   v1, GLuint   v2, GLuint   v3);
    typedef void (APIENTRYP PFNGLUNIFORM4UIVPROC)(GLint   location, GLsizei   count, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLUNIFORMBLOCKBINDINGPROC)(GLuint   program, GLuint   uniformBlockIndex, GLuint   uniformBlockBinding);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX2FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3FVPROC)(GLint   location, GLsizei   count, GLboolean   transpose, const  GLfloat  * value);
    typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERPROC)(GLenum   target);
    typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint   program);
    typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPROC)(GLuint   program);
    typedef void (APIENTRYP PFNGLVERTEX2DPROC)(GLdouble   x, GLdouble   y);
    typedef void (APIENTRYP PFNGLVERTEX2DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLVERTEX2FPROC)(GLfloat   x, GLfloat   y);
    typedef void (APIENTRYP PFNGLVERTEX2FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLVERTEX2IPROC)(GLint   x, GLint   y);
    typedef void (APIENTRYP PFNGLVERTEX2IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEX2SPROC)(GLshort   x, GLshort   y);
    typedef void (APIENTRYP PFNGLVERTEX2SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEX3DPROC)(GLdouble   x, GLdouble   y, GLdouble   z);
    typedef void (APIENTRYP PFNGLVERTEX3DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLVERTEX3FPROC)(GLfloat   x, GLfloat   y, GLfloat   z);
    typedef void (APIENTRYP PFNGLVERTEX3FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLVERTEX3IPROC)(GLint   x, GLint   y, GLint   z);
    typedef void (APIENTRYP PFNGLVERTEX3IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEX3SPROC)(GLshort   x, GLshort   y, GLshort   z);
    typedef void (APIENTRYP PFNGLVERTEX3SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEX4DPROC)(GLdouble   x, GLdouble   y, GLdouble   z, GLdouble   w);
    typedef void (APIENTRYP PFNGLVERTEX4DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLVERTEX4FPROC)(GLfloat   x, GLfloat   y, GLfloat   z, GLfloat   w);
    typedef void (APIENTRYP PFNGLVERTEX4FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLVERTEX4IPROC)(GLint   x, GLint   y, GLint   z, GLint   w);
    typedef void (APIENTRYP PFNGLVERTEX4IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEX4SPROC)(GLshort   x, GLshort   y, GLshort   z, GLshort   w);
    typedef void (APIENTRYP PFNGLVERTEX4SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB1DPROC)(GLuint   index, GLdouble   x);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB1DVPROC)(GLuint   index, const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB1FPROC)(GLuint   index, GLfloat   x);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB1FVPROC)(GLuint   index, const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB1SPROC)(GLuint   index, GLshort   x);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB1SVPROC)(GLuint   index, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB2DPROC)(GLuint   index, GLdouble   x, GLdouble   y);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB2DVPROC)(GLuint   index, const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB2FPROC)(GLuint   index, GLfloat   x, GLfloat   y);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB2FVPROC)(GLuint   index, const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB2SPROC)(GLuint   index, GLshort   x, GLshort   y);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB2SVPROC)(GLuint   index, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB3DPROC)(GLuint   index, GLdouble   x, GLdouble   y, GLdouble   z);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB3DVPROC)(GLuint   index, const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB3FPROC)(GLuint   index, GLfloat   x, GLfloat   y, GLfloat   z);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB3FVPROC)(GLuint   index, const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB3SPROC)(GLuint   index, GLshort   x, GLshort   y, GLshort   z);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB3SVPROC)(GLuint   index, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4NBVPROC)(GLuint   index, const  GLbyte  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4NIVPROC)(GLuint   index, const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4NSVPROC)(GLuint   index, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBPROC)(GLuint   index, GLubyte   x, GLubyte   y, GLubyte   z, GLubyte   w);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUBVPROC)(GLuint   index, const  GLubyte  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUIVPROC)(GLuint   index, const  GLuint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4NUSVPROC)(GLuint   index, const  GLushort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4BVPROC)(GLuint   index, const  GLbyte  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4DPROC)(GLuint   index, GLdouble   x, GLdouble   y, GLdouble   z, GLdouble   w);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4DVPROC)(GLuint   index, const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4FPROC)(GLuint   index, GLfloat   x, GLfloat   y, GLfloat   z, GLfloat   w);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4FVPROC)(GLuint   index, const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4IVPROC)(GLuint   index, const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4SPROC)(GLuint   index, GLshort   x, GLshort   y, GLshort   z, GLshort   w);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4SVPROC)(GLuint   index, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4UBVPROC)(GLuint   index, const  GLubyte  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4UIVPROC)(GLuint   index, const  GLuint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIB4USVPROC)(GLuint   index, const  GLushort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC)(GLuint   index, GLuint   divisor);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IPROC)(GLuint   index, GLint   x);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI1IVPROC)(GLuint   index, const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIPROC)(GLuint   index, GLuint   x);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI1UIVPROC)(GLuint   index, const  GLuint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IPROC)(GLuint   index, GLint   x, GLint   y);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI2IVPROC)(GLuint   index, const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIPROC)(GLuint   index, GLuint   x, GLuint   y);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI2UIVPROC)(GLuint   index, const  GLuint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IPROC)(GLuint   index, GLint   x, GLint   y, GLint   z);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI3IVPROC)(GLuint   index, const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIPROC)(GLuint   index, GLuint   x, GLuint   y, GLuint   z);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI3UIVPROC)(GLuint   index, const  GLuint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI4BVPROC)(GLuint   index, const  GLbyte  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IPROC)(GLuint   index, GLint   x, GLint   y, GLint   z, GLint   w);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI4IVPROC)(GLuint   index, const  GLint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI4SVPROC)(GLuint   index, const  GLshort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UBVPROC)(GLuint   index, const  GLubyte  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIPROC)(GLuint   index, GLuint   x, GLuint   y, GLuint   z, GLuint   w);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI4UIVPROC)(GLuint   index, const  GLuint  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBI4USVPROC)(GLuint   index, const  GLushort  * v);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint   index, GLint   size, GLenum   type, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIPROC)(GLuint   index, GLenum   type, GLboolean   normalized, GLuint   value);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIVPROC)(GLuint   index, GLenum   type, GLboolean   normalized, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIPROC)(GLuint   index, GLenum   type, GLboolean   normalized, GLuint   value);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIVPROC)(GLuint   index, GLenum   type, GLboolean   normalized, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIPROC)(GLuint   index, GLenum   type, GLboolean   normalized, GLuint   value);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIVPROC)(GLuint   index, GLenum   type, GLboolean   normalized, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIPROC)(GLuint   index, GLenum   type, GLboolean   normalized, GLuint   value);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIVPROC)(GLuint   index, GLenum   type, GLboolean   normalized, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint   index, GLint   size, GLenum   type, GLboolean   normalized, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLVERTEXP2UIPROC)(GLenum   type, GLuint   value);
    typedef void (APIENTRYP PFNGLVERTEXP2UIVPROC)(GLenum   type, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLVERTEXP3UIPROC)(GLenum   type, GLuint   value);
    typedef void (APIENTRYP PFNGLVERTEXP3UIVPROC)(GLenum   type, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLVERTEXP4UIPROC)(GLenum   type, GLuint   value);
    typedef void (APIENTRYP PFNGLVERTEXP4UIVPROC)(GLenum   type, const  GLuint  * value);
    typedef void (APIENTRYP PFNGLVERTEXPOINTERPROC)(GLint   size, GLenum   type, GLsizei   stride, const void * pointer);
    typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint   x, GLint   y, GLsizei   width, GLsizei   height);
    typedef void (APIENTRYP PFNGLWAITSYNCPROC)(GLsync   sync, GLbitfield   flags, GLuint64   timeout);
    typedef void (APIENTRYP PFNGLWINDOWPOS2DPROC)(GLdouble   x, GLdouble   y);
    typedef void (APIENTRYP PFNGLWINDOWPOS2DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLWINDOWPOS2FPROC)(GLfloat   x, GLfloat   y);
    typedef void (APIENTRYP PFNGLWINDOWPOS2FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLWINDOWPOS2IPROC)(GLint   x, GLint   y);
    typedef void (APIENTRYP PFNGLWINDOWPOS2IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLWINDOWPOS2SPROC)(GLshort   x, GLshort   y);
    typedef void (APIENTRYP PFNGLWINDOWPOS2SVPROC)(const  GLshort  * v);
    typedef void (APIENTRYP PFNGLWINDOWPOS3DPROC)(GLdouble   x, GLdouble   y, GLdouble   z);
    typedef void (APIENTRYP PFNGLWINDOWPOS3DVPROC)(const  GLdouble  * v);
    typedef void (APIENTRYP PFNGLWINDOWPOS3FPROC)(GLfloat   x, GLfloat   y, GLfloat   z);
    typedef void (APIENTRYP PFNGLWINDOWPOS3FVPROC)(const  GLfloat  * v);
    typedef void (APIENTRYP PFNGLWINDOWPOS3IPROC)(GLint   x, GLint   y, GLint   z);
    typedef void (APIENTRYP PFNGLWINDOWPOS3IVPROC)(const  GLint  * v);
    typedef void (APIENTRYP PFNGLWINDOWPOS3SPROC)(GLshort   x, GLshort   y, GLshort   z);
    typedef void (APIENTRYP PFNGLWINDOWPOS3SVPROC)(const  GLshort  * v);

    // opengl 4.0
    typedef void (APIENTRYP PFNGLMINSAMPLESHADINGPROC)(GLfloat value);
  	typedef void (APIENTRYP PFNGLBLENDEQUATIONIPROC)(GLuint buf, GLenum mode);
  	typedef void (APIENTRYP PFNGLBLENDEQUATIONSEPARATEIPROC)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
  	typedef void (APIENTRYP PFNGLBLENDFUNCIPROC)(GLuint buf, GLenum src, GLenum dst);
  	typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEIPROC)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
  	typedef void (APIENTRYP PFNGLDRAWARRAYSINDIRECTPROC)(GLenum mode, const void *indirect);
  	typedef void (APIENTRYP PFNGLDRAWELEMENTSINDIRECTPROC)(GLenum mode, GLenum type, const void *indirect);
  	typedef void (APIENTRYP PFNGLUNIFORM1DPROC)(GLint location, GLdouble x);
  	typedef void (APIENTRYP PFNGLUNIFORM2DPROC)(GLint location, GLdouble x, GLdouble y);
  	typedef void (APIENTRYP PFNGLUNIFORM3DPROC)(GLint location, GLdouble x, GLdouble y, GLdouble z);
  	typedef void (APIENTRYP PFNGLUNIFORM4DPROC)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
  	typedef void (APIENTRYP PFNGLUNIFORM1DVPROC)(GLint location, GLsizei count, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORM2DVPROC)(GLint location, GLsizei count, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORM3DVPROC)(GLint location, GLsizei count, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORM4DVPROC)(GLint location, GLsizei count, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX2X4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX3X4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLUNIFORMMATRIX4X3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLGETUNIFORMDVPROC)(GLuint program, GLint location, GLdouble *params);
  	typedef void (APIENTRYP PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)(GLuint program, GLenum shadertype, const GLchar *name);
  	typedef void (APIENTRYP PFNGLGETSUBROUTINEINDEXPROC)(GLuint program, GLenum shadertype, const GLchar *name);
  	typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint *values);
  	typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei *length, GLchar *name);
  	typedef void (APIENTRYP PFNGLGETACTIVESUBROUTINENAMEPROC)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei *length, GLchar *name);
  	typedef void (APIENTRYP PFNGLUNIFORMSUBROUTINESUIVPROC)(GLenum shadertype, GLsizei count, const GLuint *indices);
  	typedef void (APIENTRYP PFNGLGETUNIFORMSUBROUTINEUIVPROC)(GLenum shadertype, GLint location, GLuint *params);
  	typedef void (APIENTRYP PFNGLGETPROGRAMSTAGEIVPROC)(GLuint program, GLenum shadertype, GLenum pname, GLint *values);
  	typedef void (APIENTRYP PFNGLPATCHPARAMETERIPROC)(GLenum pname, GLint value);
  	typedef void (APIENTRYP PFNGLPATCHPARAMETERFVPROC)(GLenum pname, const GLfloat *values);
  	typedef void (APIENTRYP PFNGLBINDTRANSFORMFEEDBACKPROC)(GLenum target, GLuint id);
  	typedef void (APIENTRYP PFNGLDELETETRANSFORMFEEDBACKSPROC)(GLsizei n, const GLuint *ids);
  	typedef void (APIENTRYP PFNGLGENTRANSFORMFEEDBACKSPROC)(GLsizei n, GLuint *ids);
  	typedef void (APIENTRYP PFNGLISTRANSFORMFEEDBACKPROC)(GLuint id);
  	typedef void (APIENTRYP PFNGLPAUSETRANSFORMFEEDBACKPROC)(void);
  	typedef void (APIENTRYP PFNGLRESUMETRANSFORMFEEDBACKPROC)(void);
  	typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKPROC)(GLenum mode, GLuint id);
  	typedef void (APIENTRYP PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)(GLenum mode, GLuint id, GLuint stream);
  	typedef void (APIENTRYP PFNGLBEGINQUERYINDEXEDPROC)(GLenum target, GLuint index, GLuint id);
  	typedef void (APIENTRYP PFNGLENDQUERYINDEXEDPROC)(GLenum target, GLuint index);
  	typedef void (APIENTRYP PFNGLGETQUERYINDEXEDIVPROC)(GLenum target, GLuint index, GLenum pname, GLint *params);

    //opengl 4.1
    typedef void (APIENTRYP PFNGLRELEASESHADERCOMPILERPROC)(void);
  	typedef void (APIENTRYP PFNGLSHADERBINARYPROC)(GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length);
  	typedef void (APIENTRYP PFNGLGETSHADERPRECISIONFORMATPROC)(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
  	typedef void (APIENTRYP PFNGLDEPTHRANGEFPROC)(GLfloat n, GLfloat f);
  	typedef void (APIENTRYP PFNGLCLEARDEPTHFPROC)(GLfloat d);
  	typedef void (APIENTRYP PFNGLGETPROGRAMBINARYPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
  	typedef void (APIENTRYP PFNGLPROGRAMBINARYPROC)(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
  	typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIPROC)(GLuint program, GLenum pname, GLint value);
  	typedef void (APIENTRYP PFNGLUSEPROGRAMSTAGESPROC)(GLuint pipeline, GLbitfield stages, GLuint program);
  	typedef void (APIENTRYP PFNGLACTIVESHADERPROGRAMPROC)(GLuint pipeline, GLuint program);
  	typedef void (APIENTRYP PFNGLCREATESHADERPROGRAMVPROC)(GLenum type, GLsizei count, const GLchar *const*strings);
  	typedef void (APIENTRYP PFNGLBINDPROGRAMPIPELINEPROC)(GLuint pipeline);
  	typedef void (APIENTRYP PFNGLDELETEPROGRAMPIPELINESPROC)(GLsizei n, const GLuint *pipelines);
  	typedef void (APIENTRYP PFNGLGENPROGRAMPIPELINESPROC)(GLsizei n, GLuint *pipelines);
  	typedef void (APIENTRYP PFNGLISPROGRAMPIPELINEPROC)(GLuint pipeline);
  	typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEIVPROC)(GLuint pipeline, GLenum pname, GLint *params);
  	typedef void (APIENTRYP PFNGLPROGRAMPARAMETERIPROC)(GLuint program, GLenum pname, GLint value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IPROC)(GLuint program, GLint location, GLint v0);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FPROC)(GLuint program, GLint location, GLfloat v0);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DPROC)(GLuint program, GLint location, GLdouble v0);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIPROC)(GLuint program, GLint location, GLuint v0);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM1UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IPROC)(GLuint program, GLint location, GLint v0, GLint v1);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM2UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IPROC)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM3UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IPROC)(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4IVPROC)(GLuint program, GLint location, GLsizei count, const GLint *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FPROC)(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4FVPROC)(GLuint program, GLint location, GLsizei count, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DPROC)(GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4DVPROC)(GLuint program, GLint location, GLsizei count, const GLdouble *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIPROC)(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORM4UIVPROC)(GLuint program, GLint location, GLsizei count, const GLuint *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  	typedef void (APIENTRYP PFNGLVALIDATEPROGRAMPIPELINEPROC)(GLuint pipeline);
  	typedef void (APIENTRYP PFNGLGETPROGRAMPIPELINEINFOLOGPROC)(GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DPROC)(GLuint index, GLdouble x);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DPROC)(GLuint index, GLdouble x, GLdouble y);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBL1DVPROC)(GLuint index, const GLdouble *v);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBL2DVPROC)(GLuint index, const GLdouble *v);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBL3DVPROC)(GLuint index, const GLdouble *v);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBL4DVPROC)(GLuint index, const GLdouble *v);
  	typedef void (APIENTRYP PFNGLVERTEXATTRIBLPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
  	typedef void (APIENTRYP PFNGLGETVERTEXATTRIBLDVPROC)(GLuint index, GLenum pname, GLdouble *params);
  	typedef void (APIENTRYP PFNGLVIEWPORTARRAYVPROC)(GLuint first, GLsizei count, const GLfloat *v);
  	typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
  	typedef void (APIENTRYP PFNGLVIEWPORTINDEXEDFVPROC)(GLuint index, const GLfloat *v);
  	typedef void (APIENTRYP PFNGLSCISSORARRAYVPROC)(GLuint first, GLsizei count, const GLint *v);
  	typedef void (APIENTRYP PFNGLSCISSORINDEXEDPROC)(GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
  	typedef void (APIENTRYP PFNGLSCISSORINDEXEDVPROC)(GLuint index, const GLint *v);
  	typedef void (APIENTRYP PFNGLDEPTHRANGEARRAYVPROC)(GLuint first, GLsizei count, const GLdouble *v);
  	typedef void (APIENTRYP PFNGLDEPTHRANGEINDEXEDPROC)(GLuint index, GLdouble n, GLdouble f);
  	typedef void (APIENTRYP PFNGLGETFLOATI_VPROC)(GLenum target, GLuint index, GLfloat *data);
  	typedef void (APIENTRYP PFNGLGETDOUBLEI_VPROC)(GLenum target, GLuint index, GLdouble *data);

    #define WGL_DRAW_TO_WINDOW_ARB 0x2001
    #define WGL_SUPPORT_OPENGL_ARB 0x2010
    #define WGL_DOUBLE_BUFFER_ARB 0x2011
    #define WGL_PIXEL_TYPE_ARB 0x2013
    #define WGL_COLOR_BITS_ARB 0x2014
    #define WGL_DEPTH_BITS_ARB 0x2022
    #define WGL_STENCIL_BITS_ARB 0x2023
    #define WGL_TYPE_RGBA_ARB 0x202B
    #define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092

    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormat;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs;
    // PFNWGLGETCURRENTCONTEXTARBPROC wglGetCurrentContext;
    // PFNWGLMAKECURRENTPROC wglMakeCurrent;

    // PFNGLACCUMPROC glAccum; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLACTIVETEXTUREPROC glActiveTexture;
    // PFNGLALPHAFUNCPROC glAlphaFunc; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLARETEXTURESRESIDENTPROC glAreTexturesResident;// defined in GL/gl.h for v1.0 & v1.1
    // PFNGLARRAYELEMENTPROC glArrayElement;// defined in GL/gl.h for v1.0 & v1.1
    PFNGLATTACHSHADERPROC glAttachShader;
    // PFNGLBEGINPROC glBegin;// defined in GL/gl.h for v1.0 & v1.1
    PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
    PFNGLBEGINQUERYPROC glBeginQuery;
    PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
    PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
    PFNGLBINDBUFFERPROC glBindBuffer;
    PFNGLBINDBUFFERBASEPROC glBindBufferBase;
    PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
    PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation;
    PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
    PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
    PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
    PFNGLBINDSAMPLERPROC glBindSampler;
    // PFNGLBINDTEXTUREPROC glBindTexture;// defined in GL/gl.h for v1.0 & v1.1
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
    // PFNGLBITMAPPROC glBitmap;// defined in GL/gl.h for v1.0 & v1.1
    PFNGLBLENDCOLORPROC glBlendColor;
    PFNGLBLENDEQUATIONPROC glBlendEquation;
    PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
    // PFNGLBLENDFUNCPROC glBlendFunc;// defined in GL/gl.h for v1.0 & v1.1
    PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
    PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
    PFNGLBUFFERDATAPROC glBufferData;
    PFNGLBUFFERSUBDATAPROC glBufferSubData;
    // PFNGLCALLLISTPROC glCallList; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCALLLISTSPROC glCallLists; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
    PFNGLCLAMPCOLORPROC glClampColor;
    // PFNGLCLEARPROC glClear; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCLEARACCUMPROC glClearAccum; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLCLEARBUFFERFIPROC glClearBufferfi;
    PFNGLCLEARBUFFERFVPROC glClearBufferfv;
    PFNGLCLEARBUFFERIVPROC glClearBufferiv;
    PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
    // PFNGLCLEARCOLORPROC glClearColor; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCLEARDEPTHPROC glClearDepth; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCLEARINDEXPROC glClearIndex; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCLEARSTENCILPROC glClearStencil; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
    PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
    // PFNGLCLIPPLANEPROC glClipPlane; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3BPROC glColor3b; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3BVPROC glColor3bv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3DPROC glColor3d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3DVPROC glColor3dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3FPROC glColor3f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3FVPROC glColor3fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3IPROC glColor3i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3IVPROC glColor3iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3SPROC glColor3s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3SVPROC glColor3sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3UBPROC glColor3ub; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3UBVPROC glColor3ubv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3UIPROC glColor3ui; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3UIVPROC glColor3uiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3USPROC glColor3us; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR3USVPROC glColor3usv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4BPROC glColor4b; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4BVPROC glColor4bv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4DPROC glColor4d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4DVPROC glColor4dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4FPROC glColor4f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4FVPROC glColor4fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4IPROC glColor4i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4IVPROC glColor4iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4SPROC glColor4s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4SVPROC glColor4sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4UBPROC glColor4ub; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4UBVPROC glColor4ubv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4UIPROC glColor4ui; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4UIVPROC glColor4uiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4USPROC glColor4us; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLOR4USVPROC glColor4usv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLORMASKPROC glColorMask; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLORMASKIPROC glColorMaski; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOLORMATERIALPROC glColorMaterial; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLCOLORP3UIPROC glColorP3ui;
    PFNGLCOLORP3UIVPROC glColorP3uiv;
    PFNGLCOLORP4UIPROC glColorP4ui;
    PFNGLCOLORP4UIVPROC glColorP4uiv;
    // PFNGLCOLORPOINTERPROC glColorPointer; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
    PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
    PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
    PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
    PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
    PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
    // PFNGLCOPYPIXELSPROC glCopyPixels; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOPYTEXIMAGE1DPROC glCopyTexImage1D; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOPYTEXSUBIMAGE1DPROC glCopyTexSubImage1D; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLCOPYTEXSUBIMAGE2DPROC glCopyTexSubImage2D; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;
    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLCREATESHADERPROC glCreateShader;
    // PFNGLCULLFACEPROC glCullFace; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
    PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
    PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert;
    PFNGLDELETEBUFFERSPROC glDeleteBuffers;
    PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
    // PFNGLDELETELISTSPROC glDeleteLists; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDELETEPROGRAMPROC glDeleteProgram;
    PFNGLDELETEQUERIESPROC glDeleteQueries;
    PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
    PFNGLDELETESAMPLERSPROC glDeleteSamplers;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLDELETESYNCPROC glDeleteSync;
    // PFNGLDELETETEXTURESPROC glDeleteTextures; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
    // PFNGLDEPTHFUNCPROC glDepthFunc; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLDEPTHMASKPROC glDepthMask; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLDEPTHRANGEPROC glDepthRange; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDETACHSHADERPROC glDetachShader;
    // PFNGLDISABLEPROC glDisable; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLDISABLECLIENTSTATEPROC glDisableClientState; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
    PFNGLDISABLEIPROC glDisablei;
    // PFNGLDRAWARRAYSPROC glDrawArrays; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
    // PFNGLDRAWBUFFERPROC glDrawBuffer; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDRAWBUFFERSPROC glDrawBuffers;
    // PFNGLDRAWELEMENTSPROC glDrawElements; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
    PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
    PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
    // PFNGLDRAWPIXELSPROC glDrawPixels; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
    PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
    // PFNGLEDGEFLAGPROC glEdgeFlag; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEDGEFLAGPOINTERPROC glEdgeFlagPointer; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEDGEFLAGVPROC glEdgeFlagv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLENABLEPROC glEnable; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLENABLECLIENTSTATEPROC glEnableClientState; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLENABLEIPROC glEnablei;
    // PFNGLENDPROC glEnd; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
    // PFNGLENDLISTPROC glEndList; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLENDQUERYPROC glEndQuery;
    PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
    // PFNGLEVALCOORD1DPROC glEvalCoord1d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALCOORD1DVPROC glEvalCoord1dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALCOORD1FPROC glEvalCoord1f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALCOORD1FVPROC glEvalCoord1fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALCOORD2DPROC glEvalCoord2d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALCOORD2DVPROC glEvalCoord2dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALCOORD2FPROC glEvalCoord2f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALCOORD2FVPROC glEvalCoord2fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALMESH1PROC glEvalMesh1; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALMESH2PROC glEvalMesh2; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALPOINT1PROC glEvalPoint1; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLEVALPOINT2PROC glEvalPoint2; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLFEEDBACKBUFFERPROC glFeedbackBuffer; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLFENCESYNCPROC glFenceSync;
    // PFNGLFINISHPROC glFinish; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLFLUSHPROC glFlush; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
    PFNGLFOGCOORDPOINTERPROC glFogCoordPointer;
    PFNGLFOGCOORDDPROC glFogCoordd;
    PFNGLFOGCOORDDVPROC glFogCoorddv;
    PFNGLFOGCOORDFPROC glFogCoordf;
    PFNGLFOGCOORDFVPROC glFogCoordfv;
    // PFNGLFOGFPROC glFogf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLFOGFVPROC glFogfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLFOGIPROC glFogi; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLFOGIVPROC glFogiv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
    PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
    PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
    PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
    PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
    PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
    // PFNGLFRONTFACEPROC glFrontFace; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLFRUSTUMPROC glFrustum; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGENBUFFERSPROC glGenBuffers;
    PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
    // PFNGLGENLISTSPROC glGenLists; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGENQUERIESPROC glGenQueries;
    PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
    PFNGLGENSAMPLERSPROC glGenSamplers;
    // PFNGLGENTEXTURESPROC glGenTextures; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
    PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
    PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
    PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
    PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
    PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
    PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
    PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
    PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
    PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
    PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
    // PFNGLGETBOOLEANVPROC glGetBooleanv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
    PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
    PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;
    PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
    // PFNGLGETCLIPPLANEPROC glGetClipPlane; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;
    PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog;
    // PFNGLGETDOUBLEVPROC glGetDoublev; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETERRORPROC glGetError; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETFLOATVPROC glGetFloatv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex;
    PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation;
    PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
    PFNGLGETGRAPHICSRESETSTATUSARBPROC glGetGraphicsResetStatusARB;
    PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
    PFNGLGETINTEGER64VPROC glGetInteger64v;
    PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
    // PFNGLGETINTEGERVPROC glGetIntegerv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETLIGHTFVPROC glGetLightfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETLIGHTIVPROC glGetLightiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETMAPDVPROC glGetMapdv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETMAPFVPROC glGetMapfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETMAPIVPROC glGetMapiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETMATERIALFVPROC glGetMaterialfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETMATERIALIVPROC glGetMaterialiv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
    PFNGLGETOBJECTLABELPROC glGetObjectLabel;
    PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel;
    // PFNGLGETPIXELMAPFVPROC glGetPixelMapfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETPIXELMAPUIVPROC glGetPixelMapuiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETPIXELMAPUSVPROC glGetPixelMapusv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETPOINTERVPROC glGetPointerv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETPOLYGONSTIPPLEPROC glGetPolygonStipple; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v;
    PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
    PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;
    PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
    PFNGLGETQUERYIVPROC glGetQueryiv;
    PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
    PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv;
    PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;
    PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv;
    PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
    PFNGLGETSHADERSOURCEPROC glGetShaderSource;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    // PFNGLGETSTRINGPROC glGetString; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGETSTRINGIPROC glGetStringi;
    PFNGLGETSYNCIVPROC glGetSynciv;
    // PFNGLGETTEXENVFVPROC glGetTexEnvfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETTEXENVIVPROC glGetTexEnviv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETTEXGENDVPROC glGetTexGendv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETTEXGENFVPROC glGetTexGenfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETTEXGENIVPROC glGetTexGeniv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETTEXIMAGEPROC glGetTexImage; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv;
    PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv;
    // PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
    PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
    PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
    PFNGLGETUNIFORMFVPROC glGetUniformfv;
    PFNGLGETUNIFORMIVPROC glGetUniformiv;
    PFNGLGETUNIFORMUIVPROC glGetUniformuiv;
    PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
    PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
    PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
    PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
    PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
    PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
    PFNGLGETNCOLORTABLEARBPROC glGetnColorTableARB;
    PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC glGetnCompressedTexImageARB;
    PFNGLGETNCONVOLUTIONFILTERARBPROC glGetnConvolutionFilterARB;
    PFNGLGETNHISTOGRAMARBPROC glGetnHistogramARB;
    PFNGLGETNMAPDVARBPROC glGetnMapdvARB;
    PFNGLGETNMAPFVARBPROC glGetnMapfvARB;
    PFNGLGETNMAPIVARBPROC glGetnMapivARB;
    PFNGLGETNMINMAXARBPROC glGetnMinmaxARB;
    PFNGLGETNPIXELMAPFVARBPROC glGetnPixelMapfvARB;
    PFNGLGETNPIXELMAPUIVARBPROC glGetnPixelMapuivARB;
    PFNGLGETNPIXELMAPUSVARBPROC glGetnPixelMapusvARB;
    PFNGLGETNPOLYGONSTIPPLEARBPROC glGetnPolygonStippleARB;
    PFNGLGETNSEPARABLEFILTERARBPROC glGetnSeparableFilterARB;
    PFNGLGETNTEXIMAGEARBPROC glGetnTexImageARB;
    PFNGLGETNUNIFORMDVARBPROC glGetnUniformdvARB;
    PFNGLGETNUNIFORMFVARBPROC glGetnUniformfvARB;
    PFNGLGETNUNIFORMIVARBPROC glGetnUniformivARB;
    PFNGLGETNUNIFORMUIVARBPROC glGetnUniformuivARB;
    // PFNGLHINTPROC glHint; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXMASKPROC glIndexMask; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXPOINTERPROC glIndexPointer; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXDPROC glIndexd; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXDVPROC glIndexdv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXFPROC glIndexf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXFVPROC glIndexfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXIPROC glIndexi; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXIVPROC glIndexiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXSPROC glIndexs; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXSVPROC glIndexsv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXUBPROC glIndexub; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINDEXUBVPROC glIndexubv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINITNAMESPROC glInitNames; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLINTERLEAVEDARRAYSPROC glInterleavedArrays; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLISBUFFERPROC glIsBuffer;
    // PFNGLISENABLEDPROC glIsEnabled; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLISENABLEDIPROC glIsEnabledi;
    PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
    // PFNGLISLISTPROC glIsList; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLISPROGRAMPROC glIsProgram;
    PFNGLISQUERYPROC glIsQuery;
    PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
    PFNGLISSAMPLERPROC glIsSampler;
    PFNGLISSHADERPROC glIsShader;
    PFNGLISSYNCPROC glIsSync;
    // PFNGLISTEXTUREPROC glIsTexture; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLISVERTEXARRAYPROC glIsVertexArray;
    // PFNGLLIGHTMODELFPROC glLightModelf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLIGHTMODELFVPROC glLightModelfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLIGHTMODELIPROC glLightModeli; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLIGHTMODELIVPROC glLightModeliv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLIGHTFPROC glLightf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLIGHTFVPROC glLightfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLIGHTIPROC glLighti; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLIGHTIVPROC glLightiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLINESTIPPLEPROC glLineStipple; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLINEWIDTHPROC glLineWidth; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLLINKPROGRAMPROC glLinkProgram;
    // PFNGLLISTBASEPROC glListBase; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLOADIDENTITYPROC glLoadIdentity; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLOADMATRIXDPROC glLoadMatrixd; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLOADMATRIXFPROC glLoadMatrixf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLLOADNAMEPROC glLoadName; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd;
    PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;
    // PFNGLLOGICOPPROC glLogicOp; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMAP1DPROC glMap1d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMAP1FPROC glMap1f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMAP2DPROC glMap2d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMAP2FPROC glMap2f; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLMAPBUFFERPROC glMapBuffer;
    PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
    // PFNGLMAPGRID1DPROC glMapGrid1d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMAPGRID1FPROC glMapGrid1f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMAPGRID2DPROC glMapGrid2d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMAPGRID2FPROC glMapGrid2f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMATERIALFPROC glMaterialf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMATERIALFVPROC glMaterialfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMATERIALIPROC glMateriali; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMATERIALIVPROC glMaterialiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMATRIXMODEPROC glMatrixMode; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMULTMATRIXDPROC glMultMatrixd; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLMULTMATRIXFPROC glMultMatrixf; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd;
    PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf;
    PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
    PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
    PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;
    PFNGLMULTITEXCOORD1DPROC glMultiTexCoord1d;
    PFNGLMULTITEXCOORD1DVPROC glMultiTexCoord1dv;
    PFNGLMULTITEXCOORD1FPROC glMultiTexCoord1f;
    PFNGLMULTITEXCOORD1FVPROC glMultiTexCoord1fv;
    PFNGLMULTITEXCOORD1IPROC glMultiTexCoord1i;
    PFNGLMULTITEXCOORD1IVPROC glMultiTexCoord1iv;
    PFNGLMULTITEXCOORD1SPROC glMultiTexCoord1s;
    PFNGLMULTITEXCOORD1SVPROC glMultiTexCoord1sv;
    PFNGLMULTITEXCOORD2DPROC glMultiTexCoord2d;
    PFNGLMULTITEXCOORD2DVPROC glMultiTexCoord2dv;
    PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;
    PFNGLMULTITEXCOORD2FVPROC glMultiTexCoord2fv;
    PFNGLMULTITEXCOORD2IPROC glMultiTexCoord2i;
    PFNGLMULTITEXCOORD2IVPROC glMultiTexCoord2iv;
    PFNGLMULTITEXCOORD2SPROC glMultiTexCoord2s;
    PFNGLMULTITEXCOORD2SVPROC glMultiTexCoord2sv;
    PFNGLMULTITEXCOORD3DPROC glMultiTexCoord3d;
    PFNGLMULTITEXCOORD3DVPROC glMultiTexCoord3dv;
    PFNGLMULTITEXCOORD3FPROC glMultiTexCoord3f;
    PFNGLMULTITEXCOORD3FVPROC glMultiTexCoord3fv;
    PFNGLMULTITEXCOORD3IPROC glMultiTexCoord3i;
    PFNGLMULTITEXCOORD3IVPROC glMultiTexCoord3iv;
    PFNGLMULTITEXCOORD3SPROC glMultiTexCoord3s;
    PFNGLMULTITEXCOORD3SVPROC glMultiTexCoord3sv;
    PFNGLMULTITEXCOORD4DPROC glMultiTexCoord4d;
    PFNGLMULTITEXCOORD4DVPROC glMultiTexCoord4dv;
    PFNGLMULTITEXCOORD4FPROC glMultiTexCoord4f;
    PFNGLMULTITEXCOORD4FVPROC glMultiTexCoord4fv;
    PFNGLMULTITEXCOORD4IPROC glMultiTexCoord4i;
    PFNGLMULTITEXCOORD4IVPROC glMultiTexCoord4iv;
    PFNGLMULTITEXCOORD4SPROC glMultiTexCoord4s;
    PFNGLMULTITEXCOORD4SVPROC glMultiTexCoord4sv;
    PFNGLMULTITEXCOORDP1UIPROC glMultiTexCoordP1ui;
    PFNGLMULTITEXCOORDP1UIVPROC glMultiTexCoordP1uiv;
    PFNGLMULTITEXCOORDP2UIPROC glMultiTexCoordP2ui;
    PFNGLMULTITEXCOORDP2UIVPROC glMultiTexCoordP2uiv;
    PFNGLMULTITEXCOORDP3UIPROC glMultiTexCoordP3ui;
    PFNGLMULTITEXCOORDP3UIVPROC glMultiTexCoordP3uiv;
    PFNGLMULTITEXCOORDP4UIPROC glMultiTexCoordP4ui;
    PFNGLMULTITEXCOORDP4UIVPROC glMultiTexCoordP4uiv;
    // PFNGLNEWLISTPROC glNewList; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3BPROC glNormal3b; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3BVPROC glNormal3bv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3DPROC glNormal3d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3DVPROC glNormal3dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3FPROC glNormal3f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3FVPROC glNormal3fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3IPROC glNormal3i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3IVPROC glNormal3iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3SPROC glNormal3s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLNORMAL3SVPROC glNormal3sv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLNORMALP3UIPROC glNormalP3ui;
    PFNGLNORMALP3UIVPROC glNormalP3uiv;
    // PFNGLNORMALPOINTERPROC glNormalPointer; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLOBJECTLABELPROC glObjectLabel;
    PFNGLOBJECTPTRLABELPROC glObjectPtrLabel;
    // PFNGLORTHOPROC glOrtho; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPASSTHROUGHPROC glPassThrough; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPIXELMAPFVPROC glPixelMapfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPIXELMAPUIVPROC glPixelMapuiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPIXELMAPUSVPROC glPixelMapusv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPIXELSTOREFPROC glPixelStoref; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPIXELSTOREIPROC glPixelStorei; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPIXELTRANSFERFPROC glPixelTransferf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPIXELTRANSFERIPROC glPixelTransferi; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPIXELZOOMPROC glPixelZoom; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLPOINTPARAMETERFPROC glPointParameterf;
    PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
    PFNGLPOINTPARAMETERIPROC glPointParameteri;
    PFNGLPOINTPARAMETERIVPROC glPointParameteriv;
    // PFNGLPOINTSIZEPROC glPointSize; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPOLYGONMODEPROC glPolygonMode; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPOLYGONOFFSETPROC glPolygonOffset; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPOLYGONSTIPPLEPROC glPolygonStipple; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPOPATTRIBPROC glPopAttrib; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPOPCLIENTATTRIBPROC glPopClientAttrib; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLPOPDEBUGGROUPPROC glPopDebugGroup;
    // PFNGLPOPMATRIXPROC glPopMatrix; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPOPNAMEPROC glPopName; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;
    // PFNGLPRIORITIZETEXTURESPROC glPrioritizeTextures; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
    // PFNGLPUSHATTRIBPROC glPushAttrib; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPUSHCLIENTATTRIBPROC glPushClientAttrib; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup;
    // PFNGLPUSHMATRIXPROC glPushMatrix; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLPUSHNAMEPROC glPushName; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLQUERYCOUNTERPROC glQueryCounter;
    // PFNGLRASTERPOS2DPROC glRasterPos2d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS2DVPROC glRasterPos2dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS2FPROC glRasterPos2f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS2FVPROC glRasterPos2fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS2IPROC glRasterPos2i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS2IVPROC glRasterPos2iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS2SPROC glRasterPos2s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS2SVPROC glRasterPos2sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS3DPROC glRasterPos3d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS3DVPROC glRasterPos3dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS3FPROC glRasterPos3f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS3FVPROC glRasterPos3fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS3IPROC glRasterPos3i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS3IVPROC glRasterPos3iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS3SPROC glRasterPos3s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS3SVPROC glRasterPos3sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS4DPROC glRasterPos4d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS4DVPROC glRasterPos4dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS4FPROC glRasterPos4f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS4FVPROC glRasterPos4fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS4IPROC glRasterPos4i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS4IVPROC glRasterPos4iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS4SPROC glRasterPos4s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRASTERPOS4SVPROC glRasterPos4sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLREADBUFFERPROC glReadBuffer; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLREADPIXELSPROC glReadPixels; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLREADNPIXELSPROC glReadnPixels;
    PFNGLREADNPIXELSARBPROC glReadnPixelsARB;
    // PFNGLRECTDPROC glRectd; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRECTDVPROC glRectdv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRECTFPROC glRectf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRECTFVPROC glRectfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRECTIPROC glRecti; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRECTIVPROC glRectiv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRECTSPROC glRects; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRECTSVPROC glRectsv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLRENDERMODEPROC glRenderMode; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
    PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
    // PFNGLROTATEDPROC glRotated; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLROTATEFPROC glRotatef; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLSAMPLECOVERAGEPROC glSampleCoverage;
    PFNGLSAMPLECOVERAGEARBPROC glSampleCoverageARB;
    PFNGLSAMPLEMASKIPROC glSampleMaski;
    PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv;
    PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv;
    PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;
    PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
    PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
    PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv;
    // PFNGLSCALEDPROC glScaled; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLSCALEFPROC glScalef; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLSCISSORPROC glScissor; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLSECONDARYCOLOR3BPROC glSecondaryColor3b;
    PFNGLSECONDARYCOLOR3BVPROC glSecondaryColor3bv;
    PFNGLSECONDARYCOLOR3DPROC glSecondaryColor3d;
    PFNGLSECONDARYCOLOR3DVPROC glSecondaryColor3dv;
    PFNGLSECONDARYCOLOR3FPROC glSecondaryColor3f;
    PFNGLSECONDARYCOLOR3FVPROC glSecondaryColor3fv;
    PFNGLSECONDARYCOLOR3IPROC glSecondaryColor3i;
    PFNGLSECONDARYCOLOR3IVPROC glSecondaryColor3iv;
    PFNGLSECONDARYCOLOR3SPROC glSecondaryColor3s;
    PFNGLSECONDARYCOLOR3SVPROC glSecondaryColor3sv;
    PFNGLSECONDARYCOLOR3UBPROC glSecondaryColor3ub;
    PFNGLSECONDARYCOLOR3UBVPROC glSecondaryColor3ubv;
    PFNGLSECONDARYCOLOR3UIPROC glSecondaryColor3ui;
    PFNGLSECONDARYCOLOR3UIVPROC glSecondaryColor3uiv;
    PFNGLSECONDARYCOLOR3USPROC glSecondaryColor3us;
    PFNGLSECONDARYCOLOR3USVPROC glSecondaryColor3usv;
    PFNGLSECONDARYCOLORP3UIPROC glSecondaryColorP3ui;
    PFNGLSECONDARYCOLORP3UIVPROC glSecondaryColorP3uiv;
    PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer;
    // PFNGLSELECTBUFFERPROC glSelectBuffer; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLSHADEMODELPROC glShadeModel; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLSHADERSOURCEPROC glShaderSource;
    // PFNGLSTENCILFUNCPROC glStencilFunc; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
    // PFNGLSTENCILMASKPROC glStencilMask; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
    // PFNGLSTENCILOPPROC glStencilOp; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
    PFNGLTEXBUFFERPROC glTexBuffer;
    // PFNGLTEXCOORD1DPROC glTexCoord1d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD1DVPROC glTexCoord1dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD1FPROC glTexCoord1f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD1FVPROC glTexCoord1fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD1IPROC glTexCoord1i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD1IVPROC glTexCoord1iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD1SPROC glTexCoord1s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD1SVPROC glTexCoord1sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD2DPROC glTexCoord2d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD2DVPROC glTexCoord2dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD2FPROC glTexCoord2f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD2FVPROC glTexCoord2fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD2IPROC glTexCoord2i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD2IVPROC glTexCoord2iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD2SPROC glTexCoord2s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD2SVPROC glTexCoord2sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD3DPROC glTexCoord3d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD3DVPROC glTexCoord3dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD3FPROC glTexCoord3f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD3FVPROC glTexCoord3fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD3IPROC glTexCoord3i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD3IVPROC glTexCoord3iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD3SPROC glTexCoord3s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD3SVPROC glTexCoord3sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD4DPROC glTexCoord4d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD4DVPROC glTexCoord4dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD4FPROC glTexCoord4f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD4FVPROC glTexCoord4fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD4IPROC glTexCoord4i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD4IVPROC glTexCoord4iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD4SPROC glTexCoord4s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXCOORD4SVPROC glTexCoord4sv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLTEXCOORDP1UIPROC glTexCoordP1ui;
    PFNGLTEXCOORDP1UIVPROC glTexCoordP1uiv;
    PFNGLTEXCOORDP2UIPROC glTexCoordP2ui;
    PFNGLTEXCOORDP2UIVPROC glTexCoordP2uiv;
    PFNGLTEXCOORDP3UIPROC glTexCoordP3ui;
    PFNGLTEXCOORDP3UIVPROC glTexCoordP3uiv;
    PFNGLTEXCOORDP4UIPROC glTexCoordP4ui;
    PFNGLTEXCOORDP4UIVPROC glTexCoordP4uiv;
    // PFNGLTEXCOORDPOINTERPROC glTexCoordPointer; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXENVFPROC glTexEnvf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXENVFVPROC glTexEnvfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXENVIPROC glTexEnvi; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXENVIVPROC glTexEnviv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXGENDPROC glTexGend; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXGENDVPROC glTexGendv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXGENFPROC glTexGenf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXGENFVPROC glTexGenfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXGENIPROC glTexGeni; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXGENIVPROC glTexGeniv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXIMAGE1DPROC glTexImage1D; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXIMAGE2DPROC glTexImage2D; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
    PFNGLTEXIMAGE3DPROC glTexImage3D;
    PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
    PFNGLTEXPARAMETERIIVPROC glTexParameterIiv;
    PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv;
    // PFNGLTEXPARAMETERFPROC glTexParameterf; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXPARAMETERFVPROC glTexParameterfv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXPARAMETERIPROC glTexParameteri; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXPARAMETERIVPROC glTexParameteriv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXSUBIMAGE1DPROC glTexSubImage1D; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
    PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
    // PFNGLTRANSLATEDPROC glTranslated; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLTRANSLATEFPROC glTranslatef; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLUNIFORM1FPROC glUniform1f;
    PFNGLUNIFORM1FVPROC glUniform1fv;
    PFNGLUNIFORM1IPROC glUniform1i;
    PFNGLUNIFORM1IVPROC glUniform1iv;
    PFNGLUNIFORM1UIPROC glUniform1ui;
    PFNGLUNIFORM1UIVPROC glUniform1uiv;
    PFNGLUNIFORM2FPROC glUniform2f;
    PFNGLUNIFORM2FVPROC glUniform2fv;
    PFNGLUNIFORM2IPROC glUniform2i;
    PFNGLUNIFORM2IVPROC glUniform2iv;
    PFNGLUNIFORM2UIPROC glUniform2ui;
    PFNGLUNIFORM2UIVPROC glUniform2uiv;
    PFNGLUNIFORM3FPROC glUniform3f;
    PFNGLUNIFORM3FVPROC glUniform3fv;
    PFNGLUNIFORM3IPROC glUniform3i;
    PFNGLUNIFORM3IVPROC glUniform3iv;
    PFNGLUNIFORM3UIPROC glUniform3ui;
    PFNGLUNIFORM3UIVPROC glUniform3uiv;
    PFNGLUNIFORM4FPROC glUniform4f;
    PFNGLUNIFORM4FVPROC glUniform4fv;
    PFNGLUNIFORM4IPROC glUniform4i;
    PFNGLUNIFORM4IVPROC glUniform4iv;
    PFNGLUNIFORM4UIPROC glUniform4ui;
    PFNGLUNIFORM4UIVPROC glUniform4uiv;
    PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
    PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
    PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
    PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
    PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
    PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
    PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
    PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
    PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
    PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;
    PFNGLUNMAPBUFFERPROC glUnmapBuffer;
    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLVALIDATEPROGRAMPROC glValidateProgram;
    // PFNGLVERTEX2DPROC glVertex2d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX2DVPROC glVertex2dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX2FPROC glVertex2f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX2FVPROC glVertex2fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX2IPROC glVertex2i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX2IVPROC glVertex2iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX2SPROC glVertex2s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX2SVPROC glVertex2sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX3DPROC glVertex3d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX3DVPROC glVertex3dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX3FPROC glVertex3f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX3FVPROC glVertex3fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX3IPROC glVertex3i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX3IVPROC glVertex3iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX3SPROC glVertex3s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX3SVPROC glVertex3sv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX4DPROC glVertex4d; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX4DVPROC glVertex4dv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX4FPROC glVertex4f; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX4FVPROC glVertex4fv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX4IPROC glVertex4i; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX4IVPROC glVertex4iv; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX4SPROC glVertex4s; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVERTEX4SVPROC glVertex4sv; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
    PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
    PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
    PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
    PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
    PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
    PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
    PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
    PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
    PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
    PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
    PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
    PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
    PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
    PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
    PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
    PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
    PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
    PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
    PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
    PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
    PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
    PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
    PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
    PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
    PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
    PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
    PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
    PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
    PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
    PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
    PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
    PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;
    PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
    PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
    PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
    PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
    PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
    PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv;
    PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui;
    PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv;
    PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
    PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv;
    PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui;
    PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv;
    PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
    PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv;
    PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui;
    PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv;
    PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv;
    PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
    PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv;
    PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv;
    PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv;
    PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui;
    PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv;
    PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv;
    PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
    PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui;
    PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv;
    PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui;
    PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv;
    PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui;
    PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv;
    PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui;
    PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv;
    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
    PFNGLVERTEXP2UIPROC glVertexP2ui;
    PFNGLVERTEXP2UIVPROC glVertexP2uiv;
    PFNGLVERTEXP3UIPROC glVertexP3ui;
    PFNGLVERTEXP3UIVPROC glVertexP3uiv;
    PFNGLVERTEXP4UIPROC glVertexP4ui;
    PFNGLVERTEXP4UIVPROC glVertexP4uiv;
    // PFNGLVERTEXPOINTERPROC glVertexPointer; // defined in GL/gl.h for v1.0 & v1.1
    // PFNGLVIEWPORTPROC glViewport; // defined in GL/gl.h for v1.0 & v1.1
    PFNGLWAITSYNCPROC glWaitSync;
    PFNGLWINDOWPOS2DPROC glWindowPos2d;
    PFNGLWINDOWPOS2DVPROC glWindowPos2dv;
    PFNGLWINDOWPOS2FPROC glWindowPos2f;
    PFNGLWINDOWPOS2FVPROC glWindowPos2fv;
    PFNGLWINDOWPOS2IPROC glWindowPos2i;
    PFNGLWINDOWPOS2IVPROC glWindowPos2iv;
    PFNGLWINDOWPOS2SPROC glWindowPos2s;
    PFNGLWINDOWPOS2SVPROC glWindowPos2sv;
    PFNGLWINDOWPOS3DPROC glWindowPos3d;
    PFNGLWINDOWPOS3DVPROC glWindowPos3dv;
    PFNGLWINDOWPOS3FPROC glWindowPos3f;
    PFNGLWINDOWPOS3FVPROC glWindowPos3fv;
    PFNGLWINDOWPOS3IPROC glWindowPos3i;
    PFNGLWINDOWPOS3IVPROC glWindowPos3iv;
    PFNGLWINDOWPOS3SPROC glWindowPos3s;
    PFNGLWINDOWPOS3SVPROC glWindowPos3sv;
    //opengl 4.0
    PFNGLMINSAMPLESHADINGPROC glMinSampleShading;
  	PFNGLBLENDEQUATIONIPROC glBlendEquationi;
  	PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
  	PFNGLBLENDFUNCIPROC glBlendFunci;
  	PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei;
  	PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
  	PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;
  	PFNGLUNIFORM1DPROC glUniform1d;
  	PFNGLUNIFORM2DPROC glUniform2d;
  	PFNGLUNIFORM3DPROC glUniform3d;
  	PFNGLUNIFORM4DPROC glUniform4d;
  	PFNGLUNIFORM1DVPROC glUniform1dv;
  	PFNGLUNIFORM2DVPROC glUniform2dv;
  	PFNGLUNIFORM3DVPROC glUniform3dv;
  	PFNGLUNIFORM4DVPROC glUniform4dv;
  	PFNGLUNIFORMMATRIX2DVPROC glUniformMatrix2dv;
  	PFNGLUNIFORMMATRIX3DVPROC glUniformMatrix3dv;
  	PFNGLUNIFORMMATRIX4DVPROC glUniformMatrix4dv;
  	PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv;
  	PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv;
  	PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv;
  	PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv;
  	PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv;
  	PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv;
  	PFNGLGETUNIFORMDVPROC glGetUniformdv;
  	PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation;
  	PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex;
  	PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv;
  	PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
  	PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName;
  	PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv;
  	PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv;
  	PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv;
  	PFNGLPATCHPARAMETERIPROC glPatchParameteri;
  	PFNGLPATCHPARAMETERFVPROC glPatchParameterfv;
  	PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback;
  	PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks;
  	PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks;
  	PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback;
  	PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback;
  	PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback;
  	PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback;
  	PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
  	PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed;
  	PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed;
  	PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv;

    //opengl 4.1
    PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler;
  	PFNGLSHADERBINARYPROC glShaderBinary;
  	PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat;
  	PFNGLDEPTHRANGEFPROC glDepthRangef;
  	PFNGLCLEARDEPTHFPROC glClearDepthf;
  	PFNGLGETPROGRAMBINARYPROC glGetProgramBinary;
  	PFNGLPROGRAMBINARYPROC glProgramBinary;
  	PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
  	PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;
  	PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram;
  	PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
  	PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;
  	PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines;
  	PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines;
  	PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline;
  	PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv;
  	PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
  	PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i;
  	PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv;
  	PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f;
  	PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv;
  	PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d;
  	PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv;
  	PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui;
  	PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv;
  	PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i;
  	PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv;
  	PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f;
  	PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv;
  	PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d;
  	PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv;
  	PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui;
  	PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv;
  	PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i;
  	PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv;
  	PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f;
  	PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv;
  	PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d;
  	PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv;
  	PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui;
  	PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv;
  	PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i;
  	PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv;
  	PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f;
  	PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv;
  	PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d;
  	PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv;
  	PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui;
  	PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv;
  	PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv;
  	PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv;
  	PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv;
  	PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv;
  	PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv;
  	PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv;
  	PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
  	PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
  	PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
  	PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
  	PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
  	PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
  	PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv;
  	PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv;
  	PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv;
  	PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv;
  	PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv;
  	PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv;
  	PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline;
  	PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;
  	PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d;
  	PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d;
  	PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d;
  	PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d;
  	PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv;
  	PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv;
  	PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv;
  	PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv;
  	PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer;
  	PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv;
  	PFNGLVIEWPORTARRAYVPROC glViewportArrayv;
  	PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf;
  	PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv;
  	PFNGLSCISSORARRAYVPROC glScissorArrayv;
  	PFNGLSCISSORINDEXEDPROC glScissorIndexed;
  	PFNGLSCISSORINDEXEDVPROC glScissorIndexedv;
  	PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv;
  	PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed;
  	PFNGLGETFLOATI_VPROC glGetFloati_v;
  	PFNGLGETDOUBLEI_VPROC glGetDoublei_v;
  #endif
#endif


#ifdef __APPLE__
  #include <TargetConditionals.h>
  #ifdef TARGET_OS_MAC

    #include <CoreGraphics/CoreGraphics.h>
    #include <CoreAudio/CoreAudio.h>
    #include <objc/message.h>
    #include <objc/NSObjCRuntime.h>
    #include <mach/mach_time.h>
    #include <sys/sysctl.h> // for sys info like memory amount, etc

    // maybe this is available somewhere in objc runtime?
    #if __LP64__ || (TARGET_OS_EMBEDDED && !TARGET_OS_IPHONE) || \
        TARGET_OS_WIN32 || NS_BUILD_32_LIKE_64
      #define NSIntegerEncoding "q"
      #define NSUIntegerEncoding "L"
    #else
      #define NSIntegerEncoding "i"
      #define NSUIntegerEncoding "I"
    #endif

    // #ifdef __OBJC__
    // #import <Cocoa/Cocoa.h>
    // #else

    // this is how they are defined originally
    // #include <CoreGraphics/CGBase.h>
    // #include <CoreGraphics/CGGeometry.h>
    typedef CGPoint NSPoint;
    typedef CGSize NSSize;
    typedef CGRect NSRect;

    extern id NSApp;
    extern id const NSDefaultRunLoopMode;
    // #endif

  // styleMask options:
  int NSWindowStyleMaskBorderless;
  int NSWindowStyleMaskTitled;
  int NSWindowStyleMaskClosable;
  int NSWindowStyleMaskMiniaturizable;
  int NSWindowStyleMaskResizable;
  int NSWindowStyleMaskTexturedBackground; // DEPRECATED
  int NSWindowStyleMaskUnifiedTitleAndToolbar;
  int NSWindowStyleMaskFullScreen;
  int NSWindowStyleMaskFullSizeContentView;
  int NSWindowStyleMaskUtilityWindow;
  int NSWindowStyleMaskDocModalWindow;
  int NSWindowStyleMaskNonactivatingPanel;
  int NSWindowStyleMaskHUDWindow;

  // backing store options:
  int NSBackingStoreRetained; // DEPRECATED
  int NSBackingStoreNonretained; // DEPRECATED
  int NSBackingStoreBuffered;

  #if defined(__OBJC__) && __has_feature(objc_arc)
    // #define ARC_AVAILABLE
    #error \
        "Can't compile as objective-c code just yet! (see autorelease pool todo below)"``
    #endif

    // ABI is a bit different between platforms
    #ifdef __arm64__
    // objc_msgSend: Sends a message with a simple return value to an instance of a class.
    // objc_msgSend_stret: Sends a message with a data-structure return value to an instance of a class. (STruct RETurn)
    // objc_msgSend_fpret: Sends a message with a floating-point return value to an instance of a class. (Floating Point RETurn)

      #define abi_objc_msgSend_stret objc_msgSend
    #else
      #define abi_objc_msgSend_stret objc_msgSend_stret
    #endif

    #ifdef __i386__
      #define abi_objc_msgSend_fpret objc_msgSend_fpret
    #else
      #define abi_objc_msgSend_fpret objc_msgSend
    #endif

    #define objc_msgSend_id ((id(*)(id, SEL))objc_msgSend)
    #define objc_msgSend_void ((void (*)(id, SEL))objc_msgSend)
    #define objc_msgSend_void_id ((void (*)(id, SEL, id))objc_msgSend)
    #define objc_msgSend_void_bool ((void (*)(id, SEL, BOOL))objc_msgSend)
    #define objc_msgSend_id_const_char ((id(*)(id, SEL, const char *))objc_msgSend)

    bool terminated = false;

    static id global_autoreleasePool = NULL;

    void _p_clean_up_macos() {
      #ifdef ARC_AVAILABLE
      // TODO autorelease pool
      #else
        objc_msgSend_void(global_autoreleasePool, sel_registerName("drain"));
      #endif
    }

  #endif
#endif

#ifdef PLATFORM_USE_OPENGL
void p_check_gl_error(const char *state) {
  GLenum err = glGetError();
  if(err != GL_NO_ERROR){
    p_log(RED "got gl error %x when was doing %s\n" RESET, err, state);
  }
}

void p_check_shader_error(GLuint object) {
  GLint success;
  GLchar info[2048];
  glGetShaderiv(object, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(object, sizeof(info), NULL, info);
    p_log("shader compile error : %s\n", info);
  }
}

void p_check_program_error(GLuint object) {
  GLint success;
  GLchar info[2048];
  glGetProgramiv(object, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(object, sizeof(info), NULL, info);
    p_log("shader link error : %s\n", info);
  }
}
#endif

void p_error(platform_api *api, const char *message, ...) {
#ifdef __APPLE__
  (void)api;
  char tmp[1024];

  va_list args;
  va_start(args, message);
  p_vsnlog(tmp, sizeof(tmp), message, args);
  tmp[sizeof(tmp) - 1] = 0;
  va_end(args);

  CFStringRef header =
      CFStringCreateWithCString(NULL, "Error", kCFStringEncodingUTF8);
  CFStringRef msg = CFStringCreateWithCString(NULL, tmp, kCFStringEncodingUTF8);
  CFUserNotificationDisplayNotice(0.0, kCFUserNotificationStopAlertLevel, NULL,
                                  NULL, NULL, header, msg, NULL);
  CFRelease(header);
  CFRelease(msg);
#elif _WIN32
  char tmp[1024];

  va_list args;
  va_start(args, message);
  p_vsnlog(tmp, sizeof(tmp), message, args);
  tmp[sizeof(tmp) - 1] = 0;
  va_end(args);

  MessageBoxW((HWND)api->window.window_handle, unicode(tmp),
              api->window.wtitle, MB_OK | MB_ICONERROR);
  exit(1);
#endif
  exit(1);
}

void p_check_os_error(){
#ifdef _WIN32
  char err[256];
  memset(err, 0, 256);
  FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
  p_wlog(L"%s\n", err); // just for the safe case
  puts(err);
#elif __APPLE__
#endif
}

void _p_update_window_size(platform_api *api, int render_width, int render_height, float scale){
  api->window.scale = scale > 0.0 ? scale : 1.0; // never set scale to 0

  bool is_different_size = render_width != api->draw.size[0] || render_height != api->draw.size[1];

  if(is_different_size){
    // this is the size of the window, per the OS:
     // p_log("_p_update_window_size render w is %i\n", render_width);
     // p_log("_p_update_window_size render h is %i\n", render_height);
    api->window.size[0] = (int)((float)render_width / api->window.scale);
    api->window.size[1] = (int)((float)render_height / api->window.scale);
    // p_log("window size is: %i, %i\n", api->window.size[0], api->window.size[1]);
    // p_log("scale is: %f\n", scale);
    api->window.aspect = (float)render_width / (float)render_height;

    // now, set the size of the rendertarget, based on scale:
    if(!api->draw.lock_size){
      // p_log("setting draw size to %i, %i\n", render_width, render_height);
      api->draw.size[0] = render_width;
      api->draw.size[1] = render_height;
      api->draw.x = api->draw.size[0];
      api->draw.y = api->draw.size[1];
      // p_log("render size is: %i, %i\n", api->draw.size[0], api->draw.size[1]);
    }
  }
}

/***********************************************************************************************************************
  linmath.h
  Adapted from: https://github.com/datenwolf/linmath.h
  linmath.h provides the most used types required for programming computer graphics:
    vec3 -- 3 element vector of floats
    vec4 -- 4 element vector of floats (4th component used for homogenous computations)
    mat4x4 -- 4 by 4 elements matrix, computations are done in column major order
    quat -- quaternion
  The types are deliberately named like the types in GLSL. In fact they are meant to be used for the client side computations and passing to same typed GLSL uniforms.
***********************************************************************************************************************/



#define PLATFORM_MATH_DEFINE_VEC(n) \
typedef float vec##n[n]; \
static inline void p_vec##n##_add(vec##n r, vec##n const a, vec##n const b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i] + b[i]; \
} \
static inline void p_vec##n##_sub(vec##n r, vec##n const a, vec##n const b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i] - b[i]; \
} \
static inline void p_vec##n##_scale(vec##n r, vec##n const v, float const s) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = v[i] * s; \
} \
static inline float p_vec##n##_mul_inner(vec##n const a, vec##n const b) \
{ \
  float p = 0.; \
  int i; \
  for(i=0; i<n; ++i) \
  p += b[i]*a[i]; \
  return p; \
} \
static inline float p_vec##n##_len(vec##n const v) \
{ \
  return sqrtf(p_vec##n##_mul_inner(v,v)); \
} \
static inline void p_vec##n##_norm(vec##n r, vec##n const v) \
{ \
  float k = 1.0 / p_vec##n##_len(v); \
  p_vec##n##_scale(r, v, k); \
} \
static inline void p_vec##n##_min(vec##n r, vec##n a, vec##n b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i]<b[i] ? a[i] : b[i]; \
} \
static inline void p_vec##n##_max(vec##n r, vec##n a, vec##n b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i]>b[i] ? a[i] : b[i]; \
} \
static inline void p_vec##n##_set(vec##n to, vec##n from) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  to[i] = from[i]; \
} \
void p_vec##n##_debug(vec##n input){ \
  p_log("vec%i{ ", n); \
  int i; \
  for(i=0; i<n; ++i) \
  p_log("%f ", input[i]); \
  p_log("}\n"); \
}

PLATFORM_MATH_DEFINE_VEC(2)
PLATFORM_MATH_DEFINE_VEC(3)
PLATFORM_MATH_DEFINE_VEC(4)


static inline void p_vec3_mul_cross(vec3 r, vec3 const a, vec3 const b){
  r[0] = a[1]*b[2] - a[2]*b[1];
  r[1] = a[2]*b[0] - a[0]*b[2];
  r[2] = a[0]*b[1] - a[1]*b[0];
}

static inline void p_vec3_reflect(vec3 r, vec3 const v, vec3 const n){
  float p  = 2.f*p_vec3_mul_inner(v, n);
  int i;
  for(i=0;i<3;++i)
  r[i] = v[i] - p*n[i];
}

static inline void p_vec4_mul_cross(vec4 r, vec4 a, vec4 b){
  r[0] = a[1]*b[2] - a[2]*b[1];
  r[1] = a[2]*b[0] - a[0]*b[2];
  r[2] = a[0]*b[1] - a[1]*b[0];
  r[3] = 1.f;
}

static inline void p_vec4_reflect(vec4 r, vec4 v, vec4 n){
  float p  = 2.f*p_vec4_mul_inner(v, n);
  int i;
  for(i=0;i<4;++i)
  r[i] = v[i] - p*n[i];
}

typedef vec4 mat4x4[4];
static inline void p_mat4x4_identity(mat4x4 M){
  int i, j;
  for(i=0; i<4; ++i)
  for(j=0; j<4; ++j)
  M[i][j] = i==j ? 1.f : 0.f;
}
static inline void p_mat4x4_dup(mat4x4 M, mat4x4 N){
  int i, j;
  for(i=0; i<4; ++i)
  for(j=0; j<4; ++j)
  M[i][j] = N[i][j];
}
static inline void p_mat4x4_row(vec4 r, mat4x4 M, int i){
  int k;
  for(k=0; k<4; ++k)
  r[k] = M[k][i];
}
static inline void p_mat4x4_col(vec4 r, mat4x4 M, int i){
  int k;
  for(k=0; k<4; ++k)
  r[k] = M[i][k];
}
static inline void p_mat4x4_transpose(mat4x4 M, mat4x4 N){
  int i, j;
  for(j=0; j<4; ++j)
  for(i=0; i<4; ++i)
  M[i][j] = N[j][i];
}
static inline void p_mat4x4_add(mat4x4 M, mat4x4 a, mat4x4 b){
  int i;
  for(i=0; i<4; ++i)
  p_vec4_add(M[i], a[i], b[i]);
}
static inline void p_mat4x4_sub(mat4x4 M, mat4x4 a, mat4x4 b){
  int i;
  for(i=0; i<4; ++i)
  p_vec4_sub(M[i], a[i], b[i]);
}
static inline void p_mat4x4_scale(mat4x4 M, mat4x4 a, float k){
  int i;
  for(i=0; i<4; ++i)
  p_vec4_scale(M[i], a[i], k);
}
static inline void p_mat4x4_scale_aniso(mat4x4 M, mat4x4 a, float x, float y, float z){
  int i;
  p_vec4_scale(M[0], a[0], x);
  p_vec4_scale(M[1], a[1], y);
  p_vec4_scale(M[2], a[2], z);
  for(i = 0; i < 4; ++i) {
  M[3][i] = a[3][i];
  }
}
static inline void p_mat4x4_mul(mat4x4 M, mat4x4 a, mat4x4 b){
  mat4x4 temp;
  int k, r, c;
  for(c=0; c<4; ++c) for(r=0; r<4; ++r) {
  temp[c][r] = 0.f;
  for(k=0; k<4; ++k)
  temp[c][r] += a[k][r] * b[c][k];
  }
  p_mat4x4_dup(M, temp);
}
static inline void p_mat4x4_mul_vec4(vec4 r, mat4x4 M, vec4 v){
  int i, j;
  for(j=0; j<4; ++j) {
  r[j] = 0.f;
  for(i=0; i<4; ++i)
  r[j] += M[i][j] * v[i];
  }
}

static inline void p_mat4x4_translate(mat4x4 T, float x, float y, float z){
  p_mat4x4_identity(T);
  T[3][0] = x;
  T[3][1] = y;
  T[3][2] = z;
}

static inline void p_mat4x4_translate_in_place(mat4x4 M, float x, float y, float z){
  vec4 t = {x, y, z, 0};
  vec4 r;
  int i;
  for (i = 0; i < 4; ++i) {
  p_mat4x4_row(r, M, i);
  M[3][i] += p_vec4_mul_inner(r, t);
  }
}

static inline void p_mat4x4_from_vec3_mul_outer(mat4x4 M, vec3 a, vec3 b){
  int i, j;
  for(i=0; i<4; ++i) for(j=0; j<4; ++j)
  M[i][j] = i<3 && j<3 ? a[i] * b[j] : 0.f;
}

static inline void p_mat4x4_rotate(mat4x4 R, mat4x4 M, float x, float y, float z, float angle){
  float s = sinf(angle);
  float c = cosf(angle);
  vec3 u = {x, y, z};

  if(p_vec3_len(u) > 1e-4) {
  p_vec3_norm(u, u);
  mat4x4 T;
  p_mat4x4_from_vec3_mul_outer(T, u, u);

  mat4x4 S = {
  {    0,  u[2], -u[1], 0},
  {-u[2],     0,  u[0], 0},
  { u[1], -u[0],     0, 0},
  {    0,     0,     0, 0}
  };
  p_mat4x4_scale(S, S, s);

  mat4x4 C;
  p_mat4x4_identity(C);
  p_mat4x4_sub(C, C, T);

  p_mat4x4_scale(C, C, c);

  p_mat4x4_add(T, T, C);
  p_mat4x4_add(T, T, S);

  T[3][3] = 1.;
  p_mat4x4_mul(R, M, T);
  } else {
  p_mat4x4_dup(R, M);
  }
}

static inline void p_mat4x4_rotate_X(mat4x4 Q, mat4x4 M, float angle){
  float s = sinf(angle);
  float c = cosf(angle);
  mat4x4 R = {
  {1.f, 0.f, 0.f, 0.f},
  {0.f,   c,   s, 0.f},
  {0.f,  -s,   c, 0.f},
  {0.f, 0.f, 0.f, 1.f}
  };
  p_mat4x4_mul(Q, M, R);
}

static inline void p_mat4x4_rotate_Y(mat4x4 Q, mat4x4 M, float angle){
  float s = sinf(angle);
  float c = cosf(angle);
  mat4x4 R = {
  {   c, 0.f,   s, 0.f},
  { 0.f, 1.f, 0.f, 0.f},
  {  -s, 0.f,   c, 0.f},
  { 0.f, 0.f, 0.f, 1.f}
  };
  p_mat4x4_mul(Q, M, R);
}

static inline void p_mat4x4_rotate_Z(mat4x4 Q, mat4x4 M, float angle){
  float s = sinf(angle);
  float c = cosf(angle);
  mat4x4 R = {
  {   c,   s, 0.f, 0.f},
  {  -s,   c, 0.f, 0.f},
  { 0.f, 0.f, 1.f, 0.f},
  { 0.f, 0.f, 0.f, 1.f}
  };
  p_mat4x4_mul(Q, M, R);
}

static inline void p_mat4x4_invert(mat4x4 T, mat4x4 M){
  float s[6];
  float c[6];
  s[0] = M[0][0]*M[1][1] - M[1][0]*M[0][1];
  s[1] = M[0][0]*M[1][2] - M[1][0]*M[0][2];
  s[2] = M[0][0]*M[1][3] - M[1][0]*M[0][3];
  s[3] = M[0][1]*M[1][2] - M[1][1]*M[0][2];
  s[4] = M[0][1]*M[1][3] - M[1][1]*M[0][3];
  s[5] = M[0][2]*M[1][3] - M[1][2]*M[0][3];

  c[0] = M[2][0]*M[3][1] - M[3][0]*M[2][1];
  c[1] = M[2][0]*M[3][2] - M[3][0]*M[2][2];
  c[2] = M[2][0]*M[3][3] - M[3][0]*M[2][3];
  c[3] = M[2][1]*M[3][2] - M[3][1]*M[2][2];
  c[4] = M[2][1]*M[3][3] - M[3][1]*M[2][3];
  c[5] = M[2][2]*M[3][3] - M[3][2]*M[2][3];

  /* Assumes it is invertible */
  float idet = 1.0f/( s[0]*c[5]-s[1]*c[4]+s[2]*c[3]+s[3]*c[2]-s[4]*c[1]+s[5]*c[0] );

  T[0][0] = ( M[1][1] * c[5] - M[1][2] * c[4] + M[1][3] * c[3]) * idet;
  T[0][1] = (-M[0][1] * c[5] + M[0][2] * c[4] - M[0][3] * c[3]) * idet;
  T[0][2] = ( M[3][1] * s[5] - M[3][2] * s[4] + M[3][3] * s[3]) * idet;
  T[0][3] = (-M[2][1] * s[5] + M[2][2] * s[4] - M[2][3] * s[3]) * idet;

  T[1][0] = (-M[1][0] * c[5] + M[1][2] * c[2] - M[1][3] * c[1]) * idet;
  T[1][1] = ( M[0][0] * c[5] - M[0][2] * c[2] + M[0][3] * c[1]) * idet;
  T[1][2] = (-M[3][0] * s[5] + M[3][2] * s[2] - M[3][3] * s[1]) * idet;
  T[1][3] = ( M[2][0] * s[5] - M[2][2] * s[2] + M[2][3] * s[1]) * idet;

  T[2][0] = ( M[1][0] * c[4] - M[1][1] * c[2] + M[1][3] * c[0]) * idet;
  T[2][1] = (-M[0][0] * c[4] + M[0][1] * c[2] - M[0][3] * c[0]) * idet;
  T[2][2] = ( M[3][0] * s[4] - M[3][1] * s[2] + M[3][3] * s[0]) * idet;
  T[2][3] = (-M[2][0] * s[4] + M[2][1] * s[2] - M[2][3] * s[0]) * idet;

  T[3][0] = (-M[1][0] * c[3] + M[1][1] * c[1] - M[1][2] * c[0]) * idet;
  T[3][1] = ( M[0][0] * c[3] - M[0][1] * c[1] + M[0][2] * c[0]) * idet;
  T[3][2] = (-M[3][0] * s[3] + M[3][1] * s[1] - M[3][2] * s[0]) * idet;
  T[3][3] = ( M[2][0] * s[3] - M[2][1] * s[1] + M[2][2] * s[0]) * idet;
}

static inline void p_mat4x4_orthonormalize(mat4x4 R, mat4x4 M){
  p_mat4x4_dup(R, M);
  float s = 1.;
  vec3 h;

  p_vec3_norm(R[2], R[2]);

  s = p_vec3_mul_inner(R[1], R[2]);
  p_vec3_scale(h, R[2], s);
  p_vec3_sub(R[1], R[1], h);
  p_vec3_norm(R[2], R[2]);

  s = p_vec3_mul_inner(R[1], R[2]);
  p_vec3_scale(h, R[2], s);
  p_vec3_sub(R[1], R[1], h);
  p_vec3_norm(R[1], R[1]);

  s = p_vec3_mul_inner(R[0], R[1]);
  p_vec3_scale(h, R[1], s);
  p_vec3_sub(R[0], R[0], h);
  p_vec3_norm(R[0], R[0]);
}

static inline void p_mat4x4_frustum(mat4x4 M, float l, float r, float b, float t, float n, float f){
  M[0][0] = 2.f*n/(r-l);
  M[0][1] = M[0][2] = M[0][3] = 0.f;

  M[1][1] = 2.*n/(t-b);
  M[1][0] = M[1][2] = M[1][3] = 0.f;

  M[2][0] = (r+l)/(r-l);
  M[2][1] = (t+b)/(t-b);
  M[2][2] = -(f+n)/(f-n);
  M[2][3] = -1.f;

  M[3][2] = -2.f*(f*n)/(f-n);
  M[3][0] = M[3][1] = M[3][3] = 0.f;
}
static inline void p_mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f){
  M[0][0] = 2.f/(r-l);
  M[0][1] = M[0][2] = M[0][3] = 0.f;

  M[1][1] = 2.f/(t-b);
  M[1][0] = M[1][2] = M[1][3] = 0.f;

  M[2][2] = -2.f/(f-n);
  M[2][0] = M[2][1] = M[2][3] = 0.f;

  M[3][0] = -(r+l)/(r-l);
  M[3][1] = -(t+b)/(t-b);
  M[3][2] = -(f+n)/(f-n);
  M[3][3] = 1.f;
}

// per https://chaosinmotion.com/2010/09/06/goodbye-far-clipping-plane/
// we should be setting our perspective matrix to be one where values
// converge to 0 instead of (traditionally) to 1, because the (IEEE-754) floating point
// depth buffer error margins are much better.
static inline void p_mat4x4_perspective_infinite(mat4x4 m, float y_fov, float aspect, float n){
  /* NOTE: Degrees are an unhandy unit to work with.
   * linmath.h uses radians for everything! */
  float const a = 1.f / tan(y_fov / 2.f);

  m[0][0] = a / aspect;
  m[0][1] = 0.f;
  m[0][2] = 0.f;
  m[0][3] = 0.f;

  m[1][0] = 0.f;
  m[1][1] = a;
  m[1][2] = 0.f;
  m[1][3] = 0.f;

  m[2][0] = 0.f;
  m[2][1] = 0.f;
  m[2][2] = 0.f;
  m[2][3] = -1.f;

  m[3][0] = 0.f;
  m[3][1] = 0.f;
  m[3][2] = -1.0 * n;
  m[3][3] = 0.f;
}

static inline void p_mat4x4_perspective(mat4x4 m, float y_fov, float aspect, float n, float f){
  /* NOTE: Degrees are an unhandy unit to work with.
   * linmath.h uses radians for everything! */
  float const a = 1.f / tan(y_fov / 2.f);

  m[0][0] = a / aspect;
  m[0][1] = 0.f;
  m[0][2] = 0.f;
  m[0][3] = 0.f;

  m[1][0] = 0.f;
  m[1][1] = a;
  m[1][2] = 0.f;
  m[1][3] = 0.f;

  m[2][0] = 0.f;
  m[2][1] = 0.f;
  m[2][2] = -((f + n) / (f - n));
  m[2][3] = -1.f;

  m[3][0] = 0.f;
  m[3][1] = 0.f;
  m[3][2] = -((2.f * f * n) / (f - n));
  m[3][3] = 0.f;
}

static inline void p_mat4x4_look_at(mat4x4 m, vec3 eye, vec3 center, vec3 up){
  /* Adapted from Android's OpenGL Matrix.java.                        */
  /* See the OpenGL GLUT documentation for gluLookAt for a description */
  /* of the algorithm. We implement it in a straightforward way:       */

  /* TODO: The negation of of can be spared by swapping the order of
   *       operands in the following cross products in the right way. */
  vec3 f;
  p_vec3_sub(f, center, eye);
  p_vec3_norm(f, f);

  vec3 s;
  p_vec3_mul_cross(s, f, up);
  p_vec3_norm(s, s);

  vec3 t;
  p_vec3_mul_cross(t, s, f);

  m[0][0] =  s[0];
  m[0][1] =  t[0];
  m[0][2] = -f[0];
  m[0][3] =   0.f;

  m[1][0] =  s[1];
  m[1][1] =  t[1];
  m[1][2] = -f[1];
  m[1][3] =   0.f;

  m[2][0] =  s[2];
  m[2][1] =  t[2];
  m[2][2] = -f[2];
  m[2][3] =   0.f;

  m[3][0] =  0.f;
  m[3][1] =  0.f;
  m[3][2] =  0.f;
  m[3][3] =  1.f;

  p_mat4x4_translate_in_place(m, -eye[0], -eye[1], -eye[2]);
}

typedef float quat[4]; // stored as x, y, z, w
static inline void p_quat_identity(quat q){
  q[0] = q[1] = q[2] = 0.f;
  q[3] = 1.f;
}
static inline void p_quat_add(quat r, quat a, quat b){
  int i;
  for(i=0; i<4; ++i)
  r[i] = a[i] + b[i];
}
static inline void p_quat_sub(quat r, quat a, quat b){
  int i;
  for(i=0; i<4; ++i)
  r[i] = a[i] - b[i];
}
static inline void p_quat_mul(quat r, quat p, quat q){
  vec3 w;
  p_vec3_mul_cross(r, p, q);
  p_vec3_scale(w, p, q[3]);
  p_vec3_add(r, r, w);
  p_vec3_scale(w, q, p[3]);
  p_vec3_add(r, r, w);
  r[3] = p[3]*q[3] - p_vec3_mul_inner(p, q);
}
static inline void p_quat_scale(quat r, quat v, float s){
  int i;
  for(i=0; i<4; ++i)
  r[i] = v[i] * s;
}
static inline float p_quat_inner_product(quat a, quat b){
  float p = 0.f;
  int i;
  for(i=0; i<4; ++i)
  p += b[i]*a[i];
  return p;
}
static inline void p_quat_conj(quat r, quat q){
  int i;
  for(i=0; i<3; ++i)
  r[i] = -q[i];
  r[3] = q[3];
}
static inline void p_quat_rotate(quat r, float angle, vec3 axis) {
  vec3 v;
  p_vec3_scale(v, axis, sinf(angle / 2));
  int i;
  for(i=0; i<3; ++i)
  r[i] = v[i];
  r[3] = cosf(angle / 2);
}

static inline void p_quat_inverse(quat r, quat q) {
  r[0] = -q[0];
  r[1] = -q[1];
  r[2] = -q[2];
  r[3] = q[3];
}

static inline void p_quat_to_euler(vec3 e, quat q){
  e[0] = atan2(2 * q[1] * q[3]-2 * q[0] * q[2] , 1 - 2*pow(q[1],2) - 2*pow(q[2],2));
  e[1] = asin(2*q[0]*q[1] + 2*q[2]*q[3]);
  e[2] = atan2(2*q[0]*q[3]-2*q[1]*q[2] , 1 - 2*pow(q[0],2) - 2*pow(q[2],2));
}

#define p_quat_norm p_vec4_norm
static inline void p_quat_mul_vec3(vec3 r, quat q, vec3 v){
/*
 * Method by Fabian 'ryg' Giessen (of Farbrausch)
t = 2 * cross(q.xyz, v)
v' = v + q.w * t + cross(q.xyz, t)
 */
  vec3 t;
  vec3 q_xyz = {q[0], q[1], q[2]};
  vec3 u = {q[0], q[1], q[2]};

  p_vec3_mul_cross(t, q_xyz, v);
  p_vec3_scale(t, t, 2);

  p_vec3_mul_cross(u, q_xyz, t);
  p_vec3_scale(t, t, q[3]);

  p_vec3_add(r, v, t);
  p_vec3_add(r, r, u);
}
static inline void p_mat4x4_from_quat(mat4x4 M, quat q){
  float a = q[3];
  float b = q[0];
  float c = q[1];
  float d = q[2];
  float a2 = a*a;
  float b2 = b*b;
  float c2 = c*c;
  float d2 = d*d;

  M[0][0] = a2 + b2 - c2 - d2;
  M[0][1] = 2.f*(b*c + a*d);
  M[0][2] = 2.f*(b*d - a*c);
  M[0][3] = 0.f;

  M[1][0] = 2*(b*c - a*d);
  M[1][1] = a2 - b2 + c2 - d2;
  M[1][2] = 2.f*(c*d + a*b);
  M[1][3] = 0.f;

  M[2][0] = 2.f*(b*d + a*c);
  M[2][1] = 2.f*(c*d - a*b);
  M[2][2] = a2 - b2 - c2 + d2;
  M[2][3] = 0.f;

  M[3][0] = M[3][1] = M[3][2] = 0.f;
  M[3][3] = 1.f;
}

static inline void p_mat4x4o_mul_quat(mat4x4 R, mat4x4 M, quat q){
/*  XXX: The way this is written only works for othogonal matrices. */
/* TODO: Take care of non-orthogonal case. */
  p_quat_mul_vec3(R[0], q, M[0]);
  p_quat_mul_vec3(R[1], q, M[1]);
  p_quat_mul_vec3(R[2], q, M[2]);

  R[3][0] = R[3][1] = R[3][2] = 0.f;
  R[3][3] = 1.f;
}
static inline void p_quat_from_mat4x4(quat q, mat4x4 M){
  float r=0.f;
  int i;

  int perm[] = { 0, 1, 2, 0, 1 };
  int *p = perm;

  for(i = 0; i<3; i++) {
    float m = M[i][i];
    if( m < r ){ continue; }

    m = r;
    p = &perm[i];
  }

  r = sqrtf(1.f + M[p[0]][p[0]] - M[p[1]][p[1]] - M[p[2]][p[2]] );

  if(r < 1e-6) {
    q[0] = 1.f;
    q[1] = q[2] = q[3] = 0.f;
    return;
  }

  q[0] = r/2.f;
  q[1] = (M[p[0]][p[1]] - M[p[1]][p[0]])/(2.f*r);
  q[2] = (M[p[2]][p[0]] - M[p[0]][p[2]])/(2.f*r);
  q[3] = (M[p[2]][p[1]] - M[p[1]][p[2]])/(2.f*r);
}

static inline void p_quat_from_euler(quat q, float x, float y, float z){
  float c1 = cos(x/2.0);
  float c2 = cos(y/2.0);
  float c3 = cos(z/2.0);

  float s1 = sin(x/2.0);
  float s2 = sin(y/2.0);
  float s3 = sin(z/2.0);

  q[0] = c1 * c2 *  - s1 * s2 * s3;
  q[1] = s1 * s2 * c3 + c1 * c2 * s3;
  q[2] = s1 * c2 * c3 + c1 * s2 * s3;
  q[3] = c1 * s2 * c3 - s1 * c2 * s3;
}

float p_to_radians(float degrees){
  float rad = 3.1415926535/180;
  return degrees * rad;
}

static inline void p_quat_from_axis_angle(quat q, vec3 axis, float angle_deg){
  q[0] = axis[0] * sin(angle_deg/2.0);
  q[1] = axis[1] * sin(angle_deg/2.0);
  q[2] = axis[2] * sin(angle_deg/2.0);
  q[3] = cos(angle_deg/2.0);
  p_quat_norm(q, q);
}

float p_vec3_dot(vec3 a, vec3 b){
   return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

static inline void p_quat_from_normal(quat q, vec3 normal, vec3 up){
  vec3 axis;
  p_vec3_mul_cross(axis, up, normal); // axis = up x normal
  p_vec3_norm(axis, axis);
  float dotted = p_vec3_dot(up, normal);
  float angle = acos(dotted); // angle = arccos(up dot normal)
  p_quat_from_axis_angle(q, axis, angle); // set quaternion based on axis and angle
}

void p_vec3_length(float *out, vec3 input){
  *out =
      sqrt((input[0] * input[0]) + (input[1] * input[1]) + (input[2] * input[2]));
}

#ifdef __cplusplus
  extern "C" {
#endif
  // User Input -------------------------------------------------------------
  // Key scancodes. For letters/numbers, use ASCII ('A'-'Z' and '0'-'9').
  typedef enum {
    PK_PAD0=128,PK_PAD1,PK_PAD2,PK_PAD3,PK_PAD4,PK_PAD5,PK_PAD6,PK_PAD7,PK_PAD8,PK_PAD9,
    PK_PADMUL,PK_PADADD,PK_PADENTER,PK_PADSUB,PK_PADDOT,PK_PADDIV,
    PK_F1,PK_F2,PK_F3,PK_F4,PK_F5,PK_F6,PK_F7,PK_F8,PK_F9,PK_F10,PK_F11,PK_F12,
    PK_BACKSPACE,PK_TAB,PK_RETURN,PK_SHIFT,PK_CONTROL,PK_ALT,PK_PAUSE,PK_CAPSLOCK,
    PK_ESCAPE,PK_SPACE,PK_PAGEUP,PK_PAGEDN,PK_END,PK_HOME,PK_LEFT,PK_UP,PK_RIGHT,PK_DOWN,
    PK_INSERT,PK_DELETE,PK_LWIN,PK_RWIN,PK_NUMLOCK,PK_SCROLL,PK_LSHIFT,PK_RSHIFT,
    PK_LCONTROL,PK_RCONTROL,PK_LALT,PK_RALT,PK_SEMICOLON,PK_EQUALS,PK_COMMA,PK_MINUS,
    PK_DOT,PK_SLASH,PK_BACKTICK,PK_LSQUARE,PK_BACKSLASH,PK_RSQUARE,PK_TICK
  } PKey;

#ifdef __cplusplus
  }
#endif

/**********************************************************************************************************************/


// MACOS ONLY:
#ifdef __APPLE__

  static bool _p_macos_terminated = false;

  // we gonna construct objective-c class by hand in runtime, so wow, so hacker!
  NSUInteger _p_macos_app_should_terminate(id self, SEL _sel, id sender) {
    (void)self;
    (void)_sel;
    (void)sender;
    _p_macos_terminated = true;
    return 0;
  }

  void _p_window_will_close(id self, SEL _sel, id notification) {
    (void)_sel;
    (void)notification;
    NSUInteger value = true;
    object_setInstanceVariable(self, "closed", (void *)value);
  }

  // this will run when MacOS sends a message to us that our program now has
  // the focus for all key inputs. This means that we should reset our input
  // state so we're not retaining outdated inputs:
  void _p_window_did_become_key(id self, SEL _sel, id notification) {
    (void)_sel;
    (void)notification;
    platform_api *api;
    object_getInstanceVariable(self, "apiHandle", (void **)&api);

    // we're using the 'initted' flag here to check whether
    // object_getInstanceVariable actually _found_ the struct or not.
    if(api && api->system.initted) {
      memset(api->keys, 0, sizeof(api->keys));
      memset(api->prev_keys, 0, sizeof(api->prev_keys));

      memset(&api->mouse, 0, sizeof(api->mouse));
    }
  }

  void _p_macos_cocoa_window_size(id window, NSSize *render_out, NSSize *window_out, float *scale) {
    id contentView = objc_msgSend_id(window, sel_registerName("contentView"));
    NSRect window_rect;
    NSRect render_rect;

    CGFloat backingScale;
    // [window backingScaleFactor]
    backingScale = ((CGFloat(*)(id, SEL))objc_msgSend)(window, sel_registerName("backingScaleFactor"));

    //[contentView frame];
    window_rect = ((NSRect(*)(id, SEL))abi_objc_msgSend_stret)(contentView, sel_registerName("frame"));
    // window_rect = ((NSRect(*)(id, SEL))abi_objc_msgSend_stret)(contentView, sel_registerName("bounds"));

    // // [window setContentSize:window_rect.size]
    ((void (*)(id, SEL, NSSize))objc_msgSend)(window, sel_registerName("setContentSize:"), window_rect.size);

    // NSRect viewLoc = ((NSRect(*)(id, SEL, NSRect, id))objc_msgSend)(contentView, sel_registerName("convertRect:fromView:"), window_rect, contentView);

    // [contentView convertRectToBacking:window_rect]
    // OR: [contentView convertRectToBacking:[contentView bounds]]
    render_rect = ((NSRect(*)(id, SEL, NSRect))abi_objc_msgSend_stret)(contentView, sel_registerName("convertRectToBacking:"), window_rect);
    // p_log("render rect: %f, %f\n", render_rect.size.width, render_rect.size.height);

    *window_out = window_rect.size;
    *render_out = render_rect.size;
    *scale = backingScale;
  }

  void _p_window_did_resize(id self, SEL _sel, id notification){
    (void)_sel;
    (void)notification;
    platform_api *api;
    object_getInstanceVariable(self, "apiHandle", (void **)&api);

    if(api) {

      #ifdef PLATFORM_USE_OPENGL
        // THIS IS IMPORTANT, resizes require the gl context to update to the correct size.
        // [api->window.gl_context update];
        ((void (*)(id, SEL))objc_msgSend)((id)api->window.gl_context, sel_registerName("update"));
      #endif

      NSSize render_size = {};
      NSSize window_size = {};
      float scale;
      int new_wwidth, new_wheight, new_rwidth, new_rheight;
      _p_macos_cocoa_window_size((id)api->window.window_handle, &render_size, &window_size, &scale);
      new_wwidth = window_size.width;
      new_wheight = window_size.height;
      new_rwidth = render_size.width;
      new_rheight = render_size.height;
      // p_log("resized to window size: %i, %i and render size: %i, %i\n", new_wwidth, new_wheight, new_rwidth, new_rheight);
      _p_update_window_size(api, new_rwidth, new_rheight, scale);
    }
  }

  void _p_view_did_change_backing_properties(id self, SEL _sel, id notification){
    (void)_sel;
    (void)notification;
    platform_api *api;
    object_getInstanceVariable(self, "apiHandle", (void **)&api);

    if(api) {
      NSSize render_size = {};
      NSSize window_size = {};
      float scale;
      int new_wwidth, new_wheight, new_rwidth, new_rheight;
      _p_macos_cocoa_window_size((id)api->window.window_handle, &render_size, &window_size, &scale);
      new_wwidth = window_size.width;
      new_wheight = window_size.height;
      new_rwidth = render_size.width;
      new_rheight = render_size.height;
      // p_log("view changed backing props, window size: %i, %i and render size: %i, %i\n", new_wwidth, new_wheight, new_rwidth, new_rheight);
      _p_update_window_size(api, new_rwidth, new_rheight, scale);
    }
  }

  void _p_window_did_miniaturize(id self, SEL _sel, id notification){
    (void)_sel;
    (void)notification;
    platform_api *api;
    object_getInstanceVariable(self, "apiHandle", (void **)&api);
    if(api){
      p_log("window miniaturized!\n");
    }
  }

  void _p_window_did_deminiaturize(id self, SEL _sel, id notification){
    (void)_sel;
    (void)notification;
    platform_api *api;
    object_getInstanceVariable(self, "apiHandle", (void **)&api);
    if(api){
      p_log("window deminiaturized!\n");
    }

  }

  void _p_window_did_move(id self, SEL _sel, id notification){
    (void)_sel;
    (void)notification;
    platform_api *api;
    object_getInstanceVariable(self, "apiHandle", (void **)&api);
    if(api){
      p_log("window moved!\n");
    }
  }

  void _p_window_did_change_screen(id self, SEL _sel, id notification){
    (void)_sel;
    (void)notification;
    p_log("window changed screens!\n");
    platform_api *api;
    object_getInstanceVariable(self, "apiHandle", (void **)&api);

    if(api) {
      #ifdef PLATFORM_USE_OPENGL
        // THIS IS IMPORTANT, resizes require the gl context to update to the correct size.
        // [api->window.gl_context update];
        ((void (*)(id, SEL))objc_msgSend)((id)api->window.gl_context, sel_registerName("update"));
      #endif

      NSSize render_size = {};
      NSSize window_size = {};
      float scale;
      int new_wwidth, new_wheight, new_rwidth, new_rheight;
      _p_macos_cocoa_window_size((id)api->window.window_handle, &render_size, &window_size, &scale);
      new_wwidth = window_size.width;
      new_wheight = window_size.height;
      new_rwidth = render_size.width;
      new_rheight = render_size.height;
      // p_log("resized to window size: %i, %i and render size: %i, %i\n", new_wwidth, new_wheight, new_rwidth, new_rheight);
      _p_update_window_size(api, new_rwidth, new_rheight, scale);

    }
  }

  bool _p_macos_is_window_closed(platform_api *api){
    bool isWindowClosed;
    // check if this window is marked as closed by the OS:
    id wdg = objc_msgSend_id((id)api->window.window_handle, sel_registerName("delegate"));
    if(!wdg){
      isWindowClosed = false;
    } else {
      NSUInteger value = 0;
      object_getInstanceVariable(wdg, "closed", (void **)&value);
      isWindowClosed = value ? true : false;
    }
    return isWindowClosed;
  }

  uint8_t _p_map_macos_key(uint16_t key) {
    // from Carbon HIToolbox/Events.h
    enum {
      kVK_ANSI_A = 0x00,
      kVK_ANSI_S = 0x01,
      kVK_ANSI_D = 0x02,
      kVK_ANSI_F = 0x03,
      kVK_ANSI_H = 0x04,
      kVK_ANSI_G = 0x05,
      kVK_ANSI_Z = 0x06,
      kVK_ANSI_X = 0x07,
      kVK_ANSI_C = 0x08,
      kVK_ANSI_V = 0x09,
      kVK_ANSI_B = 0x0B,
      kVK_ANSI_Q = 0x0C,
      kVK_ANSI_W = 0x0D,
      kVK_ANSI_E = 0x0E,
      kVK_ANSI_R = 0x0F,
      kVK_ANSI_Y = 0x10,
      kVK_ANSI_T = 0x11,
      kVK_ANSI_1 = 0x12,
      kVK_ANSI_2 = 0x13,
      kVK_ANSI_3 = 0x14,
      kVK_ANSI_4 = 0x15,
      kVK_ANSI_6 = 0x16,
      kVK_ANSI_5 = 0x17,
      kVK_ANSI_Equal = 0x18,
      kVK_ANSI_9 = 0x19,
      kVK_ANSI_7 = 0x1A,
      kVK_ANSI_Minus = 0x1B,
      kVK_ANSI_8 = 0x1C,
      kVK_ANSI_0 = 0x1D,
      kVK_ANSI_RightBracket = 0x1E,
      kVK_ANSI_O = 0x1F,
      kVK_ANSI_U = 0x20,
      kVK_ANSI_LeftBracket = 0x21,
      kVK_ANSI_I = 0x22,
      kVK_ANSI_P = 0x23,
      kVK_ANSI_L = 0x25,
      kVK_ANSI_J = 0x26,
      kVK_ANSI_Quote = 0x27,
      kVK_ANSI_K = 0x28,
      kVK_ANSI_Semicolon = 0x29,
      kVK_ANSI_Backslash = 0x2A,
      kVK_ANSI_Comma = 0x2B,
      kVK_ANSI_Slash = 0x2C,
      kVK_ANSI_N = 0x2D,
      kVK_ANSI_M = 0x2E,
      kVK_ANSI_Period = 0x2F,
      kVK_ANSI_Grave = 0x32,
      kVK_ANSI_KeypadDecimal = 0x41,
      kVK_ANSI_KeypadMultiply = 0x43,
      kVK_ANSI_KeypadPlus = 0x45,
      kVK_ANSI_KeypadClear = 0x47,
      kVK_ANSI_KeypadDivide = 0x4B,
      kVK_ANSI_KeypadEnter = 0x4C,
      kVK_ANSI_KeypadMinus = 0x4E,
      kVK_ANSI_KeypadEquals = 0x51,
      kVK_ANSI_Keypad0 = 0x52,
      kVK_ANSI_Keypad1 = 0x53,
      kVK_ANSI_Keypad2 = 0x54,
      kVK_ANSI_Keypad3 = 0x55,
      kVK_ANSI_Keypad4 = 0x56,
      kVK_ANSI_Keypad5 = 0x57,
      kVK_ANSI_Keypad6 = 0x58,
      kVK_ANSI_Keypad7 = 0x59,
      kVK_ANSI_Keypad8 = 0x5B,
      kVK_ANSI_Keypad9 = 0x5C,
      kVK_Return = 0x24,
      kVK_Tab = 0x30,
      kVK_Space = 0x31,
      kVK_Delete = 0x33,
      kVK_Escape = 0x35,
      kVK_Command = 0x37,
      kVK_Shift = 0x38,
      kVK_CapsLock = 0x39,
      kVK_Option = 0x3A,
      kVK_Control = 0x3B,
      kVK_RightShift = 0x3C,
      kVK_RightOption = 0x3D,
      kVK_RightControl = 0x3E,
      kVK_Function = 0x3F,
      kVK_F17 = 0x40,
      kVK_VolumeUp = 0x48,
      kVK_VolumeDown = 0x49,
      kVK_Mute = 0x4A,
      kVK_F18 = 0x4F,
      kVK_F19 = 0x50,
      kVK_F20 = 0x5A,
      kVK_F5 = 0x60,
      kVK_F6 = 0x61,
      kVK_F7 = 0x62,
      kVK_F3 = 0x63,
      kVK_F8 = 0x64,
      kVK_F9 = 0x65,
      kVK_F11 = 0x67,
      kVK_F13 = 0x69,
      kVK_F16 = 0x6A,
      kVK_F14 = 0x6B,
      kVK_F10 = 0x6D,
      kVK_F12 = 0x6F,
      kVK_F15 = 0x71,
      kVK_Help = 0x72,
      kVK_Home = 0x73,
      kVK_PageUp = 0x74,
      kVK_ForwardDelete = 0x75,
      kVK_F4 = 0x76,
      kVK_End = 0x77,
      kVK_F2 = 0x78,
      kVK_PageDown = 0x79,
      kVK_F1 = 0x7A,
      kVK_LeftArrow = 0x7B,
      kVK_RightArrow = 0x7C,
      kVK_DownArrow = 0x7D,
      kVK_UpArrow = 0x7E
    };

    switch(key) {
    case kVK_ANSI_Q: return 'Q';
    case kVK_ANSI_W: return 'W';
    case kVK_ANSI_E: return 'E';
    case kVK_ANSI_R: return 'R';
    case kVK_ANSI_T: return 'T';
    case kVK_ANSI_Y: return 'Y';
    case kVK_ANSI_U: return 'U';
    case kVK_ANSI_I: return 'I';
    case kVK_ANSI_O: return 'O';
    case kVK_ANSI_P: return 'P';
    case kVK_ANSI_A: return 'A';
    case kVK_ANSI_S: return 'S';
    case kVK_ANSI_D: return 'D';
    case kVK_ANSI_F: return 'F';
    case kVK_ANSI_G: return 'G';
    case kVK_ANSI_H: return 'H';
    case kVK_ANSI_J: return 'J';
    case kVK_ANSI_K: return 'K';
    case kVK_ANSI_L: return 'L';
    case kVK_ANSI_Z: return 'Z';
    case kVK_ANSI_X: return 'X';
    case kVK_ANSI_C: return 'C';
    case kVK_ANSI_V: return 'V';
    case kVK_ANSI_B: return 'B';
    case kVK_ANSI_N: return 'N';
    case kVK_ANSI_M: return 'M';
    case kVK_ANSI_0: return '0';
    case kVK_ANSI_1: return '1';
    case kVK_ANSI_2: return '2';
    case kVK_ANSI_3: return '3';
    case kVK_ANSI_4: return '4';
    case kVK_ANSI_5: return '5';
    case kVK_ANSI_6: return '6';
    case kVK_ANSI_7: return '7';
    case kVK_ANSI_8: return '8';
    case kVK_ANSI_9: return '9';
    case kVK_ANSI_Keypad0: return PK_PAD0;
    case kVK_ANSI_Keypad1: return PK_PAD1;
    case kVK_ANSI_Keypad2: return PK_PAD2;
    case kVK_ANSI_Keypad3: return PK_PAD3;
    case kVK_ANSI_Keypad4: return PK_PAD4;
    case kVK_ANSI_Keypad5: return PK_PAD5;
    case kVK_ANSI_Keypad6: return PK_PAD6;
    case kVK_ANSI_Keypad7: return PK_PAD7;
    case kVK_ANSI_Keypad8: return PK_PAD8;
    case kVK_ANSI_Keypad9: return PK_PAD9;
    case kVK_ANSI_KeypadMultiply: return PK_PADMUL;
    case kVK_ANSI_KeypadPlus: return PK_PADADD;
    case kVK_ANSI_KeypadEnter: return PK_PADENTER;
    case kVK_ANSI_KeypadMinus: return PK_PADSUB;
    case kVK_ANSI_KeypadDecimal: return PK_PADDOT;
    case kVK_ANSI_KeypadDivide: return PK_PADDIV;
    case kVK_F1: return PK_F1;
    case kVK_F2: return PK_F2;
    case kVK_F3: return PK_F3;
    case kVK_F4: return PK_F4;
    case kVK_F5: return PK_F5;
    case kVK_F6: return PK_F6;
    case kVK_F7: return PK_F7;
    case kVK_F8: return PK_F8;
    case kVK_F9: return PK_F9;
    case kVK_F10: return PK_F10;
    case kVK_F11: return PK_F11;
    case kVK_F12: return PK_F12;
    case kVK_Shift: return PK_LSHIFT;
    case kVK_Control: return PK_LCONTROL;
    case kVK_Option: return PK_LALT;
    case kVK_CapsLock: return PK_CAPSLOCK;
    case kVK_Command: return PK_LWIN;
    case kVK_Command - 1: return PK_RWIN;
    case kVK_RightShift: return PK_RSHIFT;
    case kVK_RightControl: return PK_RCONTROL;
    case kVK_RightOption: return PK_RALT;
    case kVK_Delete: return PK_BACKSPACE;
    case kVK_Tab: return PK_TAB;
    case kVK_Return: return PK_RETURN;
    case kVK_Escape: return PK_ESCAPE;
    case kVK_Space: return PK_SPACE;
    case kVK_PageUp: return PK_PAGEUP;
    case kVK_PageDown: return PK_PAGEDN;
    case kVK_End: return PK_END;
    case kVK_Home: return PK_HOME;
    case kVK_LeftArrow: return PK_LEFT;
    case kVK_UpArrow: return PK_UP;
    case kVK_RightArrow: return PK_RIGHT;
    case kVK_DownArrow: return PK_DOWN;
    case kVK_Help: return PK_INSERT;
    case kVK_ForwardDelete: return PK_DELETE;
    case kVK_F14: return PK_SCROLL;
    case kVK_F15: return PK_PAUSE;
    case kVK_ANSI_KeypadClear: return PK_NUMLOCK;
    case kVK_ANSI_Semicolon: return PK_SEMICOLON;
    case kVK_ANSI_Equal: return PK_EQUALS;
    case kVK_ANSI_Comma: return PK_COMMA;
    case kVK_ANSI_Minus: return PK_MINUS;
    case kVK_ANSI_Slash: return PK_SLASH;
    case kVK_ANSI_Backslash: return PK_BACKSLASH;
    case kVK_ANSI_Grave: return PK_BACKTICK;
    case kVK_ANSI_Quote: return PK_TICK;
    case kVK_ANSI_LeftBracket: return PK_LSQUARE;
    case kVK_ANSI_RightBracket: return PK_RSQUARE;
    case kVK_ANSI_Period: return PK_DOT;
    default: return 0;
    }
  }

  void _p_on_cocoa_event(platform_api *api, id event) {
    if(!event){
      return;
    }


    // @TODO this stuff might be useful, but we're just passing the api object in the function now.
    // It was forcing the window out-of-focus, I'm assuming because we were trying to select
    // the delegate's information, instead of the actual window?
    //
    // let's find the API object based on property of the window:
    // id wdg = objc_msgSend_id(window, sel_registerName("delegate"));
    // if(!wdg){
    //   p_log("window delegate not found!\n");
    //   return;
    // }
    //
    // platform_api *api = 0;
    // object_getInstanceVariable(wdg, "apiHandle", (void **)&api);

    // if(!api){ // just pipe the event
    //   p_log("could not find API\n");
    //   objc_msgSend_void_id(NSApp, sel_registerName("sendEvent:"), event);
    //   return;
    // }



    // annoying, but we need to silence the keystroke event error noises by
    // preventing events from continuing if they're keystroke events:
    int silence = 0;
    bool already_down; // used/overwritten in the switch cases below

    NSUInteger eventType = ((NSUInteger(*)(id, SEL))objc_msgSend)(event, sel_registerName("type"));

    switch(eventType) {
      case 1:// NSLeftMouseDown
      {
        already_down = api->mouse.left.is_down;
        api->mouse.left.is_down = true;
        api->mouse.left.was_pressed = already_down;
        api->mouse.left.was_released = false;
        // p_log("left click down!\n");
        break;
      }
      case 2:// NSLeftMouseUp
      {
        already_down = api->mouse.left.is_down;
        api->mouse.left.is_down = false;
        api->mouse.left.was_pressed = false;
        api->mouse.left.was_released = already_down; // released since it was down, but isn't now
        // p_log("left click up!\n");
        break;
      }
      case 3:// NSRightMouseDown
      {
        already_down = api->mouse.right.is_down;
        api->mouse.right.is_down = true;
        api->mouse.right.was_pressed = already_down;
        api->mouse.right.was_released = false;
        // p_log("right click down!\n");
        break;
      }
      case 4:// NSRightMouseUp
      {
        already_down = api->mouse.right.is_down;
        api->mouse.right.is_down = false;
        api->mouse.right.was_pressed = false;
        api->mouse.right.was_released = already_down; // released since it was down, but isn't now
        // p_log("right click up!\n");
        break;
      }
      case 5: { break; }// NSEventTypeMouseMoved
      case 6: { break; } // NSEventTypeLeftMouseDragged
      case 7: { break; } // NSEventTypeRightMouseDragged
      case 8: { break; } // NSEventTypeMouseEntered
      case 9: { break; } // NSEventTypeMouseExited
      case 10:// NSKeyDown
      {
        id inputText = objc_msgSend_id(event, sel_registerName("characters"));
        const char *inputTextUTF8 = ((const char *(*)(id, SEL))objc_msgSend)(inputText, sel_registerName("UTF8String"));
        int lastChar;
        _p_decode_utf8(inputTextUTF8, &lastChar);
        api->prev_keys[lastChar].is_down = true;

        uint16_t keyCode = ((unsigned short (*)(id, SEL))objc_msgSend)(event, sel_registerName("keyCode"));
        int keyVal = _p_map_macos_key(keyCode);
        already_down = api->keys[keyVal].is_down || api->prev_keys[keyVal].is_down;

        api->keys[keyVal].was_pressed = already_down;
        api->keys[keyVal].is_down = true;
        silence = 1;// silence the error!
        break;
      }
      case 11:// NSKeyUp
      {
        uint16_t keyCode = ((unsigned short (*)(id, SEL))objc_msgSend)(event, sel_registerName("keyCode"));
        int keyVal = _p_map_macos_key(keyCode);

        already_down = api->keys[keyVal].is_down || api->prev_keys[keyVal].is_down;

        api->keys[keyVal].was_pressed = true;
        api->keys[keyVal].was_released = already_down;
        api->keys[keyVal].is_down = false;
        break;
      }
      case 12:// NSFlagsChanged
      {
        NSUInteger modifiers = ((NSUInteger(*)(id, SEL))objc_msgSend)(
            event, sel_registerName("modifierFlags"));

        // based on NSEventModifierFlags and NSDeviceIndependentModifierFlagsMask
        struct {
          union {
            struct {
              uint8_t alpha_shift : 1;
              uint8_t shift : 1;
              uint8_t control : 1;
              uint8_t alternate : 1;
              uint8_t command : 1;
              uint8_t numeric_pad : 1;
              uint8_t help : 1;
              uint8_t function : 1;
            };
            uint8_t mask;
          };
        } keys;

        keys.mask = (modifiers & 0xffff0000UL) >> 16;

        // TODO L,R variation of keys?
        api->keys[PK_CONTROL].is_down = keys.alpha_shift;
        api->keys[PK_SHIFT].is_down = keys.shift;
        api->keys[PK_CONTROL].is_down = keys.control;
        api->keys[PK_ALT].is_down = keys.alternate;
        api->keys[PK_LWIN].is_down = keys.command;
        api->keys[PK_RWIN].is_down = keys.command;
        break;
      }
      case 13: { break; } // NSEventTypeAppKitDefined
      case 14: { break; } // NSEventTypeSystemDefined
      case 15: { break; } // NSEventTypeApplicationDefined
      case 16: { break; } // NSEventTypePeriodic (An event that provides execution time to periodic tasks.)
      case 17: { break; } // NSEventTypeCursorUpdate
      case 18: { break; } // NSEventTypeRotate (The user performed a rotate gesture.)
      case 19: { break; } // NSEventTypeBeginGesture (An event marking the beginning of a gesture.)
      case 20: { break; } // NSEventTypeEndGesture (An event that marks the end of a gesture.)
      case 21: { break; } // ProcessNotification (Internal?)
      case 22:// NSScrollWheel
      {
        CGFloat deltaX = ((CGFloat(*)(id, SEL))abi_objc_msgSend_fpret)(event, sel_registerName("scrollingDeltaX"));
        CGFloat deltaY = ((CGFloat(*)(id, SEL))abi_objc_msgSend_fpret)(event, sel_registerName("scrollingDeltaY"));
        BOOL precisionScrolling = ((BOOL(*)(id, SEL))objc_msgSend)(event, sel_registerName("hasPreciseScrollingDeltas"));

        if(precisionScrolling) {
          deltaX *= 0.1f;// similar to glfw
          deltaY *= 0.1f;
        }

        if(fabs(deltaX) > 0.0f || fabs(deltaY) > 0.0f){
          p_log("mouse scroll wheel delta %f %f\n", deltaX, deltaY);
        }
        break;
      }
      case 23: { break; } // NSEventTypeTabletPoint
      case 24: { break; } // NSEventTypeTabletProximity (A pointing device is near, but not touching, the associated tablet.)
      case 25:// NSEventTypeOtherMouseDown
      {
        // number == 2 is a middle button
        NSInteger number = ((NSInteger(*)(id, SEL))objc_msgSend)(event, sel_registerName("buttonNumber"));
        if(number == 2){
          already_down = api->mouse.middle.is_down;
          api->mouse.middle.is_down = true;
          api->mouse.middle.was_pressed = already_down;
          api->mouse.middle.was_released = false;
        }
        break;
      }
      case 26:// NSEventTypeOtherMouseUp
      {
        NSInteger number = ((NSInteger(*)(id, SEL))objc_msgSend)(
            event, sel_registerName("buttonNumber"));
        if(number == 2){
          already_down = api->mouse.middle.is_down;
          api->mouse.middle.is_down = false;
          api->mouse.middle.was_pressed = false;
          api->mouse.middle.was_released = already_down;
        }
        break;
      }
      case 27: { break; } // NSEventTypeOtherMouseDragged
      case 28: { break; } //
      case 29: { break; } // NSEventTypeGesture
      case 30: { break; } // NSEventTypeMagnify (The user performed a pinch-open or pinch-close gesture.)
      case 31: { break; } // NSEventTypeSwipe
      case 32: { break; } // NSEventTypeSmartMagnify (The user performed a smart-zoom gesture.)
      case 33: { break; } // NSEventTypeQuickLook (An event that initiates a Quick Look request.)
      case 34: { break; } // NSEventTypePressure (An event that reports a change in pressure on a pressure-sensitive device.)
      case 35: { break; } // Reserved?
      case 36: { break; } // Reserved?
      case 37: { break; } // NSEventTypeDirectTouch (The user touched a portion of the touch bar.)
      case 38: { break; } // NSEventTypeChangeMode (The user changed the mode of a connected device.)

      default: break;
    }

    // pipe the event along to the OS defaults if we're not trying to
    // silence "unhandled event" beeps:
    if(silence == 0) {
      objc_msgSend_void_id(NSApp, sel_registerName("sendEvent:"), event);
    }
  }

  // A bunch of OpenGL Pixel Format Attributes:
  #define NSOpenGLPFAAccelerated 73 // A Boolean attribute. If present, this attribute indicates that only hardware-accelerated renderers are considered. If not present, accelerated renderers are still preferred.
  #define NSOpenGLPFAAcceleratedCompute 97 //  If present, this attribute indicates that only renderers that can execute OpenCL programs should be used.
  #define NSOpenGLPFAAccumSize 14 // Value is a nonnegative buffer size specification. An accumulation buffer that most closely matches the specified size is preferred. (for glAccum)
  #define NSOpenGLPFAAllRenderers 1 // A Boolean attribute. If present, this attribute indicates that the pixel format selection is open to all available renderers, including debug and special-purpose renderers that are not OpenGL compliant.
  #define NSOpenGLPFAAllowOfflineRenderers 96 // A Boolean attribute. If present, this attribute indicates that offline renderers may be used.
  #define NSOpenGLPFAAlphaSize 11 // Value is a nonnegative buffer size specification. An alpha buffer that most closely matches the specified size is preferred.
  #define NSOpenGLPFAAuxBuffers 7 // Value is a nonnegative integer that indicates the desired number of auxiliary buffers. Pixel formats with the smallest number of auxiliary buffers that meets or exceeds the specified number are preferred.
  #define NSOpenGLPFAAuxDepthStencil 57 // Each auxiliary buffer has its own depth stencil.
  #define NSOpenGLPFABackingStore 76 // A Boolean attribute. If present, this attribute indicates that OpenGL only considers renderers that have a back color buffer the full size of the drawable (regardless of window visibility) and that guarantee the back buffer contents to be valid after a call to NSOpenGLContext object’s flushBuffer.
  #define NSOpenGLPFAClosestPolicy 74 // A Boolean attribute. If present, this attribute indicates that the pixel format choosing policy is altered for the color buffer such that the buffer closest to the requested size is preferred, regardless of the actual color buffer depth of the supported graphics device.
  #define NSOpenGLPFAColorFloat 58 // ???
  #define NSOpenGLPFAColorSize 8 // Value is a nonnegative buffer size specification. A color buffer that most closely matches the specified size is preferred. If unspecified, OpenGL chooses a color size that matches the screen.
  #define NSOpenGLPFACompliant 83 // A Boolean attribute. If present, this attribute indicates that pixel format selection is only open to OpenGL-compliant renderers. This attribute is implied unless NSOpenGLPFAAllRenderers is specified. This attribute is not useful in the attribute array.
  #define NSOpenGLPFADepthSize 12 // Value is a nonnegative depth buffer size specification. A depth buffer that most closely matches the specified size is preferred.
  #define NSOpenGLPFADoubleBuffer 5 // A Boolean attribute. If present, this attribute indicates that only double-buffered pixel formats are considered. Otherwise, only single-buffered pixel formats are considered.
  #define NSOpenGLPFAFullScreen 54 // A Boolean attribute. If present, this attribute indicates that only renderers that are capable of rendering to a full-screen drawable are considered. The NSOpenGLPFASingleRenderer attribute is implied.
  #define NSOpenGLPFAMPSafe 78 // A Boolean attribute. If present, this attribute indicates that the renderer is multi-processor safe.
  #define NSOpenGLPFAMaximumPolicy 52 // A Boolean attribute. If present, this attribute indicates that the pixel format choosing policy is altered for the color, depth, and accumulation buffers such that, if a nonzero buffer size is requested, the largest available buffer is preferred.
  #define NSOpenGLPFAMinimumPolicy 51 // A Boolean attribute. If present, this attribute indicates that the pixel format choosing policy is altered for the color, depth, and accumulation buffers such that only buffers of size greater than or equal to the desired size are considered.
  #define NSOpenGLPFAMultiScreen 81 // A Boolean attribute. If present, this attribute indicates that only renderers capable of driving multiple screens are considered. This attribute is not generally useful.
  #define NSOpenGLPFAMultisample 59 // A Boolean attribute. If present and used with NSOpenGLPFASampleBuffers and NSOpenGLPFASamples, this attribute hints to OpenGL to prefer multi-sampling. Multi-sampling will sample textures at the back buffer dimensions vice the multi-sample buffer dimensions and use that single sample for all fragments with coverage on the back buffer location. This means less total texture samples than with super-sampling (by a factor of the number of samples requested) and will likely be faster though less accurate (texture sample wise) than super-sampling. If the underlying video card does not have enough VRAM to support this feature, this hint does nothing. The NSOpenGLPFASampleBuffers and NSOpenGLPFASamples attributes must be configured to request anti-aliasing.
  #define NSOpenGLPFANoRecovery 72 // A Boolean attribute. If present, this attribute indicates that OpenGL’s failure recovery mechanisms are disabled. Normally, if an accelerated renderer fails due to lack of resources, OpenGL automatically switches to another renderer. This attribute disables these features so that rendering is always performed by the chosen renderer. This attribute is not generally useful.
  #define NSOpenGLPFAOffScreen 53 // A Boolean attribute. If present, this attribute indicates that only renderers that are capable of rendering to an offscreen memory area and have buffer depth exactly equal to the desired buffer depth are considered. The NSOpenGLPFAClosestPolicy attribute is implied.
  #define NSOpenGLPFAOpenGLProfile 99 // The associated value can be any of the constants defined in OpenGL Profiles. If it is present in the attribute arrays, only renderers capable of supporting an OpenGL context that provides the functionality promised by the profile are considered.
  #define NSOpenGLPFAPixelBuffer 90 // A Boolean attribute. If present, this attribute indicates that rendering to a pixel buffer is enabled.
  #define NSOpenGLPFARemotePixelBuffer 91 // A Boolean attribute. If present, this attribute indicates that rendering to a pixel buffer on an offline renderer is enabled.
  #define NSOpenGLPFARendererID 70 // Value is a nonnegative renderer ID number. OpenGL renderers that match the specified ID are preferred. Constants to select specific renderers are provided in the CGLRenderers.h header of the OpenGL framework. Of note is kCGLRendererGenericID which selects the Apple software renderer. The other constants select renderers for specific hardware vendors.
  #define NSOpenGLPFARobust 75 // A Boolean attribute. If present, this attribute indicates that only renderers that do not have any failure modes associated with a lack of video card resources are considered. This attribute is not generally useful.
  #define NSOpenGLPFASampleAlpha 61 // A Boolean attribute. If present and used with NSOpenGLPFASampleBuffers and NSOpenGLPFASampleBuffers, this attribute hints to OpenGL to update multi-sample alpha values to ensure the most accurate rendering. If pixel format is not requesting antialiasing then this hint does nothing.
  #define NSOpenGLPFASampleBuffers 55 // Value is a nonnegative number indicating the number of multisample buffers.
  #define NSOpenGLPFASamples 56 // Value is a nonnegative indicating the number of samples per multisample buffer.
  #define NSOpenGLPFAScreenMask 84 // Value is a bit mask of supported physical screens. All screens specified in the bit mask are guaranteed to be supported by the pixel format. Screens not specified in the bit mask may still be supported. The bit mask is managed by the CoreGraphics’s DirectDisplay, available in the CGDirectDisplay.h header of the ApplicationServices umbrella framework. A CGDirectDisplayID must be converted to an OpenGL display mask using the function CGDisplayIDToOpenGLDisplayMask. This attribute is not generally useful.
  #define NSOpenGLPFASingleRenderer 71 // A Boolean attribute. If present, this attribute indicates that a single rendering engine is chosen. On systems with multiple screens, this disables OpenGL’s ability to drive different monitors through different graphics accelerator cards with a single context. This attribute is not generally useful.
  #define NSOpenGLPFAStencilSize 13 // Value is a nonnegative integer that indicates the desired number of stencil bitplanes. The smallest stencil buffer of at least the specified size is preferred.
  #define NSOpenGLPFAStereo 6 // A Boolean attribute. If present, this attribute indicates that only stereo pixel formats are considered. Otherwise, only monoscopic pixel formats are considered.
  #define NSOpenGLPFASupersample 60 // A Boolean attribute. If present and used with NSOpenGLPFASampleBuffers and NSOpenGLPFASamples, this attribute hints to OpenGL to prefer super-sampling. Super-sampling will process fragments with a texture sample per fragment and would likely be slower than multi-sampling. If the pixel format is not requesting anti-aliasing, this hint does nothing.
  #define NSOpenGLPFATripleBuffer 3 // A Boolean attribute. If present, this attribute indicates that only triple-buffered pixel formats are considered. Otherwise, only single-buffered pixel formats are considered.
  #define NSOpenGLPFAVirtualScreenCount 128 // The number of virtual screens in this format.
  #define NSOpenGLPFAWindow 80 // A Boolean attribute. If present, this attribute indicates that only renderers that are capable of rendering to a window are considered. This attribute is implied if neither NSOpenGLPFAFullScreen nor NSOpenGLPFAOffScreen is specified.
  #define NSOpenGLProfileVersionLegacy 0x1000 // The requested profile is a legacy (pre-OpenGL 3.0) profile.
  #define NSOpenGLProfileVersion3_2Core 0x3200 // The requested profile must implement the OpenGL 3.2 core functionality.
  #define NSOpenGLProfileVersion4_1Core 0x4100 // The requested profile must implement the OpenGL 4.1 core functionality.




/**********************************************************************************************************************/

// closing ifdef __APPLE__
// BEGIN WINDOWS ONLY:
#elif _WIN32

void _p_enter_borderless_windowed(platform_api *api) {
  // Enter borderless windowed mode.
  MONITORINFO mi;

  GetWindowRect((HWND)api->window.window_handle, &api->window.oldPos);

  GetMonitorInfo(MonitorFromWindow((HWND)api->window.window_handle, MONITOR_DEFAULTTONEAREST), &mi);
  api->window.dwStyle = WS_VISIBLE | WS_POPUP;
  SetWindowLong((HWND)api->window.window_handle, GWL_STYLE, api->window.dwStyle);
  SetWindowPos((HWND)api->window.window_handle, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
               mi.rcMonitor.right - mi.rcMonitor.left,
               mi.rcMonitor.bottom - mi.rcMonitor.top, 0);

  // @TODO set our window size to 0,0 so everyone knows we're maximized:
  api->window.size[1] = (mi.rcMonitor.right - mi.rcMonitor.left);
  api->window.size[0] = (mi.rcMonitor.bottom - mi.rcMonitor.top);
  if(!api->draw.lock_size){
    api->draw.size[0] = (mi.rcMonitor.right - mi.rcMonitor.left) *  api->window.scale;
    api->draw.size[1] = (mi.rcMonitor.bottom - mi.rcMonitor.top) *  api->window.scale;
    api->draw.x = api->draw.size[0];
    api->draw.y = api->draw.size[1];
  }
}

void _p_leave_borderless_windowed(platform_api *api) {

  api->window.dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
  SetWindowLong((HWND)api->window.window_handle, GWL_STYLE, api->window.dwStyle);

  SetWindowPos((HWND)api->window.window_handle, NULL, api->window.oldPos.left, api->window.oldPos.top,
               api->window.oldPos.right - api->window.oldPos.left,
               api->window.oldPos.bottom - api->window.oldPos.top, 0);

  // set our window size to the old size so everyone knows we're maximized:
  api->window.size[0] = api->window.oldPos.right - api->window.oldPos.left;
  api->window.size[1] = api->window.oldPos.bottom - api->window.oldPos.top;
  if(!api->draw.lock_size){
    api->draw.size[0] = api->window.size[0] * api->window.scale;
    api->draw.size[1] = api->window.size[1] * api->window.scale;
    api->draw.x = api->draw.size[0];
    api->draw.y = api->draw.size[1];
  }
}

LRESULT CALLBACK TestWindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;       // message handled
}

// Good resource to refer to:
// https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/

LRESULT CALLBACK p_api_WndProc(HWND hWnd, UINT message, WPARAM wParam,
                             LPARAM lParam) {
// (void)hWnd,
// (void)message,
// (void)wParam,
// (void)lParam;
// return 0;
  RECT rc;
  int dw, dh;
  bool already_down;

  GetClientRect(hWnd, &rc);
  // p_log("client rect is %li, %li, %li, %li\n", rc.left, rc.right, rc.top, rc.bottom);
  dw = rc.right - rc.left;
  dh = rc.bottom - rc.top;

  platform_api *api = (platform_api *)GetPropW(hWnd, L"pAPI");

  switch(message) {
  case WM_PAINT:
    if(!SwapBuffers(api->window.dc)) {
      p_error(api, "Cannot swap Window buffers.\n");
    }
    ValidateRect(hWnd, NULL);
    break;
  case WM_CLOSE:
    if(api){
      api->quit = true;
    }
    PostQuitMessage(0);
    break;
  // case WM_GETMINMAXINFO:
  //   if(api) {
  //     MINMAXINFO *info = (MINMAXINFO *)lParam;
  //     RECT rc;
  //     rc.left = 0;
  //     rc.top = 0;
  //     // if(autosize) {
  //     //   rc.right = 32;
  //     //   rc.bottom = 32;
  //     // } else {
  //       // int minscale = enforceScale(1, win->flags);
  //       int minscale = 1;
  //       rc.right = api->window.size[0] * minscale;
  //       rc.bottom = api->window.size[1] * minscale;
  //     // }
  //     AdjustWindowRectEx(&rc, api->window.dwStyle, FALSE, 0);
  //     info->ptMinTrackSize.x = rc.right - rc.left;
  //     info->ptMinTrackSize.y = rc.bottom - rc.top;
  //   }
  //   return 0;
  case WM_SIZING:
    if(api) {
      // Calculate scale-constrained sizes.
      RECT *rc = (RECT *)lParam;
      int dx, dy;
      UnadjustWindowRectEx(rc, api->window.dwStyle, FALSE, 0);
      dx = (rc->right - rc->left) % (long)api->window.scale;
      dy = (rc->bottom - rc->top) % (long)api->window.scale;
      switch(wParam) {
      case WMSZ_LEFT: rc->left += dx; break;
      case WMSZ_RIGHT: rc->right -= dx; break;
      case WMSZ_TOP: rc->top += dy; break;
      case WMSZ_TOPLEFT:
        rc->left += dx;
        rc->top += dy;
        break;
      case WMSZ_TOPRIGHT:
        rc->right -= dx;
        rc->top += dy;
        break;
      case WMSZ_BOTTOM: rc->bottom -= dy; break;
      case WMSZ_BOTTOMLEFT:
        rc->left += dx;
        rc->bottom -= dy;
        break;
      case WMSZ_BOTTOMRIGHT:
        rc->right -= dx;
        rc->bottom -= dy;
        break;
      }
      AdjustWindowRectEx(rc, api->window.dwStyle, FALSE, 0);
      p_log("updating window pixel scale in WM_SIZING\n");
      _p_update_window_size(api, dw, dh, api->window.scale);
      // p_log("resize event changing window size to: %i, %i\n", dw, dh);
    }
    return TRUE;
  case WM_SIZE:
    if(api) {
      if(wParam != SIZE_MINIMIZED) {
        // Detect window size changes and update our bitmap accordingly.
        dw = LOWORD(lParam);
        dh = HIWORD(lParam);

          // p_log("resize event changing window size to: %i, %i\n", dw, dh);
          p_log("updating window pixel scale in WM_SIZE\n");
          _p_update_window_size(api, dw, dh, api->window.scale);
      }

      // If someone tried to maximize us (e.g. via shortcut launch options),
      // prefer instead to be borderless.
      if(wParam == SIZE_MAXIMIZED) {
        ShowWindow((HWND)api->window.window_handle, SW_NORMAL);
        // @TODO THERE'S AN ISSUE WITH BORDERLESS WINDOWED
        _p_enter_borderless_windowed(api);
      }
    }
    return 0;
  case WM_WINDOWPOSCHANGED: {
    if(api->window.preserve_window_pos){
      // Save our position.
      WINDOWPLACEMENT wp;
      GetWindowPlacement(hWnd, &wp);

      if(api->window.dwStyle & WS_POPUP) wp.showCmd = SW_MAXIMIZE;
      RegSetValueExW(api->window.regKey, api->window.wtitle, 0, REG_BINARY, (BYTE *)&wp,
                     sizeof(wp));
      return DefWindowProcW(hWnd, message, wParam, lParam);
    }
  }
  case WM_ACTIVATE:
    if(api) {
      memset(&api->keys, 0, P_NUM_KEYS);
      memset(&api->prev_keys, 0, P_NUM_KEYS);
      // api->window.lastChar = 0;
    }
    return 0;
  case 0x00ff: {  // WM_INPUT
    // UINT size;
    // GetRawInputData((HRAWINPUT)lparam, RID_INPUT, 0, &size, sizeof(RAWINPUTHEADER));
    // void *buffer = _alloca(size);
    // if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) == size) {
    //     RAWINPUT *raw_input = (RAWINPUT *)buffer;
    //     if (raw_input->header.dwType == RIM_TYPEMOUSE && raw_input->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {
    //         mu->mouse.delta_position.x += raw_input->data.mouse.lLastX;
    //         mu->mouse.delta_position.y += raw_input->data.mouse.lLastY;
    //
    //         USHORT button_flags = raw_input->data.mouse.usButtonFlags;
    //
    //         Mu_Bool left_button_down = mu->mouse.left_button.down;
    //         if (button_flags & RI_MOUSE_LEFT_BUTTON_DOWN) {
    //             left_button_down = MU_TRUE;
    //         }
    //         if (button_flags & RI_MOUSE_LEFT_BUTTON_UP) {
    //             left_button_down = MU_FALSE;
    //         }
    //         Mu_UpdateDigitalButton(&mu->mouse.left_button, left_button_down);
    //
    //         Mu_Bool right_button_down = mu->mouse.right_button.down;
    //         if (button_flags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
    //             right_button_down = MU_TRUE;
    //         }
    //         if (button_flags & RI_MOUSE_RIGHT_BUTTON_UP) {
    //             right_button_down = MU_FALSE;
    //         }
    //         Mu_UpdateDigitalButton(&mu->mouse.right_button, right_button_down);
    //
    //         if (button_flags & RI_MOUSE_WHEEL) {
    //             mu->mouse.delta_wheel += ((SHORT)raw_input->data.mouse.usButtonData) / WHEEL_DELTA;
    //         }
    //     }
    // }
    // result = DefWindowProcA(window, message, wparam, lparam);
    // break;
  }
  case WM_CHAR:
    if(api) {
      if(wParam == '\r') wParam = '\n';
      // api->window.lastChar = wParam;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
  case WM_MENUCHAR:
    // Disable beep on Alt+Enter
    if(LOWORD(wParam) == VK_RETURN) return MNC_CLOSE << 16;
    return DefWindowProcW(hWnd, message, wParam, lParam);
  case WM_SYSKEYDOWN:
    if(api) {
      if(wParam == VK_RETURN) {
        // Alt+Enter
        if(api->window.dwStyle & WS_POPUP){
          _p_leave_borderless_windowed(api);
        }
        else{
          //@TODO THERE IS AN ISSUE WITH THIS FUNC
          _p_enter_borderless_windowed(api);
        }
        return 0;
      }
    }
    // fall-thru
  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) {
        PostQuitMessage(0);
    }
    if(api){
      already_down = api->keys[wParam].is_down || api->prev_keys[wParam].is_down;
      api->keys[wParam].was_pressed = already_down;
      api->keys[wParam].was_released = false;
      api->keys[wParam].is_down = true;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
  case WM_SYSKEYUP:
    // fall-thru
  case WM_KEYUP:
    if(api){
      already_down = api->keys[wParam].is_down || api->prev_keys[wParam].is_down;
      api->keys[wParam].was_pressed = true;
      api->keys[wParam].was_released = already_down;
      api->keys[wParam].is_down = false;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
  default: return DefWindowProcW(hWnd, message, wParam, lParam);
 }
 return 0;
}

int _p_is_gl_11_only(platform_api *api) {
  int pixel_format;
  p_log("running _p_is_gl_11_only\n");
  PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
                               1,
                               PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                                   PFD_DOUBLEBUFFER | PFD_SWAP_EXCHANGE,
                               PFD_TYPE_RGBA,
                               32,// color bits
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               32,// depth
                               8, // stencil
                               0,
                               PFD_MAIN_PLANE,// is it ignored ?
                               0,
                               0,
                               0,
                               0};
  if(!(api->window.dc = GetDC((HWND)api->window.window_handle))) {
    p_error(api, "Cannot create OpenGL device context.\n");
    return 1;
  }
  if(!(pixel_format = ChoosePixelFormat(api->window.dc, &pfd))) {
    p_error(api, "Cannot choose OpenGL pixel format.\n");
    return 1;
  }
  if(!SetPixelFormat(api->window.dc, pixel_format, &pfd)) {
    p_error(api, "Cannot set OpenGL pixel format.\n");
    return 1;
  }
  if(!(api->window.hglrc = wglCreateContext(api->window.dc))) {
    p_error(api, "Cannot create OpenGL context.\n");
    return 1;
  }
  if(!wglMakeCurrent(api->window.dc, api->window.hglrc)) {
    p_error(api, "Cannot activate OpenGL context.\n");
    return 1;
  }

  api->window.gl_legacy = 1;
  return 0;
}

int _p_init_gl_41(platform_api *api) {
  int pixel_format;
  UINT num_formats;
  // p_log("running _p_init_gl_41\n");
  wglChoosePixelFormat = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
  wglCreateContextAttribs = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
  // wglGetCurrentContext = (PFNWGLGETCURRENTCONTEXTARBPROC)wglGetProcAddress("wglGetCurrentContextARB");
  // wglMakeCurrent = (PFNWGLMAKECURRENTPROC)wglGetProcAddress("wglMakeCurrentARB");
  p_log("initting opengl 1.0 api "); // should be already included in Windows
  // opengl 1.0
  // glCullFace = (PFNGLCULLFACEPROC)wglGetProcAddress("glCullFace");
  // glFrontFace = (PFNGLFRONTFACEPROC)wglGetProcAddress("glFrontFace");
  // glHint = (PFNGLHINTPROC)wglGetProcAddress("glHint");
  // glLineWidth = (PFNGLLINEWIDTHPROC)wglGetProcAddress("glLineWidth");
  // glPointSize = (PFNGLPOINTSIZEPROC)wglGetProcAddress("glPointSize");
  // glPolygonMode = (PFNGLPOLYGONMODEPROC)wglGetProcAddress("glPolygonMode");
  // glScissor = (PFNGLSCISSORPROC)wglGetProcAddress("glScissor");
  // glTexParameterf = (PFNGLTEXPARAMETERFPROC)wglGetProcAddress("glTexParameterf");
  // glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)wglGetProcAddress("glTexParameterfv");
  // glTexParameteri = (PFNGLTEXPARAMETERIPROC)wglGetProcAddress("glTexParameteri");
  // glTexParameteriv = (PFNGLTEXPARAMETERIVPROC)wglGetProcAddress("glTexParameteriv");
  // glTexImage1D = (PFNGLTEXIMAGE1DPROC)wglGetProcAddress("glTexImage1D");
  // glTexImage2D = (PFNGLTEXIMAGE2DPROC)wglGetProcAddress("glTexImage2D");
  // glDrawBuffer = (PFNGLDRAWBUFFERPROC)wglGetProcAddress("glDrawBuffer");
  // glClear = (PFNGLCLEARPROC)wglGetProcAddress("glClear");
  // glClearColor = (PFNGLCLEARCOLORPROC)wglGetProcAddress("glClearColor");
  // glClearStencil = (PFNGLCLEARSTENCILPROC)wglGetProcAddress("glClearStencil");
  // glClearDepth = (PFNGLCLEARDEPTHPROC)wglGetProcAddress("glClearDepth");
  // glStencilMask = (PFNGLSTENCILMASKPROC)wglGetProcAddress("glStencilMask");
  // glColorMask = (PFNGLCOLORMASKPROC)wglGetProcAddress("glColorMask");
  // glDepthMask = (PFNGLDEPTHMASKPROC)wglGetProcAddress("glDepthMask");
  // glDisable = (PFNGLDISABLEPROC)wglGetProcAddress("glDisable");
  // glEnable = (PFNGLENABLEPROC)wglGetProcAddress("glEnable");
  // glFinish = (PFNGLFINISHPROC)wglGetProcAddress("glFinish");
  // glFlush = (PFNGLFLUSHPROC)wglGetProcAddress("glFlush");
  // glBlendFunc = (PFNGLBLENDFUNCPROC)wglGetProcAddress("glBlendFunc");
  // glLogicOp = (PFNGLLOGICOPPROC)wglGetProcAddress("glLogicOp");
  // glStencilFunc = (PFNGLSTENCILFUNCPROC)wglGetProcAddress("glStencilFunc");
  // glStencilOp = (PFNGLSTENCILOPPROC)wglGetProcAddress("glStencilOp");
  // glDepthFunc = (PFNGLDEPTHFUNCPROC)wglGetProcAddress("glDepthFunc");
  // glPixelStoref = (PFNGLPIXELSTOREFPROC)wglGetProcAddress("glPixelStoref");
  // glPixelStorei = (PFNGLPIXELSTOREIPROC)wglGetProcAddress("glPixelStorei");
  // glReadBuffer = (PFNGLREADBUFFERPROC)wglGetProcAddress("glReadBuffer");
  // glReadPixels = (PFNGLREADPIXELSPROC)wglGetProcAddress("glReadPixels");
  // glGetBooleanv = (PFNGLGETBOOLEANVPROC)wglGetProcAddress("glGetBooleanv");
  // glGetDoublev = (PFNGLGETDOUBLEVPROC)wglGetProcAddress("glGetDoublev");
  // glGetError = (PFNGLGETERRORPROC)wglGetProcAddress("glGetError");
  // glGetFloatv = (PFNGLGETFLOATVPROC)wglGetProcAddress("glGetFloatv");
  // glGetIntegerv = (PFNGLGETINTEGERVPROC)wglGetProcAddress("glGetIntegerv");
  // glGetString = (PFNGLGETSTRINGPROC)wglGetProcAddress("glGetString");
  // glGetTexImage = (PFNGLGETTEXIMAGEPROC)wglGetProcAddress("glGetTexImage");
  // glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC)wglGetProcAddress("glGetTexParameterfv");
  // glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC)wglGetProcAddress("glGetTexParameteriv");
  // glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC)wglGetProcAddress("glGetTexLevelParameterfv");
  // glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC)wglGetProcAddress("glGetTexLevelParameteriv");
  // glIsEnabled = (PFNGLISENABLEDPROC)wglGetProcAddress("glIsEnabled");
  // glDepthRange = (PFNGLDEPTHRANGEPROC)wglGetProcAddress("glDepthRange");
  // glViewport = (PFNGLVIEWPORTPROC)wglGetProcAddress("glViewport");
  // glNewList = (PFNGLNEWLISTPROC)wglGetProcAddress("glNewList");
  // glEndList = (PFNGLENDLISTPROC)wglGetProcAddress("glEndList");
  // glCallList = (PFNGLCALLLISTPROC)wglGetProcAddress("glCallList");
  // glCallLists = (PFNGLCALLLISTSPROC)wglGetProcAddress("glCallLists");
  // glDeleteLists = (PFNGLDELETELISTSPROC)wglGetProcAddress("glDeleteLists");
  // glGenLists = (PFNGLGENLISTSPROC)wglGetProcAddress("glGenLists");
  // glListBase = (PFNGLLISTBASEPROC)wglGetProcAddress("glListBase");
  // glBegin = (PFNGLBEGINPROC)wglGetProcAddress("glBegin");
  // glBitmap = (PFNGLBITMAPPROC)wglGetProcAddress("glBitmap");
  // glColor3b = (PFNGLCOLOR3BPROC)wglGetProcAddress("glColor3b");
  // glColor3bv = (PFNGLCOLOR3BVPROC)wglGetProcAddress("glColor3bv");
  // glColor3d = (PFNGLCOLOR3DPROC)wglGetProcAddress("glColor3d");
  // glColor3dv = (PFNGLCOLOR3DVPROC)wglGetProcAddress("glColor3dv");
  // glColor3f = (PFNGLCOLOR3FPROC)wglGetProcAddress("glColor3f");
  // glColor3fv = (PFNGLCOLOR3FVPROC)wglGetProcAddress("glColor3fv");
  // glColor3i = (PFNGLCOLOR3IPROC)wglGetProcAddress("glColor3i");
  // glColor3iv = (PFNGLCOLOR3IVPROC)wglGetProcAddress("glColor3iv");
  // glColor3s = (PFNGLCOLOR3SPROC)wglGetProcAddress("glColor3s");
  // glColor3sv = (PFNGLCOLOR3SVPROC)wglGetProcAddress("glColor3sv");
  // glColor3ub = (PFNGLCOLOR3UBPROC)wglGetProcAddress("glColor3ub");
  // glColor3ubv = (PFNGLCOLOR3UBVPROC)wglGetProcAddress("glColor3ubv");
  // glColor3ui = (PFNGLCOLOR3UIPROC)wglGetProcAddress("glColor3ui");
  // glColor3uiv = (PFNGLCOLOR3UIVPROC)wglGetProcAddress("glColor3uiv");
  // glColor3us = (PFNGLCOLOR3USPROC)wglGetProcAddress("glColor3us");
  // glColor3usv = (PFNGLCOLOR3USVPROC)wglGetProcAddress("glColor3usv");
  // glColor4b = (PFNGLCOLOR4BPROC)wglGetProcAddress("glColor4b");
  // glColor4bv = (PFNGLCOLOR4BVPROC)wglGetProcAddress("glColor4bv");
  // glColor4d = (PFNGLCOLOR4DPROC)wglGetProcAddress("glColor4d");
  // glColor4dv = (PFNGLCOLOR4DVPROC)wglGetProcAddress("glColor4dv");
  // glColor4f = (PFNGLCOLOR4FPROC)wglGetProcAddress("glColor4f");
  // glColor4fv = (PFNGLCOLOR4FVPROC)wglGetProcAddress("glColor4fv");
  // glColor4i = (PFNGLCOLOR4IPROC)wglGetProcAddress("glColor4i");
  // glColor4iv = (PFNGLCOLOR4IVPROC)wglGetProcAddress("glColor4iv");
  // glColor4s = (PFNGLCOLOR4SPROC)wglGetProcAddress("glColor4s");
  // glColor4sv = (PFNGLCOLOR4SVPROC)wglGetProcAddress("glColor4sv");
  // glColor4ub = (PFNGLCOLOR4UBPROC)wglGetProcAddress("glColor4ub");
  // glColor4ubv = (PFNGLCOLOR4UBVPROC)wglGetProcAddress("glColor4ubv");
  // glColor4ui = (PFNGLCOLOR4UIPROC)wglGetProcAddress("glColor4ui");
  // glColor4uiv = (PFNGLCOLOR4UIVPROC)wglGetProcAddress("glColor4uiv");
  // glColor4us = (PFNGLCOLOR4USPROC)wglGetProcAddress("glColor4us");
  // glColor4usv = (PFNGLCOLOR4USVPROC)wglGetProcAddress("glColor4usv");
  // glEdgeFlag = (PFNGLEDGEFLAGPROC)wglGetProcAddress("glEdgeFlag");
  // glEdgeFlagv = (PFNGLEDGEFLAGVPROC)wglGetProcAddress("glEdgeFlagv");
  // glEnd = (PFNGLENDPROC)wglGetProcAddress("glEnd");
  // glIndexd = (PFNGLINDEXDPROC)wglGetProcAddress("glIndexd");
  // glIndexdv = (PFNGLINDEXDVPROC)wglGetProcAddress("glIndexdv");
  // glIndexf = (PFNGLINDEXFPROC)wglGetProcAddress("glIndexf");
  // glIndexfv = (PFNGLINDEXFVPROC)wglGetProcAddress("glIndexfv");
  // glIndexi = (PFNGLINDEXIPROC)wglGetProcAddress("glIndexi");
  // glIndexiv = (PFNGLINDEXIVPROC)wglGetProcAddress("glIndexiv");
  // glIndexs = (PFNGLINDEXSPROC)wglGetProcAddress("glIndexs");
  // glIndexsv = (PFNGLINDEXSVPROC)wglGetProcAddress("glIndexsv");
  // glNormal3b = (PFNGLNORMAL3BPROC)wglGetProcAddress("glNormal3b");
  // glNormal3bv = (PFNGLNORMAL3BVPROC)wglGetProcAddress("glNormal3bv");
  // glNormal3d = (PFNGLNORMAL3DPROC)wglGetProcAddress("glNormal3d");
  // glNormal3dv = (PFNGLNORMAL3DVPROC)wglGetProcAddress("glNormal3dv");
  // glNormal3f = (PFNGLNORMAL3FPROC)wglGetProcAddress("glNormal3f");
  // glNormal3fv = (PFNGLNORMAL3FVPROC)wglGetProcAddress("glNormal3fv");
  // glNormal3i = (PFNGLNORMAL3IPROC)wglGetProcAddress("glNormal3i");
  // glNormal3iv = (PFNGLNORMAL3IVPROC)wglGetProcAddress("glNormal3iv");
  // glNormal3s = (PFNGLNORMAL3SPROC)wglGetProcAddress("glNormal3s");
  // glNormal3sv = (PFNGLNORMAL3SVPROC)wglGetProcAddress("glNormal3sv");
  // glRasterPos2d = (PFNGLRASTERPOS2DPROC)wglGetProcAddress("glRasterPos2d");
  // glRasterPos2dv = (PFNGLRASTERPOS2DVPROC)wglGetProcAddress("glRasterPos2dv");
  // glRasterPos2f = (PFNGLRASTERPOS2FPROC)wglGetProcAddress("glRasterPos2f");
  // glRasterPos2fv = (PFNGLRASTERPOS2FVPROC)wglGetProcAddress("glRasterPos2fv");
  // glRasterPos2i = (PFNGLRASTERPOS2IPROC)wglGetProcAddress("glRasterPos2i");
  // glRasterPos2iv = (PFNGLRASTERPOS2IVPROC)wglGetProcAddress("glRasterPos2iv");
  // glRasterPos2s = (PFNGLRASTERPOS2SPROC)wglGetProcAddress("glRasterPos2s");
  // glRasterPos2sv = (PFNGLRASTERPOS2SVPROC)wglGetProcAddress("glRasterPos2sv");
  // glRasterPos3d = (PFNGLRASTERPOS3DPROC)wglGetProcAddress("glRasterPos3d");
  // glRasterPos3dv = (PFNGLRASTERPOS3DVPROC)wglGetProcAddress("glRasterPos3dv");
  // glRasterPos3f = (PFNGLRASTERPOS3FPROC)wglGetProcAddress("glRasterPos3f");
  // glRasterPos3fv = (PFNGLRASTERPOS3FVPROC)wglGetProcAddress("glRasterPos3fv");
  // glRasterPos3i = (PFNGLRASTERPOS3IPROC)wglGetProcAddress("glRasterPos3i");
  // glRasterPos3iv = (PFNGLRASTERPOS3IVPROC)wglGetProcAddress("glRasterPos3iv");
  // glRasterPos3s = (PFNGLRASTERPOS3SPROC)wglGetProcAddress("glRasterPos3s");
  // glRasterPos3sv = (PFNGLRASTERPOS3SVPROC)wglGetProcAddress("glRasterPos3sv");
  // glRasterPos4d = (PFNGLRASTERPOS4DPROC)wglGetProcAddress("glRasterPos4d");
  // glRasterPos4dv = (PFNGLRASTERPOS4DVPROC)wglGetProcAddress("glRasterPos4dv");
  // glRasterPos4f = (PFNGLRASTERPOS4FPROC)wglGetProcAddress("glRasterPos4f");
  // glRasterPos4fv = (PFNGLRASTERPOS4FVPROC)wglGetProcAddress("glRasterPos4fv");
  // glRasterPos4i = (PFNGLRASTERPOS4IPROC)wglGetProcAddress("glRasterPos4i");
  // glRasterPos4iv = (PFNGLRASTERPOS4IVPROC)wglGetProcAddress("glRasterPos4iv");
  // glRasterPos4s = (PFNGLRASTERPOS4SPROC)wglGetProcAddress("glRasterPos4s");
  // glRasterPos4sv = (PFNGLRASTERPOS4SVPROC)wglGetProcAddress("glRasterPos4sv");
  // glRectd = (PFNGLRECTDPROC)wglGetProcAddress("glRectd");
  // glRectdv = (PFNGLRECTDVPROC)wglGetProcAddress("glRectdv");
  // glRectf = (PFNGLRECTFPROC)wglGetProcAddress("glRectf");
  // glRectfv = (PFNGLRECTFVPROC)wglGetProcAddress("glRectfv");
  // glRecti = (PFNGLRECTIPROC)wglGetProcAddress("glRecti");
  // glRectiv = (PFNGLRECTIVPROC)wglGetProcAddress("glRectiv");
  // glRects = (PFNGLRECTSPROC)wglGetProcAddress("glRects");
  // glRectsv = (PFNGLRECTSVPROC)wglGetProcAddress("glRectsv");
  // glTexCoord1d = (PFNGLTEXCOORD1DPROC)wglGetProcAddress("glTexCoord1d");
  // glTexCoord1dv = (PFNGLTEXCOORD1DVPROC)wglGetProcAddress("glTexCoord1dv");
  // glTexCoord1f = (PFNGLTEXCOORD1FPROC)wglGetProcAddress("glTexCoord1f");
  // glTexCoord1fv = (PFNGLTEXCOORD1FVPROC)wglGetProcAddress("glTexCoord1fv");
  // glTexCoord1i = (PFNGLTEXCOORD1IPROC)wglGetProcAddress("glTexCoord1i");
  // glTexCoord1iv = (PFNGLTEXCOORD1IVPROC)wglGetProcAddress("glTexCoord1iv");
  // glTexCoord1s = (PFNGLTEXCOORD1SPROC)wglGetProcAddress("glTexCoord1s");
  // glTexCoord1sv = (PFNGLTEXCOORD1SVPROC)wglGetProcAddress("glTexCoord1sv");
  // glTexCoord2d = (PFNGLTEXCOORD2DPROC)wglGetProcAddress("glTexCoord2d");
  // glTexCoord2dv = (PFNGLTEXCOORD2DVPROC)wglGetProcAddress("glTexCoord2dv");
  // glTexCoord2f = (PFNGLTEXCOORD2FPROC)wglGetProcAddress("glTexCoord2f");
  // glTexCoord2fv = (PFNGLTEXCOORD2FVPROC)wglGetProcAddress("glTexCoord2fv");
  // glTexCoord2i = (PFNGLTEXCOORD2IPROC)wglGetProcAddress("glTexCoord2i");
  // glTexCoord2iv = (PFNGLTEXCOORD2IVPROC)wglGetProcAddress("glTexCoord2iv");
  // glTexCoord2s = (PFNGLTEXCOORD2SPROC)wglGetProcAddress("glTexCoord2s");
  // glTexCoord2sv = (PFNGLTEXCOORD2SVPROC)wglGetProcAddress("glTexCoord2sv");
  // glTexCoord3d = (PFNGLTEXCOORD3DPROC)wglGetProcAddress("glTexCoord3d");
  // glTexCoord3dv = (PFNGLTEXCOORD3DVPROC)wglGetProcAddress("glTexCoord3dv");
  // glTexCoord3f = (PFNGLTEXCOORD3FPROC)wglGetProcAddress("glTexCoord3f");
  // glTexCoord3fv = (PFNGLTEXCOORD3FVPROC)wglGetProcAddress("glTexCoord3fv");
  // glTexCoord3i = (PFNGLTEXCOORD3IPROC)wglGetProcAddress("glTexCoord3i");
  // glTexCoord3iv = (PFNGLTEXCOORD3IVPROC)wglGetProcAddress("glTexCoord3iv");
  // glTexCoord3s = (PFNGLTEXCOORD3SPROC)wglGetProcAddress("glTexCoord3s");
  // glTexCoord3sv = (PFNGLTEXCOORD3SVPROC)wglGetProcAddress("glTexCoord3sv");
  // glTexCoord4d = (PFNGLTEXCOORD4DPROC)wglGetProcAddress("glTexCoord4d");
  // glTexCoord4dv = (PFNGLTEXCOORD4DVPROC)wglGetProcAddress("glTexCoord4dv");
  // glTexCoord4f = (PFNGLTEXCOORD4FPROC)wglGetProcAddress("glTexCoord4f");
  // glTexCoord4fv = (PFNGLTEXCOORD4FVPROC)wglGetProcAddress("glTexCoord4fv");
  // glTexCoord4i = (PFNGLTEXCOORD4IPROC)wglGetProcAddress("glTexCoord4i");
  // glTexCoord4iv = (PFNGLTEXCOORD4IVPROC)wglGetProcAddress("glTexCoord4iv");
  // glTexCoord4s = (PFNGLTEXCOORD4SPROC)wglGetProcAddress("glTexCoord4s");
  // glTexCoord4sv = (PFNGLTEXCOORD4SVPROC)wglGetProcAddress("glTexCoord4sv");
  // glVertex2d = (PFNGLVERTEX2DPROC)wglGetProcAddress("glVertex2d");
  // glVertex2dv = (PFNGLVERTEX2DVPROC)wglGetProcAddress("glVertex2dv");
  // glVertex2f = (PFNGLVERTEX2FPROC)wglGetProcAddress("glVertex2f");
  // glVertex2fv = (PFNGLVERTEX2FVPROC)wglGetProcAddress("glVertex2fv");
  // glVertex2i = (PFNGLVERTEX2IPROC)wglGetProcAddress("glVertex2i");
  // glVertex2iv = (PFNGLVERTEX2IVPROC)wglGetProcAddress("glVertex2iv");
  // glVertex2s = (PFNGLVERTEX2SPROC)wglGetProcAddress("glVertex2s");
  // glVertex2sv = (PFNGLVERTEX2SVPROC)wglGetProcAddress("glVertex2sv");
  // glVertex3d = (PFNGLVERTEX3DPROC)wglGetProcAddress("glVertex3d");
  // glVertex3dv = (PFNGLVERTEX3DVPROC)wglGetProcAddress("glVertex3dv");
  // glVertex3f = (PFNGLVERTEX3FPROC)wglGetProcAddress("glVertex3f");
  // glVertex3fv = (PFNGLVERTEX3FVPROC)wglGetProcAddress("glVertex3fv");
  // glVertex3i = (PFNGLVERTEX3IPROC)wglGetProcAddress("glVertex3i");
  // glVertex3iv = (PFNGLVERTEX3IVPROC)wglGetProcAddress("glVertex3iv");
  // glVertex3s = (PFNGLVERTEX3SPROC)wglGetProcAddress("glVertex3s");
  // glVertex3sv = (PFNGLVERTEX3SVPROC)wglGetProcAddress("glVertex3sv");
  // glVertex4d = (PFNGLVERTEX4DPROC)wglGetProcAddress("glVertex4d");
  // glVertex4dv = (PFNGLVERTEX4DVPROC)wglGetProcAddress("glVertex4dv");
  // glVertex4f = (PFNGLVERTEX4FPROC)wglGetProcAddress("glVertex4f");
  // glVertex4fv = (PFNGLVERTEX4FVPROC)wglGetProcAddress("glVertex4fv");
  // glVertex4i = (PFNGLVERTEX4IPROC)wglGetProcAddress("glVertex4i");
  // glVertex4iv = (PFNGLVERTEX4IVPROC)wglGetProcAddress("glVertex4iv");
  // glVertex4s = (PFNGLVERTEX4SPROC)wglGetProcAddress("glVertex4s");
  // glVertex4sv = (PFNGLVERTEX4SVPROC)wglGetProcAddress("glVertex4sv");
  // glClipPlane = (PFNGLCLIPPLANEPROC)wglGetProcAddress("glClipPlane");
  // glColorMaterial = (PFNGLCOLORMATERIALPROC)wglGetProcAddress("glColorMaterial");
  // glFogf = (PFNGLFOGFPROC)wglGetProcAddress("glFogf");
  // glFogfv = (PFNGLFOGFVPROC)wglGetProcAddress("glFogfv");
  // glFogi = (PFNGLFOGIPROC)wglGetProcAddress("glFogi");
  // glFogiv = (PFNGLFOGIVPROC)wglGetProcAddress("glFogiv");
  // glLightf = (PFNGLLIGHTFPROC)wglGetProcAddress("glLightf");
  // glLightfv = (PFNGLLIGHTFVPROC)wglGetProcAddress("glLightfv");
  // glLighti = (PFNGLLIGHTIPROC)wglGetProcAddress("glLighti");
  // glLightiv = (PFNGLLIGHTIVPROC)wglGetProcAddress("glLightiv");
  // glLightModelf = (PFNGLLIGHTMODELFPROC)wglGetProcAddress("glLightModelf");
  // glLightModelfv = (PFNGLLIGHTMODELFVPROC)wglGetProcAddress("glLightModelfv");
  // glLightModeli = (PFNGLLIGHTMODELIPROC)wglGetProcAddress("glLightModeli");
  // glLightModeliv = (PFNGLLIGHTMODELIVPROC)wglGetProcAddress("glLightModeliv");
  // glLineStipple = (PFNGLLINESTIPPLEPROC)wglGetProcAddress("glLineStipple");
  // glMaterialf = (PFNGLMATERIALFPROC)wglGetProcAddress("glMaterialf");
  // glMaterialfv = (PFNGLMATERIALFVPROC)wglGetProcAddress("glMaterialfv");
  // glMateriali = (PFNGLMATERIALIPROC)wglGetProcAddress("glMateriali");
  // glMaterialiv = (PFNGLMATERIALIVPROC)wglGetProcAddress("glMaterialiv");
  // glPolygonStipple = (PFNGLPOLYGONSTIPPLEPROC)wglGetProcAddress("glPolygonStipple");
  // glShadeModel = (PFNGLSHADEMODELPROC)wglGetProcAddress("glShadeModel");
  // glTexEnvf = (PFNGLTEXENVFPROC)wglGetProcAddress("glTexEnvf");
  // glTexEnvfv = (PFNGLTEXENVFVPROC)wglGetProcAddress("glTexEnvfv");
  // glTexEnvi = (PFNGLTEXENVIPROC)wglGetProcAddress("glTexEnvi");
  // glTexEnviv = (PFNGLTEXENVIVPROC)wglGetProcAddress("glTexEnviv");
  // glTexGend = (PFNGLTEXGENDPROC)wglGetProcAddress("glTexGend");
  // glTexGendv = (PFNGLTEXGENDVPROC)wglGetProcAddress("glTexGendv");
  // glTexGenf = (PFNGLTEXGENFPROC)wglGetProcAddress("glTexGenf");
  // glTexGenfv = (PFNGLTEXGENFVPROC)wglGetProcAddress("glTexGenfv");
  // glTexGeni = (PFNGLTEXGENIPROC)wglGetProcAddress("glTexGeni");
  // glTexGeniv = (PFNGLTEXGENIVPROC)wglGetProcAddress("glTexGeniv");
  // glFeedbackBuffer = (PFNGLFEEDBACKBUFFERPROC)wglGetProcAddress("glFeedbackBuffer");
  // glSelectBuffer = (PFNGLSELECTBUFFERPROC)wglGetProcAddress("glSelectBuffer");
  // glRenderMode = (PFNGLRENDERMODEPROC)wglGetProcAddress("glRenderMode");
  // glInitNames = (PFNGLINITNAMESPROC)wglGetProcAddress("glInitNames");
  // glLoadName = (PFNGLLOADNAMEPROC)wglGetProcAddress("glLoadName");
  // glPassThrough = (PFNGLPASSTHROUGHPROC)wglGetProcAddress("glPassThrough");
  // glPopName = (PFNGLPOPNAMEPROC)wglGetProcAddress("glPopName");
  // glPushName = (PFNGLPUSHNAMEPROC)wglGetProcAddress("glPushName");
  // glClearAccum = (PFNGLCLEARACCUMPROC)wglGetProcAddress("glClearAccum");
  // glClearIndex = (PFNGLCLEARINDEXPROC)wglGetProcAddress("glClearIndex");
  // glIndexMask = (PFNGLINDEXMASKPROC)wglGetProcAddress("glIndexMask");
  // glAccum = (PFNGLACCUMPROC)wglGetProcAddress("glAccum");
  // glPopAttrib = (PFNGLPOPATTRIBPROC)wglGetProcAddress("glPopAttrib");
  // glPushAttrib = (PFNGLPUSHATTRIBPROC)wglGetProcAddress("glPushAttrib");
  // glMap1d = (PFNGLMAP1DPROC)wglGetProcAddress("glMap1d");
  // glMap1f = (PFNGLMAP1FPROC)wglGetProcAddress("glMap1f");
  // glMap2d = (PFNGLMAP2DPROC)wglGetProcAddress("glMap2d");
  // glMap2f = (PFNGLMAP2FPROC)wglGetProcAddress("glMap2f");
  // glMapGrid1d = (PFNGLMAPGRID1DPROC)wglGetProcAddress("glMapGrid1d");
  // glMapGrid1f = (PFNGLMAPGRID1FPROC)wglGetProcAddress("glMapGrid1f");
  // glMapGrid2d = (PFNGLMAPGRID2DPROC)wglGetProcAddress("glMapGrid2d");
  // glMapGrid2f = (PFNGLMAPGRID2FPROC)wglGetProcAddress("glMapGrid2f");
  // glEvalCoord1d = (PFNGLEVALCOORD1DPROC)wglGetProcAddress("glEvalCoord1d");
  // glEvalCoord1dv = (PFNGLEVALCOORD1DVPROC)wglGetProcAddress("glEvalCoord1dv");
  // glEvalCoord1f = (PFNGLEVALCOORD1FPROC)wglGetProcAddress("glEvalCoord1f");
  // glEvalCoord1fv = (PFNGLEVALCOORD1FVPROC)wglGetProcAddress("glEvalCoord1fv");
  // glEvalCoord2d = (PFNGLEVALCOORD2DPROC)wglGetProcAddress("glEvalCoord2d");
  // glEvalCoord2dv = (PFNGLEVALCOORD2DVPROC)wglGetProcAddress("glEvalCoord2dv");
  // glEvalCoord2f = (PFNGLEVALCOORD2FPROC)wglGetProcAddress("glEvalCoord2f");
  // glEvalCoord2fv = (PFNGLEVALCOORD2FVPROC)wglGetProcAddress("glEvalCoord2fv");
  // glEvalMesh1 = (PFNGLEVALMESH1PROC)wglGetProcAddress("glEvalMesh1");
  // glEvalPoint1 = (PFNGLEVALPOINT1PROC)wglGetProcAddress("glEvalPoint1");
  // glEvalMesh2 = (PFNGLEVALMESH2PROC)wglGetProcAddress("glEvalMesh2");
  // glEvalPoint2 = (PFNGLEVALPOINT2PROC)wglGetProcAddress("glEvalPoint2");
  // glAlphaFunc = (PFNGLALPHAFUNCPROC)wglGetProcAddress("glAlphaFunc");
  // glPixelZoom = (PFNGLPIXELZOOMPROC)wglGetProcAddress("glPixelZoom");
  // glPixelTransferf = (PFNGLPIXELTRANSFERFPROC)wglGetProcAddress("glPixelTransferf");
  // glPixelTransferi = (PFNGLPIXELTRANSFERIPROC)wglGetProcAddress("glPixelTransferi");
  // glPixelMapfv = (PFNGLPIXELMAPFVPROC)wglGetProcAddress("glPixelMapfv");
  // glPixelMapuiv = (PFNGLPIXELMAPUIVPROC)wglGetProcAddress("glPixelMapuiv");
  // glPixelMapusv = (PFNGLPIXELMAPUSVPROC)wglGetProcAddress("glPixelMapusv");
  // glCopyPixels = (PFNGLCOPYPIXELSPROC)wglGetProcAddress("glCopyPixels");
  // glDrawPixels = (PFNGLDRAWPIXELSPROC)wglGetProcAddress("glDrawPixels");
  // glGetClipPlane = (PFNGLGETCLIPPLANEPROC)wglGetProcAddress("glGetClipPlane");
  // glGetLightfv = (PFNGLGETLIGHTFVPROC)wglGetProcAddress("glGetLightfv");
  // glGetLightiv = (PFNGLGETLIGHTIVPROC)wglGetProcAddress("glGetLightiv");
  // glGetMapdv = (PFNGLGETMAPDVPROC)wglGetProcAddress("glGetMapdv");
  // glGetMapfv = (PFNGLGETMAPFVPROC)wglGetProcAddress("glGetMapfv");
  // glGetMapiv = (PFNGLGETMAPIVPROC)wglGetProcAddress("glGetMapiv");
  // glGetMaterialfv = (PFNGLGETMATERIALFVPROC)wglGetProcAddress("glGetMaterialfv");
  // glGetMaterialiv = (PFNGLGETMATERIALIVPROC)wglGetProcAddress("glGetMaterialiv");
  // glGetPixelMapfv = (PFNGLGETPIXELMAPFVPROC)wglGetProcAddress("glGetPixelMapfv");
  // glGetPixelMapuiv = (PFNGLGETPIXELMAPUIVPROC)wglGetProcAddress("glGetPixelMapuiv");
  // glGetPixelMapusv = (PFNGLGETPIXELMAPUSVPROC)wglGetProcAddress("glGetPixelMapusv");
  // glGetPolygonStipple = (PFNGLGETPOLYGONSTIPPLEPROC)wglGetProcAddress("glGetPolygonStipple");
  // glGetTexEnvfv = (PFNGLGETTEXENVFVPROC)wglGetProcAddress("glGetTexEnvfv");
  // glGetTexEnviv = (PFNGLGETTEXENVIVPROC)wglGetProcAddress("glGetTexEnviv");
  // glGetTexGendv = (PFNGLGETTEXGENDVPROC)wglGetProcAddress("glGetTexGendv");
  // glGetTexGenfv = (PFNGLGETTEXGENFVPROC)wglGetProcAddress("glGetTexGenfv");
  // glGetTexGeniv = (PFNGLGETTEXGENIVPROC)wglGetProcAddress("glGetTexGeniv");
  // glIsList = (PFNGLISLISTPROC)wglGetProcAddress("glIsList");
  // glFrustum = (PFNGLFRUSTUMPROC)wglGetProcAddress("glFrustum");
  // glLoadIdentity = (PFNGLLOADIDENTITYPROC)wglGetProcAddress("glLoadIdentity");
  // glLoadMatrixf = (PFNGLLOADMATRIXFPROC)wglGetProcAddress("glLoadMatrixf");
  // glLoadMatrixd = (PFNGLLOADMATRIXDPROC)wglGetProcAddress("glLoadMatrixd");
  // glMatrixMode = (PFNGLMATRIXMODEPROC)wglGetProcAddress("glMatrixMode");
  // glMultMatrixf = (PFNGLMULTMATRIXFPROC)wglGetProcAddress("glMultMatrixf");
  // glMultMatrixd = (PFNGLMULTMATRIXDPROC)wglGetProcAddress("glMultMatrixd");
  // glOrtho = (PFNGLORTHOPROC)wglGetProcAddress("glOrtho");
  // glPopMatrix = (PFNGLPOPMATRIXPROC)wglGetProcAddress("glPopMatrix");
  // glPushMatrix = (PFNGLPUSHMATRIXPROC)wglGetProcAddress("glPushMatrix");
  // glRotated = (PFNGLROTATEDPROC)wglGetProcAddress("glRotated");
  // glRotatef = (PFNGLROTATEFPROC)wglGetProcAddress("glRotatef");
  // glScaled = (PFNGLSCALEDPROC)wglGetProcAddress("glScaled");
  // glScalef = (PFNGLSCALEFPROC)wglGetProcAddress("glScalef");
  // glTranslated = (PFNGLTRANSLATEDPROC)wglGetProcAddress("glTranslated");
  // glTranslatef = (PFNGLTRANSLATEFPROC)wglGetProcAddress("glTranslatef");

  p_log("...1.1 "); // should be already included in Windows
  // opengl 1.1
  // glDrawArrays = (PFNGLDRAWARRAYSPROC)wglGetProcAddress("glDrawArrays");
  // glDrawElements = (PFNGLDRAWELEMENTSPROC)wglGetProcAddress("glDrawElements");
  // glGetPointerv = (PFNGLGETPOINTERVPROC)wglGetProcAddress("glGetPointerv");
  // glPolygonOffset = (PFNGLPOLYGONOFFSETPROC)wglGetProcAddress("glPolygonOffset");
  // glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC)wglGetProcAddress("glCopyTexImage1D");
  // glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC)wglGetProcAddress("glCopyTexImage2D");
  // glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC)wglGetProcAddress("glCopyTexSubImage1D");
  // glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC)wglGetProcAddress("glCopyTexSubImage2D");
  // glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC)wglGetProcAddress("glTexSubImage1D");
  // glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)wglGetProcAddress("glTexSubImage2D");
  // glBindTexture = (PFNGLBINDTEXTUREPROC)wglGetProcAddress("glBindTexture");
  // glDeleteTextures = (PFNGLDELETETEXTURESPROC)wglGetProcAddress("glDeleteTextures");
  // glGenTextures = (PFNGLGENTEXTURESPROC)wglGetProcAddress("glGenTextures");
  // glIsTexture = (PFNGLISTEXTUREPROC)wglGetProcAddress("glIsTexture");
  // glArrayElement = (PFNGLARRAYELEMENTPROC)wglGetProcAddress("glArrayElement");
  // glColorPointer = (PFNGLCOLORPOINTERPROC)wglGetProcAddress("glColorPointer");
  // glDisableClientState = (PFNGLDISABLECLIENTSTATEPROC)wglGetProcAddress("glDisableClientState");
  // glEdgeFlagPointer = (PFNGLEDGEFLAGPOINTERPROC)wglGetProcAddress("glEdgeFlagPointer");
  // glEnableClientState = (PFNGLENABLECLIENTSTATEPROC)wglGetProcAddress("glEnableClientState");
  // glIndexPointer = (PFNGLINDEXPOINTERPROC)wglGetProcAddress("glIndexPointer");
  // glInterleavedArrays = (PFNGLINTERLEAVEDARRAYSPROC)wglGetProcAddress("glInterleavedArrays");
  // glNormalPointer = (PFNGLNORMALPOINTERPROC)wglGetProcAddress("glNormalPointer");
  // glTexCoordPointer = (PFNGLTEXCOORDPOINTERPROC)wglGetProcAddress("glTexCoordPointer");
  // glVertexPointer = (PFNGLVERTEXPOINTERPROC)wglGetProcAddress("glVertexPointer");
  // glAreTexturesResident = (PFNGLARETEXTURESRESIDENTPROC)wglGetProcAddress("glAreTexturesResident");
  // glPrioritizeTextures = (PFNGLPRIORITIZETEXTURESPROC)wglGetProcAddress("glPrioritizeTextures");
  // glIndexub = (PFNGLINDEXUBPROC)wglGetProcAddress("glIndexub");
  // glIndexubv = (PFNGLINDEXUBVPROC)wglGetProcAddress("glIndexubv");
  // glPopClientAttrib = (PFNGLPOPCLIENTATTRIBPROC)wglGetProcAddress("glPopClientAttrib");
  // glPushClientAttrib = (PFNGLPUSHCLIENTATTRIBPROC)wglGetProcAddress("glPushClientAttrib");

  p_log("...1.2 ");
  // opengl 1.2
  glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)wglGetProcAddress("glDrawRangeElements");
  glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
  glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)wglGetProcAddress("glTexSubImage3D");
  glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)wglGetProcAddress("glCopyTexSubImage3D");

  p_log("...1.3 ");
  // opengl 1.3
  glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
  glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)wglGetProcAddress("glSampleCoverage");
  glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)wglGetProcAddress("glCompressedTexImage3D");
  glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress("glCompressedTexImage2D");
  glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)wglGetProcAddress("glCompressedTexImage1D");
  glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)wglGetProcAddress("glCompressedTexSubImage3D");
  glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)wglGetProcAddress("glCompressedTexSubImage2D");
  glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)wglGetProcAddress("glCompressedTexSubImage1D");
  glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)wglGetProcAddress("glGetCompressedTexImage");
  glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress("glClientActiveTexture");
  glMultiTexCoord1d = (PFNGLMULTITEXCOORD1DPROC)wglGetProcAddress("glMultiTexCoord1d");
  glMultiTexCoord1dv = (PFNGLMULTITEXCOORD1DVPROC)wglGetProcAddress("glMultiTexCoord1dv");
  glMultiTexCoord1f = (PFNGLMULTITEXCOORD1FPROC)wglGetProcAddress("glMultiTexCoord1f");
  glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVPROC)wglGetProcAddress("glMultiTexCoord1fv");
  glMultiTexCoord1i = (PFNGLMULTITEXCOORD1IPROC)wglGetProcAddress("glMultiTexCoord1i");
  glMultiTexCoord1iv = (PFNGLMULTITEXCOORD1IVPROC)wglGetProcAddress("glMultiTexCoord1iv");
  glMultiTexCoord1s = (PFNGLMULTITEXCOORD1SPROC)wglGetProcAddress("glMultiTexCoord1s");
  glMultiTexCoord1sv = (PFNGLMULTITEXCOORD1SVPROC)wglGetProcAddress("glMultiTexCoord1sv");
  glMultiTexCoord2d = (PFNGLMULTITEXCOORD2DPROC)wglGetProcAddress("glMultiTexCoord2d");
  glMultiTexCoord2dv = (PFNGLMULTITEXCOORD2DVPROC)wglGetProcAddress("glMultiTexCoord2dv");
  glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC)wglGetProcAddress("glMultiTexCoord2f");
  glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC)wglGetProcAddress("glMultiTexCoord2fv");
  glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC)wglGetProcAddress("glMultiTexCoord2i");
  glMultiTexCoord2iv = (PFNGLMULTITEXCOORD2IVPROC)wglGetProcAddress("glMultiTexCoord2iv");
  glMultiTexCoord2s = (PFNGLMULTITEXCOORD2SPROC)wglGetProcAddress("glMultiTexCoord2s");
  glMultiTexCoord2sv = (PFNGLMULTITEXCOORD2SVPROC)wglGetProcAddress("glMultiTexCoord2sv");
  glMultiTexCoord3d = (PFNGLMULTITEXCOORD3DPROC)wglGetProcAddress("glMultiTexCoord3d");
  glMultiTexCoord3dv = (PFNGLMULTITEXCOORD3DVPROC)wglGetProcAddress("glMultiTexCoord3dv");
  glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC)wglGetProcAddress("glMultiTexCoord3f");
  glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVPROC)wglGetProcAddress("glMultiTexCoord3fv");
  glMultiTexCoord3i = (PFNGLMULTITEXCOORD3IPROC)wglGetProcAddress("glMultiTexCoord3i");
  glMultiTexCoord3iv = (PFNGLMULTITEXCOORD3IVPROC)wglGetProcAddress("glMultiTexCoord3iv");
  glMultiTexCoord3s = (PFNGLMULTITEXCOORD3SPROC)wglGetProcAddress("glMultiTexCoord3s");
  glMultiTexCoord3sv = (PFNGLMULTITEXCOORD3SVPROC)wglGetProcAddress("glMultiTexCoord3sv");
  glMultiTexCoord4d = (PFNGLMULTITEXCOORD4DPROC)wglGetProcAddress("glMultiTexCoord4d");
  glMultiTexCoord4dv = (PFNGLMULTITEXCOORD4DVPROC)wglGetProcAddress("glMultiTexCoord4dv");
  glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC)wglGetProcAddress("glMultiTexCoord4f");
  glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVPROC)wglGetProcAddress("glMultiTexCoord4fv");
  glMultiTexCoord4i = (PFNGLMULTITEXCOORD4IPROC)wglGetProcAddress("glMultiTexCoord4i");
  glMultiTexCoord4iv = (PFNGLMULTITEXCOORD4IVPROC)wglGetProcAddress("glMultiTexCoord4iv");
  glMultiTexCoord4s = (PFNGLMULTITEXCOORD4SPROC)wglGetProcAddress("glMultiTexCoord4s");
  glMultiTexCoord4sv = (PFNGLMULTITEXCOORD4SVPROC)wglGetProcAddress("glMultiTexCoord4sv");
  glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC)wglGetProcAddress("glLoadTransposeMatrixf");
  glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC)wglGetProcAddress("glLoadTransposeMatrixd");
  glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC)wglGetProcAddress("glMultTransposeMatrixf");
  glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC)wglGetProcAddress("glMultTransposeMatrixd");

  p_log("...1.4 ");
  // opengl 1.4
  glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)wglGetProcAddress("glBlendFuncSeparate");
  glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)wglGetProcAddress("glMultiDrawArrays");
  glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)wglGetProcAddress("glMultiDrawElements");
  glPointParameterf = (PFNGLPOINTPARAMETERFPROC)wglGetProcAddress("glPointParameterf");
  glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)wglGetProcAddress("glPointParameterfv");
  glPointParameteri = (PFNGLPOINTPARAMETERIPROC)wglGetProcAddress("glPointParameteri");
  glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)wglGetProcAddress("glPointParameteriv");
  glFogCoordf = (PFNGLFOGCOORDFPROC)wglGetProcAddress("glFogCoordf");
  glFogCoordfv = (PFNGLFOGCOORDFVPROC)wglGetProcAddress("glFogCoordfv");
  glFogCoordd = (PFNGLFOGCOORDDPROC)wglGetProcAddress("glFogCoordd");
  glFogCoorddv = (PFNGLFOGCOORDDVPROC)wglGetProcAddress("glFogCoorddv");
  glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC)wglGetProcAddress("glFogCoordPointer");
  glSecondaryColor3b = (PFNGLSECONDARYCOLOR3BPROC)wglGetProcAddress("glSecondaryColor3b");
  glSecondaryColor3bv = (PFNGLSECONDARYCOLOR3BVPROC)wglGetProcAddress("glSecondaryColor3bv");
  glSecondaryColor3d = (PFNGLSECONDARYCOLOR3DPROC)wglGetProcAddress("glSecondaryColor3d");
  glSecondaryColor3dv = (PFNGLSECONDARYCOLOR3DVPROC)wglGetProcAddress("glSecondaryColor3dv");
  glSecondaryColor3f = (PFNGLSECONDARYCOLOR3FPROC)wglGetProcAddress("glSecondaryColor3f");
  glSecondaryColor3fv = (PFNGLSECONDARYCOLOR3FVPROC)wglGetProcAddress("glSecondaryColor3fv");
  glSecondaryColor3i = (PFNGLSECONDARYCOLOR3IPROC)wglGetProcAddress("glSecondaryColor3i");
  glSecondaryColor3iv = (PFNGLSECONDARYCOLOR3IVPROC)wglGetProcAddress("glSecondaryColor3iv");
  glSecondaryColor3s = (PFNGLSECONDARYCOLOR3SPROC)wglGetProcAddress("glSecondaryColor3s");
  glSecondaryColor3sv = (PFNGLSECONDARYCOLOR3SVPROC)wglGetProcAddress("glSecondaryColor3sv");
  glSecondaryColor3ub = (PFNGLSECONDARYCOLOR3UBPROC)wglGetProcAddress("glSecondaryColor3ub");
  glSecondaryColor3ubv = (PFNGLSECONDARYCOLOR3UBVPROC)wglGetProcAddress("glSecondaryColor3ubv");
  glSecondaryColor3ui = (PFNGLSECONDARYCOLOR3UIPROC)wglGetProcAddress("glSecondaryColor3ui");
  glSecondaryColor3uiv = (PFNGLSECONDARYCOLOR3UIVPROC)wglGetProcAddress("glSecondaryColor3uiv");
  glSecondaryColor3us = (PFNGLSECONDARYCOLOR3USPROC)wglGetProcAddress("glSecondaryColor3us");
  glSecondaryColor3usv = (PFNGLSECONDARYCOLOR3USVPROC)wglGetProcAddress("glSecondaryColor3usv");
  glSecondaryColorPointer = (PFNGLSECONDARYCOLORPOINTERPROC)wglGetProcAddress("glSecondaryColorPointer");
  glWindowPos2d = (PFNGLWINDOWPOS2DPROC)wglGetProcAddress("glWindowPos2d");
  glWindowPos2dv = (PFNGLWINDOWPOS2DVPROC)wglGetProcAddress("glWindowPos2dv");
  glWindowPos2f = (PFNGLWINDOWPOS2FPROC)wglGetProcAddress("glWindowPos2f");
  glWindowPos2fv = (PFNGLWINDOWPOS2FVPROC)wglGetProcAddress("glWindowPos2fv");
  glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");
  glWindowPos2iv = (PFNGLWINDOWPOS2IVPROC)wglGetProcAddress("glWindowPos2iv");
  glWindowPos2s = (PFNGLWINDOWPOS2SPROC)wglGetProcAddress("glWindowPos2s");
  glWindowPos2sv = (PFNGLWINDOWPOS2SVPROC)wglGetProcAddress("glWindowPos2sv");
  glWindowPos3d = (PFNGLWINDOWPOS3DPROC)wglGetProcAddress("glWindowPos3d");
  glWindowPos3dv = (PFNGLWINDOWPOS3DVPROC)wglGetProcAddress("glWindowPos3dv");
  glWindowPos3f = (PFNGLWINDOWPOS3FPROC)wglGetProcAddress("glWindowPos3f");
  glWindowPos3fv = (PFNGLWINDOWPOS3FVPROC)wglGetProcAddress("glWindowPos3fv");
  glWindowPos3i = (PFNGLWINDOWPOS3IPROC)wglGetProcAddress("glWindowPos3i");
  glWindowPos3iv = (PFNGLWINDOWPOS3IVPROC)wglGetProcAddress("glWindowPos3iv");
  glWindowPos3s = (PFNGLWINDOWPOS3SPROC)wglGetProcAddress("glWindowPos3s");
  glWindowPos3sv = (PFNGLWINDOWPOS3SVPROC)wglGetProcAddress("glWindowPos3sv");
  glBlendColor = (PFNGLBLENDCOLORPROC)wglGetProcAddress("glBlendColor");
  glBlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation");

  p_log("...1.5 ");
  // opengl 1.5
  glGenQueries = (PFNGLGENQUERIESPROC)wglGetProcAddress("glGenQueries");
  glDeleteQueries = (PFNGLDELETEQUERIESPROC)wglGetProcAddress("glDeleteQueries");
  glIsQuery = (PFNGLISQUERYPROC)wglGetProcAddress("glIsQuery");
  glBeginQuery = (PFNGLBEGINQUERYPROC)wglGetProcAddress("glBeginQuery");
  glEndQuery = (PFNGLENDQUERYPROC)wglGetProcAddress("glEndQuery");
  glGetQueryiv = (PFNGLGETQUERYIVPROC)wglGetProcAddress("glGetQueryiv");
  glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)wglGetProcAddress("glGetQueryObjectiv");
  glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuiv");
  glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
  glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
  glIsBuffer = (PFNGLISBUFFERPROC)wglGetProcAddress("glIsBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
  glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
  glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)wglGetProcAddress("glGetBufferSubData");
  glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
  glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
  glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetBufferParameteriv");
  glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)wglGetProcAddress("glGetBufferPointerv");

  p_log("...2.0 ");
  //opengl 2.0
  glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)wglGetProcAddress("glBlendEquationSeparate");
  glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
  glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)wglGetProcAddress("glStencilOpSeparate");
  glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)wglGetProcAddress("glStencilFuncSeparate");
  glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)wglGetProcAddress("glStencilMaskSeparate");
  glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
  glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
  glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
  glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
  glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
  glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
  glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
  glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress("glGetActiveAttrib");
  glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");
  glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)wglGetProcAddress("glGetAttachedShaders");
  glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
  glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
  glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)wglGetProcAddress("glGetShaderSource");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
  glGetUniformfv = (PFNGLGETUNIFORMFVPROC)wglGetProcAddress("glGetUniformfv");
  glGetUniformiv = (PFNGLGETUNIFORMIVPROC)wglGetProcAddress("glGetUniformiv");
  glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)wglGetProcAddress("glGetVertexAttribdv");
  glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)wglGetProcAddress("glGetVertexAttribfv");
  glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)wglGetProcAddress("glGetVertexAttribiv");
  glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)wglGetProcAddress("glGetVertexAttribPointerv");
  glIsProgram = (PFNGLISPROGRAMPROC)wglGetProcAddress("glIsProgram");
  glIsShader = (PFNGLISSHADERPROC)wglGetProcAddress("glIsShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
  glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
  glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
  glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
  glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
  glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
  glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
  glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
  glUniform2i = (PFNGLUNIFORM2IPROC)wglGetProcAddress("glUniform2i");
  glUniform3i = (PFNGLUNIFORM3IPROC)wglGetProcAddress("glUniform3i");
  glUniform4i = (PFNGLUNIFORM4IPROC)wglGetProcAddress("glUniform4i");
  glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
  glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
  glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
  glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
  glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
  glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
  glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
  glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
  glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)wglGetProcAddress("glUniformMatrix2fv");
  glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");
  glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
  glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
  glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)wglGetProcAddress("glVertexAttrib1d");
  glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)wglGetProcAddress("glVertexAttrib1dv");
  glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
  glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
  glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)wglGetProcAddress("glVertexAttrib1s");
  glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)wglGetProcAddress("glVertexAttrib1sv");
  glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)wglGetProcAddress("glVertexAttrib2d");
  glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)wglGetProcAddress("glVertexAttrib2dv");
  glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)wglGetProcAddress("glVertexAttrib2f");
  glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
  glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)wglGetProcAddress("glVertexAttrib2s");
  glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)wglGetProcAddress("glVertexAttrib2sv");
  glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)wglGetProcAddress("glVertexAttrib3d");
  glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)wglGetProcAddress("glVertexAttrib3dv");
  glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)wglGetProcAddress("glVertexAttrib3f");
  glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
  glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)wglGetProcAddress("glVertexAttrib3s");
  glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)wglGetProcAddress("glVertexAttrib3sv");
  glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)wglGetProcAddress("glVertexAttrib4Nbv");
  glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)wglGetProcAddress("glVertexAttrib4Niv");
  glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)wglGetProcAddress("glVertexAttrib4Nsv");
  glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)wglGetProcAddress("glVertexAttrib4Nub");
  glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)wglGetProcAddress("glVertexAttrib4Nubv");
  glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)wglGetProcAddress("glVertexAttrib4Nuiv");
  glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)wglGetProcAddress("glVertexAttrib4Nusv");
  glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)wglGetProcAddress("glVertexAttrib4bv");
  glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)wglGetProcAddress("glVertexAttrib4d");
  glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)wglGetProcAddress("glVertexAttrib4dv");
  glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)wglGetProcAddress("glVertexAttrib4f");
  glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
  glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)wglGetProcAddress("glVertexAttrib4iv");
  glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)wglGetProcAddress("glVertexAttrib4s");
  glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)wglGetProcAddress("glVertexAttrib4sv");
  glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)wglGetProcAddress("glVertexAttrib4ubv");
  glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)wglGetProcAddress("glVertexAttrib4uiv");
  glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)wglGetProcAddress("glVertexAttrib4usv");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");

  p_log("...2.1 ");
  //opengl 2.1
  glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)wglGetProcAddress("glUniformMatrix2x3fv");
  glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)wglGetProcAddress("glUniformMatrix3x2fv");
  glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)wglGetProcAddress("glUniformMatrix2x4fv");
  glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)wglGetProcAddress("glUniformMatrix4x2fv");
  glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)wglGetProcAddress("glUniformMatrix3x4fv");
  glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)wglGetProcAddress("glUniformMatrix4x3fv");

  p_log("...3.0 ");
  //opengl 3.0
  // glColorMaski = (PFNGLCOLORMASKIPROC)wglGetProcAddress("glColorMaski");
  glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)wglGetProcAddress("glGetBooleani_v");
  glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)wglGetProcAddress("glGetIntegeri_v");
  glEnablei = (PFNGLENABLEIPROC)wglGetProcAddress("glEnablei");
  glDisablei = (PFNGLDISABLEIPROC)wglGetProcAddress("glDisablei");
  glIsEnabledi = (PFNGLISENABLEDIPROC)wglGetProcAddress("glIsEnabledi");
  glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)wglGetProcAddress("glBeginTransformFeedback");
  glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)wglGetProcAddress("glEndTransformFeedback");
  glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)wglGetProcAddress("glBindBufferRange");
  glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)wglGetProcAddress("glBindBufferBase");
  glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)wglGetProcAddress("glTransformFeedbackVaryings");
  glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)wglGetProcAddress("glGetTransformFeedbackVarying");
  glClampColor = (PFNGLCLAMPCOLORPROC)wglGetProcAddress("glClampColor");
  glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)wglGetProcAddress("glBeginConditionalRender");
  glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)wglGetProcAddress("glEndConditionalRender");
  glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");
  glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)wglGetProcAddress("glGetVertexAttribIiv");
  glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)wglGetProcAddress("glGetVertexAttribIuiv");
  glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)wglGetProcAddress("glVertexAttribI1i");
  glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)wglGetProcAddress("glVertexAttribI2i");
  glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)wglGetProcAddress("glVertexAttribI3i");
  glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)wglGetProcAddress("glVertexAttribI4i");
  glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)wglGetProcAddress("glVertexAttribI1ui");
  glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)wglGetProcAddress("glVertexAttribI2ui");
  glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)wglGetProcAddress("glVertexAttribI3ui");
  glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)wglGetProcAddress("glVertexAttribI4ui");
  glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)wglGetProcAddress("glVertexAttribI1iv");
  glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)wglGetProcAddress("glVertexAttribI2iv");
  glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)wglGetProcAddress("glVertexAttribI3iv");
  glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)wglGetProcAddress("glVertexAttribI4iv");
  glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)wglGetProcAddress("glVertexAttribI1uiv");
  glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)wglGetProcAddress("glVertexAttribI2uiv");
  glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)wglGetProcAddress("glVertexAttribI3uiv");
  glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)wglGetProcAddress("glVertexAttribI4uiv");
  glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)wglGetProcAddress("glVertexAttribI4bv");
  glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)wglGetProcAddress("glVertexAttribI4sv");
  glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)wglGetProcAddress("glVertexAttribI4ubv");
  glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)wglGetProcAddress("glVertexAttribI4usv");
  glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)wglGetProcAddress("glGetUniformuiv");
  glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)wglGetProcAddress("glBindFragDataLocation");
  glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)wglGetProcAddress("glGetFragDataLocation");
  glUniform1ui = (PFNGLUNIFORM1UIPROC)wglGetProcAddress("glUniform1ui");
  glUniform2ui = (PFNGLUNIFORM2UIPROC)wglGetProcAddress("glUniform2ui");
  glUniform3ui = (PFNGLUNIFORM3UIPROC)wglGetProcAddress("glUniform3ui");
  glUniform4ui = (PFNGLUNIFORM4UIPROC)wglGetProcAddress("glUniform4ui");
  glUniform1uiv = (PFNGLUNIFORM1UIVPROC)wglGetProcAddress("glUniform1uiv");
  glUniform2uiv = (PFNGLUNIFORM2UIVPROC)wglGetProcAddress("glUniform2uiv");
  glUniform3uiv = (PFNGLUNIFORM3UIVPROC)wglGetProcAddress("glUniform3uiv");
  glUniform4uiv = (PFNGLUNIFORM4UIVPROC)wglGetProcAddress("glUniform4uiv");
  glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)wglGetProcAddress("glTexParameterIiv");
  glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)wglGetProcAddress("glTexParameterIuiv");
  glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)wglGetProcAddress("glGetTexParameterIiv");
  glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)wglGetProcAddress("glGetTexParameterIuiv");
  glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)wglGetProcAddress("glClearBufferiv");
  glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)wglGetProcAddress("glClearBufferuiv");
  glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)wglGetProcAddress("glClearBufferfv");
  glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)wglGetProcAddress("glClearBufferfi");
  glGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");
  glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbuffer");
  glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
  glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
  glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
  glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
  glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
  glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebuffer");
  glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
  glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
  glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
  glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
  glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1D");
  glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
  glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3D");
  glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
  glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
  glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
  glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");
  glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)wglGetProcAddress("glRenderbufferStorageMultisample");
  glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)wglGetProcAddress("glFramebufferTextureLayer");
  glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)wglGetProcAddress("glMapBufferRange");
  glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)wglGetProcAddress("glFlushMappedBufferRange");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
  glIsVertexArray = (PFNGLISVERTEXARRAYPROC)wglGetProcAddress("glIsVertexArray");

  p_log("...3.1 ");
  // opengl 3.1
  glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)wglGetProcAddress("glDrawArraysInstanced");
  glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)wglGetProcAddress("glDrawElementsInstanced");
  glTexBuffer = (PFNGLTEXBUFFERPROC)wglGetProcAddress("glTexBuffer");
  glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)wglGetProcAddress("glPrimitiveRestartIndex");
  glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)wglGetProcAddress("glCopyBufferSubData");
  glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)wglGetProcAddress("glGetUniformIndices");
  glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)wglGetProcAddress("glGetActiveUniformsiv");
  glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)wglGetProcAddress("glGetActiveUniformName");
  glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)wglGetProcAddress("glGetUniformBlockIndex");
  glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)wglGetProcAddress("glGetActiveUniformBlockiv");
  glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)wglGetProcAddress("glGetActiveUniformBlockName");
  glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)wglGetProcAddress("glUniformBlockBinding");
  glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)wglGetProcAddress("glBindBufferRange");
  glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)wglGetProcAddress("glBindBufferBase");
  glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)wglGetProcAddress("glGetIntegeri_v");

  p_log("...3.2 ");
  //opengl 3.2
  glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)wglGetProcAddress("glDrawElementsBaseVertex");
  glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)wglGetProcAddress("glDrawRangeElementsBaseVertex");
  glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)wglGetProcAddress("glDrawElementsInstancedBaseVertex");
  glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)wglGetProcAddress("glMultiDrawElementsBaseVertex");
  glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)wglGetProcAddress("glProvokingVertex");
  glFenceSync = (PFNGLFENCESYNCPROC)wglGetProcAddress("glFenceSync");
  glIsSync = (PFNGLISSYNCPROC)wglGetProcAddress("glIsSync");
  glDeleteSync = (PFNGLDELETESYNCPROC)wglGetProcAddress("glDeleteSync");
  glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)wglGetProcAddress("glClientWaitSync");
  glWaitSync = (PFNGLWAITSYNCPROC)wglGetProcAddress("glWaitSync");
  glGetInteger64v = (PFNGLGETINTEGER64VPROC)wglGetProcAddress("glGetInteger64v");
  glGetSynciv = (PFNGLGETSYNCIVPROC)wglGetProcAddress("glGetSynciv");
  glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)wglGetProcAddress("glGetInteger64i_v");
  glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)wglGetProcAddress("glGetBufferParameteri64v");
  glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)wglGetProcAddress("glFramebufferTexture");
  glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)wglGetProcAddress("glTexImage2DMultisample");
  glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)wglGetProcAddress("glTexImage3DMultisample");
  glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)wglGetProcAddress("glGetMultisamplefv");
  glSampleMaski = (PFNGLSAMPLEMASKIPROC)wglGetProcAddress("glSampleMaski");

  p_log("...3.3 ");
  //opengl 3.3
  glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)wglGetProcAddress("glBindFragDataLocationIndexed");
  glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)wglGetProcAddress("glGetFragDataIndex");
  glGenSamplers = (PFNGLGENSAMPLERSPROC)wglGetProcAddress("glGenSamplers");
  glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)wglGetProcAddress("glDeleteSamplers");
  glIsSampler = (PFNGLISSAMPLERPROC)wglGetProcAddress("glIsSampler");
  glBindSampler = (PFNGLBINDSAMPLERPROC)wglGetProcAddress("glBindSampler");
  glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)wglGetProcAddress("glSamplerParameteri");
  glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)wglGetProcAddress("glSamplerParameteriv");
  glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)wglGetProcAddress("glSamplerParameterf");
  glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)wglGetProcAddress("glSamplerParameterfv");
  glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)wglGetProcAddress("glSamplerParameterIiv");
  glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)wglGetProcAddress("glSamplerParameterIuiv");
  glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)wglGetProcAddress("glGetSamplerParameteriv");
  glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)wglGetProcAddress("glGetSamplerParameterIiv");
  glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)wglGetProcAddress("glGetSamplerParameterfv");
  glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)wglGetProcAddress("glGetSamplerParameterIuiv");
  glQueryCounter = (PFNGLQUERYCOUNTERPROC)wglGetProcAddress("glQueryCounter");
  glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)wglGetProcAddress("glGetQueryObjecti64v");
  glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)wglGetProcAddress("glGetQueryObjectui64v");
  glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)wglGetProcAddress("glVertexAttribDivisor");
  glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)wglGetProcAddress("glVertexAttribP1ui");
  glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)wglGetProcAddress("glVertexAttribP1uiv");
  glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)wglGetProcAddress("glVertexAttribP2ui");
  glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)wglGetProcAddress("glVertexAttribP2uiv");
  glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)wglGetProcAddress("glVertexAttribP3ui");
  glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)wglGetProcAddress("glVertexAttribP3uiv");
  glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)wglGetProcAddress("glVertexAttribP4ui");
  glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)wglGetProcAddress("glVertexAttribP4uiv");
  glVertexP2ui = (PFNGLVERTEXP2UIPROC)wglGetProcAddress("glVertexP2ui");
  glVertexP2uiv = (PFNGLVERTEXP2UIVPROC)wglGetProcAddress("glVertexP2uiv");
  glVertexP3ui = (PFNGLVERTEXP3UIPROC)wglGetProcAddress("glVertexP3ui");
  glVertexP3uiv = (PFNGLVERTEXP3UIVPROC)wglGetProcAddress("glVertexP3uiv");
  glVertexP4ui = (PFNGLVERTEXP4UIPROC)wglGetProcAddress("glVertexP4ui");
  glVertexP4uiv = (PFNGLVERTEXP4UIVPROC)wglGetProcAddress("glVertexP4uiv");
  glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC)wglGetProcAddress("glTexCoordP1ui");
  glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC)wglGetProcAddress("glTexCoordP1uiv");
  glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC)wglGetProcAddress("glTexCoordP2ui");
  glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC)wglGetProcAddress("glTexCoordP2uiv");
  glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC)wglGetProcAddress("glTexCoordP3ui");
  glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC)wglGetProcAddress("glTexCoordP3uiv");
  glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC)wglGetProcAddress("glTexCoordP4ui");
  glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC)wglGetProcAddress("glTexCoordP4uiv");
  glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC)wglGetProcAddress("glMultiTexCoordP1ui");
  glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC)wglGetProcAddress("glMultiTexCoordP1uiv");
  glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC)wglGetProcAddress("glMultiTexCoordP2ui");
  glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC)wglGetProcAddress("glMultiTexCoordP2uiv");
  glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC)wglGetProcAddress("glMultiTexCoordP3ui");
  glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC)wglGetProcAddress("glMultiTexCoordP3uiv");
  glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC)wglGetProcAddress("glMultiTexCoordP4ui");
  glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC)wglGetProcAddress("glMultiTexCoordP4uiv");
  glNormalP3ui = (PFNGLNORMALP3UIPROC)wglGetProcAddress("glNormalP3ui");
  glNormalP3uiv = (PFNGLNORMALP3UIVPROC)wglGetProcAddress("glNormalP3uiv");
  glColorP3ui = (PFNGLCOLORP3UIPROC)wglGetProcAddress("glColorP3ui");
  glColorP3uiv = (PFNGLCOLORP3UIVPROC)wglGetProcAddress("glColorP3uiv");
  glColorP4ui = (PFNGLCOLORP4UIPROC)wglGetProcAddress("glColorP4ui");
  glColorP4uiv = (PFNGLCOLORP4UIVPROC)wglGetProcAddress("glColorP4uiv");
  glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC)wglGetProcAddress("glSecondaryColorP3ui");
  glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC)wglGetProcAddress("glSecondaryColorP3uiv");

  p_log("...4.0 ");
  //opengl 4.0
  glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)wglGetProcAddress("glMinSampleShading");
  glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)wglGetProcAddress("glBlendEquationi");
  glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)wglGetProcAddress("glBlendEquationSeparatei");
  glBlendFunci = (PFNGLBLENDFUNCIPROC)wglGetProcAddress("glBlendFunci");
  glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)wglGetProcAddress("glBlendFuncSeparatei");
  glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)wglGetProcAddress("glDrawArraysIndirect");
  glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)wglGetProcAddress("glDrawElementsIndirect");
  glUniform1d = (PFNGLUNIFORM1DPROC)wglGetProcAddress("glUniform1d");
  glUniform2d = (PFNGLUNIFORM2DPROC)wglGetProcAddress("glUniform2d");
  glUniform3d = (PFNGLUNIFORM3DPROC)wglGetProcAddress("glUniform3d");
  glUniform4d = (PFNGLUNIFORM4DPROC)wglGetProcAddress("glUniform4d");
  glUniform1dv = (PFNGLUNIFORM1DVPROC)wglGetProcAddress("glUniform1dv");
  glUniform2dv = (PFNGLUNIFORM2DVPROC)wglGetProcAddress("glUniform2dv");
  glUniform3dv = (PFNGLUNIFORM3DVPROC)wglGetProcAddress("glUniform3dv");
  glUniform4dv = (PFNGLUNIFORM4DVPROC)wglGetProcAddress("glUniform4dv");
  glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC)wglGetProcAddress("glUniformMatrix2dv");
  glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC)wglGetProcAddress("glUniformMatrix3dv");
  glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC)wglGetProcAddress("glUniformMatrix4dv");
  glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC)wglGetProcAddress("glUniformMatrix2x3dv");
  glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC)wglGetProcAddress("glUniformMatrix2x4dv");
  glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC)wglGetProcAddress("glUniformMatrix3x2dv");
  glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC)wglGetProcAddress("glUniformMatrix3x4dv");
  glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC)wglGetProcAddress("glUniformMatrix4x2dv");
  glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC)wglGetProcAddress("glUniformMatrix4x3dv");
  glGetUniformdv = (PFNGLGETUNIFORMDVPROC)wglGetProcAddress("glGetUniformdv");
  glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)wglGetProcAddress("glGetSubroutineUniformLocation");
  glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC)wglGetProcAddress("glGetSubroutineIndex");
  glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)wglGetProcAddress("glGetActiveSubroutineUniformiv");
  glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)wglGetProcAddress("glGetActiveSubroutineUniformName");
  glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC)wglGetProcAddress("glGetActiveSubroutineName");
  glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC)wglGetProcAddress("glUniformSubroutinesuiv");
  glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC)wglGetProcAddress("glGetUniformSubroutineuiv");
  glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC)wglGetProcAddress("glGetProgramStageiv");
  glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)wglGetProcAddress("glPatchParameteri");
  glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)wglGetProcAddress("glPatchParameterfv");
  glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)wglGetProcAddress("glBindTransformFeedback");
  glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)wglGetProcAddress("glDeleteTransformFeedbacks");
  glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)wglGetProcAddress("glGenTransformFeedbacks");
  glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)wglGetProcAddress("glIsTransformFeedback");
  glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)wglGetProcAddress("glPauseTransformFeedback");
  glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)wglGetProcAddress("glResumeTransformFeedback");
  glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)wglGetProcAddress("glDrawTransformFeedback");
  glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)wglGetProcAddress("glDrawTransformFeedbackStream");
  glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC)wglGetProcAddress("glBeginQueryIndexed");
  glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC)wglGetProcAddress("glEndQueryIndexed");
  glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC)wglGetProcAddress("glGetQueryIndexediv");

  p_log("...4.1 ");
  //opengl 4.1
  glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)wglGetProcAddress("glReleaseShaderCompiler");
  glShaderBinary = (PFNGLSHADERBINARYPROC)wglGetProcAddress("glShaderBinary");
  glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)wglGetProcAddress("glGetShaderPrecisionFormat");
  glDepthRangef = (PFNGLDEPTHRANGEFPROC)wglGetProcAddress("glDepthRangef");
  glClearDepthf = (PFNGLCLEARDEPTHFPROC)wglGetProcAddress("glClearDepthf");
  glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)wglGetProcAddress("glGetProgramBinary");
  glProgramBinary = (PFNGLPROGRAMBINARYPROC)wglGetProcAddress("glProgramBinary");
  glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)wglGetProcAddress("glProgramParameteri");
  glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)wglGetProcAddress("glUseProgramStages");
  glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)wglGetProcAddress("glActiveShaderProgram");
  glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv");
  glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)wglGetProcAddress("glBindProgramPipeline");
  glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)wglGetProcAddress("glDeleteProgramPipelines");
  glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)wglGetProcAddress("glGenProgramPipelines");
  glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)wglGetProcAddress("glIsProgramPipeline");
  glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)wglGetProcAddress("glGetProgramPipelineiv");
  glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)wglGetProcAddress("glProgramParameteri");
  glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)wglGetProcAddress("glProgramUniform1i");
  glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)wglGetProcAddress("glProgramUniform1iv");
  glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)wglGetProcAddress("glProgramUniform1f");
  glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)wglGetProcAddress("glProgramUniform1fv");
  glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)wglGetProcAddress("glProgramUniform1d");
  glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)wglGetProcAddress("glProgramUniform1dv");
  glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)wglGetProcAddress("glProgramUniform1ui");
  glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)wglGetProcAddress("glProgramUniform1uiv");
  glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)wglGetProcAddress("glProgramUniform2i");
  glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)wglGetProcAddress("glProgramUniform2iv");
  glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)wglGetProcAddress("glProgramUniform2f");
  glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)wglGetProcAddress("glProgramUniform2fv");
  glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)wglGetProcAddress("glProgramUniform2d");
  glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)wglGetProcAddress("glProgramUniform2dv");
  glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)wglGetProcAddress("glProgramUniform2ui");
  glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)wglGetProcAddress("glProgramUniform2uiv");
  glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)wglGetProcAddress("glProgramUniform3i");
  glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)wglGetProcAddress("glProgramUniform3iv");
  glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)wglGetProcAddress("glProgramUniform3f");
  glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)wglGetProcAddress("glProgramUniform3fv");
  glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)wglGetProcAddress("glProgramUniform3d");
  glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)wglGetProcAddress("glProgramUniform3dv");
  glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)wglGetProcAddress("glProgramUniform3ui");
  glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)wglGetProcAddress("glProgramUniform3uiv");
  glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)wglGetProcAddress("glProgramUniform4i");
  glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)wglGetProcAddress("glProgramUniform4iv");
  glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)wglGetProcAddress("glProgramUniform4f");
  glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)wglGetProcAddress("glProgramUniform4fv");
  glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)wglGetProcAddress("glProgramUniform4d");
  glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)wglGetProcAddress("glProgramUniform4dv");
  glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)wglGetProcAddress("glProgramUniform4ui");
  glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)wglGetProcAddress("glProgramUniform4uiv");
  glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)wglGetProcAddress("glProgramUniformMatrix2fv");
  glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)wglGetProcAddress("glProgramUniformMatrix3fv");
  glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)wglGetProcAddress("glProgramUniformMatrix4fv");
  glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)wglGetProcAddress("glProgramUniformMatrix2dv");
  glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)wglGetProcAddress("glProgramUniformMatrix3dv");
  glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)wglGetProcAddress("glProgramUniformMatrix4dv");
  glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)wglGetProcAddress("glProgramUniformMatrix2x3fv");
  glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)wglGetProcAddress("glProgramUniformMatrix3x2fv");
  glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)wglGetProcAddress("glProgramUniformMatrix2x4fv");
  glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)wglGetProcAddress("glProgramUniformMatrix4x2fv");
  glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)wglGetProcAddress("glProgramUniformMatrix3x4fv");
  glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)wglGetProcAddress("glProgramUniformMatrix4x3fv");
  glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)wglGetProcAddress("glProgramUniformMatrix2x3dv");
  glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)wglGetProcAddress("glProgramUniformMatrix3x2dv");
  glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)wglGetProcAddress("glProgramUniformMatrix2x4dv");
  glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)wglGetProcAddress("glProgramUniformMatrix4x2dv");
  glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)wglGetProcAddress("glProgramUniformMatrix3x4dv");
  glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)wglGetProcAddress("glProgramUniformMatrix4x3dv");
  glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)wglGetProcAddress("glValidateProgramPipeline");
  glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)wglGetProcAddress("glGetProgramPipelineInfoLog");
  glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)wglGetProcAddress("glVertexAttribL1d");
  glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)wglGetProcAddress("glVertexAttribL2d");
  glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)wglGetProcAddress("glVertexAttribL3d");
  glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)wglGetProcAddress("glVertexAttribL4d");
  glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)wglGetProcAddress("glVertexAttribL1dv");
  glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)wglGetProcAddress("glVertexAttribL2dv");
  glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)wglGetProcAddress("glVertexAttribL3dv");
  glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)wglGetProcAddress("glVertexAttribL4dv");
  glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)wglGetProcAddress("glVertexAttribLPointer");
  glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)wglGetProcAddress("glGetVertexAttribLdv");
  glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)wglGetProcAddress("glViewportArrayv");
  glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)wglGetProcAddress("glViewportIndexedf");
  glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)wglGetProcAddress("glViewportIndexedfv");
  glScissorArrayv = (PFNGLSCISSORARRAYVPROC)wglGetProcAddress("glScissorArrayv");
  glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)wglGetProcAddress("glScissorIndexed");
  glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)wglGetProcAddress("glScissorIndexedv");
  glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)wglGetProcAddress("glDepthRangeArrayv");
  glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)wglGetProcAddress("glDepthRangeIndexed");
  glGetFloati_v = (PFNGLGETFLOATI_VPROC)wglGetProcAddress("glGetFloati_v");
  glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)wglGetProcAddress("glGetDoublei_v");

  if(!wglChoosePixelFormat || !wglCreateContextAttribs) {
    if(!wglChoosePixelFormat){
      p_log(RED "No wglChoosePixelFormat!\n");
    }
    if(!wglCreateContextAttribs){
      p_log(RED "No wglCreateContextAttribs!\n");
    }
    p_error(api, "Cannot create OpenGL context.\n");
    return -1;
  }
  const int attribList[] = {WGL_DRAW_TO_WINDOW_ARB,
                            GL_TRUE,
                            WGL_SUPPORT_OPENGL_ARB,
                            GL_TRUE,
                            WGL_DOUBLE_BUFFER_ARB,
                            GL_TRUE,
                            WGL_PIXEL_TYPE_ARB,
                            WGL_TYPE_RGBA_ARB,
                            WGL_COLOR_BITS_ARB,
                            32,
                            WGL_DEPTH_BITS_ARB,
                            24,
                            WGL_STENCIL_BITS_ARB,
                            8,
                            0};
  int attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                   WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                   // WGL_CONTEXT_FLAGS_ARB, ,
                   // unavailable on windows:
                   #ifdef __APPLE__
                   WGL_CONTEXT_PROFILE_MASK_ARB, api->opengl.compatibility ? WGL_CONTEXT_CORE_PROFILE_BIT_ARB : WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB),
                   GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE,
                   #endif
                   // GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE,
                   0};
  if(!wglChoosePixelFormat(api->window.dc, attribList, NULL, 1, &pixel_format,
                           &num_formats)) {
    p_error(api, "Cannot choose OpenGL pixel format.\n");
    return -1;
  }
  if(!(api->window.hglrc = wglCreateContextAttribs(api->window.dc, api->window.hglrc, attribs))) {
    p_error(api, "Cannot create OpenGL context attribs.\n");
    return -1;
  }
  if(!wglMakeCurrent(api->window.dc, api->window.hglrc)) {
    p_error(api, "Cannot activate OpenGL context.\n");
    return -1;
  }

  p_log("no errors with opengl init!\n");
  api->window.gl_legacy = 0;
  return 0;
}

static int _p_map_win_key(int key) {
  if(key >= 'A' && key <= 'Z') return key;
  if(key >= '0' && key <= '9') return key;
  switch(key) {
  case PK_BACKSPACE: return VK_BACK;
  case PK_TAB: return VK_TAB;
  case PK_RETURN: return VK_RETURN;
  case PK_SHIFT: return VK_SHIFT;
  case PK_CONTROL: return VK_CONTROL;
  case PK_ALT: return VK_MENU;
  case PK_PAUSE: return VK_PAUSE;
  case PK_CAPSLOCK: return VK_CAPITAL;
  case PK_ESCAPE: return VK_ESCAPE;
  case PK_SPACE: return VK_SPACE;
  case PK_PAGEUP: return VK_PRIOR;
  case PK_PAGEDN: return VK_NEXT;
  case PK_END: return VK_END;
  case PK_HOME: return VK_HOME;
  case PK_LEFT: return VK_LEFT;
  case PK_UP: return VK_UP;
  case PK_RIGHT: return VK_RIGHT;
  case PK_DOWN: return VK_DOWN;
  case PK_INSERT: return VK_INSERT;
  case PK_DELETE: return VK_DELETE;
  case PK_LWIN: return VK_LWIN;
  case PK_RWIN: return VK_RWIN;
  // case PK_APPS: return VK_APPS; // this key doesn't exist on OS X
  case PK_PAD0: return VK_NUMPAD0;
  case PK_PAD1: return VK_NUMPAD1;
  case PK_PAD2: return VK_NUMPAD2;
  case PK_PAD3: return VK_NUMPAD3;
  case PK_PAD4: return VK_NUMPAD4;
  case PK_PAD5: return VK_NUMPAD5;
  case PK_PAD6: return VK_NUMPAD6;
  case PK_PAD7: return VK_NUMPAD7;
  case PK_PAD8: return VK_NUMPAD8;
  case PK_PAD9: return VK_NUMPAD9;
  case PK_PADMUL: return VK_MULTIPLY;
  case PK_PADADD: return VK_ADD;
  case PK_PADENTER: return VK_SEPARATOR;
  case PK_PADSUB: return VK_SUBTRACT;
  case PK_PADDOT: return VK_DECIMAL;
  case PK_PADDIV: return VK_DIVIDE;
  case PK_F1: return VK_F1;
  case PK_F2: return VK_F2;
  case PK_F3: return VK_F3;
  case PK_F4: return VK_F4;
  case PK_F5: return VK_F5;
  case PK_F6: return VK_F6;
  case PK_F7: return VK_F7;
  case PK_F8: return VK_F8;
  case PK_F9: return VK_F9;
  case PK_F10: return VK_F10;
  case PK_F11: return VK_F11;
  case PK_F12: return VK_F12;
  case PK_NUMLOCK: return VK_NUMLOCK;
  case PK_SCROLL: return VK_SCROLL;
  case PK_LSHIFT: return VK_LSHIFT;
  case PK_RSHIFT: return VK_RSHIFT;
  case PK_LCONTROL: return VK_LCONTROL;
  case PK_RCONTROL: return VK_RCONTROL;
  case PK_LALT: return VK_LMENU;
  case PK_RALT: return VK_RMENU;
  case PK_SEMICOLON: return VK_OEM_1;
  case PK_EQUALS: return VK_OEM_PLUS;
  case PK_COMMA: return VK_OEM_COMMA;
  case PK_MINUS: return VK_OEM_MINUS;
  case PK_DOT: return VK_OEM_PERIOD;
  case PK_SLASH: return VK_OEM_2;
  case PK_BACKTICK: return VK_OEM_3;
  case PK_LSQUARE: return VK_OEM_4;
  case PK_BACKSLASH: return VK_OEM_5;
  case PK_RSQUARE: return VK_OEM_6;
  case PK_TICK: return VK_OEM_7;
  }
  return 0;
}

#endif // END CHECK FOR OS-SPECIFIC DEFS

/**********************************************************************************************************************/


void _p_opengl_window_setup(platform_api *api){
#ifdef __APPLE__
  id contentView = objc_msgSend_id((id)api->window.window_handle, sel_registerName("contentView"));

  // [contentView setWantsLayer:YES];
  objc_msgSend_void_bool(contentView, sel_registerName("setWantsLayer:"), YES);

  //@TODO @NOTE the NSView object we're creating here is currently emulating the
  // NSOpenGLView NSView object, which holds the NSOpenGLPixelFormat and
  // NSOpenGLContext objects for drawing with OpenGL.
  // In the future, we should be emulating the MTKView object, for Metal
  // rendering.
  // source: https://developer.apple.com/videos/play/wwdc2019/611/


  uint32_t attribs[] = {
    NSOpenGLPFAColorSize, 24,
    NSOpenGLPFAAlphaSize, 8,
    NSOpenGLPFADepthSize, 32,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFANoRecovery,
    NSOpenGLPFASampleBuffers, 1, // have one sample buffer set up
    NSOpenGLPFASamples, 4, // sample each sample buffer 4 times
    NSOpenGLPFABackingStore,
    NSOpenGLPFAMultisample, // enable multisampling
    // NSOpenGLPFASupersample, // enable supersampling
    NSOpenGLPFASampleAlpha,
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
    0
  };

  id pixelFormatAlloc = objc_msgSend_id((id)objc_getClass("NSOpenGLPixelFormat"), sel_registerName("alloc"));
  id pixelFormat = ((id(*)(id, SEL, const uint32_t *))objc_msgSend)(pixelFormatAlloc, sel_registerName("initWithAttributes:"), attribs);

  #ifndef ARC_AVAILABLE
    objc_msgSend_void(pixelFormat, sel_registerName("autorelease"));
  #endif

  // [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
  id openGLContextAlloc = objc_msgSend_id((id)objc_getClass("NSOpenGLContext"), sel_registerName("alloc"));
  id openGLContext = ((id(*)(id, SEL, id, id))objc_msgSend)(openGLContextAlloc, sel_registerName("initWithFormat:shareContext:"), pixelFormat, nil);

  #ifndef ARC_AVAILABLE
    objc_msgSend_void(openGLContext, sel_registerName("autorelease"));
  #endif

  objc_msgSend_void_id(openGLContext, sel_registerName("setView:"), contentView);

  objc_msgSend_void_bool(contentView, sel_registerName("setWantsBestResolutionOpenGLSurface:"), YES);

  api->window.gl_context = openGLContext;

  objc_msgSend_void(openGLContext, sel_registerName("makeCurrentContext"));

  id swapIntervalAlloc = objc_msgSend_id((id)objc_getClass("NSOpenGLCPSwapInterval"), sel_registerName("alloc"));
  // we want to do this to set vsync correctly:
  // [window->context.nsgl.object setValues:&sync forParameter:NSOpenGLCPSwapInterval];
  int vsync = api->window.vsync ? 1 : 0;
  ((id(*)(id, SEL, int, id))objc_msgSend)((id)api->window.gl_context, sel_registerName("setValues:forParameter:"), vsync, swapIntervalAlloc);

  // objc_msgSend_void_bool(openGLContext, sel_registerName("NSOpenGLCPSwapInterval:"), api->window.vsync ? 1 : 0);

  // id setSwapIntervalValues = objc_msgSend_id((id)objc_getClass("NSOpenGLCPSwapInterval"), sel_registerName("setValues"));
  // ((id(*)(id, SEL, const uint32_t *))objc_msgSend)(setSwapIntervalValues, sel_registerName("NSOpenGLCPSwapInterval:"), (uint32_t *)api->window.vsync);
  // objc_msgSend_void(openGLContext, sel_registerName("setValues:NSOpenGLCPSwapInterval:"), api->window.vsync);

#elif _WIN32
  if(!_p_is_gl_11_only(api)){
    _p_init_gl_41(api);
    p_log(RED "checking opengl getError!\n" RESET);
    p_check_gl_error("initialization");
    p_log(GREEN "Opengl getError working fine...\n" RESET);
  }
#endif
}

void _p_metal_window_setup(platform_api *api){
#ifdef __APPLE__
  // When enabled, rendering occurs on the main application thread.
  // This can make responding to UI events during redraw simpler
  // to manage because UI calls usually must occur on the main thread.
  // When disabled, rendering occurs on a background thread, allowing
  // the UI to respond more quickly in some cases because events can be
  // processed asynchronously from potentially CPU-intensive rendering code.
  #define RENDER_ON_MAIN_THREAD 1

  // When enabled, the view continually animates and renders
  // frames 60 times a second.  When disabled, rendering is event
  // based, occurring when a UI event requests a redraw.
  #define ANIMATION_RENDERING   1

  // When enabled, the drawable's size is updated automatically whenever
  // the view is resized. When disabled, you can update the drawable's
  // size explicitly outside the view class.
  #define AUTOMATICALLY_RESIZE  1

  // When enabled, the renderer creates a depth target (i.e. depth buffer)
  // and attaches with the render pass descritpr along with the drawable
  // texture for rendering.  This enables the app properly perform depth testing.
  #define CREATE_DEPTH_BUFFER   1

  id contentView = objc_msgSend_id((id)api->window.window_handle, sel_registerName("contentView"));

  // Here we are trying to emulate the MTKView object, for Metal rendering.
  // Source: https://developer.apple.com/videos/play/wwdc2019/611/

  // id metalLayer = [[objc_getClass("CAMetalLayer") alloc] init];
  id CAMetalLayerAlloc = objc_msgSend_id((id)objc_getClass("CAMetalLayer"), sel_registerName("alloc"));
  id metalLayer = objc_msgSend_id(CAMetalLayerAlloc, sel_registerName("init"));

  // [contentView setWantsLayer:YES];
  objc_msgSend_void_bool(contentView, sel_registerName("setWantsLayer:"), YES);
  objc_msgSend_void_id(contentView, sel_registerName("setLayer:"), metalLayer);

  // id device = MTLCreateSystemDefaultDevice();
  // metalLayer.device = device;
  // metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

  // _drawableRenderDescriptor = [MTLRenderPassDescriptor new];
  // _drawableRenderDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
  // _drawableRenderDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
  // _drawableRenderDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 1, 1, 1);



  // uint32_t attribs[] = {
  //     8, 24,//  NSOpenGLPFAColorSize, 24,
  //     11, 8,//  NSOpenGLPFAAlphaSize, 8,
  //     5,    //  NSOpenGLPFADoubleBuffer,
  //     73,   //  NSOpenGLPFAAccelerated,
  //     72,      //  NSOpenGLPFANoRecovery,
  //     55, 1,    //  NSOpenGLPFASampleBuffers, 1,
  //     56, 4,    //  NSOpenGLPFASamples, 4,
  //     99, 0x3200,//  NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
  //     0};
  //
  // id pixelFormatAlloc = objc_msgSend_id((id)objc_getClass("MTLPixelFormat"), sel_registerName("alloc"));
  // id pixelFormat = ((id(*)(id, SEL, const uint32_t *))objc_msgSend)(pixelFormatAlloc, sel_registerName("initWithAttributes:"), attribs);
  //
  // #ifndef ARC_AVAILABLE
  //   objc_msgSend_void(pixelFormat, sel_registerName("autorelease"));
  // #endif
  //
  // // [[MTLContext alloc] initWithFormat:pixelFormat shareContext:nil];
  // id MTLContextAlloc = objc_msgSend_id((id)objc_getClass("MTLContext"), sel_registerName("alloc"));
  // id MTLContext = ((id(*)(id, SEL, id, id))objc_msgSend)(MTLContextAlloc, sel_registerName("initWithFormat:shareContext:"), pixelFormat, nil);
  //
  // #ifndef ARC_AVAILABLE
  //   objc_msgSend_void(MTLContext, sel_registerName("autorelease"));
  // #endif
  //
  // objc_msgSend_void_id(MTLContext, sel_registerName("setView:"), contentView);
  //
  // // objc_msgSend_void_bool(contentView, sel_registerName("setWantsBestResolutionOpenGLSurface:"), YES);
  //
  // api->window.metal_context = MTLContext;
  //
  // objc_msgSend_void(MTLContext, sel_registerName("makeCurrentContext"));
#elif _WIN32
(void)api;
#endif
}

// returns an incrementing system clock time, without updating any API values
void _p_check_system_clock(unsigned long long *out_time){
#ifdef _WIN32
  LARGE_INTEGER large_integer;
  long long ticks_per_second;

  QueryPerformanceFrequency(&large_integer);
  ticks_per_second = large_integer.QuadPart;

  QueryPerformanceCounter(&large_integer);
  uint64_t current_ticks = large_integer.QuadPart;

  *out_time = (1000 * 1000 * 1000 * current_ticks) / ticks_per_second;

#elif __APPLE__

  uint64_t start;
  uint64_t end;

  /* Get the timebase info */
  mach_timebase_info_data_t info;
  mach_timebase_info(&info);

  // Start the clock.

  start = mach_absolute_time();

  // Call getpid. This will produce inaccurate results because
  // we're only making a single system call. For more accurate
  // results you should call getpid multiple times and average
  // the results.

  (void)getpid();

  // Stop the clock.

  end = mach_absolute_time();

  /* Convert to nanoseconds */
  end *= info.numer;
  end /= info.denom;

  *out_time = end;
#endif
}

void _p_update_time(p_time_info_toplevel *tm) {
#ifdef _WIN32

  // static int first = 1;
  // static float timestamp;
  // static uint64_t program_start;
  // static LARGE_INTEGER prev;
  // static double factor;
  //
  // LARGE_INTEGER now;
  // QueryPerformanceCounter(&now);
  //
  // if (first) {
  //   first = 0;
  //   prev = now;
  //   LARGE_INTEGER freq;
  //   QueryPerformanceFrequency(&freq);
  //   factor = 1.0 / (long double)freq.QuadPart;
  //   program_start = (long double)((now.QuadPart - prev.QuadPart) * factor);
  // }
  //
  // long double elapsed = (long double)((now.QuadPart - prev.QuadPart) * factor);
  // // p_log("now is %lld or %Lf\n", now.QuadPart, elapsed);
  // prev = now;
  // timestamp = timestamp + elapsed;
  // // p_log("elapsed: %f\n", elapsed);
  // // p_log("timestamp is: %llu\n", timestamp);
  // tm->ns_delta = (uint64_t)(timestamp * 1000000000.0) - tm->ns;
  // tm->ns = (uint64_t)(timestamp * 1000000000.0);
  // // p_log("totalMicro is %llu\n", *totalMicro);
  //
  // tm->ms_delta = (uint64_t)(timestamp * 1000.0) - tm->ms;
  // tm->ms = (uint64_t)timestamp * 1000.0;
  //
  // tm->seconds_delta = (tm->ms / 1000.0) - tm->seconds;
  // tm->seconds = (tm->ms / 1000.0);

  static int first = 1;
  LARGE_INTEGER large_integer;

  if(first){
    QueryPerformanceFrequency(&large_integer);
    tm->ticks_per_second = large_integer.QuadPart;
    QueryPerformanceCounter(&large_integer);
    tm->initial_ticks = large_integer.QuadPart;
    first = 0;
  }

  QueryPerformanceCounter(&large_integer);
  uint64_t current_ticks = large_integer.QuadPart;

  tm->delta_ticks = (current_ticks - tm->initial_ticks) - tm->ticks;
  tm->ticks = current_ticks - tm->initial_ticks;
  // p_log("delta ticks are: %lli\r", tm->delta_ticks);
  // fflush(stdout);

  tm->ns_delta = (1000 * 1000 * 1000 * tm->delta_ticks) / tm->ticks_per_second;
  tm->ms_delta = tm->ns_delta / (1000 * 1000);
  tm->seconds_delta = (float)tm->delta_ticks / (float)tm->ticks_per_second;

  tm->ns = (1000 * 1000 * 1000 * tm->ticks) / tm->ticks_per_second;
  tm->ms = tm->ns / (1000*1000);
  tm->seconds = (float)tm->ticks / (float)tm->ticks_per_second;

#elif __APPLE__

  uint64_t start;
  uint64_t end;
  static unsigned long long program_start;
  static unsigned long long timestamp;

  /* Get the timebase info */
  mach_timebase_info_data_t info;
  mach_timebase_info(&info);

  // Start the clock.

  start = mach_absolute_time();

  // Call getpid. This will produce inaccurate results because
  // we're only making a single system call. For more accurate
  // results you should call getpid multiple times and average
  // the results.

  (void)getpid();

  // Stop the clock.

  end = mach_absolute_time();

  if (program_start == 0) {
    program_start = start;
  }

  timestamp = end - program_start;

  /* Convert to nanoseconds */
  timestamp *= info.numer;
  timestamp /= info.denom;

  // first, shift our historical deltas:
  tm->ns_delta_0 = tm->ns_delta_1;
  tm->ns_delta_1 = tm->ns_delta_2;
  tm->ns_delta_2 = tm->ns_delta;
  tm->ns_delta = timestamp - tm->ns;

  tm->ns_smoothed = (tm->ns_delta_0 + tm->ns_delta_1 + tm->ns_delta_2 + tm->ns_delta) / 4;

  tm->ms_delta = (timestamp / 1000000) - tm->ms;
  tm->seconds_delta = (tm->ms / 1000) - tm->seconds;


  tm->ns = timestamp;
  tm->ms = timestamp / 1000000;
  tm->seconds = (tm->ms / 1000);
#endif
}

void _p_set_keys(platform_api *api) { (void)api; }

void p_init_camera(platform_api *api){

  api->window.mouse_lock = false;
  api->window.prev_mouse_lock = false;

  // let's set default camera stuff here:
  api->camera.location[0] = 0.0;
  api->camera.location[1] = 0.0;
  api->camera.location[2] = 0.0;

  api->camera.ortho = false;

  api->camera.fov = 45.0;
  api->camera.fov_rad = p_to_radians(api->camera.fov);
  api->camera.near_clip = 0.01;
  api->camera.far_clip = 100;
  api->camera.sensitivity = 0.000007;
  api->camera.move_speed = 0.01;

  api->camera.scale = 1.0f;

}

// init
platform_api *p_init(){
  platform_api *api = (platform_api *)malloc(sizeof(platform_api));
  memset(api, 0, sizeof(platform_api)); // initialize to zero

  api->quit = false;
  api->window.shown = false;

  api->draw.lock_size = false; // default to false here...

  api->window.scale = 1.0;

  p_init_camera(api);

  #ifdef PLATFORM_USE_OPENGL
    // default to vsync off...
    api->window.vsync = false;
  #endif

  // start timer
  _p_update_time(&api->time);
  // init OS necessary stuff:
  #ifdef __APPLE__
    if(!api->system.initted){

    NSWindowStyleMaskBorderless = 0;
    NSWindowStyleMaskTitled = 1 << 0;
    NSWindowStyleMaskClosable = 1 << 1;
    NSWindowStyleMaskMiniaturizable = 1 << 2;
    NSWindowStyleMaskResizable = 1 << 3;
    NSWindowStyleMaskTexturedBackground = 1 << 8; // DEPRECATED
    NSWindowStyleMaskUnifiedTitleAndToolbar = 1 << 12;
    NSWindowStyleMaskFullScreen = 1 << 14;
    NSWindowStyleMaskFullSizeContentView = 1 << 15;
    NSWindowStyleMaskUtilityWindow = 1 << 4;
    NSWindowStyleMaskDocModalWindow = 1 << 6;
    NSWindowStyleMaskNonactivatingPanel = 1 << 7;
    NSWindowStyleMaskHUDWindow = 1 << 13;

    NSBackingStoreRetained = 0;
    NSBackingStoreNonretained = 1;
    NSBackingStoreBuffered = 2;

    #ifdef ARC_AVAILABLE
    // TODO and what do we do now? it's a bit too tricky to use @autoreleasepool
    // here
    #error TODO this code should be compiled as C for now
    #else
      // would be nice to use objc_autoreleasePoolPush instead, but it's not
      // publically available in the headers:
      // [[NSAutoreleasePool alloc] init]
      id poolAlloc = objc_msgSend_id((id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc"));
      global_autoreleasePool = objc_msgSend_id(poolAlloc, sel_registerName("init"));
      api->window.autoreleasepool = global_autoreleasePool;

      atexit(&_p_clean_up_macos);
    #endif

      objc_msgSend_id((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
      ((void (*)(id, SEL, NSInteger))objc_msgSend)(NSApp, sel_registerName("setActivationPolicy:"), 0);

      Class appDelegateClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "AppDelegate", 0);
      bool resultAddProtoc = class_addProtocol(appDelegateClass, objc_getProtocol("NSApplicationDelegate"));
      assert(resultAddProtoc);
      bool resultAddMethod = class_addMethod(appDelegateClass, sel_registerName("_p_macos_app_should_terminate:"), (IMP)_p_macos_app_should_terminate, NSUIntegerEncoding "@:@");
      assert(resultAddMethod);
      id dgAlloc = objc_msgSend_id((id)appDelegateClass, sel_registerName("alloc"));
      id dg = objc_msgSend_id(dgAlloc, sel_registerName("init"));
    #ifndef ARC_AVAILABLE
      objc_msgSend_void(dg, sel_registerName("autorelease"));
    #endif

      objc_msgSend_void_id(NSApp, sel_registerName("setDelegate:"), dg);
      objc_msgSend_void(NSApp, sel_registerName("finishLaunching"));

      // [[NSMenu alloc] init]
      id menubarAlloc = objc_msgSend_id((id)objc_getClass("NSMenu"), sel_registerName("alloc"));
      id menubar = objc_msgSend_id(menubarAlloc, sel_registerName("init"));
    #ifndef ARC_AVAILABLE
      objc_msgSend_void(menubar, sel_registerName("autorelease"));
    #endif

      // [[NSMenuItem alloc] init]
      id appMenuItemAlloc = objc_msgSend_id((id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
      id appMenuItem = objc_msgSend_id(appMenuItemAlloc, sel_registerName("init"));

    #ifndef ARC_AVAILABLE
      objc_msgSend_void(appMenuItem, sel_registerName("autorelease"));
    #endif

      objc_msgSend_void_id(menubar, sel_registerName("addItem:"), appMenuItem);
      ((id(*)(id, SEL, id))objc_msgSend)(NSApp, sel_registerName("setMainMenu:"), menubar);

      // [[NSMenu alloc] init]
      id appMenuAlloc = objc_msgSend_id((id)objc_getClass("NSMenu"), sel_registerName("alloc"));
      id appMenu = objc_msgSend_id(appMenuAlloc, sel_registerName("init"));

    #ifndef ARC_AVAILABLE
      objc_msgSend_void(appMenu, sel_registerName("autorelease"));
    #endif

      // [[NSProcessInfo processInfo] processName]
      id processInfo = objc_msgSend_id((id)objc_getClass("NSProcessInfo"), sel_registerName("processInfo"));
      id appName = objc_msgSend_id(processInfo, sel_registerName("processName"));

      // join "Quit" string with the app name, so we get "Quit XYZ" as a menu item:
      id quitTitlePrefixString = objc_msgSend_id_const_char((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "Quit ");
      id quitTitle = ((id(*)(id, SEL, id))objc_msgSend)(quitTitlePrefixString, sel_registerName("stringByAppendingString:"), appName);

      id quitMenuItemKey = objc_msgSend_id_const_char((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "q");
      id quitMenuItemAlloc = objc_msgSend_id((id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
      id quitMenuItem = ((id(*)(id, SEL, id, SEL, id))objc_msgSend)(quitMenuItemAlloc, sel_registerName("initWithTitle:action:keyEquivalent:"), quitTitle, sel_registerName("terminate:"), quitMenuItemKey);

    #ifndef ARC_AVAILABLE
      objc_msgSend_void(quitMenuItem, sel_registerName("autorelease"));
    #endif

      objc_msgSend_void_id(appMenu, sel_registerName("addItem:"), quitMenuItem);
      objc_msgSend_void_id(appMenuItem, sel_registerName("setSubmenu:"), appMenu);

      int mib[2];
      int64_t physical_memory;
      size_t length;

      // Get the Physical memory size
      mib[0] = CTL_HW;
      mib[1] = HW_MEMSIZE;
      length = sizeof(int64_t);
      sysctl(mib, 2, &physical_memory, &length, NULL, 0);
      api->system.hardware.physical_memory = physical_memory;

      int64_t cpu_count;
      mib[1] = HW_NCPU;
      sysctl(mib, 2, &cpu_count, &length, NULL, 0);
      api->system.hardware.cpu_count = cpu_count;

      // typedef struct {
      //   NSInteger majorVersion;
      //   NSInteger minorVersion;
      //   NSInteger patchVersion;
      // } NSOperatingSystemVersion;

      // NSOperatingSystemVersion versions = [[objc_getClass("NSProcessInfo") processInfo] sel_registerName("operatingSystemVersion")];
      // #define NSOperatingSystemVersion objc_getClass("NSOperatingSystemVersion");
      // NSOperatingSystemVersion versions = ((NSOperatingSystemVersion(*)(id, SEL))objc_msgSend)(processInfo, sel_registerName("operatingSystemVersion"));
      // id versions = objc_msgSend_id(processInfo, sel_registerName("operatingSystemVersion"));
      // NSOperatingSystemVersion v = (NSOperatingSystemVersion)versions;
      // p_log("os version: %li\n", versions.majorVersion);
      // p_log("os version is %li.%li.%li\n", versions.majorVersion, versions.minorVersion, versions.patchVersion);


      api->system.initted = true;
    }
  #elif _WIN32
    api->system.initted = true;
  #endif

  return api;
}

void p_swap_buffers(platform_api *api){
  // p_log("running p_end_frame()\n");
  #ifdef _WIN32
    if(!SwapBuffers(api->window.dc)) {
      p_error(api, "Cannot swap Window buffers.\n");
    }
  #elif __APPLE__
    #ifdef PLATFORM_USE_OPENGL
      objc_msgSend_void((id)api->window.gl_context, sel_registerName("flushBuffer"));
      objc_msgSend_void((id)objc_getClass("NSOpenGLContext"), sel_registerName("clearCurrentContext"));
      objc_msgSend_void((id)api->window.gl_context, sel_registerName("makeCurrentContext"));
    #endif
  #endif
}

// close
void p_close(platform_api *api){
  #ifdef _WIN32
    DestroyWindow((HWND)api->window.window_handle);
  #elif __APPLE__
  if(!_p_macos_is_window_closed(api) && !_p_macos_terminated){
    objc_msgSend_void((id)api->window.window_handle, sel_registerName("close"));
  }

  // this already runs on atexit, defined above
  // _p_clean_up_macos();
  #endif

  free(api);
}

void p_set_cursor_position(platform_api *api, float in_x, float in_y) {
#ifdef _WIN32
  POINT pt;
  pt.x = in_x;
  pt.y = in_y;

  ClientToScreen((HWND)api->window.window_handle, &pt);
  SetCursorPos(pt.x, pt.y);

#elif __APPLE__
  id window = (id)api->window.window_handle;
  // x and y are based on window size here:
  NSSize render_size = {};
  NSSize window_size = {};
  float scale;
  _p_macos_cocoa_window_size((id)api->window.window_handle, &render_size, &window_size, &scale);

  float rel_x = in_x / window_size.width;
  float rel_y = in_y / window_size.height;

  // get the whole window's context:
  id windowContentView = objc_msgSend_id(window, sel_registerName("contentView"));

  // the view's frame rectangle (position and size)
  NSRect adjustFrame = ((NSRect(*)(id, SEL))abi_objc_msgSend_stret)(windowContentView, sel_registerName("frame"));

  // calculate "actual" x and y based on relative values:
  float x = rel_x * adjustFrame.size.width;
  float y = rel_y * adjustFrame.size.height;

  NSRect localRect = {{x, adjustFrame.size.height - y - 1}, {0, 0}};
  // need to get the screen coords based on where the window is:
  NSRect r = ((NSRect(*)(id, SEL, NSRect))abi_objc_msgSend_stret)( window, sel_registerName("convertRectToScreen:"), localRect);
  NSPoint newP = {r.origin.x, CGDisplayBounds(CGMainDisplayID()).size.height - r.origin.y - 1};

  CGWarpMouseCursorPosition(newP);// does not fire events
  // https://stackoverflow.com/questions/8215413/why-is-cgwarpmousecursorposition-causing-a-delay-if-it-is-not-what-is
  CGAssociateMouseAndMouseCursorPosition(true);
  // p_log("platform setting position to %f, %f\n", newP.x, newP.y);
#endif
}

void p_keep_cursor_centered(platform_api *api, float *newX, float *newY) {
  api->window.constrain_mouse = true;

#ifdef _WIN32
  // p_log("centering cursor. \n");
  int dw, dh;
  RECT rc;

  GetClientRect((HWND)api->window.window_handle, &rc);
  dw = rc.right - rc.left;
  dh = rc.bottom - rc.top;

  // p_log("window size is set to %f %f\n", dw, dh);

  *newX = dw / 2.0;
  *newY = dh / 2.0;
  // *newX = 0;
  // *newY = 0;
  // p_log("running setCursorPosition to %f %f\n", *newX, *newY);

#elif __APPLE__
  // p_log("centering cursor. \n");
  id window;
  window = (id)api->window.window_handle;
  NSSize render_size = {};
  NSSize window_size = {};
  float scale;
  _p_macos_cocoa_window_size((id)api->window.window_handle, &render_size, &window_size, &scale);
  // p_log("window size is set to %f %f\n", windowSize.width,
  // windowSize.height);
  *newX = window_size.width / 2.0;
  *newY = window_size.height / 2.0;
  // *newX = 0;
  // *newY = 0;
  // p_log("running setCursorPosition to %f %f\n", *newX, *newY);
#endif

  p_set_cursor_position(api, *newX, *newY);
}

void _p_handle_mouse(platform_api *api) {
  bool mouse_locked = api->window.mouse_lock;
#ifdef _WIN32
  if(GetFocus() != api->window.window_handle){
    return;
  }

  POINT pt;

  GetCursorPos(&pt);
  ScreenToClient((HWND)api->window.window_handle, &pt);
  // p_log("raw mouse loc is %li, %li\n", pt.x, pt.y);
  api->mouse.raw_delta_position[0] = pt.x - api->mouse.window_position[0];
  api->mouse.raw_delta_position[1] = pt.y - api->mouse.window_position[1];

  api->mouse.window_position[0] = pt.x;
  api->mouse.window_position[1] = pt.y;

  api->mouse.position[0] = pt.x / api->window.scale;
  api->mouse.position[1] = pt.y / api->window.scale;

  api->mouse.delta_position[0] = api->mouse.raw_delta_position[0] / api->window.scale;
  api->mouse.delta_position[1] = api->mouse.raw_delta_position[1] / api->window.scale;

  // reset the mouse state every frame:
  api->mouse.left.is_down = false;
  api->mouse.middle.is_down = false;
  api->mouse.right.is_down = false;

  if(GetAsyncKeyState(VK_LBUTTON) & 0x8000){
    api->mouse.left.was_pressed = api->mouse.left.is_down;
    api->mouse.left.is_down = true;
    api->mouse.left.was_released = false;
  }
  if(GetAsyncKeyState(VK_MBUTTON) & 0x8000){
    api->mouse.middle.was_pressed = api->mouse.middle.is_down;
    api->mouse.middle.is_down = true;
    api->mouse.middle.was_released = false;
  }
  if(GetAsyncKeyState(VK_RBUTTON) & 0x8000){
    api->mouse.right.was_pressed = api->mouse.right.is_down;
    api->mouse.right.is_down = true;
    api->mouse.right.was_released = false;
  }

#elif __APPLE__

  id window;
  window = (id)api->window.window_handle;
  NSSize renderSize = {};
  NSSize windowSize = {};
  float scale;
  _p_macos_cocoa_window_size(window, &renderSize, &windowSize, &scale);
  id contentView = objc_msgSend_id(window, sel_registerName("contentView"));
  // NSPoint is small enough to fit a register, so no need for
  // objc_msgSend_stret

  // The current location of the pointer reckoned in the window’s base coordinate system, regardless of the current event being handled or of any events pending:
  NSPoint p = ((NSPoint(*)(id, SEL))objc_msgSend)(window, sel_registerName("mouseLocationOutsideOfEventStream"));

  // Converts a point from view coordinates to scene coordinates:
  // NSPoint viewLoc = [self convertPoint:windowLoc fromView:nil];
  NSPoint viewLoc = ((NSPoint(*)(id, SEL, NSPoint, id))objc_msgSend)(contentView, sel_registerName("convertPoint:fromView:"), p, contentView);

  // Converts a point from the view’s interior coordinate system to its pixel aligned backing store coordinate system:
  // NSPoint backingLoc = [self convertPointToBacking:viewLoc];
  // @NOTE only supported in MACOS 10.7+
  NSPoint backingLoc = ((NSPoint(*)(id, SEL, NSPoint))objc_msgSend)(window, sel_registerName("convertPointToBacking:"), viewLoc);

  // map input to content view rect
  // comment this out if we want mouse events to
  // be tracked outside of the window too.
  if(api->window.constrain_mouse){
    if(backingLoc.x < 0) {
      backingLoc.x = 0;
    } else if(backingLoc.x >
              renderSize.width) {// greater than off the window to the right
      backingLoc.x = renderSize.width;
    }

    if(backingLoc.y < 0) {
      backingLoc.y = 0;
    } else if(backingLoc.y >
              renderSize.height) {// greater than off the window to the bottom
      backingLoc.y = renderSize.height;
    }
  }

  // map input to pixels
  NSRect r = {{backingLoc.x, backingLoc.y}, {0, 0}};
  backingLoc = r.origin;

  api->mouse.raw_delta_position[0] = backingLoc.x - api->mouse.window_position[0];
  api->mouse.raw_delta_position[1] = backingLoc.y - api->mouse.window_position[1];

  api->mouse.window_position[0] = backingLoc.x;
  api->mouse.window_position[1] = backingLoc.y;

  api->mouse.delta_position[0] = (backingLoc.x / api->window.scale) - api->mouse.position[0];
  api->mouse.delta_position[1] = ((renderSize.height - backingLoc.y) / api->window.scale) - api->mouse.position[1];

  api->mouse.position[0] = (backingLoc.x) / api->window.scale;
  api->mouse.position[1] = (renderSize.height - backingLoc.y) / api->window.scale; // subtract y from height so we reverse our y-coords

  // if(buttons) {
  //   id keyWindow = objc_msgSend_id(NSApp, sel_registerName("keyWindow"));
  //   *buttons = keyWindow != bmp->handle ? 0 : win->mouseButtons;
  // }

#endif

  if(mouse_locked){
    // if we're using mouselock, we need to calculate the center differently:
    // mainly, delta is now "how far from center we are":
    api->mouse.delta_position[0] = api->mouse.position[0] - (api->window.size[0] / 2.0);
    api->mouse.delta_position[1] = api->mouse.position[1] - (api->window.size[1] / 2.0);

    // p_log("mouse pos centered: %i ", api->mouse.position[0] == api->window.size[0] / 2.0 && api->mouse.position[1] == api->window.size[1] / 2.0);
    // p_log("mouse pos: [%i, %i] ", api->mouse.position[0], api->mouse.position[1]);

    // now, we should actually center the cursor here...
    float newX;
    float newY;
    p_keep_cursor_centered(api, &newX, &newY);
  }
}

// only check for the initial keystroke (vs key_held which continuously runs every frame)
int p_key_down(platform_api *api, int key) {
  int k;
  #ifdef __APPLE__
    assert(key < P_NUM_KEYS);
    // @NOTE I'm not sure why we're checking this, unless we support programs with multiple windows. Which we don't, really.
    // commenting out for now, since it's a pain. Same for p_key_held.
    // @TODO for some reason, sending this makes the key tracker delay by a keystroke, not sure why?
    // if our current window isn't the one that's handing keystrokes right now, ignore it:
    // id keyWindow = objc_msgSend_id(NSApp, sel_registerName("keyWindow"));
    // if(keyWindow != api->window.window_handle){
    //   p_log("key listener is not the same as our window\n");
    //   return 0;
    // }
    k = key;
  #elif _WIN32
    k = _p_map_win_key(key);
    if(GetFocus() != api->window.window_handle){
      return 0;
    }
  #endif

  return api->keys[k].is_down && !api->prev_keys[k].is_down;
}

int p_key_held(platform_api *api, int key) {
  int k;
  #ifdef __APPLE__
    assert(key < P_NUM_KEYS);
    // id keyWindow = objc_msgSend_id(NSApp, sel_registerName("keyWindow"));
    // if(keyWindow != api->window.window_handle){
    //   return 0;
    // }
    k = key;
  #elif _WIN32
    k = _p_map_win_key(key);
    if(GetFocus() != api->window.window_handle) return 0;
  #endif

  return api->keys[k].is_down && api->prev_keys[k].is_down;
}

int p_key_up(platform_api *api, int key) {
  int k;
#ifdef __APPLE__
  assert(key < P_NUM_KEYS);
  // id keyWindow = objc_msgSend_id(NSApp, sel_registerName("keyWindow"));
  // if(keyWindow != api->window.window_handle){
  //   return 0;
  // }
  k = key;
#elif _WIN32
  k = _p_map_win_key(key);
  if(GetFocus() != api->window.window_handle) return 0;
#endif

// trigger only if it's been marked as released _this frame_ and not previously:
return api->keys[key].was_released && !api->prev_keys[key].was_released;
}

bool p_toggle_mouse_visible() {
  static bool is_hidden = false;

  /*
  This function sets an internal display counter that determines whether the
  cursor should be displayed. The cursor is displayed only if the display count
  is greater than or equal to 0. If a mouse is installed, the initial display
  count is 0. If no mouse is installed, the display count is –1.
  */

#ifdef _WIN32
  if(is_hidden) {
    // show the cursor:
    ShowCursor(true);
    is_hidden = false;
  } else {
    ShowCursor(false);
    is_hidden = true;
  }
#elif __APPLE__
if(is_hidden) {
  // show the cursor:
  objc_msgSend_id((id)objc_getClass("NSCursor"), sel_registerName("unhide"));
  is_hidden = false;
} else {
  // each "hide" event must have a matching "unhide" to show the cursor again.
  // if we run this twice, and "unhide" only once, it will still be hidden.
  // hide the cursor:
  // [NSCursor hide];
  objc_msgSend_id((id)objc_getClass("NSCursor"), sel_registerName("hide"));
  is_hidden = true;
}
#endif

return is_hidden;
}

void p_mouselock(platform_api *api){
  api->window.prev_mouse_lock = api->window.mouse_lock;

  if(!api->window.mouse_lock){
    bool mouse_lock = p_toggle_mouse_visible();
    api->window.mouse_lock = mouse_lock;
  }
}

void p_handle_mouselock_toggle(platform_api *api, int key){
  bool mouse_lock;

  api->window.prev_mouse_lock = api->window.mouse_lock;

  if (p_key_down(api, key)) {
    p_log("toggling mouselock\n");
    mouse_lock = p_toggle_mouse_visible();
    // setting this also makes sure the mouse stays centered, in _p_handle_mouse:
    api->window.mouse_lock = mouse_lock;
  }
}

void p_update_camera_3d(platform_api *api, int forward, int back, int right, int left){

  static float camera_horizontal_angle;
  static float camera_vertical_angle;
  static float camera_forward_delta[3];
  static float camera_right_delta[3];
  float vertical_clamp_degrees = 90;
  float horizontal_wrap_degrees = 720;

  // This block will reset the mouse input angles as soon as the mouse lock toggles on,
  // so we don't count the initial mouse centering movement as "input"
  bool just_toggled_on = api->window.mouse_lock && !api->window.prev_mouse_lock;
  if(just_toggled_on){
    p_log("just_toggled on is based on current: %i, prev: %i\n", api->window.mouse_lock, api->window.prev_mouse_lock);
  }

  int mouse_delta_x = just_toggled_on ? 0 : api->mouse.delta_position[0];
  int mouse_delta_y = just_toggled_on ? 0 : api->mouse.delta_position[1];


  // if(){
  //   // p_log("we just turned on mouselock!\n");
  //   // p_log("angles were: %f, %f\n", camera_horizontal_angle, camera_vertical_angle);
  //   // camera_horizontal_angle = 3.14;
  //   // camera_vertical_angle = 0.0;
  // }

  // p_log("time delta: %llu, mouse delta: %d, hor angle: %f\r", api->time.ns_smoothed, (api->mouse.delta_position[0] * -1), camera_horizontal_angle);
  // fflush(stdout);

  // first, figure out the delta of the mouse movement:
  if(mouse_delta_x != 0 || mouse_delta_y != 0){
    // @TODO multiply by -1 at the end for "normal" controls, but we should be checking an "invert controls" flag, probably
    camera_horizontal_angle += api->camera.sensitivity * (float)(api->mouse.delta_position[0] * -1);
    camera_vertical_angle += api->camera.sensitivity * (float)(api->mouse.delta_position[1] * -1);

    float vertical_clamp_radians = p_to_radians(vertical_clamp_degrees);
    float horizontal_wrap_radians = p_to_radians(horizontal_wrap_degrees);

    if(camera_vertical_angle > vertical_clamp_radians) {
      camera_vertical_angle = vertical_clamp_radians;
    }
    if(camera_vertical_angle < (-1 * vertical_clamp_radians)) {
      camera_vertical_angle = -1 * vertical_clamp_radians;
    }

    // if we go over 720 degrees, subtract so we stay at a reasonable angle...
    if(camera_horizontal_angle > horizontal_wrap_radians || camera_horizontal_angle < (horizontal_wrap_radians * -1.0)) {
      // if we've somehow got a multiple of 720, make sure we're subtracting as much as we can:
      int multiplier = floor(camera_horizontal_angle / horizontal_wrap_radians);
      camera_horizontal_angle -= (horizontal_wrap_radians * multiplier);
    }

    // p_log("hor,ver ang: [%f, %f] ", camera_horizontal_angle, camera_vertical_angle);

    // Then, set up the camera right, forward, up vectors:
    api->camera.camera_forward[0] = cos(camera_vertical_angle) * sin(camera_horizontal_angle);
    api->camera.camera_forward[1] = sin(camera_vertical_angle);
    api->camera.camera_forward[2] = cos(camera_vertical_angle) * cos(camera_horizontal_angle);

    api->camera.camera_right[0] = sin(camera_horizontal_angle - 3.141592f / 2.0f);
    api->camera.camera_right[1] = 0;
    api->camera.camera_right[2] = cos(camera_horizontal_angle - 3.141592f / 2.0f);

    p_vec3_norm(api->camera.camera_right, api->camera.camera_right);
    p_vec3_norm(api->camera.camera_forward, api->camera.camera_forward);

    // "up" vector is the cross product of right and forward vectors.
    // (orthogonal to right & forward)
    p_vec3_mul_cross(api->camera.camera_up, api->camera.camera_right, api->camera.camera_forward);

    // set up the deltas for camera movement:
    if(api->time.ns_delta <= 0){
      p_log("ns delta is 0!\n");
    }

    camera_forward_delta[0] = api->camera.camera_forward[0] * api->time.ns_delta * api->camera.move_speed;
    camera_forward_delta[1] = api->camera.camera_forward[1] * api->time.ns_delta * api->camera.move_speed;
    camera_forward_delta[2] = api->camera.camera_forward[2] * api->time.ns_delta * api->camera.move_speed;

    camera_right_delta[0] = api->camera.camera_right[0] * api->time.ns_delta * api->camera.move_speed;
    camera_right_delta[1] = api->camera.camera_right[1] * api->time.ns_delta * api->camera.move_speed;
    camera_right_delta[2] = api->camera.camera_right[2] * api->time.ns_delta * api->camera.move_speed;
  }

  // p_log("hor_angle is %f, delta_position is [%i, %i] ", camera_horizontal_angle, api->mouse.delta_position[0], api->mouse.delta_position[1]);

  if(p_key_held(api, forward)) {
    // p_log("moving forward delta: [%f, %f, %f]", camera_forward_delta[0], camera_forward_delta[1], camera_forward_delta[2]);
    p_vec3_add(api->camera.location, camera_forward_delta, api->camera.location);
  }
  if(p_key_held(api, back)) {
    // p_log("moving backward");
    // p_log("moving backward delta: [%f, %f, %f]", camera_forward_delta[0], camera_forward_delta[1], camera_forward_delta[2]);
    p_vec3_sub(api->camera.location, api->camera.location, camera_forward_delta);
  }

  if(p_key_held(api, right)) {
    // p_log("moving right");
    p_vec3_add(api->camera.location, camera_right_delta, api->camera.location);
  }
  if(p_key_held(api, left)) {
    // p_log("moving left");
    p_vec3_sub(api->camera.location, api->camera.location, camera_right_delta);
  }

  // finally, create the projection matrix
  p_mat4x4_identity(api->camera.projection_matrix);// start with identity

  if(api->camera.ortho){
    // @NOTE we need to work on the near/far clip for ortho...
    // also, figure out movement in ortho? Seems like we need to handle zooming in/out manually here...
    // possibly even "fake" an ortho camera with a reallly low-fov perspective camera?
    int left = -10 * api->window.aspect,
        right = 10 * api->window.aspect,
        bottom = -10,
        top = 10;
    p_mat4x4_ortho(api->camera.projection_matrix, left, right, bottom, top, api->camera.near_clip, api->camera.far_clip);
  } else {
    if(!api->camera.far_clip){
      p_mat4x4_perspective_infinite(
        api->camera.projection_matrix,
        api->camera.fov_rad,
        api->window.aspect,
        api->camera.near_clip
      );
    } else {
      p_mat4x4_perspective(
        api->camera.projection_matrix,
        api->camera.fov_rad,
        api->window.aspect,
        api->camera.near_clip,
        api->camera.far_clip
      );
    }
  }

  // Now, generate a lookat matrix for "view"
  p_mat4x4_identity(api->camera.view_matrix);// start with identity

  static float look_at[3];


  p_vec3_add(look_at, api->camera.location, api->camera.camera_forward);
  api->camera.fov_rad = p_to_radians(api->camera.fov);

  p_mat4x4_look_at( api->camera.view_matrix, api->camera.location, look_at, api->camera.camera_up);

}

void p_update_camera_2d(platform_api *api) {
  // "up" vector is the cross product of right and forward vectors.
  // (orthogonal to right & forward)
  p_vec3_mul_cross(api->camera.camera_up, api->camera.camera_right, api->camera.camera_forward);

  // Now, generate a lookat matrix for "view"
  p_mat4x4_identity(api->camera.view_matrix);// start with identity
  p_vec3_add(api->camera.look_matrix, api->camera.location, api->camera.camera_forward);
  p_mat4x4_look_at( api->camera.view_matrix, api->camera.location, api->camera.look_matrix, api->camera.camera_up);
  p_mat4x4_scale(api->camera.view_matrix, api->camera.view_matrix, api->camera.scale);
}

void p_timer_start(platform_api *api, int index) {
  // find the first open timer slot:
  // int i;
  // for(i = 0; i < P_MAX_TIMERS; i++){
  //   if(api->system)
  // }

  if(index > (int)(sizeof(api->system.timers) / sizeof(p_timer))) {
    p_log("index too large for timers pool.");
    return;
  }

  p_timer found = api->system.timers[index];
  unsigned long long current_ns;
  _p_check_system_clock(&current_ns);
  int timestamp = current_ns;

  if(found.already_started == true) {
    p_log("we've already started this timer somewhere else.");
    return;
  } else {
    api->system.timers[index].startTime = timestamp;
    api->system.timers[index].already_started = true;
  }
}

int p_timer_end(platform_api *api, int index) {
  if(index > (int)(sizeof(api->system.timers) / sizeof(p_timer))) {
    p_log("index too large for timers pool.");
    return -1;
  }

  p_timer found = api->system.timers[index];
  unsigned long long current_ns;
  _p_check_system_clock(&current_ns);
  int timestamp = current_ns;

  if(found.already_started == false) {
    p_log("we haven't started this timer yet.");
    return -1;
  } else {
    api->system.timers[index].endTime = timestamp;
    api->system.timers[index].delta = timestamp - api->system.timers[index].startTime;
    api->system.timers[index].already_started = false;
    return api->system.timers[index].delta;
  }
}

// @TODO this should be _p_handle_inputs(platform_api *api)
// for now though, this needs to stay with _update, since it needs to run every frame for MACOS,
// because the OS needs to know we're handling window input to show a window, I guess?
void p_update(platform_api *api){

  int new_wwidth, new_wheight, new_rwidth, new_rheight;
  float new_scale;
  bool is_different_size, is_not_zero;

  // p_log("running p_start_frame()\n");
  // update timers:
  _p_update_time(&api->time);

  // Move keys from last frame into prev_keys
  memcpy(api->prev_keys, api->keys, sizeof(api->keys));
  // memset(api->keys, 0, sizeof(api->keys));
  // p_log("setting keys to 0;\n");

#ifdef _WIN32
  MSG msg;
  RECT rc;
  // RECT window_rect;

  // Get the window size.
  GetClientRect((HWND)api->window.window_handle, &rc);
  new_rwidth = rc.right - rc.left;
  new_rheight = rc.bottom - rc.top;

  // GetWindowRect((HWND)api->window.window_handle, &window_rect);
  // new_wwidth = window_rect.right - window_rect.left;
  // new_wheight = window_rect.bottom - window_rect.top;

  // @NOTE for now, let's just assume that the render and window size will be the same
  // for Windows - later, we should figure out the "correct" way to handle this.
  new_wwidth = new_rwidth;
  new_wheight = new_rheight;

  if(!api->window.shown) {
    api->window.shown = 1;
    UpdateWindow((HWND)api->window.window_handle);
    ShowWindow((HWND)api->window.window_handle, SW_SHOW);
  }

  // Run the message pump.
  while(PeekMessage(&msg, (HWND)api->window.window_handle, 0, 0, PM_REMOVE)) {
    if(msg.message == WM_QUIT){
      p_log("QUITTING DUE TO OS MESSAGE\n");
      api->quit = true;
      break;
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  is_different_size = new_rwidth != api->draw.size[0] || new_rheight != api->draw.size[1];
  is_not_zero = new_rwidth != 0 && new_rheight != 0;
  new_scale = (float)new_rwidth / (float)new_wwidth;

  if(!api->window.shown) {
    api->window.shown = 1;

    UpdateWindow((HWND)api->window.window_handle);
    // p_check_os_error();
    ShowWindow((HWND)api->window.window_handle, SW_SHOW);
    // p_check_os_error();
  }

#elif __APPLE__
  bool done = false;

  while (!done){
    // NSEvent * event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
    id distantPast = objc_msgSend_id((id)objc_getClass("NSDate"), sel_registerName("distantPast"));
    id event = ((id(*)(id, SEL, NSUInteger, id, id, BOOL))objc_msgSend)(NSApp, sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"), NSUIntegerMax, distantPast, NSDefaultRunLoopMode, YES);

    if(event){
      _p_on_cocoa_event(api, event);
    } else {
      done = true;
    }
  }

  if(_p_macos_terminated || _p_macos_is_window_closed(api)){
    api->quit = true;
    return;
  }


  NSSize render_size = {};
  NSSize window_size = {};
  float scale = 1.0;
  _p_macos_cocoa_window_size((id)api->window.window_handle, &render_size, &window_size, &scale);
  new_wwidth = window_size.width;
  new_wheight = window_size.height;
  new_rwidth = render_size.width;
  new_rheight = render_size.height;

  is_different_size = render_size.width != api->draw.size[0] || render_size.height != api->draw.size[1];
  is_not_zero = render_size.width != 0 && render_size.height != 0;
  new_scale = scale;

  id swapIntervalAlloc = objc_msgSend_id((id)objc_getClass("NSOpenGLCPSwapInterval"), sel_registerName("alloc"));
  // // we want to do this to set vsync correctly:
  // // [window->context.nsgl.object setValues:&sync forParameter:NSOpenGLCPSwapInterval];
  int vsync = api->window.vsync ? 1 : 0;
  ((id(*)(id, SEL, int, id))objc_msgSend)((id)api->window.gl_context, sel_registerName("setValues:forParameter:"), vsync, swapIntervalAlloc);

  // do runloop stuff:
  objc_msgSend_void(NSApp, sel_registerName("updateWindows"));

#endif

  if(is_different_size && is_not_zero && !api->draw.lock_size){
    // p_log("p_update new dims are: window w,h: %i,%i : render w,h: %i,%i\n", new_wwidth, new_wheight, new_rwidth, new_rheight);
    // p_log("updating window pixel scale in p_update\n");
    _p_update_window_size(api, new_rwidth, new_rheight, new_scale);
  }

  _p_handle_mouse(api);
}

void p_set_title(platform_api *api, const char *title) {
#ifdef _WIN32
  // wchar_t *wtitle = unicode(title);
  SetWindowTextA(api->window.window_handle, title);
#elif __APPLE__
  id titleString = objc_msgSend_id_const_char((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), title);
  objc_msgSend_void_id((id)api->window.window_handle, sel_registerName("setTitle:"), titleString);
#endif
}

// create window:
void p_create_window(platform_api *api, int w, int h, char *title, float scale){
  api->window.size[0] = w; p_log("setting w to %i\n", api->window.size[0]);
  api->window.size[1] = h; p_log("setting h to %i\n", api->window.size[1]);
  api->window.scale = scale > 0.0 ? scale : 1.0; // never set scale to 0
  api->window.title = title;

  // now, set the size of the rendertarget, based on scale.
  // 0,0 means fullscreen:

  bool is_full_screen = (w == 0 && h == 0); // 0,0 for size means fullscreen:
#ifdef __APPLE__

  // int scale;

  if (is_full_screen) {
    // See how big we can make it and still fit on-screen:
    CGRect mainMonitor = CGDisplayBounds(CGMainDisplayID());
    int maxW = CGRectGetWidth(mainMonitor);
    int maxH = CGRectGetHeight(mainMonitor);
    api->window.size[0] = w = maxW;
    api->window.size[1] = h = maxH;
  }

  /*
  // style mask options:
  NSWindowStyleMaskBorderless
  NSWindowStyleMaskTitled
  NSWindowStyleMaskClosable
  NSWindowStyleMaskMiniaturizable
  NSWindowStyleMaskResizable
  NSWindowStyleMaskTexturedBackground // DEPRECATED
  NSWindowStyleMaskUnifiedTitleAndToolbar
  NSWindowStyleMaskFullScreen
  NSWindowStyleMaskFullSizeContentView
  NSWindowStyleMaskUtilityWindow
  NSWindowStyleMaskDocModalWindow
  NSWindowStyleMaskNonactivatingPanel
  NSWindowStyleMaskHUDWindow
  */

  int styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

  NSRect rect = {{0, 0}, {(float)w, (float)h}};
  id windowAlloc = objc_msgSend_id((id)objc_getClass("NSWindow"), sel_registerName("alloc"));
  // (instancetype)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag;
  id window = ((id(*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(windowAlloc, sel_registerName("initWithContentRect:styleMask:backing:defer:"), rect, styleMask, NSBackingStoreBuffered, NO);

  api->window.window_handle = window;

  #ifndef ARC_AVAILABLE
    objc_msgSend_void(window, sel_registerName("autorelease"));
  #endif

  // when we are not using ARC, than window will be added to autorelease pool
  // so if we close it by hand (pressing red button), we don't want it to be
  // released for us so it will be released by autorelease pool later
  objc_msgSend_void_bool(window, sel_registerName("setReleasedWhenClosed:"), NO);

  Class WindowDelegateClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "WindowDelegate", 0);

  bool resultAddProtoc = class_addProtocol(WindowDelegateClass, objc_getProtocol("NSWindowDelegate"));
  assert(resultAddProtoc);

  bool resultAddIvar = class_addIvar(WindowDelegateClass, "closed", sizeof(NSUInteger), rint(log2(sizeof(NSUInteger))), NSUIntegerEncoding);
  assert(resultAddIvar);

  resultAddIvar = class_addIvar(WindowDelegateClass, "apiHandle", sizeof(void *), rint(log2(sizeof(void *))), "ˆv");
  assert(resultAddIvar);

  bool resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowWillClose:"), (IMP)_p_window_will_close, "v@:@");
  assert(resultAddMethod);

  resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowDidBecomeKey:"), (IMP)_p_window_did_become_key, "v@:@");
  assert(resultAddMethod);

  resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowDidResize:"), (IMP)_p_window_did_resize, "v@:@");
  assert(resultAddMethod);

  resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("viewDidChangeBackingProperties:"), (IMP)_p_view_did_change_backing_properties, "v@:@");
  assert(resultAddMethod);

  resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowDidMiniaturize:"), (IMP)_p_window_did_miniaturize, "v@:@");
  assert(resultAddMethod);

  resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowDidDeminiaturize:"), (IMP)_p_window_did_deminiaturize, "v@:@");
  assert(resultAddMethod);

  resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowDidChangeScreen:"), (IMP)_p_window_did_change_screen, "v@:@");
  assert(resultAddMethod);

  resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowDidMove:"), (IMP)_p_window_did_move, "v@:@");
  assert(resultAddMethod);

  id wdgAlloc = objc_msgSend_id((id)WindowDelegateClass, sel_registerName("alloc"));
  id wdg = objc_msgSend_id(wdgAlloc, sel_registerName("init"));

  #ifndef ARC_AVAILABLE
    objc_msgSend_void(wdg, sel_registerName("autorelease"));
  #endif

  objc_msgSend_void_id(window, sel_registerName("setDelegate:"), wdg);

  NSPoint point = {0, 0};
  ((void (*)(id, SEL, NSPoint))objc_msgSend)(window, sel_registerName("cascadeTopLeftFromPoint:"), point);

  id titleString = objc_msgSend_id_const_char((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), api->window.title);
  objc_msgSend_void_id(window, sel_registerName("setTitle:"), titleString);

  objc_msgSend_void_id(window, sel_registerName("makeKeyAndOrderFront:"), window);
  objc_msgSend_void_bool(window, sel_registerName("setAcceptsMouseMovedEvents:"), YES);

  id contentView = objc_msgSend_id(window, sel_registerName("contentView"));

  // [window makeFirstResponder:contentView];
  objc_msgSend_void_id(window, sel_registerName("makeFirstResponder:"), contentView);
  //[window performKeyEquivalent YES];
  objc_msgSend_void_bool(contentView, sel_registerName("performKeyEquivalent:"), YES);
  //[window acceptsFirstResponder YES];
  objc_msgSend_void_bool(contentView, sel_registerName("acceptsFirstResponder"), YES);

  // //@TODO @NOTE the NSView object we're creating here is currently emulating the
  // // NSOpenGLView NSView object, which holds the NSOpenGLPixelFormat and
  // // NSOpenGLContext objects for drawing with OpenGL.
  // // In the future, we should be emulating the MTKView object, for Metal
  // // rendering.
  // // source: https://developer.apple.com/videos/play/wwdc2019/611/
  //
  //   uint32_t glAttributes[] = {
  //       8, 24,//  NSOpenGLPFAColorSize, 24,
  //       11, 8,//  NSOpenGLPFAAlphaSize, 8,
  //       5,    //  NSOpenGLPFADoubleBuffer,
  //       73,   //  NSOpenGLPFAAccelerated,
  //       // 72,      //  NSOpenGLPFANoRecovery,
  //       // 55, 1,    //  NSOpenGLPFASampleBuffers, 1,
  //       // 56, 4,    //  NSOpenGLPFASamples, 4,
  //       99, 0x3200,//  NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
  //       0};
  //
  //   id pixelFormatAlloc = objc_msgSend_id((id)objc_getClass("NSOpenGLPixelFormat"), sel_registerName("alloc"));
  //   id pixelFormat = ((id(*)(id, SEL, const uint32_t *))objc_msgSend)(pixelFormatAlloc, sel_registerName("initWithAttributes:"), glAttributes);
  //
  // #ifndef ARC_AVAILABLE
  //   objc_msgSend_void(pixelFormat, sel_registerName("autorelease"));
  // #endif
  //
  //   id openGLContextAlloc = objc_msgSend_id((id)objc_getClass("NSOpenGLContext"), sel_registerName("alloc"));
  //   id openGLContext = ((id(*)(id, SEL, id, id))objc_msgSend)(openGLContextAlloc, sel_registerName("initWithFormat:shareContext:"), pixelFormat, nil);
  //
  // #ifndef ARC_AVAILABLE
  //   objc_msgSend_void(openGLContext, sel_registerName("autorelease"));
  // #endif
  //
  //   objc_msgSend_void_id(openGLContext, sel_registerName("setView:"), contentView);
  //


  id blackColor = objc_msgSend_id((id)objc_getClass("NSColor"), sel_registerName("blackColor"));
  objc_msgSend_void_id(window, sel_registerName("setBackgroundColor:"), blackColor);

  // @TODO do we really need this?
  objc_msgSend_void_bool(NSApp, sel_registerName("activateIgnoringOtherApps:"), YES);

  // Set the handle
  object_setInstanceVariable(wdg, "apiHandle", (void *)api);

  if(is_full_screen){
    objc_msgSend_void(window, sel_registerName("toggleFullScreen:"));
  }

  // For MacOS only, we're going to keep the scale for the life of the app - so
  // we should set scale here, since it won't get updated later:
  NSSize render_size = {};
  NSSize window_size = {};

  _p_macos_cocoa_window_size(window, &render_size, &window_size, &scale);
  // overwrite scale here with the macOS version, in case we're on retina:
  api->window.scale =  scale;

// WINDOWS VERSION:
#elif _WIN32

  WNDCLASSEXW wcex = {0};
  HWND hWnd;
  DWORD dwStyle;
  RECT rc;
  DWORD err;

  if (is_full_screen) {

  }

  api->window.wtitle = unicode(api->window.title);

  // Register a window class.
  wcex.cbSize = sizeof(WNDCLASSEXW);
  wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wcex.lpfnWndProc = p_api_WndProc;
  // wcex.lpfnWndProc = TestWindowProcedure;
  wcex.hInstance = GetModuleHandle(NULL);
  wcex.hIcon = NULL;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.lpszClassName = L"MY_P_API";
  RegisterClassExW(&wcex);

  // Always use a 1:1 pixel size.
  // @TODO figure out how to solve this better, so we can allow
  // people to render to a smaller/larger scale/density
    scale = 1.0;

  // Get the final window size.
  dwStyle = WS_OVERLAPPEDWINDOW;
  p_log("_p_create_window setting w, h, s to: %i, %i, %f\n", w, h, scale);
  rc.left = 0;
  rc.top = 0;
  rc.right = w * scale;
  rc.bottom = h * scale;
  p_log("_p_create_window setting right, bottom to: %li, %li\n", rc.right, rc.bottom);
  AdjustWindowRect(&rc, dwStyle, FALSE);

  // Make a window.
  hWnd = CreateWindowW(L"MY_P_API", api->window.wtitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
                       rc.right - rc.left, rc.bottom - rc.top, NULL, NULL,
                       wcex.hInstance, NULL);
  err = GetLastError();
  if(!hWnd) ExitProcess(1);

  api->window.window_handle = hWnd;

  // Set up the Windows parts.
  api->window.dwStyle = dwStyle;

  // sets the window struct as the p_API property in the OS window:
  SetPropW(hWnd, L"pAPI", api);


  // Try and restore our window position.
  if(api->window.preserve_window_pos){
    WINDOWPLACEMENT wp;
    DWORD wpsize = sizeof(wp);
    // Find our registry key.
    RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\P_API", 0, NULL,
                    REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &api->window.regKey,
                    NULL);

    if(RegQueryValueExW(api->window.regKey, api->window.wtitle, NULL, NULL, (BYTE *)&wp, &wpsize) ==
       ERROR_SUCCESS) {
      if(wp.showCmd == SW_MAXIMIZE){
        // Enter borderless windowed mode:
        // @TODO THERE'S AN ISSUE HERE, NEED TO RESOLVE
        // p_log("Just created window, entering borderless windowed mode\n");
        _p_enter_borderless_windowed(api);
      }
      else {
        p_log("Just created window, setting window placement\n");
        SetWindowPlacement(hWnd, &wp);
      }
    }
  }
/*
  // now, let's set up audio:
  // 1. Load the dsound library
  HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

  // 2. Get a DirectSound object - cooperative
  GetProcAddress(DSoundLibrary, "DirectSoundCreate");

  int SamplesPerSecond = 48000;
  int BufferSize = SamplesPerSecond * sizeof(uint16)*2;

  LPDIRECTSOUND DirectSound;
  if(DirectSoundCreate(8, &DirectSound, 8) == ERROR_SUCCESS){

    WAVEFORMATEX WaveFormat = {};
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 2;
    WaveFormat.nSamplesPerSec = SamplesPerSecond;
    WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
    WaveFormat.wBitsPerSample = 16;
    WaveFormat.cbSize = 8;

    if(DirectSound->SetCooperativeLevel(api->window.window_handle, DSSCL_PRIORITY) == ERROR_SUCCESS){

      // 3. "Create" a primary buffer
      LPDIRECTSOUNDBUFFER PrimaryBuffer;
      DSBUFFERDESC BufferDescription = {};
      BufferDescription.dwSize = sizeof(BufferDescription);
      BufferDescription.dwFlags = DSBCAPS_PRIMARY_BUFFER;

      if(DirectSound->CreateSoundBuffer(BufferDescription, &PrimaryBuffer, 8) == ERROR_SUCCESS){
        HRESULT Error = PrimaryBuffer->SetFormat(&WaveFormat);
        if(Error == ERROR_SUCCESS){
          p_log("Primary sound buffer created successfully!");
        } else {
          // error!
        }
      } else {
        // error!
      }
    } else {
      // error!
    }

    // 4. "Create" a secondary buffer
    // DSBCAPS_GETCURRENTPOSITION2???
    LPDIRECTSOUNDBUFFER SecondaryBuffer;
    DSBUFFERDESC BufferDescription = {};
    BufferDescription.dwSize = sizeof(BufferDescription);
    BufferDescription.dwFlags = DSBCAPS_PRIMARY_BUFFER;
    BufferDescription.dwBufferBytes = BufferSize;
    BufferDescription.lpwfxFormat = &WaveFormat;
    HRESULT Error = DirectSound->CreateSoundBuffer(BufferDescription, &SecondaryBuffer, 8);
    if(Error == ERROR_SUCCESS){
      p_log("Secondary sound buffer created successfully!");
      // 5. Start it playing!
    }

  } else {

  }
*/
#endif

  #ifdef PLATFORM_USE_OPENGL
    _p_opengl_window_setup(api);
  #endif
  #ifdef PLATFORM_USE_METAL
    _p_metal_window_setup(api);
  #endif

  // set the scale here, because we may have just set some settings previously that changes the
  // rendertarget scale:
  api->draw.size[0] = w * api->window.scale;
  api->draw.size[1] = h * api->window.scale;
  p_log("set draw size to: %i, %i\n", api->draw.size[0], api->draw.size[1]);
  p_log("scale is %f\n", api->window.scale);
  api->draw.x = api->draw.size[0];
  api->draw.y = api->draw.size[1];

  api->window.aspect = (float)api->window.size[0] / (float)api->window.size[1];

  p_update(api);
}


/***********************************************************************************************************************
  Extra stuff, just useful things to have (possibly some stb ripoffs, ahem):
***********************************************************************************************************************/

#if defined(_WIN32) && defined(__STDC_WANT_SECURE_LIB__)
  #include <dbghelp.h>
#else
  #include <execinfo.h>// for backtrace
#endif
void p_stacktrace(){
#ifdef _WIN32
  unsigned int i;
  void *stack[100];
  unsigned short frames;
  SYMBOL_INFO *symbol;
  HANDLE process;

  process = GetCurrentProcess();
  SymInitialize(process, NULL, TRUE);

  frames = CaptureStackBackTrace(0, 100, stack, NULL);
  symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
  symbol->MaxNameLen = 255;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

  p_log(YELLOW "----\n");
  for(i = 0; i < frames; i++) {
    SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
    p_log("%i: %s - 0x%0llX\n", frames - i - 1, symbol->Name, symbol->Address);
  }
  p_log(RESET "----\n");

  free(symbol);
#elif __APPLE__
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  p_log(YELLOW "----\n");
  // print out all the frames to log
  backtrace_symbols_fd(array, size, STDOUT_FILENO);
  p_log(RESET "----\n");
#endif
}

#if defined(_WIN32) && defined(__STDC_WANT_SECURE_LIB__)

char *p_strcat_s(char *a, size_t size, const char *b){
   strcat_s(a,size,b);
   return a;
}
static char *p_strcpy_s(char *a, size_t size, const char *b){
  strcpy_s(a,size,b);
  return a;
}

// static char *p_strncpy_s(char *a, size_t size, const char *b, size_t count){
//   strncpy_s(a,size,b,count);
//   return a;
// }

#define p_mktemp(s)  (_mktemp_s(s, strlen(s)+1) == 0)
#define p_sprintf    sprintf_s
#define p_size(x)    ,(x)

#define p_sscanf_s sscanf_s

#ifdef _MSC_VER
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

void handler(int sig) {
  p_flog(stderr, "Error: signal %d:\n", sig);
  p_stacktrace();
  exit(1);
}

static FILE *p_fopen(const char *filename, const char *mode) {
  FILE *f;
  if (0 == fopen_s(&f, filename, mode))
    return f;
  else
    return NULL;
}
#else
#include <unistd.h>// for STDERR_FILENO

char *p_strcat_s(char *a, size_t size, const char *b){
   p_snlog(a, size, "%s%s", a, b);
   return a;
}
// #define p_strcat_s strcat;
#define p_sscanf_s sscanf

#define p_fopen fopen
#define p_wfopen _wfopen
#define p_strcpy_s(a,s,b) strcpy(a,b)
// #define p_strncpy_s(a,s,b,c) strncpy(a,b,c)
#define p_mktemp(s) (mktemp(s) != NULL)

#define p_sprintf sprintf
#define p_size(x)

void handler(int sig) {
  p_flog(stderr, "Error: signal %d:\n", sig);
  p_stacktrace();
  exit(1);
}

#define p_fopen fopen
#endif

/* Tiny function that just checks if the filename passed has the extension passed */
bool has_extension(char const *name, char const *ext) {
  int len = strlen(name);
  int extLen = strlen(ext);

  return len > extLen && strcmp(name + len - extLen, ext) == 0;
}

bool has_txt_extension(char const *name){
  int len = strlen(name);
  return len > 4 && strcmp(name + len - 4, ".txt") == 0;
}

char *remove_ext(char *mystr, char dot, char sep) {
  char *retstr, *lastdot, *lastsep;

  // Error checks and allocate string.

  if (mystr == NULL) {
    return NULL;
  }

  if ((retstr = malloc(strlen(mystr) + 1)) == NULL) {
    return NULL;
  }

  // Make a copy and find the relevant characters.
  // probably switch to use stb_strncpy
  p_strcpy_s(retstr, strlen(retstr) + 1, mystr);
  lastdot = strrchr(retstr, dot);
  lastsep = (sep == 0) ? NULL : strrchr(retstr, sep);

  // If it has an extension separator.

  if (lastdot != NULL) {
    // and it's before the extenstion separator.

    if (lastsep != NULL) {
      if (lastsep < lastdot) {
        // then remove it.
        *lastdot = '\0';
      }
    } else {
      // Has extension separator with no path separator.
      *lastdot = '\0';
    }
  }

  // Return the modified string.
  return retstr;
}

/* A simple function that will read a file into an allocated char pointer buffer */
char *p_filetobuf(char *file) {
  FILE *fptr;
  long length;
  char *buf;
  // p_log("running filetobuf for %s\n", file);
  fptr = p_fopen(file, "r"); /* Open file for reading */
  if(!fptr) {                    /* Return NULL on failure */
    p_flog(stderr, "%s %s\n", "failed to read file:", file);
    return NULL;
  }
  fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
  length = ftell(fptr);     /* Find out how many bytes into the file we are */
  buf = (char *)malloc(length + 1); /* Allocate a buffer for the entire length
                                       of the file and a null terminator */
  memset(buf, 0, length + 1); /* clear the buffer in case we have garbage from previous memory */
  fseek(fptr, 0, SEEK_SET);         /* Go back to the beginning of the file */
  fread(buf, length, 1,
        fptr);     /* Read the contents of the file in to the buffer */
  fclose(fptr);    /* Close the file */
  buf[length] = 0; /* Null terminator */
  return buf;      /* Return the buffer */
}

int p_strlen(char *text){
  int count = 0;
  while (text[count] != '\0'){ // string must be null-terminated
    count++;
  }

  return count;
}

// public domain Mersenne Twister by Michael Brundage
#define P__MT_LEN       624

int p__mt_index = P__MT_LEN*sizeof(int)+1;
unsigned int p__mt_buffer[P__MT_LEN];

void p_srand(unsigned int seed){
   int i;
   p__mt_buffer[0]= seed & 0xffffffffUL;
   for (i=1 ; i < P__MT_LEN; ++i)
      p__mt_buffer[i] = (1812433253UL * (p__mt_buffer[i-1] ^ (p__mt_buffer[i-1] >> 30)) + i);
   p__mt_index = P__MT_LEN*sizeof(unsigned int);
}

#define P__MT_IA           397
#define P__MT_IB           (P__MT_LEN - P__MT_IA)
#define P__UPPER_MASK      0x80000000
#define P__LOWER_MASK      0x7FFFFFFF
#define P__MATRIX_A        0x9908B0DF
#define P__TWIST(b,i,j)    ((b)[i] & P__UPPER_MASK) | ((b)[j] & P__LOWER_MASK)
#define P__MAGIC(s)        (((s)&1)*P__MATRIX_A)

unsigned int p_rand(){
  unsigned int  * b = p__mt_buffer;
  unsigned long idx = p__mt_index;
  unsigned int  s,r;
  int i;

  if (idx >= P__MT_LEN*sizeof(unsigned int)) {
    if (idx > P__MT_LEN*sizeof(unsigned int))
       p_srand(0);
    idx = 0;
    i = 0;
    for (; i < P__MT_IB; i++) {
       s = P__TWIST(b, i, i+1);
       b[i] = b[i + P__MT_IA] ^ (s >> 1) ^ P__MAGIC(s);
    }
    for (; i < P__MT_LEN-1; i++) {
       s = P__TWIST(b, i, i+1);
       b[i] = b[i - P__MT_IB] ^ (s >> 1) ^ P__MAGIC(s);
    }

    s = P__TWIST(b, P__MT_LEN-1, 0);
    b[P__MT_LEN-1] = b[P__MT_IA-1] ^ (s >> 1) ^ P__MAGIC(s);
  }
  p__mt_index = idx + sizeof(unsigned int);

  r = *(unsigned int *)((unsigned char *)b + idx);

  r ^= (r >> 11);
  r ^= (r << 7) & 0x9D2C5680;
  r ^= (r << 15) & 0xEFC60000;
  r ^= (r >> 18);

  return r;
}

void vec3_on_sphere(float u1, float u2, float out_vec[]){
  float latitude = acos(2.0 * u1 - 1.0) - (PI / 2.0);
  float longitude = 2.0 * PI * u2;

  out_vec[0] = cos(latitude) * cos(longitude);
  out_vec[1] = cos(latitude) * sin(longitude);
  out_vec[2] = sin(latitude);
}

// defaults here, should be overwritten in main program:
#define FLOAT_POOL_SIZE Kilobytes(128)
#define INT_POOL_SIZE Kilobytes(128)

typedef struct pool {
  char * next;
  char * end;
} POOL;

// POOL *float_pool = pool_create(FLOAT_POOL_SIZE);
POOL * pool_create( size_t size ) {
    POOL * p = (POOL*)malloc( size + sizeof(POOL) );
    p->next = (char*)&p[1];
    p->end = p->next + size;
    return p;
}
// pool_destroy(float_pool);
void pool_destroy( POOL *p ) { free(p); }

// size_t mem_left = pool_available(float_pool);
size_t pool_available( POOL *p ) { return p->end - p->next; }

// void *vertex_arr = pool_alloc(float_pool, sizeof(float) * vertex_count * 3);
void * pool_alloc( POOL *p, size_t size ) {
    if( pool_available(p) < size ){
      printf("POOL not large enough for this pool_alloc attempt!\n");
      return NULL;
    }

    void *mem = (void*)p->next;
    p->next += size;

    return mem;
}

// usage above to allocate new space, but to actually _set_ the data:
//
