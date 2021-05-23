#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
  #define _CRT_SECURE_NO_WARNINGS NOPE
#endif

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
  // #include <DSound.h> // may require linking?
  #include <sysinfoapi.h>


  // typedef long GLintptr;
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

    #ifdef __OBJC__
      // #import <Cocoa/Cocoa.h>
    #endif

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


#include "_log.h" // for p_log functions
#include "_glext.h" // OpenGL constants & function names
#include "_defines.h" // for all OpenGL constants & function names

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

#include "_math.h"

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

  // [[ NSOpenGLPixelFormat alloc ] sel_initWithAttributes:attribs];
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

  // id swapIntervalAlloc = objc_msgSend_id((id)objc_getClass("NSOpenGLCPSwapInterval"), sel_registerName("alloc"));
  id swapIntervalAlloc = [ objc_getClass("NSOpenGLCPSwapInterval") alloc ];

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

  id metalLayer = [[objc_getClass("CAMetalLayer") alloc] init];
  // id CAMetalLayerAlloc = objc_msgSend_id((id)objc_getClass("CAMetalLayer"), sel_registerName("alloc"));
  // id metalLayer = objc_msgSend_id(CAMetalLayerAlloc, sel_registerName("init"));

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
      // id NSAutoreleasePool = objc_getClass("NSAutoreleasePool");
      // global_autoreleasePool = [[NSAutoreleasePool alloc] init];
      id poolAlloc = objc_msgSend_id((id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc"));
      global_autoreleasePool = objc_msgSend_id(poolAlloc, sel_registerName("init"));
      api->window.autoreleasepool = global_autoreleasePool;

      atexit(&_p_clean_up_macos);
    #endif
      // id NSApplication = objc_getClass("NSApplication");
      // SEL sApplication = sel_registerName("sharedApplication");
      // [NSApplication sApplication];
      objc_msgSend_id((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
      ((void (*)(id, SEL, NSInteger))objc_msgSend)(NSApp, sel_registerName("setActivationPolicy:"), 0);

      Class appDelegateClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "AppDelegate", 0);
      bool resultAddProtoc = class_addProtocol(appDelegateClass, objc_getProtocol("NSApplicationDelegate"));
      assert(resultAddProtoc);
      bool resultAddMethod = class_addMethod(appDelegateClass, sel_registerName("_p_macos_app_should_terminate:"), (IMP)_p_macos_app_should_terminate, NSUIntegerEncoding "@:@");
      assert(resultAddMethod);

      id dg = [[appDelegateClass alloc] init];
      // id dgAlloc = objc_msgSend_id((id)appDelegateClass, sel_registerName("alloc"));
      // id dg = objc_msgSend_id(dgAlloc, sel_registerName("init"));

    #ifndef ARC_AVAILABLE
      objc_msgSend_void(dg, sel_registerName("autorelease"));
    #endif
      // [NSApp setDelegate:dg];
      objc_msgSend_void_id(NSApp, sel_registerName("setDelegate:"), dg);
      objc_msgSend_void(NSApp, sel_registerName("finishLaunching"));

      id menubar = [[objc_getClass("NSMenu") alloc] init];
      // id menubarAlloc = objc_msgSend_id((id)objc_getClass("NSMenu"), sel_registerName("alloc"));
      // id menubar = objc_msgSend_id(menubarAlloc, sel_registerName("init"));
    #ifndef ARC_AVAILABLE
      objc_msgSend_void(menubar, sel_registerName("autorelease"));
    #endif

      id appMenuItem = [[objc_getClass("NSMenuItem") alloc] init];
      // id appMenuItemAlloc = objc_msgSend_id((id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
      // id appMenuItem = objc_msgSend_id(appMenuItemAlloc, sel_registerName("init"));

    #ifndef ARC_AVAILABLE
      objc_msgSend_void(appMenuItem, sel_registerName("autorelease"));
    #endif
      // [menubar addItem:appMenuItem];
      objc_msgSend_void_id(menubar, sel_registerName("addItem:"), appMenuItem);
      ((id(*)(id, SEL, id))objc_msgSend)(NSApp, sel_registerName("setMainMenu:"), menubar);

      // [[NSMenu alloc] init]
      // id appMenuAlloc = objc_msgSend_id((id)objc_getClass("NSMenu"), sel_registerName("alloc"));
      // id appMenu = objc_msgSend_id(appMenuAlloc, sel_registerName("init"));
      id appMenu = [[objc_getClass("NSMenu") alloc] init];

    #ifndef ARC_AVAILABLE
      objc_msgSend_void(appMenu, sel_registerName("autorelease"));
    #endif

      // id appName = [[NSProcessInfo processInfo] processName];
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
      // objc_msgSend_void((id)objc_getClass("NSOpenGLContext"), sel_registerName("clearCurrentContext"));
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

#include "_camera.h"
#include "_timer.h"

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

  // id swapIntervalAlloc = objc_msgSend_id((id)objc_getClass("NSOpenGLCPSwapInterval"), sel_registerName("alloc"));
  id swapIntervalAlloc =  [ objc_getClass("NSOpenGLCPSwapInterval") alloc ];

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
  // id windowAlloc = [ NSWindow alloc ];
  // (instancetype)initWithContentRect:(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag;
  id window = ((id(*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(windowAlloc, sel_registerName("initWithContentRect:styleMask:backing:defer:"), rect, styleMask, NSBackingStoreBuffered, NO);
  // id window = [[NSWindow alloc] initWithContentRect:rect styleMask:styleMask backing:NSBackingStoreBuffered defer:NO];


  api->window.window_handle = window;

  #ifndef ARC_AVAILABLE
    [window autorelease];
    // objc_msgSend_void(window, sel_registerName("autorelease"));
  #endif

  // when we are not using ARC, than window will be added to autorelease pool
  // so if we close it by hand (pressing red button), we don't want it to be
  // released for us so it will be released by autorelease pool later
  // [window setReleasedWhenClosed:NO];
	((void (*)(id, SEL, BOOL))objc_msgSend)(window, sel_registerName("setReleasedWhenClosed:"), NO);

  Class WindowDelegateClass = objc_allocateClassPair([NSObject class], "WindowDelegate", 0);

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

  // id wdgAlloc = objc_msgSend_id((id)WindowDelegateClass, sel_registerName("alloc"));
  // id wdg = objc_msgSend_id(wdgAlloc, sel_registerName("init"));
  id wdg = [[WindowDelegateClass alloc] init];

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
  // starting on audio in windows, but commenting out for now:

  #define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN  pUnkOuter )
  typedef DIRECT_SOUND_CREATE(direct_sound_create);

  // now, let's set up audio:
  // 1. Load the dsound library
  HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
  if(!DSoundLibrary){

  }

  // 2. Get a DirectSound object - cooperative
  direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

  int SamplesPerSecond = 48000;
  int BufferSize = SamplesPerSecond * sizeof(int16)*2;

  LPDIRECTSOUND DirectSound;
  if(DirectSoundCreate(8, &DirectSound, 8) == ERROR_SUCCESS){

    WAVEFORMATEX WaveFormat = {};
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 2;
    WaveFormat.nSamplesPerSec = SamplesPerSecond;
    WaveFormat.wBitsPerSample = 16;
    WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
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
