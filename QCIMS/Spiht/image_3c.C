
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//                     I M A G E   C L A S S

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//           > > > >    C++ version  1.04 -  06/25/95   < < < <

// Amir Said - amir@densis.fee.unicamp.br
// University of Campinas (UNICAMP)
// Campinas, SP 13081, Brazil

// William A. Pearlman - pearlman@ecse.rpi.edu
// Rensselaer Polytechnic Institute
// Troy, NY 12180, USA

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Copyright (c) 1995 Amir Said & William A. Pearlman

// This program is Copyright (c) by Amir Said & William A. Pearlman.
// It may be freely redistributed in its entirety provided that this
// copyright notice is not removed. It may not be sold for profit or
// incorporated in commercial programs without the written permission
// of the copyright holders. This program is provided as is, without any
// express or implied warranty, without even the warranty of fitness
// for a particular purpose.


// - - Inclusion - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "general.h"
#include "image_3c.h"
#include <string.h>


// - - Constants - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static char * M_MSG = "< Image_3C >";

static char * R_MSG = "< Image_3C > cannot read from file";

static char * W_MSG = "< Image_3C > cannot write to file";

static char * L_MSG = "< Image_3C > larger than specified dimension";

static const int NumbTap = 4;

static const float T_LowPass[5] =
  { 0.852699,  0.377403, -0.110624, -0.023849, 0.037829 };

static const float T_HighPass[5] =
  { 0.788485, -0.418092, -0.040690,  0.064539, 0.0 };

static const float R_LowPass[5] =
  { 0.852699,  0.418092, -0.110624, -0.064539, 0.037829 };

static const float R_HighPass[5] =
  { 0.788485, -0.377403, -0.040690,  0.023849, 0.0 };


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Auxiliary functions

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

