
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//      V E C T O R - T R E E   I M A G E   C O M P R E S S I O N

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//           > > > >    C++ version  1.04  -  06/25/95   < < < <

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
#include "aritcode.h"


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

int LSP_plane, LSP_part, LSP_idx, threshold_bits, pyramid_levels;

int mean[3], transf_matrix[3][3];

long byte_budget, root_code;

float act_value, threshold, bit_rate, rate_mult;

float bias[32], * LSP_mark[32], *** LSP_mtx, ** LSP_ptr;

Tree_Node * LISP_head, * LISP_end;

Image_Coord dimension, pyramid_dim, root_dim, LSP_dim;

Image_3C image;

Adaptive_Model group_model[5], node_model[34], desc_model[34];

#ifdef ENCODER

  Encoder data_file;

  Image_3C max_image;

#else

  Decoder data_file;

#endif


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Function Implementations

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

int Node_Transition(int old_st, const Image_Coord & pc, int * signs)
{
  int st_trans = 0, os = old_st, ps = old_st, mask = 1;
  int msk[3], count[4];

  for (int i = 0; i < 4; i++) count[i] = 0;
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
    if (fabs(image(pc.x + SHF_x[i], pc.y + SHF_y[i])) >= threshold) {
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

  for (int i = 0; i < 4; i++) count[i] = 0;
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
    if (max_image(pc.x + SHF_x[i], pc.y + SHF_y[i]) >= threshold) {
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
#ifdef ENCODER
      if (image[nc] < 0) b |= m;
#else
      image[nc] = (b & m ? -act_value : act_value);
#endif
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
      m <<= 1; }

#ifdef ENCODER
  data_file.code_bits(nb, b);
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Node_Test(Tree_Node * act_node)
{
  int st_trans, signs;
  st_trans = Node_Transition(int(act_node->state & 0xFFL),
    act_node->coord, &signs);

  if (st_trans) {
    act_node->state |= long(st_trans);
    Output_Signs(signs, st_trans, act_node->coord); }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Desc_Test(Tree_Node * act_node)
{
  Image_Coord pc = act_node->coord;
  int st_trans = Desc_Transition(int(act_node->state & 0xFFL), pc);

  if (!st_trans) return;

  act_node->state |= st_trans;  st_trans >>= 1;

  Tree_Node * new_node;
  long ns = (act_node->state & 0xFFF00L) - 0x2000L;
  if (ns < 0x3000L) ns |= 0xAAL;
  boolean root = ((act_node->state & 0xFF000) >= root_code);

  for (int i = 0; i < 4; i++, st_trans >>= 2)
    if (st_trans & 0x1) {
      NEW_OBJECT(new_node, Tree_Node, "LISP entry");
      if (root) {
        if (i == 3) ns -= 0x1000L;
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

int Max_Levels(int n)
{
  int l1, l2;
  for (l1 = 0; !(n & 1); l1++) n >>= 1;
  for (l2 = l1 - 2; n; l2++) n >>= 1;
  return (l1 < l2 ? l1 : l2) - 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

boolean Sorting_Pass(void)
{
  int color;
  Tree_Node * prev = LISP_head, * node = prev;

  act_value = threshold + (bias[LSP_plane] = 0.38 * threshold);

  for (; node = node->next; prev = node)
    if ((node->state & 0x55L) != 0x55L) {
      color = int(node->state >> 8) & 0x3;
      image.set_color(color);
      Node_Test(node);
      if (data_file.bytes_used() >= byte_budget) return true; }

  for (node = prev = LISP_head; node = node->next; prev = node) {
    if ((node->state & 0xAAL) != 0xAAL) {
      color = int(node->state >> 8) & 0x3;
      image.set_color(color);
#ifdef ENCODER
      max_image.set_color(color);
#endif
      Desc_Test(node);
      if (data_file.bytes_used() >= byte_budget) return true; }
    if ((node->state & 0xFFL) == 0xFFL) {
      if (LISP_end == node) LISP_end = prev;
      prev->next = node->next;  delete node;  node = prev; } }

  return false;
}

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
    bias[p] = threshold * (0.5 - 0.25 / Sqr(LSP_plane - p));
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Process_Image(void)
{
  threshold = pow(2, threshold_bits);
  LSP_idx = LSP_part = LSP_plane = 0;  LSP_ptr = 0;
  LSP_dim.x = pyramid_dim.x >> 2;  LSP_dim.y = 12 * pyramid_dim.y;
  NEW_VECTOR(LSP_mtx, LSP_dim.x, float **, "LSP");

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
  root_dim.x = pyramid_dim.x >> pyramid_levels;
  root_dim.y = pyramid_dim.y >> pyramid_levels;
  root_code = long(pyramid_levels + 1) << 13;

  for (int c = 0; c < 3; c++) {
    long st = 0x2L + root_code + long(c << 8);
    for (cd.x = 0; cd.x < root_dim.x; cd.x += 2)
      for (cd.y = 0; cd.y < root_dim.y; cd.y += 2) {
        NEW_OBJECT(node, Tree_Node, "LISP entry");
        node->coord = cd;  node->state = st;
        LISP_end->next = node;  LISP_end = node; } }
  LISP_end->next = NULL;

  for (;; threshold *= 0.5) {
    if (Sorting_Pass()) break;
    if (Refinement_Pass()) break; }

  code_time.stop();

  for (c = LSP_part; c > 0;) delete [] LSP_mtx[--c];
  delete [] LSP_mtx;

  for (node = LISP_head; node;) {
    prev = node;  node = node->next;  delete prev; }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Encoder functions - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef ENCODER

float Max_Desc_Val(int l, int s, int x, int y)
{
  float t, m = 0;
  Image_Coord ds;
  --l;  --s;
  for (int d = 0; d < 4; d++) {
    ds.x = x + SHF_x[d];  ds.y = y + SHF_y[d];
    t = fabs(image[ds]);
    if (m < t) m = t;
    if (l > 0) {
      max_image[ds] = t = Max_Desc_Val(l, s, ds.x << 1, ds.y << 1);
      if (m < t) m = t; } }
  return m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Calc_Max_Image(void)
{
  Image_Coord tc, pc = pyramid_dim, lm = pc;
  int c, i, sd, lv = pyramid_levels, sp = lv + 1;
  float t, max = 0;

  lm.x >>= lv;  lm.y >>= lv;  pc.x >>= 1;  pc.y >>= 1;

  max_image.reset(pc);

  for (c = 0; c < 3; c++) {
    image.set_color(c);  max_image.set_color(c);
    for (pc.x = 0; pc.x < lm.x; pc.x += 2)
      for (pc.y = 0; pc.y < lm.y; pc.y += 2) {
        max_image[pc] = t = fabs(image[pc]);
        if (max < t) max = t;
        for (sd = sp, i = 1; i < 4; i++) {
          tc.x = pc.x + SHF_x[i];  tc.y = pc.y + SHF_y[i];
          t = fabs(image[tc]);
          if (max < t) max = t;
          if (i == 3) --sd;
          max_image[tc] = t = Max_Desc_Val(lv, sd,
            pc.x + SHF_x[i] * lm.x, pc.y + SHF_y[i] * lm.y);
          if (max < t) max = t; } } }

  return int(log(max) / log(2.0));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(int numb_arg, char * arg[])
{
  if (numb_arg != 2)
    printf("\n\t* Please read the file `codecolr.doc' for "
      "instructions\n\t\tand Copyright/Patent information.\n\t* Use"
      " `%s -u' for command-line usage information\n\n", arg[0]);
  else {
    printf("\n* Usage:\n\n%s image_file coded_file number_lines "
      "pixels/line bpp_rate", arg[0]);
    puts(" \n");      exit(0); }

  if (numb_arg == 6) {
    pic_f_name = arg[1];  cod_f_name = arg[2];
    dimension.x = atoi(arg[3]);  dimension.y = atoi(arg[4]);
    bit_rate = atof(arg[5]); }
  else {
    Input_Line("File with original image", pic_f_name);
    Input_Line("File with compressed image", cod_f_name);
    dimension.x = Input_Int("Number of image lines");
    dimension.y = Input_Int("Number of pixels per line");
    bit_rate = Input_Float("Rate (bits/pixel)"); }

  rate_mult = 8.0 / (float(dimension.x) * float(dimension.y));
  if (Max(dimension.x, dimension.y)>8192) Error("Image is too large!");
  byte_budget = long(bit_rate / rate_mult) - 1;

  total_time.start();
  image.read_pic(dimension, pic_f_name);
  image.transform();
  pyramid_dim = image.pyramid_dim();  image.averages(mean);
  image.color_transform(transf_matrix);
  pyramid_levels = Min(Max_Levels(pyramid_dim.x),
                       Max_Levels(pyramid_dim.y));

  Chronometer max_time;
  max_time.start("\n  Starting computation of maximum magnitudes...");
  threshold_bits = Calc_Max_Image();
  max_time.display(" Maximum magnitudes computed in");

  int ii, jj;
  data_file.open_file(cod_f_name);
  data_file.code_bits(6, dimension.x >> 8);
  data_file.code_bits(6, dimension.y >> 8);
  data_file.code_bits(8, dimension.x & 0xFF);
  data_file.code_bits(8, dimension.y & 0xFF);
  data_file.code_bits(5, threshold_bits);
  for (ii = 0; ii < 3; ii++) {
    data_file.code_bits(8, mean[ii]);
    for (jj = 0; jj < 3; jj++) {
      data_file.code_bits(8, transf_matrix[ii][jj] >> 8);
      data_file.code_bits(8, transf_matrix[ii][jj] & 0xFF); } }

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
  if (numb_arg != 2)
    printf("\n\t* Please read the file `codecolr.doc' for "
      "instructions\n\t\tand Copyright/Patent information.\n\t* Use"
      " `%s -u' for command-line usage information\n\n", arg[0]);
  else {
    printf("\n* Usage to compare the recovered image with the original:"
      "\n\n%s  coded_file  original_image  bpp_rate\n", arg[0]);
    printf("\n* Usage to save the recovered image:\n\n%s -s  "
      "coded_file  new_image_file  bpp_rate", arg[0]);
    puts(" \n");  exit(0); }

  FILE * plot_file;
  float mse, psnr, max_rate = 0.0, rate_inc;
  boolean cont, comp, interactive, cml;

  if (cml = ((numb_arg == 4) || (numb_arg == 5))) {
    interactive = false;
    if (numb_arg == 4) {
      cod_f_name = arg[1];  pic_f_name = arg[2];
      bit_rate = atof(arg[3]);  comp = true; }
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

    int ii, jj;
    data_file.open_file(cod_f_name);
    dimension.x = data_file.decode_bits(6) << 8;
    dimension.y = data_file.decode_bits(6) << 8;
    dimension.x += data_file.decode_bits(8);
    dimension.y += data_file.decode_bits(8);
    rate_mult = 8.0 / (float(dimension.x) * float(dimension.y));
    threshold_bits = data_file.decode_bits(5);
    for (ii = 0; ii < 3; ii++) {
      mean[ii] = data_file.decode_bits(8);
      for (jj = 0; jj < 3; jj++) {
        transf_matrix[ii][jj] = data_file.decode_bits(8) << 8;
        transf_matrix[ii][jj] += data_file.decode_bits(8); } }

    printf("\n  Image size = %d x %d\n", dimension.x, dimension.y);
    if (interactive)
      bit_rate = Input_Float("Code rate (bits/pixel)");

    byte_budget = long(bit_rate / rate_mult) - 1;

    code_time.reset();  total_time.reset();  total_time.start();
    image.reset(dimension, mean, transf_matrix);
    pyramid_dim = image.pyramid_dim();
    pyramid_levels = Min(Max_Levels(pyramid_dim.x), 
                         Max_Levels(pyramid_dim.y));

    code_time.start("\n  Starting image recovery...");
    Process_Image();
    code_time.display(" Image recovered in");

    data_file.close_file();
    image.recover();
    total_time.stop();

    if (comp) {
      printf("\n  Rate =%6.3f bits/pixel --> ", rate_mult * 
        data_file.bytes_used());
      mse = image.compare(pic_f_name);
      if (mse > 0.0) {
        psnr = dBW(65025.0 / mse);
        printf("mean squared-error =%8.3f =%7.2f dB PSNR\n",
          mse, psnr); }
      else
        puts("no differences encountered"); }

    total_time.display("\n  Total execution time (I/O included) =");

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
  puts(" ");

  return 0;
}

#endif

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file < CodeColr.C >
