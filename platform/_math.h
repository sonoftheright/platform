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
static inline int p_vec##n##_is_equal(vec##n to, vec##n from) \
{ \
  int i; \
  int isEqual = 1; \
  for(i=0; i<n; ++i){ \
    if(to[i] != from[i]){ \
      isEqual = 0; \
      break; \
    } \
  } \
  return isEqual; \
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

#define p_quat_is_equal p_vec4_is_equal
#define p_quat_set p_vec4_set
#define p_quat_debug p_vec4_debug

#define p_lerp(t,a,b)               ( (a) + (t) * (float) ((b)-(a)) )
#define p_unlerp(t,a,b)             ( ((t) - (a)) / (float) ((b) - (a)) )

#define p_clamp(x,xmin,xmax)  ((x) < (xmin) ? (xmin) : (x) > (xmax) ? (xmax) : (x))

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
