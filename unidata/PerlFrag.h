// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//   Perl Code Interface
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef  _PERLCODE_H
#define  _PERLCODE_H


// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#include <stdio.h>
#include <string.h>
#include <math.h>             // We need to get to it before perl does
#include <iostream.h>

extern "C" {
#    define  explicit explic      // A C++ keyword?
#    include "EXTERN.h"
#    include "perl.h"
#    undef   explicit
}

#include "Lexer.h"

class Attr;

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

enum FragType {
    FT_NONE=0,      // Nothing (an empty fragment).
    FT_EXPR,        // An expression.
    FT_GLOB,        // Global code.
    FT_SUBR         // A subroutine.
};

// A small fragment of perl code
class PerlFrag {

  private:
    FragType _type;        // Type of this fragment (see enum above).

    char    *_src;         // The text that this frag was generated from.

    I32      _flags;       // The flags to use when evaluating.

    int      _nrets;       // The number of return values desired.

    SV      *_code;        // Pointer to some perl code or sub name.

    Attr    *_attr;        // The attribute we are inside of.

                           // The perl interpreter to use.
    PerlInterpreter *_perl; 


  public:

    AV      *_args;        // Params being passed in, if any.
    AV      *_rets;        // and the return values, if any.

    PerlFrag *next;        // For building a list of fragments.

    PerlFrag(Attr *owner=NULL, FragType type=FT_NONE);
   ~PerlFrag();

    void set_interp(PerlInterpreter *perl) {
        if (this) {
            _perl = perl;
            if (next)
                next->set_interp(perl);
        }
    }

    void set_src(char *src) { delete [] _src; _src = src; }

    void set_fmt(char *fmt);

    void set_nrets(int nr) { if (this) _nrets = nr; }

        // Add this to the end of the list.
    void enlist(PerlFrag *pf);

        // Duplicate this perl fragment.
    PerlFrag *dup();

        // Compile the stored code.
    void compile(unsigned int& subrcnt, char *flat_name);

        // Set a argument to pass in as the @_ parameter.
    void set_arg(char *line);

        // Evaluate this fragment.
        // (Should already have args setup.)
    I32 Eval();

      // Stream printing (for debugging)
    friend ostream& operator<< (ostream& out, const PerlFrag& pf); 
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif // _PERLCODE_H
