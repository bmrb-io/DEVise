
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//      V E C T O R - T R E E   I M A G E   C O M P R E S S I O N

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//           > > > >    C++ version  7.10  -  06/25/95   < < < <

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
#include "image_bw.h"

#ifdef BINCODE
#include "bincode.h"
#else
#include "aritcode.h"
#endif


// - - Definitions - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct Tree_Node
{
  Image_Coord coord;
  long state;
  Tree_Node * next;
};


// - - Static variables  - - - - - - - - - - - - - - - - - - - - - - - -

const int SHF_x[4] = { 0, 0, 1, 1 }, SHF_y[4] = { 0, 1, 0, 1 };

char names[3][80], * pic_f_name = names[0], * cod_f_name = names[1],
      * new_f_name = names[2];

Chronometer code_time, total_time;

int pel_bytes, threshold_bits, min_bits, pyramid_levels, smoothing,
    mean, mean_shift;

long byte_budget, root_code;

Pel_Type act_value, threshold;

float bit_rate, rate_mult;

Tree_Node * LISP_head, * LISP_end;

Image_Coord dimension, pyramid_dim, root_dim;

Image_BW image;

#ifdef ENCODER

  Encoder data_file;

  Image_BW max_image;

#else

  Decoder data_file;

#endif

#ifndef BINCODE

  Adaptive_Model group_model[5], node_model[34], desc_model[34];

#endif

#ifndef LOSSLESS

  Image_Coord LSP_dim;

  int LSP_plane, LSP_part, LSP_idx;

  float bias_const_1, bias_const_2, bias[32];

  float * LSP_mark[32], *** LSP_mtx, ** LSP_ptr;

#endif


// - - Function prototypes - - - - - - - - - - - - - - - - - - - - - - -

boolean Sorting_Pass(void);

boolean Refinement_Pass(void);

void Node_Test(Tree_Node *);

void Desc_Test(Tree_Node *);

#ifndef BINCODE

  int Node_Transition(int, const Image_Coord &, int *);

  int Desc_Transition(int, const Image_Coord &);

#endif

#ifdef LOSSLESS

  void Process_Bits_Left(void);

#endif



// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Function Implementations

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

