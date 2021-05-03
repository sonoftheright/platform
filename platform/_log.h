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
