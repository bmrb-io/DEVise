
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

#include "bincode.h"


// - - External definitions  - - - - - - - - - - - - - - - - - - - - - -

static char * W_MSG = "< Encoder > cannot write to file";

static char * R_MSG = "< Decoder > cannot read from file";


// - - External function - - - - - - - - - - - - - - - - - - - - - - - -

int Rep_Bits(int n)
{
  if (n < 0) n = -n;
  if (n < 3) return n;
  n >>= 2;
  for (int k = 2; n; k++) n >>= 1;
  return k;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Member functions of the class < Encoder >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void Encoder::open_file(char * file_name, int wd, int ht, int npl, int pelb)
{
  if (!closed)  Error("< Encoder > file already open");
  if ((out_file = fopen(file_name, "wb")) == NULL) Error(W_MSG);
  bit_index = 8;  byte_counter = 14;  closed = bit_buffer = 0;
  // added magic string, changed byte_counter from 1 to 6 --VR 
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
  if (putc(0x6E, out_file) == EOF) Error(W_MSG);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::close_file(void)
{
  if (closed) Error("< Encoder > file already closed");
  if (putc(bit_buffer >> bit_index, out_file) == EOF) Error(W_MSG);
  ++byte_counter;  closed = true;
  if (fclose(out_file) == EOF) Error(W_MSG);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Encoder::code_bits(int bits, int symbol)
{
  for (int m = 1 << (bits - 1); m > 0; m >>= 1) {
    bit_buffer >>= 1;
    if (symbol & m) bit_buffer |= 0x80;
    if (!(--bit_index)) {
      byte_counter++;  bit_index = 8;
      if (putc(bit_buffer, out_file) == EOF) Error(W_MSG); } }
}


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Member functions of the class < Decoder >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void Decoder::open_file(char * file_name)
{
  int i; 
  if (!closed) Error("< Decoder > file already open");
  if ((in_file = fopen(file_name, "rb")) == NULL) Error(R_MSG);
  // added Magic string, changed byte_counter from 1 to 6 --VR
  if (getc(in_file) != 'S') Error("invalid < Decoder > file");
  if (getc(in_file) != 'P') Error("invalid < Decoder > file");
  if (getc(in_file) != 'H') Error("invalid < Decoder > file");
  if (getc(in_file) != 'T') Error("invalid < Decoder > file");
  if (getc(in_file) != 'G') Error("invalid < Decoder > file");
  for (i=0;i<8;i++) getc(in_file); 
  if (getc(in_file) != 0x6E) Error("invalid < Decoder > file");
  byte_counter = 14;  bit_index = closed = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void Decoder::close_file(void)
{
  if (closed) Error("< Decoder > file already closed");
  if (fclose(in_file) == EOF) Error ("< Decoder > cannot close file");
  closed = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Decoder::decode_bits(int bits)
{
  int symbol = 0;
  for (int m = 1 << (--bits); m; m >>= 1) {
    if (!bit_index) {
      byte_counter++;  bit_index = 8;
      if ((bit_buffer = getc(in_file)) == EOF) Error(R_MSG); }
    if (bit_buffer & 1) symbol |= m;
    bit_buffer >>= 1;  bit_index--; }

  return symbol;
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file < BinCode.C >
