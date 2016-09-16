
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//         G E N E R A L   P U R P O S E   F U N C T I O N S

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
//           > > > >    C++ version  1.07 -  05/29/95   < < < <

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


// - - Control definition  - - - - - - - - - - - - - - - - - - - - - - -

#ifndef General_H
#define General_H

// - - Inclusion - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#undef abs


// - - Type definitions  - - - - - - - - - - - - - - - - - - - - - - - -

#define boolean int
#define true    1
#define false   0

typedef char Char_Line[80];
typedef unsigned char byte;
typedef unsigned long word;


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Macros

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

#define NEW_VECTOR(X,N,TYPE,MSG) Test_Pointer(X = new TYPE[N], MSG)

#define CREATE_VECTOR(X,N,TYPE,MSG) TYPE * X = new TYPE[N];\
  Test_Pointer(X, MSG)

#define NEW_OBJECT(X,TYPE,MSG) Test_Pointer(X = new TYPE, MSG)

#define CREATE_OBJECT(X,TYPE,MSG) TYPE * X = new TYPE;\
  Test_Pointer(X, MSG)

#define DELETE_VECTOR(X) delete [] X;  X = NULL

#define DELETE_OBJECT(X) delete X;  X = NULL

#define SWAP(X,Y,T) T = X;  X = Y;  Y = T


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Inline functions

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

inline int Min(int a, int b)
  { return (a < b ? a : b); }

inline int Max(int a, int b)
  { return (a > b ? a : b); }

inline long Round(double x)
  { return (x >= 0.0 ? long(x + 0.5) : long(x - 0.5)); }

inline float Sqr(float x)
  { return x * x; }

inline double dBW(double x) { return 4.34294481904 * log(x); }

inline double dBW_inv(double x) { return exp(0.2302585093 * x); }


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Function prototypes

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


void Warning(char *);

void Error(char *);

void Test_Pointer(void * ptr, char * msg = NULL);

void Pause(void);

char * Input_Line(char * msg = NULL, char * res = NULL);

int Input_Int(char *);

float Input_Float(char *);

boolean Input_Answer(char *);

FILE * Open_File(char * msg, char * mode);


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

//  Definition of the class < Chronometer >

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

class Chronometer
{
  // . private data .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    int stat;         // chronometer status: 0 = off, 1 = on

    long mark, elp;   // initial and elapsed time

  // . constructor  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

  public:

    Chronometer(void) { elp = stat = 0; }

  // . public functions   .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .

    void reset(void) { elp = stat = 0; }

    void start(char * s = NULL);

    void stop(void);

    float read(void);

    void display(char * s = NULL);

};  // end definition of class < Chronometer >

#endif

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

// end of file < General.H >
