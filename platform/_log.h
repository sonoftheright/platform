#define p_log printf
#define p_vsnlog vsnprintf
#define p_snlog snprintf
#define p_flog fprintf
#define p_wlog wprintf

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
#define p_line_inline() p_log(CYAN "line: " BOLDCYAN "%i" RESET, __LINE__)

void p_prevent_output(){ fclose(stdout); }

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
