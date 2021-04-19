#ifndef LINMATH_H
#define LINMATH_H

#include <math.h>
#include <float.h>

#define LINMATH_H_DEFINE_VEC(n) \
typedef float vec##n[n]; \
static inline void vec##n##_add(vec##n r, vec##n const a, vec##n const b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i] + b[i]; \
} \
static inline void vec##n##_sub(vec##n r, vec##n const a, vec##n const b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i] - b[i]; \
} \
static inline void vec##n##_scale(vec##n r, vec##n const v, float const s) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = v[i] * s; \
} \
static inline float vec##n##_mul_inner(vec##n const a, vec##n const b) \
{ \
  float p = 0.; \
  int i; \
  for(i=0; i<n; ++i) \
  p += b[i]*a[i]; \
  return p; \
} \
static inline float vec##n##_len(vec##n const v) \
{ \
  return sqrtf(vec##n##_mul_inner(v,v)); \
} \
static inline void vec##n##_norm(vec##n r, vec##n const v) \
{ \
  float k = 1.0 / vec##n##_len(v); \
  vec##n##_scale(r, v, k); \
} \
static inline void vec##n##_min(vec##n r, vec##n a, vec##n b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i]<b[i] ? a[i] : b[i]; \
} \
static inline void vec##n##_max(vec##n r, vec##n a, vec##n b) \
{ \
  int i; \
  for(i=0; i<n; ++i) \
  r[i] = a[i]>b[i] ? a[i] : b[i]; \
}

LINMATH_H_DEFINE_VEC(2)
LINMATH_H_DEFINE_VEC(3)
LINMATH_H_DEFINE_VEC(4)

static inline void vec3_mul_cross(vec3 r, vec3 const a, vec3 const b)
{
  r[0] = a[1]*b[2] - a[2]*b[1];
  r[1] = a[2]*b[0] - a[0]*b[2];
  r[2] = a[0]*b[1] - a[1]*b[0];
}

static inline void vec3_reflect(vec3 r, vec3 const v, vec3 const n)
{
  float p  = 2.f*vec3_mul_inner(v, n);
  int i;
  for(i=0;i<3;++i)
  r[i] = v[i] - p*n[i];
}

static inline void vec4_mul_cross(vec4 r, vec4 a, vec4 b)
{
  r[0] = a[1]*b[2] - a[2]*b[1];
  r[1] = a[2]*b[0] - a[0]*b[2];
  r[2] = a[0]*b[1] - a[1]*b[0];
  r[3] = 1.f;
}

static inline void vec4_reflect(vec4 r, vec4 v, vec4 n)
{
  float p  = 2.f*vec4_mul_inner(v, n);
  int i;
  for(i=0;i<4;++i)
  r[i] = v[i] - p*n[i];
}

