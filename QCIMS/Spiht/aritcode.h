
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

#ifndef General_H
#include "general.h"
#endif


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Class definitions

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class Adaptive_Model
{
  // . private data .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    int max_f, nsb, * cum_f, * freq, * indx_to_sb, * sb_to_indx;

  // . friend classes  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    friend class Encoder;

    friend class Decoder;

  // . private functions  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    void update(int index);

    int select_symbol(long value, long * l, long * h);

    void new_interval(int symb, long * l, long * h);

  // . constructors and destructor .  .  .  .  .  .  .  .  .  .  .  .  .

  public :

    Adaptive_Model(void) { nsb = 0;  cum_f = NULL; }

    Adaptive_Model(int ns) { nsb = 0;  cum_f = NULL;  set_symbols(ns); }

    ~Adaptive_Model(void) { nsb = 0;  delete cum_f; }

  // . public functions   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    void reset(void);

    void set_symbols(int ns);

};  // end definition of class < Adaptive_Model >


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class Encoder
{
  // . private data .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    FILE * out_file;

    int bit_buffer, bit_index, closed, temp;

    long byte_counter, symbol_counter, low, high, bits_to_follow;

  // . private functions  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    void bit_plus_follow(int b);

    void update_interval(void);

    void reset(char * file_name);

    void stop(void);

  // . constructor  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

  public :

    Encoder(void) { closed = 1; }

  // . public functions   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    long bytes_used(void) { return byte_counter; }

    long symbols_encoded(void) { return symbol_counter; }

    void open_file(void);

    void close_file(char * file_name);

    void open_file(char * file_name, int wd, int ht, int npl, int pelb);

    void close_file(void);

    void code_symbol(int s, Adaptive_Model &);

    void code_bits(int bits, int word);

    int code_bit(int bit);

};  // end definition of class < Encoder >


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class Decoder
{
  // . private data .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    FILE * in_file;

    int bit_buffer, bit_index, extra_bits, ext_stop, closed;

    long low, high, value, symbol_counter, byte_counter, mss_symbols;

  // . private functions  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    void input_byte(void);

    void update_interval(void);

  // . constructor  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

  public :

    Decoder(void) { closed = 1; }

  // . public functions   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    int end_of_file(void) { return (symbol_counter >= mss_symbols); }

    long bytes_used(void) { return byte_counter; }

    long symbols_decoded(void) { return symbol_counter; }

    long message_symbols(void) { return mss_symbols; }

    void open_file(char * file_name);

    void close_file(void);

    int decode_symbol(Adaptive_Model &);

    int decode_bits(int bits);

    int decode_bit(void);

};  // end definition of class < Decoder >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Inline functions

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

inline int Encoder::code_bit(int bit)
{
  long lm1 = low - 1, range = high - lm1;
  if (bit) {
    high = lm1 + range;  low += range >> 1; }
  else
    high = lm1 + (range >> 1);

  update_interval();
  return bit;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline int Decoder::decode_bit(void)
{
  long lm1 = low - 1, range = high - lm1;
  int bit = int((((value - lm1) << 1) - 1) / range);
  if (bit) {
    high = lm1 + range;  low += range >> 1; }
  else
    high = lm1 + (range >> 1);

  update_interval();
  return bit;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file < AritCode.H >
