
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//            A R I T H M E T I C   C O D E   C L A S S E S

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//         > > > >    C++ version 3.01  -  04/07/95    < < < <

// Amir Said - amir@densis.fee.unicamp.br
// University of Campinas (UNICAMP)
// 13081 Campinas, SP, Brazil

// C++ implementation of the arithmetic-coding algorithm by I.H. Witten,
// R.M. Neal, and J.G. Cleary, published in ``Arithmetic coding for data
// compression,'' {\em Commun. ACM}, vol.~30, pp.~520--540, June 1987.



// - - Inclusion - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include "aritcode.h"


// - - Constants - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static const int CodeValueBits =    16;

static const long TopValue     = 65535L;  // 2^CodeValueBits - 1

static const long FirstQtr     = 16384L;  // (TopValue + 1) / 4

static const long Half         = 32768L;  // 2 * FirstQtr

static const long ThirdQtr     = 49152L;  // 3 * FirstQtr

static const int MaxFrequency  = 16383;  //  2^14 - 1

static const int MaxSymbols    =  1024;

static char * R_MSG = "< Encoder > cannot read from file";

static char * W_MSG = "< Decoder > cannot write to file";


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Functions of the base class < Adaptive_Model >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - - Private functions - - - - - - - - - - - - - - - - - - - - - - - -