typedef vec4 mat4x4[4];
static inline void mat4x4_identity(mat4x4 M)
{
  int i, j;
  for(i=0; i<4; ++i)
  for(j=0; j<4; ++j)
  M[i][j] = i==j ? 1.f : 0.f;
}
static inline void mat4x4_dup(mat4x4 M, mat4x4 N)
{
  int i, j;
  for(i=0; i<4; ++i)
  for(j=0; j<4; ++j)
  M[i][j] = N[i][j];
}
static inline void mat4x4_row(vec4 r, mat4x4 M, int i)
{
  int k;
  for(k=0; k<4; ++k)
  r[k] = M[k][i];
}
static inline void mat4x4_col(vec4 r, mat4x4 M, int i)
{
  int k;
  for(k=0; k<4; ++k)
  r[k] = M[i][k];
}
static inline void mat4x4_transpose(mat4x4 M, mat4x4 N)
{
  int i, j;
  for(j=0; j<4; ++j)
  for(i=0; i<4; ++i)
  M[i][j] = N[j][i];
}
static inline void mat4x4_add(mat4x4 M, mat4x4 a, mat4x4 b)
{
  int i;
  for(i=0; i<4; ++i)
  vec4_add(M[i], a[i], b[i]);
}
static inline void mat4x4_sub(mat4x4 M, mat4x4 a, mat4x4 b)
{
  int i;
  for(i=0; i<4; ++i)
  vec4_sub(M[i], a[i], b[i]);
}
static inline void mat4x4_scale(mat4x4 M, mat4x4 a, float k)
{
  int i;
  for(i=0; i<4; ++i)
  vec4_scale(M[i], a[i], k);
}
static inline void mat4x4_scale_aniso(mat4x4 M, mat4x4 a, float x, float y, float z)
{
  int i;
  vec4_scale(M[0], a[0], x);
  vec4_scale(M[1], a[1], y);
  vec4_scale(M[2], a[2], z);
  for(i = 0; i < 4; ++i) {
  M[3][i] = a[3][i];
  }
}
static inline void mat4x4_mul(mat4x4 M, mat4x4 a, mat4x4 b)
{
  mat4x4 temp;
  int k, r, c;
  for(c=0; c<4; ++c) for(r=0; r<4; ++r) {
  temp[c][r] = 0.f;
  for(k=0; k<4; ++k)
  temp[c][r] += a[k][r] * b[c][k];
  }
  mat4x4_dup(M, temp);
}
static inline void mat4x4_mul_vec4(vec4 r, mat4x4 M, vec4 v)
{
  int i, j;
  for(j=0; j<4; ++j) {
  r[j] = 0.f;
  for(i=0; i<4; ++i)
  r[j] += M[i][j] * v[i];
  }
}
static inline void mat4x4_translate(mat4x4 T, float x, float y, float z)
{
  mat4x4_identity(T);
  T[3][0] = x;
  T[3][1] = y;
  T[3][2] = z;
}
static inline void mat4x4_translate_in_place(mat4x4 M, float x, float y, float z)
{
  vec4 t = {x, y, z, 0};
  vec4 r;
  int i;
  for (i = 0; i < 4; ++i) {
  mat4x4_row(r, M, i);
  M[3][i] += vec4_mul_inner(r, t);
  }
}
static inline void mat4x4_from_vec3_mul_outer(mat4x4 M, vec3 a, vec3 b)
{
  int i, j;
  for(i=0; i<4; ++i) for(j=0; j<4; ++j)
  M[i][j] = i<3 && j<3 ? a[i] * b[j] : 0.f;
}
static inline void mat4x4_rotate(mat4x4 R, mat4x4 M, float x, float y, float z, float angle)
{
  float s = sinf(angle);
  float c = cosf(angle);
  vec3 u = {x, y, z};

  if(vec3_len(u) > 1e-4) {
  vec3_norm(u, u);
  mat4x4 T;
  mat4x4_from_vec3_mul_outer(T, u, u);

  mat4x4 S = {
  {    0,  u[2], -u[1], 0},
  {-u[2],     0,  u[0], 0},
  { u[1], -u[0],     0, 0},
  {    0,     0,     0, 0}
  };
  mat4x4_scale(S, S, s);

  mat4x4 C;
  mat4x4_identity(C);
  mat4x4_sub(C, C, T);

  mat4x4_scale(C, C, c);

  mat4x4_add(T, T, C);
  mat4x4_add(T, T, S);

  T[3][3] = 1.;
  mat4x4_mul(R, M, T);
  } else {
  mat4x4_dup(R, M);
  }
}
static inline void mat4x4_rotate_X(mat4x4 Q, mat4x4 M, float angle)
{
  float s = sinf(angle);
  float c = cosf(angle);
  mat4x4 R = {
  {1.f, 0.f, 0.f, 0.f},
  {0.f,   c,   s, 0.f},
  {0.f,  -s,   c, 0.f},
  {0.f, 0.f, 0.f, 1.f}
  };
  mat4x4_mul(Q, M, R);
}
static inline void mat4x4_rotate_Y(mat4x4 Q, mat4x4 M, float angle)
{
  float s = sinf(angle);
  float c = cosf(angle);
  mat4x4 R = {
  {   c, 0.f,   s, 0.f},
  { 0.f, 1.f, 0.f, 0.f},
  {  -s, 0.f,   c, 0.f},
  { 0.f, 0.f, 0.f, 1.f}
  };
  mat4x4_mul(Q, M, R);
}
static inline void mat4x4_rotate_Z(mat4x4 Q, mat4x4 M, float angle)
{
  float s = sinf(angle);
  float c = cosf(angle);
  mat4x4 R = {
  {   c,   s, 0.f, 0.f},
  {  -s,   c, 0.f, 0.f},
  { 0.f, 0.f, 1.f, 0.f},
  { 0.f, 0.f, 0.f, 1.f}
  };
  mat4x4_mul(Q, M, R);
}
static inline void mat4x4_invert(mat4x4 T, mat4x4 M)
{
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
static inline void mat4x4_orthonormalize(mat4x4 R, mat4x4 M)
{
  mat4x4_dup(R, M);
  float s = 1.;
  vec3 h;

  vec3_norm(R[2], R[2]);

  s = vec3_mul_inner(R[1], R[2]);
  vec3_scale(h, R[2], s);
  vec3_sub(R[1], R[1], h);
  vec3_norm(R[2], R[2]);

  s = vec3_mul_inner(R[1], R[2]);
  vec3_scale(h, R[2], s);
  vec3_sub(R[1], R[1], h);
  vec3_norm(R[1], R[1]);

  s = vec3_mul_inner(R[0], R[1]);
  vec3_scale(h, R[1], s);
  vec3_sub(R[0], R[0], h);
  vec3_norm(R[0], R[0]);
}

static inline void mat4x4_frustum(mat4x4 M, float l, float r, float b, float t, float n, float f)
{
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
static inline void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f)
{
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
static inline void mat4x4_perspective(mat4x4 m, float y_fov, float aspect, float n, float f)
{
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
static inline void mat4x4_look_at(mat4x4 m, vec3 eye, vec3 center, vec3 up)
{
  /* Adapted from Android's OpenGL Matrix.java.                        */
  /* See the OpenGL GLUT documentation for gluLookAt for a description */
  /* of the algorithm. We implement it in a straightforward way:       */

  /* TODO: The negation of of can be spared by swapping the order of
   *       operands in the following cross products in the right way. */
  vec3 f;
  vec3_sub(f, center, eye);
  vec3_norm(f, f);

  vec3 s;
  vec3_mul_cross(s, f, up);
  vec3_norm(s, s);

  vec3 t;
  vec3_mul_cross(t, s, f);

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

  mat4x4_translate_in_place(m, -eye[0], -eye[1], -eye[2]);
}

typedef float quat[4];
static inline void quat_identity(quat q)
{
  q[0] = q[1] = q[2] = 0.f;
  q[3] = 1.f;
}
static inline void quat_add(quat r, quat a, quat b)
{
  int i;
  for(i=0; i<4; ++i)
  r[i] = a[i] + b[i];
}
static inline void quat_sub(quat r, quat a, quat b)
{
  int i;
  for(i=0; i<4; ++i)
  r[i] = a[i] - b[i];
}
static inline void quat_mul(quat r, quat p, quat q)
{
  vec3 w;
  vec3_mul_cross(r, p, q);
  vec3_scale(w, p, q[3]);
  vec3_add(r, r, w);
  vec3_scale(w, q, p[3]);
  vec3_add(r, r, w);
  r[3] = p[3]*q[3] - vec3_mul_inner(p, q);
}
static inline void quat_scale(quat r, quat v, float s)
{
  int i;
  for(i=0; i<4; ++i)
  r[i] = v[i] * s;
}
static inline float quat_inner_product(quat a, quat b)
{
  float p = 0.f;
  int i;
  for(i=0; i<4; ++i)
  p += b[i]*a[i];
  return p;
}
static inline void quat_conj(quat r, quat q)
{
  int i;
  for(i=0; i<3; ++i)
  r[i] = -q[i];
  r[3] = q[3];
}
static inline void quat_rotate(quat r, float angle, vec3 axis) {
  vec3 v;
  vec3_scale(v, axis, sinf(angle / 2));
  int i;
  for(i=0; i<3; ++i)
  r[i] = v[i];
  r[3] = cosf(angle / 2);
}
#define quat_norm vec4_norm
static inline void quat_mul_vec3(vec3 r, quat q, vec3 v)
{
/*
 * Method by Fabian 'ryg' Giessen (of Farbrausch)
t = 2 * cross(q.xyz, v)
v' = v + q.w * t + cross(q.xyz, t)
 */
  vec3 t;
  vec3 q_xyz = {q[0], q[1], q[2]};
  vec3 u = {q[0], q[1], q[2]};

  vec3_mul_cross(t, q_xyz, v);
  vec3_scale(t, t, 2);

  vec3_mul_cross(u, q_xyz, t);
  vec3_scale(t, t, q[3]);

  vec3_add(r, v, t);
  vec3_add(r, r, u);
}
static inline void mat4x4_from_quat(mat4x4 M, quat q)
{
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

static inline void mat4x4o_mul_quat(mat4x4 R, mat4x4 M, quat q)
{
/*  XXX: The way this is written only works for othogonal matrices. */
/* TODO: Take care of non-orthogonal case. */
  quat_mul_vec3(R[0], q, M[0]);
  quat_mul_vec3(R[1], q, M[1]);
  quat_mul_vec3(R[2], q, M[2]);

  R[3][0] = R[3][1] = R[3][2] = 0.f;
  R[3][3] = 1.f;
}
static inline void quat_from_mat4x4(quat q, mat4x4 M)
{
  float r=0.f;
  int i;

  int perm[] = { 0, 1, 2, 0, 1 };
  int *p = perm;

  for(i = 0; i<3; i++) {
  float m = M[i][i];
  if( m < r )
  continue;
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


float to_radians(float degrees){
  float rad = 3.1415926535/180;
  return degrees * rad;
}

void set_vec3(vec3 to, vec3 from){
  to[0] = from[0];
  to[1] = from[1];
  to[2] = from[2];
}

float vec3_dot(vec3 a, vec3 b){
   return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

void vec3_length(float *out, vec3 input){
  *out =
      sqrt((input[0] * input[0]) + (input[1] * input[1]) + (input[2] * input[2]));
}

void debug_vec3(vec3 input){
  printf("vec3{%f, %f, %f}\n", input[0], input[1], input[2]);
}

// from https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
// and https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf

// @NOTE This requires triangles to be ordered clockwise, otherwise it will end early
// due to determinant being less than 0.
// int ray_intersects_triangle(vec3 rayOrigin,
//                            vec3 rayVector,
//                            vec3 inTriangle[],
//                            vec3 outIntersectionPoint){
// 		static bool intersects = true;
// 		bool changed = false;
//
//     vec3 edge1, edge2, h, s, q;
//     float determinant,f,u,v,sh_dot,rayq_dot;
//
//    // find vectors for two edges sharing vert:
// 		vec3_sub(edge1, inTriangle[1], inTriangle[0]);
// 		vec3_sub(edge2, inTriangle[2], inTriangle[0]);
//
// 		// begin calculating determinant, also used to calculate U parameter:
//     vec3_mul_cross(h, rayVector, edge2);
// 		// if determinant is near zero ray lies in plane of triangle
// 		determinant = vec3_dot(edge1, h);
//
//     // if (determinant > -EPSILON && determinant < EPSILON){
//     //   printf("ray is parallel to triangle\n");
// 		// 	if(intersects == true){
//     //     intersects = false;
// 		// 		changed = true;
// 		// 	}
//     //     return 0;    // This ray is parallel to this triangle.
//     // }
//
//     f = 1.0 / determinant;
// 		// calculate distance from vert to ray origin
// 		vec3_sub(s, rayOrigin, inTriangle[0]);
//
// 		sh_dot = vec3_dot(s, h);
// 		// U parameter is one axis of the triangle after transforming into its own
// 		// unit coordinate system.
// 		// calculate U parameter and test bounds:
// 		u = f * sh_dot;
//     if (u < 0.0 || u > 1.0){
// 			// printf("ray is outside U coordinate of triangle\n");
//
// 			if(intersects == true){
// 				intersects = false;
// 				changed = true;
// 				// printf("rayOrigin: %f, %f, %f\n", rayOrigin[0], rayOrigin[1], rayOrigin[2]);
// 				// printf("rayVector: %f, %f, %f\n", rayVector[0], rayVector[1], rayVector[2]);
// 				// printf("inTriangle[0]: %f, %f, %f\n", inTriangle[0][0], inTriangle[0][1], inTriangle[0][2]);
// 				// printf("inTriangle[1]: %f, %f, %f\n", inTriangle[1][0], inTriangle[1][1], inTriangle[1][2]);
// 				// printf("inTriangle[2]: %f, %f, %f\n", inTriangle[2][0], inTriangle[2][1], inTriangle[2][2]);
// 			}
//         return 0;
// 			}
// 		//  prepare to test V parameter
// 		vec3_mul_cross(q, s, edge1);
// 		// V parameter is the other axis of the triangle after transforming into its
// 		// own unit coordinate system.
// 		// calculate V parameter and test bounds:
// 		rayq_dot = vec3_dot(rayVector, q);
//     v = f * rayq_dot;
//     if (v < 0.0 || u + v > 1.0){
//         // printf("ray is outside V coordinate of triangle\n");
// 				if(intersects == true){
// 					intersects = false;
// 					changed = true;
// 				}
//         return 0;
//     }
//     // At this stage we can compute t to find out where the intersection point is on the line.
//     float t = f * vec3_dot(edge2, q);
//     if (t > FLT_EPSILON && t < 1/FLT_EPSILON) // ray intersection
//     {
// 			intersects = true;
// 			changed = true;
// 			// printf("ray is intersecting triangle\n");
// 			vec3 scaled;
// 			vec3_scale(scaled, rayVector, t);
//
// 			vec3_add(outIntersectionPoint, rayOrigin, scaled);
//       return 1;
//     }
//     else {// This means that there is a line intersection but not a ray intersection.
// 			// printf("line intersection, but not ray intersection.\n");
//         return 0;
// 			}
// 		if(changed == true){
// 			printf("ray intersects: %i\n", intersects);
// 		}
// }

// @NOTE other Moller-Trumbore intersection implementaion:
int ray_intersects_triangle(vec3 orig, vec3 dir, vec3 inTriangle[], vec3 outIntersectionPoint){

    float t, u, v;

    vec3 v0v1;
    vec3_sub(v0v1, inTriangle[1], inTriangle[0]);
    vec3 v0v2;
    vec3_sub(v0v2, inTriangle[2], inTriangle[0]);
    vec3 pvec;
    vec3_mul_cross(pvec, dir, v0v2);

    float det = vec3_dot(v0v1, pvec);

// #ifdef CULLING
//     // if the determinant is negative the triangle is backfacing
//     // if the determinant is close to 0, the ray misses the triangle
//     if (det < FLT_EPSILON){
//       return 0;
//     }
// #else
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < FLT_EPSILON){
      return 0;
    }
// #endif

  float invDet = 1 / det;

  vec3 tvec;
  vec3_sub(tvec, orig, inTriangle[0]);

  u = vec3_dot(tvec, pvec) * invDet;
  if (u < 0 || u > 1){
    // printf("ray is outside the U bounds");
    return 0;
  }

  vec3 qvec;
  vec3_mul_cross(qvec, tvec, v0v1);
  v = vec3_dot(dir, qvec) * invDet;
  if (v < 0 || u + v > 1){
    // printf("ray is outside the V bounds");
    return 0;
  }

  t = vec3_dot(v0v2, qvec) * invDet;

  vec3 scaled;
  vec3_scale(scaled, dir, t);
  vec3_add(outIntersectionPoint, orig, scaled);
  // printf("intersection! ");
  return 1;
}

// @NOTE other Moller-Trumbore intersection implementaion:
// bool ray_triangle_intersect(vec3 orig, vec3 dir, vec3 vert0,
//     vec3 vert1, vec3 vert2, double* t, double* u, double* v) {
//     vec3 edge1, edge2;
//     vec3 tvec, pvec, qvec;
//     float det, inv_det;
//
//     /* edges */
//     vec3_sub(edge1, vert1, vert0);
//     vec3_sub(edge2, vert2, vert0);
//
//     /* determinant */
//     vec3_mul_cross(pvec, dir, edge2);
//
//     /* ray in plane of triangle if near zero */
//     det = vec3_dot(edge1, pvec);
//
//     if(det < EPSILON)
//         return 0;
//
//     vec3_sub(tvec, orig, vert0);
//
//     inv_det = 1.0 / det;
//
//     /* calculate, check bounds */
//     *u = vec3_dot(tvec, pvec) * inv_det;
//     if(*u < 0.0 || *u > 1.0)
//         return 0;
//
//     vec3_mul_cross(qvec, tvec, edge1);
//
//     /* calculate, check bounds */
//     *v = vec3_dot(dir, qvec) * inv_det;
//     if(*v < 0.0 || *u + *v > 1.0)
//         return 0;
//
//     *t = vec3_dot(edge2, qvec) * inv_det;
//
//     return 1;
// }

#endif