inline float Filter_L(const float * f, float * v)
{
  return f[0] * v[0] +
    f[1] * (v[1] + v[-1]) + f[2] * (v[2] + v[-2]) +
    f[3] * (v[3] + v[-3]) + f[4] * (v[4] + v[-4]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline float Filter_H(const float * f, float * v)
{
  return f[0] * v[0] +
    f[1] * (v[1] + v[-1]) + f[2] * (v[2] + v[-2]) +
    f[3] * (v[3] + v[-3]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void Reflection(float * h, float * t)
{  
  for (int i = 1; i <= NumbTap; i++) {
    h[-i] = h[i];  t[i] = t[-i]; }
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Functions of the class  < Image_3C >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - - Private functions - - - - - - - - - - - - - - - - - - - - - - - -

int Image_3C::max_levels(int n)
{
  int l1, l2;
  for (l1 = 0; !(n & 1); l1++) n >>= 1;
  for (l2 = l1 - 4; n; l2++) n >>= 1;
  return (l1 < l2 ? l1 : l2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::assign_mem(Image_Coord d)
{
  if ((levels >= 0) && (dim.x == d.x) && (dim.y == d.y)) return;
  free_mem();
  if ((d.x < 8) || (d.y < 8))
    Error("< Image_3C > dimension is too small or negative");
  dim = d;
  pdim.x = (d.x < 200 ? (d.x + 7) & 0x3FF8 : (d.x + 15) & 0x3FF0);
  pdim.y = (d.y < 200 ? (d.y + 7) & 0x3FF8 : (d.y + 15) & 0x3FF0);

  int c, i = NumbTap + NumbTap + Max(pdim.x, pdim.y);
  NEW_VECTOR(r_line, i, float, M_MSG);
  t_line = r_line + NumbTap;
  for (c = 0; c < 3; c++) {
    NEW_VECTOR(coeff, pdim.x, float *, M_MSG);
    for (i = 0; i < pdim.x; i++) {
      NEW_VECTOR(coeff[i], pdim.y, float, M_MSG); }
    cor[c] = coeff; }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::free_mem(void)
{
  if (levels >= 0) {
    int c, i;
    for (c = 2; c >= 0; c--) {
      coeff = cor[c];
      for (i = pdim.x - 1; i >= 0; i--) delete [] coeff[i];
      delete [] coeff; }
    delete [] r_line; }
  dim.x = dim.y = 0;  levels = -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::extend_image(int c)
{
  coeff = cor[c];
  int i, j;
  for (j = dim.y - 1; j < pdim.y - 1; j++) {
    coeff[0][j+1] = (coeff[0][j] + coeff[1][j]) / 2;
    coeff[dim.x-1][j+1] = (coeff[dim.x-1][j] + coeff[dim.x-2][j]) / 2;
    for (i = dim.x - 2; i > 0; i--)
      coeff[i][j+1] = (coeff[i-1][j] + coeff[i][j] + coeff[i+1][j]) / 3; }
  for (i = dim.x - 1; i < pdim.x - 1; i++) {
    coeff[i+1][0] = (coeff[i][0] + coeff[i][1]) / 2;
    coeff[i+1][pdim.y-1] = (coeff[i][pdim.y-1] + coeff[i][pdim.y-2]) / 2;
    for (j = pdim.y - 2; j > 0; j--)
      coeff[i+1][j] = (coeff[i][j-1] + coeff[i][j] + coeff[i][j+1]) / 3; }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define ROTATION(M,i,j,k,l) g = M[i][j];  h = M[k][l];\
         M[i][j] = g-s*(h+g*tau);  M[k][l] = h+s*(g-h*tau)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::Jacobi(double m[3], double A[3][3])
{
  int i, j, k, p, q, itr = 0;
  double t = 1.0 / (double(dim.x) * double(dim.y));
  double v, tau, s, h, g, c;
  double b[3], d[3], X[3][3];

  for (i = 0; i < 3; i++) m[i] *= t;
  for (i = 0; i < 3; i++) {
    for (j = i; j < 3; j++) {
      X[i][j] = X[j][i] = 0;
      A[i][j] = A[j][i] = t * A[i][j] - m[i] * m[j]; }
    mean[i] = int(0.5 + m[i]);  m[i] = 0;
    b[i] = d[i] = A[i][i];  X[i][i] = 1; }

  for (;;) {

    for (s = i = 0; i < 2; i++)
      for (j = i + 1; j < 3; j++)
        if (s < (t = fabs(A[i][j]))) {
          s = t;  p = i;  q = j; }

    if (s == 0) break;

    if (++itr > 50)
      Error("Too many iterations in function < Image_3C::Jacobi >");

    g = 100 * s;  h = d[q] - d[p];
    if (g + fabs(h) == fabs(h))
      t = A[p][q] / h;
    else {
      v = 0.5 * h / A[p][q];  t = 1 / (fabs(v) + sqrt(1 + v * v));
      if (v < 0) t = -t; }
    c = 1 / sqrt(1 + t * t);  s = t * c;
    tau = s / (1 + c);  h = t * A[p][q];  A[p][q] = 0;
    m[p] -= h;  m[q] += h;  d[p] -= h;  d[q] += h;

    for (k = 0; k < p; k++)     { ROTATION(A,k,p,k,q); }
    for (k = p + 1; k < q; k++) { ROTATION(A,p,k,k,q); }
    for (k = q + 1; k < 3; k++) { ROTATION(A,p,k,q,k); }
    for (k = 0; k < 3; k++)     { ROTATION(X,k,p,k,q); }

    for (k = 0; k < 3; k++) {
      b[k] += m[k];  d[k] = b[k];  m[k] = 0; }
  }

  for (i = 0; i < 3; i++) {
    for (s = k = -1, j = 0; j < 3; j++)
      if (d[j] > s) s = d[k=j];
    if (k < 0) Error("Incorrect decomposition");
    for (d[k] = -2, j = 0; j < 3;j++) 
      R[i][j] = 1e-4 * Round(1e4 * X[j][k]); }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Public functions  - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::averages(int M[3])
{
  for (int i = 0; i < 3; i++) M[i] = mean[i];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::color_transform(int T[3][3])
{
  int i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      T[i][j] = int(Round(1e4 * (R[i][j] + 1)));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::set_color(int c)
{
  if ((c < 0) || (c > 3)) Error("invalid < Image_3C > color number");
  coeff = cor[c];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::read_pic(Image_Coord d, char * file_name)
{
  assign_mem(d);  levels = 0;

  int i, j, k, l, v, p[3];
  double m[3], S[3][3];
  FILE * in_file = fopen(file_name, "rb");
  if (in_file == NULL) Error(R_MSG);

  for (k = 0; k < 3; k++)
    for (m[k] = l = 0; l < 3; l++) S[k][l] = 0;

  for (i = 0; i < dim.x; i++)
    for (j = 0; j < dim.y; j++) {
      for (k = 0; k < 3; k++) {
        if ((v = getc(in_file)) == EOF) Error(R_MSG);
        cor[k][i][j] = p[k] = v;  m[k] += v; }
      for (k = 0; k < 3; k++)
        for (l = k; l < 3; l++) S[k][l] += p[k] * p[l]; }
  if (getc(in_file) != EOF) Error(L_MSG);
  fclose(in_file);

  Jacobi(m, S);

  for (k = 0; k < 3; k++) extend_image(k);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

float Image_3C::compare(char * file_name)
{
  if (levels) Error("cannot compare < Image_3C >");

  FILE * in_file = fopen(file_name, "rb");
  if (in_file == NULL) Error(R_MSG);

  double mse = 0.0;
  int i, j, k, c;
  for (i = 0; i < dim.x; i++)
    for (j = 0; j < dim.y; j++)
      for (k = 0; k < 3; k++) {
        if ((c = getc(in_file)) == EOF) Error(R_MSG);
        mse += Sqr(c - int(0.499 + cor[k][i][j])); }
  if (getc(in_file) != EOF) Error(L_MSG);
  fclose(in_file);

  return 0.333333 * (mse / dim.x) / dim.y;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::write_pic(char * file_name)
{
  if (levels) Error("cannot write < Image_3C >");

  FILE * out_file = fopen(file_name, "wb");
  if (out_file == NULL) Error(W_MSG);

  int i, j, k;
  for (i = 0; i < dim.x; i++)
    for (j = 0; j < dim.y; j++)
      for (k = 0; k < 3; k++) {
      if (putc(int(0.499 + cor[k][i][j]), out_file) == EOF)
        Error(W_MSG); }

  fclose(out_file);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::reset(Image_Coord d)
{
  assign_mem(d);

  int i, k, j = pdim.y * sizeof(float);
  for (i = 0; i < 3; i++)
    for (mean[i] = j = 0; j < 3; j++) R[i][j] = 0;

  levels = Min(max_levels(pdim.x), max_levels(pdim.y));
//  for (k = 0; k < 3; k++)
//    for (i = 0; i < pdim.x; i++) memset(cor[k][i], 0, j);
  for (k = 0; k < 3; k++)
    for (i = 0; i < pdim.x; i++)
      for (j = 0; j < pdim.y; j++) cor[k][i][j] = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::reset(Image_Coord d, int m[3], int T[3][3])
{
  reset(d);

  int i, j;
  for (i = 0; i < 3; i++) {
    mean[i] = m[i];
    for (j = 0; j < 3; j++) R[i][j] = 1e-4 * T[i][j] - 1.0; }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::transform(void)
{
  if (levels) Error("cannot transform < Image_3C >");
  if ((dim.x < 32) || (dim.y < 32))
    Error("< Image_3C > is too small to transform");

  Chronometer cpu_time;
  cpu_time.start("\n  Starting image transformation...");

  levels = Min(max_levels(pdim.x), max_levels(pdim.y));
  if (levels < 2) Error("invalid < Image_3C > dimension");

  int i, j, k, l;
  float s, p[3];

  for (i = 0; i < pdim.x; i++)
    for (j = 0; j < pdim.y; j++) {
      for (k = 0; k < 3; k++) p[k] = cor[k][i][j] - mean[k];
      for (k = 0; k < 3; k++) {
        for (s = l = 0; l < 3; l++) s += R[k][l] * p[l];
        cor[k][i][j] = s; } }

  for (k = 0; k < 3; k++) w_transform(cor[k]);

  cpu_time.display(" Image transformed in");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::w_transform(float ** tcf)
{
  int i, j, nx, ny, mx = pdim.x, my = pdim.y;
  float * t;

  for (int lv = 0; lv < levels; lv++) {

    nx = mx;  mx >>= 1;  ny = my;  my >>= 1;

    for (j = 0; j < ny; j++) {
      for (i = 0; i < nx; i++) t_line[i] = tcf[i][j];
      Reflection(t_line, t_line + nx - 1);
      for (i = 0, t = t_line; i < mx; i++) {
        tcf[i][j] = Filter_L(T_LowPass, t++);
        tcf[i+mx][j] = Filter_H(T_HighPass, t++); } }

    for (i = 0; i < nx; i++) {
      memcpy(t_line, tcf[i], ny * sizeof(float));
      Reflection(t_line, t_line + ny - 1);
      for (j = 0, t = t_line; j < my; j++) {
        tcf[i][j] = Filter_L(T_LowPass, t++);
        tcf[i][j+my] = Filter_H(T_HighPass, t++); } } }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::recover(void)
{
  if (levels <= 0) Error("cannot recover < Image_3C >");

  Chronometer cpu_time;
  cpu_time.start("\n  Starting inverse transformation...");

  int i, j, k, l;
  float s, p[3];

  for (k = 0; k < 3; k++) w_recover(cor[k]);

  for (i = 0; i < pdim.x; i++)
    for (j = 0; j < pdim.y; j++) {
      for (k = 0; k < 3; k++) p[k] = cor[k][i][j];
      for (k = 0; k < 3; k++) {
        for (s = l = 0; l < 3; l++) s += R[l][k] * p[l];
        l = int(0.499 + s + mean[k]);
        if (l < 0) l = 0;
        if (l > 255) l = 255;
        cor[k][i][j] = l; } }

  levels = 0;
  cpu_time.display(" Image transformed in");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Image_3C::w_recover(float ** tcf)
{
  float * t;
  int i, j, mx, my, nx = pdim.x >> levels, ny = pdim.y >> levels;

  for (int lv = 0; lv < levels; lv++) {

    mx = nx;  nx <<= 1;  my = ny;  ny <<= 1;

    for (i = 0; i < nx; i++) {
      for (j = 0, t = t_line; j < my; j++) {
        *(t++) = tcf[i][j];  *(t++) = tcf[i][j+my]; }
      Reflection(t_line, t_line + ny - 1);
      for (j = 0, t = t_line; j < ny;) {
        tcf[i][j++] = Filter_H(R_HighPass, t++);
        tcf[i][j++] = Filter_L(R_LowPass, t++); } }

    for (j = 0; j < ny; j++) {
      for (i = 0, t = t_line; i < mx; i++) {
        *(t++) = tcf[i][j];  *(t++) = tcf[i+mx][j]; }
      Reflection(t_line, t_line + nx - 1);
      for (i = 0, t = t_line; i < nx;) {
        tcf[i++][j] = Filter_H(R_HighPass, t++);
        tcf[i++][j] = Filter_L(R_LowPass, t++); } } }
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file  < Image_3C.C >
