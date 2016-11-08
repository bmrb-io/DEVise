
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//              B I N A R Y   C O D E   C L A S S E S

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//         > > > >    C++ version  4.05  -  05/29/95   < < < <

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

#ifndef General_H
#include "general.h"
#endif


// - - External function - - - - - - - - - - - - - - - - - - - - - - - -

int Rep_Bits(int);


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Class definitions

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class Encoder
{
  // . class data   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

  protected :

    FILE * out_file;

    int bit_buffer, bit_index, closed;

    long byte_counter;

  // . constructor  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

  public :

    Encoder(void) { closed = true; }

  // . class functions .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    long bytes_used(void) { return byte_counter; }

    void open_file(char * file_name, int wd, int ht, int npl, int pelb);

    void close_file(void);

    void open_file(void);

    void close_file(char * file_name);

    void code_bits(int numb_bits, int symb);

    int code_bit(int symb);

}; // end of class < Encoder >


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class Decoder
{
  // . class data   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

  protected :

    FILE * in_file;

    int bit_buffer, bit_index;

    boolean closed;

    long byte_counter;

  // . constructor  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

  public :

    Decoder(void) { closed = true; }

  // . class functions .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    long bytes_used(void) { return byte_counter; }

    void open_file(char * file_name);

    void close_file(void);

    int decode_bits(int numb_bits);

    int decode_bit(void);

};  // end of class < Decoder >


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Inline functions

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

inline int Encoder::code_bit(int b)
{
  bit_buffer >>= 1;
  if (b) bit_buffer |= 0x80;
  if (!(--bit_index)) {
    byte_counter++;  bit_index = 8;
    if (putc(bit_buffer, out_file) == EOF)
      Error("< Encoder > cannot write to file"); }
  return b;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline int Decoder::decode_bit(void)
{
  if (!bit_index) {
    byte_counter++;  bit_index = 8;
    if ((bit_buffer = getc(in_file)) == EOF)
      Error("< Decoder > cannot read from file"); }
  int b = bit_buffer & 1;  bit_buffer >>= 1;  bit_index--;
  return b;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file < BinCode.H >