void Adaptive_Model::update(int index)
{
  if (cum_f[0] == max_f) {
    int cum = 0;
    for (int j = nsb; j >= 0; j--) {
      cum_f[j] = cum;  cum += (freq[j] = ((freq[j] + 1) / 2)); } }

  int i;
  for (i = index; freq[i] == freq[i-1]; i--) ;
  if (i < index) {
    int sb_1 = indx_to_sb[i], sb_2 = indx_to_sb[index];
    indx_to_sb[i] = sb_2;  indx_to_sb[index] = sb_1;
    sb_to_indx[sb_1] = index;  sb_to_indx[sb_2] = i; }

  freq[i]++;
  while (i > 0) cum_f[--i]++;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Adaptive_Model::select_symbol(long value, long * l, long * h)
{
  long lm1 = *l - 1, range = *h - lm1;
  int cum = int(((value - lm1) * cum_f[0] - 1) / range);

  int index;
  for (index = 1; cum_f[index] > cum; index++);

  *h = lm1 + (range * cum_f[index-1]) / cum_f[0];
  *l += (range * cum_f[index]) / cum_f[0];

  int symbol = indx_to_sb[index];
  update(index);
  return symbol;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Adaptive_Model::new_interval(int symbol, long * l, long * h)
{
  int index = sb_to_indx[symbol];
  long lm1 = *l - 1, range = *h - lm1;
  *h = lm1 + (range * cum_f[index-1]) / cum_f[0];
  *l += (range * cum_f[index]) / cum_f[0];
  update(index);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Public functions  - - - - - - - - - - - - - - - - - - - - - - - -

void Adaptive_Model::set_symbols(int s)
{
  if ((s < 2) || (s > MaxSymbols))
    Error("invalid number of < Adaptive_Model > symbols");

  if (s != nsb) {
    nsb = s++;  max_f = Min(nsb << 5, MaxFrequency);
    delete [] cum_f;
    if ((cum_f = new int[s << 2]) == NULL)
      Error("< Adaptive_Model > : insufficient memory");
    freq = cum_f + s;  indx_to_sb = freq + s;
    sb_to_indx = indx_to_sb + s; }
  reset();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Adaptive_Model::reset(void)
{
  if (nsb == 0) return;

  int i;
  for (i = 0; i <= nsb; i++) {
    freq[i] = 1;  cum_f[i] = nsb - i; }
  freq[0] = 0;

  for (i = 0; i < nsb; i++) {
    sb_to_indx[i] = i + 1;  indx_to_sb[i+1] = i; }
}


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Functions of the class < Encoder >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - - Private functions - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::bit_plus_follow(int b)
{
  bit_buffer >>= 1;
  if (b) bit_buffer |= 0x80;
  if (!(--bit_index)) {
    byte_counter++;  bit_index = 8;
    if (putc(bit_buffer, out_file) == EOF) Error(W_MSG); }
  while (bits_to_follow > 0) {
    bits_to_follow--;  bit_buffer >>= 1;
    if (!b) bit_buffer |= 0x80;
    if (!(--bit_index)) {
      byte_counter++;  bit_index = 8;
      if (putc(bit_buffer, out_file) == EOF) Error(W_MSG); } }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::update_interval(void)
{
  ++symbol_counter;
  for (;;) {
    if (high < Half)
      bit_plus_follow(0);
    else
      if (low >= Half) {
        bit_plus_follow(1);  low -= Half;  high -= Half; }
      else
        if ((low >= FirstQtr) && (high < ThirdQtr)) {
          bits_to_follow++;  low -= FirstQtr;  high -= FirstQtr; }
        else
          break;
    low <<= 1;  high += high + 1; }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::reset(char * file_name)
{
  if (!closed) Error("< Encoder > file already open");

  out_file = (temp ? tmpfile() : fopen(file_name, "wb"));
  if (out_file == NULL) Error(W_MSG);
  symbol_counter = low = bits_to_follow = bit_buffer = closed = 0;  
  bit_index = 8;  high = TopValue;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::stop(void)
{
  if (closed) Error("< Encoder > file already closed");

  bits_to_follow++;
  bit_plus_follow(low >= FirstQtr);
  if (putc(bit_buffer >> bit_index, out_file) == EOF) Error(W_MSG);
  byte_counter++;  closed = 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Public functions  - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::open_file(void)
{
  temp = 1;  reset(NULL);  byte_counter = 5;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::open_file(char * file_name, int wd, int ht, int npl, int pelb)
{
  temp = 0;  reset(file_name);  byte_counter = 14;
  // added magic string, changed byte_counter from 1 to 14 
  if (putc('S', out_file) == EOF) Error(W_MSG);
  if (putc('P', out_file) == EOF) Error(W_MSG);
  if (putc('H', out_file) == EOF) Error(W_MSG);
  if (putc('T', out_file) == EOF) Error(W_MSG);
  if (putc('G', out_file) == EOF) Error(W_MSG);
  if (putc(wd & 0xFF, out_file) == EOF) Error(W_MSG);
  if (putc((wd >> 8) & 0xFF, out_file) == EOF) Error(W_MSG);
  if (putc((wd >> 16) & 0xFF, out_file) == EOF) Error(W_MSG);
  if (putc(ht & 0xFF, out_file) == EOF) Error(W_MSG);
  if (putc((ht >> 8) & 0xFF, out_file) == EOF) Error(W_MSG);
  if (putc((ht >> 16) & 0xFF, out_file) == EOF) Error(W_MSG);
  if (putc(npl & 0xFF, out_file) == EOF) Error(W_MSG);
  if (putc(pelb & 0xFF, out_file) == EOF) Error(W_MSG);
  if (putc(0x6F, out_file) == EOF) Error(W_MSG);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::close_file(char * file_name)
{
  if (!temp) Error("< Encoder > invalid close file function");

  stop();

// copy from temporary file to output file

  FILE * new_file = fopen(file_name, "wb");
  if (new_file == NULL) Error(W_MSG);

  unsigned char buff[256];
  long ns = symbol_counter;

  buff[0] = 0x53;
  for (int i = 4; i >= 1; i--) {
    buff[i] = (unsigned char) (ns & 0xFF);  ns >>= 8; }
  if (fwrite((char *) buff, sizeof(char), 5, new_file) != 5)
     Error(W_MSG);

  if (fflush(new_file)) Error(W_MSG);
  rewind(out_file);

  int nbs;
  do {
    nbs = fread((char *) buff, sizeof(char), 256, out_file);
    if (nbs == 0) break;
    if (fwrite((char *) buff, sizeof(char), nbs, new_file) != nbs)
      Error(W_MSG);
  } while (nbs == 256);

  if ((fclose(new_file) == EOF) || (fclose(out_file) == EOF))
    Error(W_MSG);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::close_file(void)
{
  if (temp)
    Error("< Encoder > invalid command to close file");
  stop();
  if (fclose(out_file) == EOF) Error(W_MSG);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::code_symbol(int s, Adaptive_Model & model)
{
  model.new_interval(s, &low, &high);
  update_interval();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::code_bits(int bits, int word)
{
  long lm1 = low - 1, range = high - lm1;
  word &= (1 << bits) - 1;
  high = lm1 + ((range * (word + 1)) >> bits);
  low += (range * word) >> bits;

  update_interval();
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Functions of the class < Decoder >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// - - Private functions - - - - - - - - - - - - - - - - - - - - - - - -

void Decoder::input_byte(void)
{
  if ((bit_buffer = getc(in_file)) == EOF)
    if (++extra_bits > CodeValueBits - 2)
      Error("< Decoder > end of file reached");
  bit_index = 8;  ++byte_counter;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Decoder::update_interval(void)
{
  if (++symbol_counter >= mss_symbols)
    close_file();
  else
    for (;;) {
      if (high >= Half)
        if (low >= Half) {
          value -= Half;  low -= Half;  high -= Half; }
        else
          if ((low >= FirstQtr) && (high < ThirdQtr)) {
            value -= FirstQtr;  low -= FirstQtr;  high -= FirstQtr; }
          else
            break;
      low <<= 1;  high += high + 1;  value <<= 1;
      if (!bit_index) input_byte();
      if (bit_buffer & 1) value++;
      bit_buffer >>= 1;  bit_index--; }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - Public functions  - - - - - - - - - - - - - - - - - - - - - - - -

void Decoder::open_file(char * file_name)
{
  if (!closed) Error("< Decoder > file already open");

  if ((in_file = fopen(file_name, "rb")) == NULL) Error(R_MSG);

  byte_counter = 14;  symbol_counter = 0;

  int i, magic_char = getc(in_file);

  // added magic string, changed byte_counter from 1 to 14 
  if (magic_char != 'S') Error("invalid < Decoder > file"); 
  magic_char = getc(in_file); 
  if (magic_char != 'P') Error("invalid < Decoder > file"); 
  magic_char = getc(in_file); 
  if (magic_char != 'H') Error("invalid < Decoder > file"); 
  magic_char = getc(in_file); 
  if (magic_char != 'T') Error("invalid < Decoder > file"); 
  magic_char = getc(in_file); 
  if (magic_char != 'G') Error("invalid < Decoder > file"); 
  for (i=0;i<8;i++) magic_char = getc(in_file); 
  magic_char = getc(in_file); 

  if (magic_char == 0x6F) {
    ext_stop = 1;  mss_symbols = 0x1000000L; }
  else
    if (magic_char == 0x53) {
      ext_stop = 0;  byte_counter += 4;
      unsigned char buff[4];
      if (fread((char *) buff, sizeof(char), 4, in_file) != 4)
        Error(R_MSG);
      for (mss_symbols = i = 0; i < 4; i++)
        mss_symbols = (mss_symbols << 8) + long(buff[i]); }
    else
      Error("invalid < Decoder > file");

  high = TopValue;  value = low = bit_index = extra_bits = closed = 0;
  for (i = 0; i < CodeValueBits; i++) {
    if (!bit_index) input_byte();
    value += value + (bit_buffer & 1);
    bit_buffer >>= 1;  bit_index--; }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Decoder::close_file(void)
{
  if (closed) Error("< Decoder > file already closed");
  fclose(in_file);  closed = 1;  mss_symbols = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Decoder::decode_symbol(Adaptive_Model & model)
{
  int symbol = model.select_symbol(value, &low, &high);
  update_interval();
  return symbol;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Decoder::decode_bits(int bits)
{
  long lm1 = low - 1, range = high - lm1;
  int word = int((((value - lm1) << bits) - 1) / range);
  long prod = word * range;
  high = lm1 + ((prod + range) >> bits);
  low += prod >> bits;
  update_interval();
  return word;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file < AritCode.C >
