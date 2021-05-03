/***********************************************************************************************************************
  Extra stuff, just useful things to have (possibly some stb ripoffs, ahem):
***********************************************************************************************************************/

#define PI         3.14159265   // The value of pi
#define Kilobytes(Value) ( (Value) * 1024 )
#define Megabytes(Value) ( Kilobytes(Value) * 1024 )
#define Gigabytes(Value) ( Megabytes(Value) * 1024 )

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

void p_handler(int sig) {
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

void p_handler(int sig) {
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