int Max_Levels(int n)
{
  int l1, l2;
  for (l1 = 0; !(n & 1); l1++) n >>= 1;
  for (l2 = l1 - 2; n; l2++) n >>= 1;
  return (l1 < l2 ? l1 : l2) - 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef LOSSLESS

int Magn_Val(int n, int s)
{
  if (n < 0) n = -n;
  if (n < 3) return n + s;
  n >>= 2;
  int k;
  for (k = 2; n; k++) n >>= 1;
  return k + s;
}

#else
inline float Magn_Val(float & p, int & s) { return fabs(p); }
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean Sorting_Pass(void)
{
#ifndef LOSSLESS
  act_value = threshold + (bias[LSP_plane] = bias_const_1 * threshold);
#endif

  Tree_Node * prev = LISP_head, * node = prev;

  for (; node = node->next; prev = node)
    if ((node->state & 0x55L) != 0x55L) {
      Node_Test(node);
      if (data_file.bytes_used() >= byte_budget) return true; }

  for (node = prev = LISP_head; node = node->next; prev = node) {
    if ((node->state & 0xAAL) != 0xAAL) {
      Desc_Test(node);
      if (data_file.bytes_used() >= byte_budget) return true; }
#ifndef LOSSLESS
    if ((node->state & 0xFFL) == 0xFFL) {
      if (LISP_end == node) LISP_end = prev;
      prev->next = node->next;  delete node;  node = prev; }
#endif
  }

  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Process_Image(void)
{
#ifdef LOSSLESS
  min_bits = 3;
#else
  bias_const_1 = 0.38 - 0.02 * smoothing;
  bias_const_2 = 0.25 + 0.07 * smoothing;
  threshold = pow(2, threshold_bits);  min_bits = -32;
  LSP_idx = LSP_part = LSP_plane = 0;  LSP_ptr = 0;
  LSP_dim.x = pyramid_dim.x >> 2;  LSP_dim.y = pyramid_dim.y << 2;
  NEW_VECTOR(LSP_mtx, LSP_dim.x, float **, "LSP");
#endif

#ifndef BINCODE
  int i, j, k, n = -1, t;
  for (i = 0; i <= 4; i++) {
    group_model[i].set_symbols(2);
    for (j = 0; j <= 4 - i; j++)
      for (k = 0;  k <= 4 - i - j; k++, n++)
        if (n >= 0) {
          if (t = i + k) node_model[n].set_symbols(1 << t);
          if (t = i + j) desc_model[n].set_symbols(1 << t); } }
#endif

  Image_Coord cd;
  Tree_Node * prev, * node;
  NEW_OBJECT(node, Tree_Node, "LISP entry");
  LISP_head = LISP_end = node;

  int m = pyramid_levels;
  root_dim.x = pyramid_dim.x >> m;  root_dim.y = pyramid_dim.y >> m;
  long st = 0x2L + (root_code = long(m + 1) << 9);
  for (cd.x = 0; cd.x < root_dim.x; cd.x += 2)
    for (cd.y = 0; cd.y < root_dim.y; cd.y += 2) {
      NEW_OBJECT(node, Tree_Node, "LISP entry");
      node->coord = cd;  node->state = st;
      LISP_end->next = node;  LISP_end = node; }
  LISP_end->next = NULL;

  for (;--threshold_bits >= min_bits; threshold /= 2) {
    if (Sorting_Pass()) break;
    if (Refinement_Pass()) break; }

#ifdef LOSSLESS
  if ((threshold_bits < min_bits) && (threshold_bits >= 0))
    Process_Bits_Left();
#else
  for (m = LSP_part; m > 0;) delete [] LSP_mtx[--m];
  delete [] LSP_mtx;
#endif

#ifdef ENCODER
  code_time.stop();
#endif

  for (node = LISP_head; node;) {
    prev = node;  node = node->next;  delete prev; }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Binary coding functions - - - - - - - - - - - - - - - - - - - - -

#ifdef BINCODE

void Node_Test(Tree_Node * act_node)
{
  int old_st = int(act_node->state), mask = 1;
  Image_Coord nc;
  float * ptr;

  for (int i = 0; i < 4; i++, mask <<= 2)
    if (!(old_st & mask)) {
      nc.x = act_node->coord.x + SHF_x[i];
      nc.y = act_node->coord.y + SHF_y[i];
#ifdef ENCODER
      if (data_file.code_bit(fabs(image[nc]) >= threshold)) {
#else
      if (data_file.decode_bit()) {
#endif
        if (!LSP_idx) {
          NEW_VECTOR(LSP_ptr, LSP_dim.y, float *, "LSP");
          LSP_mtx[LSP_part++] = LSP_ptr;  LSP_idx = LSP_dim.y; }
        LSP_ptr[--LSP_idx] = ptr = image.address(nc);
        act_node->state |= mask;
#ifdef ENCODER
        data_file.code_bit(*ptr < 0);
        *ptr = fabs(*ptr) - threshold; } }
#else
        *ptr = (data_file.decode_bit() ? -act_value : act_value); } }
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Desc_Test(Tree_Node * act_node)
{
  Tree_Node * new_node;
  Image_Coord pc = act_node->coord;
  int i, b, mask, old_st = int(act_node->state & 0xFFL);

#ifdef ENCODER
  for (mask = 2, b = i = 0;  i < 4; i++, mask <<= 2)
    if (!(old_st & mask)) {
      if (max_image(pc.x + SHF_x[i], pc.y + SHF_y[i]) >= threshold)
        b |= mask; }
#endif

  if ((old_st & 0xAA) == 0) 
#ifdef ENCODER
    if (data_file.code_bit(b == 0)) return;
#else
    if (data_file.decode_bit()) return;
#endif

  long ns = (act_node->state & 0xFF00L) - 0x200L;
  if (ns < 0x300L) ns |= 0xAAL;
  boolean root = (act_node->state >= root_code);

  for (mask = 2, i = 0; i < 4; i++, mask <<= 2)
    if (!(old_st & mask)) {
#ifdef ENCODER
      if (data_file.code_bit((b & mask) != 0)) {
#else
      if (data_file.decode_bit()) {
#endif
        act_node->state |= long(mask);
        NEW_OBJECT(new_node, Tree_Node, "LISP entry");
        if (root) {
          if (i == 3) ns -= 0x100L;
          new_node->coord.x = pc.x + SHF_x[i] * root_dim.x;
          new_node->coord.y = pc.y + SHF_y[i] * root_dim.y; }
        else {
          new_node->coord.x = (pc.x + SHF_x[i]) << 1;
          new_node->coord.y = (pc.y + SHF_y[i]) << 1; }
        new_node->next = NULL;  new_node->state = ns;
        LISP_end->next = new_node;  LISP_end = new_node;
        Node_Test(new_node); } }
}

#else

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Arithmetic coding functions - - - - - - - - - - - - - - - - - - -

void Output_Signs(int nb, int sd, const Image_Coord & pc)
{
  Image_Coord nc;
#ifdef ENCODER
  int m = 1, b = 0;
#else
  int m = 1, b = data_file.decode_bits(nb);
#endif

  for (int i = 0; i < 4; i++, sd >>= 2)
    if (sd & 1) {
    nc.x = pc.x + SHF_x[i];  nc.y = pc.y + SHF_y[i];
#ifdef LOSSLESS
#ifdef ENCODER
      if (image[nc] < 0) b |= m;
#else
      image[nc] = (b & m ? -act_value : act_value);
#endif
#else
      if (!LSP_idx) {
        NEW_VECTOR(LSP_ptr, LSP_dim.y, float *, "LSP");
        LSP_mtx[LSP_part++] = LSP_ptr;  LSP_idx = LSP_dim.y; }
      float * ptr = LSP_ptr[--LSP_idx] = image.address(nc);
#ifdef ENCODER
      if (*ptr < 0) b |= m;
      *ptr = fabs(*ptr) - threshold;
#else
      *ptr = (b & m ? -act_value : act_value);
#endif
#endif
      m <<= 1; }

#ifdef ENCODER
  data_file.code_bits(nb, b);
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Desc_Test(Tree_Node * act_node)
{
  Image_Coord pc = act_node->coord;
#ifdef LOSSLESS
  int st_trans = Desc_Transition(int((act_node->state |
    (act_node->state >> 16)) & 0xFFL), pc);
#else
  int st_trans = Desc_Transition(int(act_node->state & 0xFFL), pc);
#endif

  if (!st_trans) return;

  act_node->state |= st_trans;  st_trans >>= 1;

  Tree_Node * new_node;
  long ns = (act_node->state & 0xFF00L) - 0x200L;
  if (ns < 0x300L) ns |= 0xAAL;
  boolean root = ((act_node->state & 0xFF00L) >= root_code);

  for (int i = 0; i < 4; i++, st_trans >>= 2)
    if (st_trans & 0x1) {
      NEW_OBJECT(new_node, Tree_Node, "LISP entry");
      if (root) {
        if (i == 3) ns -= 0x100L;
        new_node->coord.x = pc.x + SHF_x[i] * root_dim.x;
        new_node->coord.y = pc.y + SHF_y[i] * root_dim.y; }
      else {
        new_node->coord.x = (pc.x + SHF_x[i]) << 1;
        new_node->coord.y = (pc.y + SHF_y[i]) << 1; }
      new_node->next = NULL;  new_node->state = ns;
      LISP_end->next = new_node;  LISP_end = new_node;
      Node_Test(new_node); }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Node_Transition(int old_st, const Image_Coord & pc, int * signs)
{
  int st_trans = 0, os = old_st, ps = old_st, mask = 1;
  int msk[3], count[4];

  int i;
  for (i = 0; i < 4; i++) count[i] = 0;
  for (i = 0; i < 4; i++, ps >>= 2) ++count[ps&0x3];
  msk[0] = msk[1] = 1;  msk[2] = 1 << count[0];
  int mod = count[2] - count[0] * count[1] - 1 +
    (count[0] * (count[0] * (count[0] - 18) + 107)) / 6 +
    (count[1] * (11 - count[1])) / 2;

#ifdef ENCODER
  int b = 0;
#else
  int b = data_file.decode_symbol(node_model[mod]);
#endif

  for (*signs = i = 0; i < 4; i++, mask <<= 2, os >>= 2) {
    if (old_st & mask) continue;
    ps = os & 0x3;
#ifdef ENCODER
    if (ABS(image(pc.x + SHF_x[i], pc.y + SHF_y[i])) >= threshold) {
      b |= msk[ps];
#else
    if (b & msk[ps]) {
#endif
      st_trans |= mask;  ++(*signs); }
    msk[ps] <<= 1; }

#ifdef ENCODER
  data_file.code_symbol(b, node_model[mod]);
#endif

  return st_trans;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Desc_Transition(int old_st, const Image_Coord & pc)
{
  int os = old_st, ps = os, st_trans = 0, mask = 2;
  int msk[3], count[4];

  int i;
  for (i = 0; i < 4; i++) count[i] = 0;
  for (i = 0; i < 4; i++, ps >>= 2) ++count[ps & 0x3];
  msk[0] = 1;  msk[1] = msk[2] = 1 << count[0];
  int mod = count[2] - count[0] * count[1] - 1 +
    (count[0] * (count[0] * (count[0] - 18) + 107)) / 6 +
    (count[1] * (11 - count[1])) / 2;

#ifdef ENCODER
  int b = 0;
#else
  if ((old_st & 0xAA) == 0)
    if (data_file.decode_symbol(group_model[count[0]])) return 0;
  int b = data_file.decode_symbol(desc_model[mod]);
#endif

  for (i = 0; i < 4; i++, mask <<= 2, os >>= 2) {
    if (old_st & mask) continue;
    ps = os & 0x3;
#ifdef ENCODER
#ifdef LOSSLESS
    if (max_image(pc.x + SHF_x[i], pc.y + SHF_y[i]) > threshold_bits) {
#else
    if (max_image(pc.x + SHF_x[i], pc.y + SHF_y[i]) >= threshold) {
#endif
      b |= msk[ps];
#else
    if (b & msk[ps]) {
#endif
      st_trans |= mask; }
    msk[ps] <<= 1; }

#ifdef ENCODER
  if ((old_st & 0xAA) == 0) {
    data_file.code_symbol(st_trans == 0, group_model[count[0]]);
    if (st_trans == 0) return 0; }
  data_file.code_symbol(b, desc_model[mod]);
#endif

  return st_trans;
}

#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Lossless version functions  - - - - - - - - - - - - - - - - - - -

#ifdef LOSSLESS

void Process_Bits(int st, const Image_Coord & pc)
{
  Image_Coord nc;
#ifdef DECODER
  int a, ht = (threshold >> 1) + (threshold == 1);
#endif

  for (int i = 0; i < 4; i++, st >>= 2)
    if (st & 1) {
      nc.x = pc.x + SHF_x[i];  nc.y = pc.y + SHF_y[i];
#ifdef ENCODER
      data_file.code_bit((abs(image[nc]) & threshold) != 0); }
#else
      a = (data_file.decode_bit() ? ht : ht - threshold);
      if (image[nc] < 0) a = -a;
      image[nc] += a; }
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Node_Test(Tree_Node * act_node)
{
  int shift = int((act_node->state & 0xFF00L) >> 9);
  if (threshold_bits < shift) {
    act_node->state |= 0x55L;
    return; }

  threshold = 1 << (threshold_bits - shift);
  int signs, state_trans;
  state_trans = Node_Transition(int(act_node->state & 0xFFL),
    act_node->coord, &signs);

  if (state_trans) {
    act_value = threshold + (threshold >> 1) - (threshold > 1);
    act_node->state |= long(state_trans) << 16;
    Output_Signs(signs, state_trans, act_node->coord); }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean Refinement_Pass(void)
{
  int node_data, shift;
  Tree_Node * prev = LISP_head, * node = prev;

  for (; node = node->next; prev = node) {

    if (data_file.bytes_used() >= byte_budget) return true;

    shift = int((node->state & 0xFF00L) >> 9);
    if (threshold_bits < shift)
      threshold = 0;
    else {
      threshold = 1 << (threshold_bits - shift);
      if (node_data = int(node->state & 0x55L))
        Process_Bits(node_data, node->coord); }

    node->state |= (node->state & 0xFF0000L) >> 16;
    node->state &= 0xFFFFL;

    if ((threshold <= 1) && ((node->state & 0xAAL) == 0xAAL)) {
      if (LISP_end == node) LISP_end = prev;
      prev->next = node->next;  delete node;  node = prev; } }

  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean Process_Quadrant(int s, int q, Image_Coord d)
{
  const int NModels = 5;
  Adaptive_Model vl_model[NModels], bt_model[NModels];
  Image_Coord cd, min, max;
  min.x = d.x * SHF_x[q];  min.y = d.y * SHF_y[q];
  max.x = min.x + d.x;   max.y = min.y + d.y;

  threshold = 1 << (s + 1);
  int ht = (threshold >> 1) - (threshold > 1);
  int up, rp1, rp2, ctx, bias = threshold - 1;

  for (int i = 0; i < NModels; i++) {
    vl_model[i].set_symbols(2 * bias + 1);
    bt_model[i].set_symbols(bias + 1); }

  rp1 = rp2 = ctx = 0;
  for (cd.x = min.x; cd.x < max.x; cd.x++) {
    up = cd.x & 1;  cd.y = (up ? min.y : max.y - 1);
    for (;;) {
      if (abs(image[cd]) < threshold)
#ifdef ENCODER
        data_file.code_symbol(image[cd] + bias, vl_model[ctx]);
      else
        data_file.code_symbol(abs(image[cd]) & bias, bt_model[ctx]);
#else
        image[cd] = data_file.decode_symbol(vl_model[ctx]) - bias;
      else
        if (image[cd] < 0)
          image[cd] -= data_file.decode_symbol(bt_model[ctx]) - ht;
        else
          image[cd] += data_file.decode_symbol(bt_model[ctx]) - ht;
#endif

      if (data_file.bytes_used() >= byte_budget) return true;

      rp2 = rp1;  rp1 = Magn_Val(image[cd], 0);
      ctx = Min(NModels - 1, (rp1 + rp2) >> 1);
      if (up) {
        if (++cd.y >= max.y) break; }
      else {
        if (--cd.y < min.y) break; } } }

  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Process_Bits_Left(void)
{
  printf("\n  Switching coding method at rate%5.2f bits/pixel.\n\n",
    rate_mult * data_file.bytes_used());

  Image_Coord d = image.pyramid_dim();
  int l, q, lv = pyramid_levels, shift = lv;
  d.x >>= lv;  d.y >>= lv;

  for (l = lv - 1; l >= 0; l--) {
    for (q = 1; q <= 3; q++) {
      if (shift <= threshold_bits)
        if (Process_Quadrant(threshold_bits - shift, q, d)) return;
      if (q == 2) shift--; }
    d.x <<= 1;  d.y <<= 1; }
}

#else

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Lossy version functions - - - - - - - - - - - - - - - - - - - - -

#ifndef BINCODE

void Node_Test(Tree_Node * act_node)
{
  int st_trans, signs;
  st_trans = Node_Transition(int(act_node->state & 0xFFL),
    act_node->coord, &signs);

  if (st_trans) {
    act_node->state |= long(st_trans);
    Output_Signs(signs, st_trans, act_node->coord); }
}

#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean Refinement_Pass(void)
{
#ifdef DECODER
  float t, ht1, ht2;
#endif
  int b, p, m, n = LSP_plane, k = LSP_dim.y;
  float * ptr, * smk, ** mtx = LSP_mtx[0];

  LSP_mark[LSP_plane++] = (LSP_ptr ? LSP_ptr[LSP_idx] : 0);

  for (m = p = 0; p < n; p++) {
#ifdef DECODER
    t = bias[p];
    bias[p] = threshold * (0.5 - bias_const_2 / Sqr(LSP_plane - p));
    ht1 = bias[p] - t;  ht2 = ht1 + threshold;
#endif
    for (smk = LSP_mark[p], ptr = 0; ptr != smk;) {
      if (!k) {
        mtx = LSP_mtx[++m];  k = LSP_dim.y; }
      ptr = mtx[--k];
#ifdef ENCODER
      if (b = (*ptr >= threshold)) *ptr -= threshold;
      data_file.code_bit(b);
#else
      t = (data_file.decode_bit() ? ht2 : ht1);
      if (*ptr > 0) *ptr += t; else *ptr -= t;
#endif
      if (data_file.bytes_used() >= byte_budget) return true; } }

  return false;
}

#endif


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Encoder functions - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef ENCODER

Pel_Type Max_Desc_Val(int l, int s, int x, int y)
{
  Pel_Type t, m = 0;
  Image_Coord ds;
  --l;  --s;
  for (int d = 0; d < 4; d++) {
    ds.x = x + SHF_x[d];  ds.y = y + SHF_y[d];
    t = Magn_Val(image[ds], s);
    if (m < t) m = t;
    if (l > 0) {
      max_image[ds] = t = Max_Desc_Val(l, s, ds.x << 1, ds.y << 1);
      if (m < t) m = t; } }
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Calc_Max_Image(void)
{
  Image_Coord tc, pc = image.pyramid_dim(), lm = pc;
  int i, sd, lv = pyramid_levels, sp = lv + 1;
  Pel_Type t, max = 0;

  lm.x >>= lv;  lm.y >>= lv;  pc.x >>= 1;  pc.y >>= 1;
  max_image.reset(pc);

  for (pc.x = 0; pc.x < lm.x; pc.x += 2)
    for (pc.y = 0; pc.y < lm.y; pc.y += 2) {
      t = max_image[pc] = Magn_Val(image[pc], sp);
      if (max < t) max = t;
      for (sd = sp, i = 1; i < 4; i++) {
        tc.x = pc.x + SHF_x[i];  tc.y = pc.y + SHF_y[i];
        t = Magn_Val(image[tc], sp);
        if (max < t) max = t;
        if (i == 3) --sd;
        t = max_image[tc] = Max_Desc_Val(lv, sd,
          pc.x + SHF_x[i] * lm.x, pc.y + SHF_y[i] * lm.y);
        if (max < t) max = t; } }

#ifdef LOSSLESS
  return max;
#else
  return int(log(max) / log(2.0));
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int numb_arg, char * arg[])
{
  if (numb_arg != 2)
    printf("\n\t* Please read the file `codetree.doc' for "
      "instructions\n\t\tand Copyright/Patent information.\n\t* Use"
      " `%s -u' for command-line usage information\n\n", arg[0]);
  else {
    printf("\n* Usage:\n\n%s image_file coded_file number_lines\n\t"
      "pixels/line bytes/pixel bpp_rate [smoothing]", arg[0]);
#ifdef LOSSLESS
    printf("\n\nFor lossless compression use bpp_rate = 0");
#endif
    puts(" \n");  exit(0); }

  smoothing = 0;
  if ((numb_arg == 7) || (numb_arg == 8)) {
    pic_f_name = arg[1];  cod_f_name = arg[2];
    dimension.x = atoi(arg[3]);  dimension.y = atoi(arg[4]);
    pel_bytes = atoi(arg[5]);
    bit_rate = atof(arg[6]);
    if (numb_arg == 8) smoothing = atoi(arg[7]); }
  else {
    Input_Line("File with original image", pic_f_name);
    Input_Line("File with compressed image", cod_f_name);
    dimension.x = Input_Int("Number of image lines");
    dimension.y = Input_Int("Number of pixels per line");
    pel_bytes = Input_Int("Number of bytes per pixel");
#ifdef LOSSLESS
    bit_rate = Input_Float("Rate (bits/pixel, 0 = lossless)"); }
#else
    bit_rate = Input_Float("Rate (bits/pixel)");
    smoothing = Input_Int("Smoothing (0 = none, 7 = max)"); }
#endif

#ifdef LOSSLESS
  if (bit_rate <= 0) bit_rate = 8;
#endif
  rate_mult = 8.0 / (float(dimension.x) * float(dimension.y));
  if (Max(dimension.x, dimension.y)>8192) Error("Image is too large!");
  byte_budget = long(bit_rate / rate_mult) - 1;

  total_time.start();
  image.read_pic(dimension, pic_f_name, pel_bytes);
  image.transform(smoothing);  pyramid_dim = image.pyramid_dim();
  mean = image.transform_mean();  mean_shift = image.mean_shift();
  pyramid_levels = Min(Max_Levels(pyramid_dim.x),
                       Max_Levels(pyramid_dim.y));

  Chronometer max_time;
  max_time.start("\n  Starting computation of maximum magnitudes...");
  threshold_bits = Calc_Max_Image();
  max_time.display(" Maximum magnitudes computed in");

  data_file.open_file(cod_f_name, dimension.x, dimension.y,
       1, pel_bytes);
  data_file.code_bits(6, dimension.x >> 8);
  data_file.code_bits(6, dimension.y >> 8);
  data_file.code_bits(8, dimension.x & 0xFF);
  data_file.code_bits(8, dimension.y & 0xFF);
  data_file.code_bits(5, threshold_bits);
  data_file.code_bits(1, pel_bytes - 1);
  data_file.code_bits(3, smoothing);
  data_file.code_bits(4, mean_shift);
  data_file.code_bits(10, mean);

  code_time.start("\n  Starting image compression...");
  Process_Image();
  code_time.display(" Image compressed in");

  data_file.close_file();

  printf("\n  Compressed file size = %ld bytes (%6.3f bits/pixel).\n",
    data_file.bytes_used(), rate_mult * data_file.bytes_used());

  total_time.display("\n  Total execution time (I/O included) =");
  puts(" ");

  return 0;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Decoder functions - - - - - - - - - - - - - - - - - - - - - - - -

#else

int main(int numb_arg, char * arg[])
{
  if (numb_arg != 2) { 
    // printf("\n\t* Please read the file `codetree.doc' for "
    //   "instructions\n\t\tand Copyright/Patent information.\n\t* Use"
    //   " `%s -u' for command-line usage information\n\n", arg[0]);
  } else {
    printf("\n* Usage to compare the recovered image with the original:"
      "\n\n%s  coded_file  original_image  bpp_rate\n", arg[0]);
    printf("\n* Usage to save the recovered image:\n\n%s -s  "
      "coded_file  new_image_file  bpp_rate", arg[0]);
#ifdef LOSSLESS
    printf("\n\nFor lossless recovery use bpp_rate = 0");
#endif
    puts(" \n");  exit(0); }

  FILE * plot_file;
  float mse, psnr, max_rate = 0.0, rate_inc, peak_2 = 0.0;
  boolean cont, comp, interactive, cml;

  if (cml = ((numb_arg == 4) || (numb_arg == 5))) {
    interactive = false;
    if (numb_arg == 4) {
      cod_f_name = arg[1];  pic_f_name = arg[2];
      bit_rate = atof(arg[3]);  comp = true;  peak_2 = 65025.0; }
    else {
      if ((arg[1][0] != '-') || (arg[1][1] != 's'))
        Error("invalid command-line option");
      cod_f_name = arg[2];  new_f_name = arg[3];
      bit_rate = atof(arg[4]);  comp = false; } }
  else {
    interactive = Input_Answer("Interactive mode");
    Input_Line("Name of the compressed image file", cod_f_name);
    comp = !interactive ||
      Input_Answer("Recovered image will be compared to original");
    if (comp)
      Input_Line("File with original image", pic_f_name);
    if (!interactive) {
      plot_file = Open_File("File with rate x PSNR results", "wt");
      bit_rate = Input_Float("First rate (bits/pixel)");
      max_rate = Input_Float("Last rate  (bits/pixel)");
      rate_inc = Input_Float("Rate increment"); } }

  do {

    data_file.open_file(cod_f_name);
    dimension.x = data_file.decode_bits(6) << 8;
    dimension.y = data_file.decode_bits(6) << 8;
    dimension.x += data_file.decode_bits(8);
    dimension.y += data_file.decode_bits(8);
    rate_mult = 8.0 / (float(dimension.x) * float(dimension.y));
    threshold_bits = data_file.decode_bits(5);
    pel_bytes = 1 + data_file.decode_bits(1);
    smoothing = data_file.decode_bits(3);
    mean_shift = data_file.decode_bits(4);
    mean = data_file.decode_bits(10);

    // printf("\n  Image size = %d x %d, %d byte(s)/pixel\n",
    //   dimension.x, dimension.y, pel_bytes);
    // if (smoothing)
    //   printf("  Coded with smoothing factor %d\n", smoothing);
    if (interactive)
#ifdef LOSSLESS
      bit_rate = Input_Float("Code rate (bits/pixel, 0 = lossless)");
#else
      bit_rate = Input_Float("Code rate (bits/pixel)");
#endif

#ifdef LOSSLESS
    if (bit_rate <= 0) bit_rate = 8;
#endif
    if ((numb_arg > 1) && (arg[1][0] == '-') && (arg[1][1] == 's')) {
      byte_budget = long(bit_rate); 
      bit_rate *= rate_mult; 
    } else {
      byte_budget = long(bit_rate / rate_mult) - 1;
    }

    if (comp && (peak_2 == 0.0))
      peak_2 = (pel_bytes == 1 ? 65025.0 :
        Sqr(Input_Float("Peak value for PSNR computations")));

    // code_time.reset();  total_time.reset();  total_time.start();
    image.reset(dimension, pel_bytes, mean, mean_shift, smoothing);
    pyramid_dim = image.pyramid_dim();
    pyramid_levels = Min(Max_Levels(pyramid_dim.x),
                         Max_Levels(pyramid_dim.y));

    // code_time.start("\n  Starting image recovery...");
    Process_Image();
    // code_time.display(" Image recovered in");

    data_file.close_file();
    image.recover();
    // total_time.stop();

    if (comp) {
      printf("\n  Rate =%6.3f bits/pixel --> ", rate_mult *
        data_file.bytes_used());
      mse = image.compare(pic_f_name);
      if (mse > 0.0) {
        psnr = dBW(peak_2 / mse);
        printf("mean squared-error =%8.3f =%7.2f dB PSNR\n",
          mse, psnr); }
      else
        puts("no differences encountered"); }

    // total_time.display("\n  Total execution time (I/O included) =");

    if (interactive) {
      if (Input_Answer("Save new image to disk")) {
        Input_Line("Name of the new image file", new_f_name);
        image.write_pic(new_f_name); }
      cont = Input_Answer("Test new rate"); }
    else
      if (cml) {
        if (!comp) image.write_pic(new_f_name);
        cont = false; }
      else {
        fprintf(plot_file, "%9.4f %9.4f\n", bit_rate, psnr);
        bit_rate += rate_inc;
        cont = (bit_rate <= max_rate); }

  } while (cont);

  if (!(cml || interactive)) fclose(plot_file);
  // puts(" ");

  return 0;
}

#endif

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file < CodeTree.C >
