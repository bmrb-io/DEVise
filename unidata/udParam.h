// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Universal Data Reader Function Types and Parameters
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef    _UDPARAM_H
#define    _UDPARAM_H

#include  <iostream.h>
#include  <stdlib.h>

#include  "RecId.h"
#include  "Schema.h"

class IntStack;

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifdef  NO_FUNCNAME
  // Defense in case you're unfortunate enough to not have g++
#    define __FUNCTION__  "<Unknown Function>"
#endif

class udParam;
class UniData;
class ParamStk;

typedef int (UniData::*AttrFunc)(char *dst, char *src, udParam *ud);

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class udParam {

  friend class UniData;
  friend class Attr;

  private:
    Attr     *attr;       // The attribute corresponding to this parameter
    AttrFunc  attrfunc;   // The function to read this attribute.

    int       flat_indx;  // The index into the flat attributes.
    int       dst_off;    // Where your data goes.

    off_t     buf_pos;    // The file offset this attribute considers to
                          // be position 0, for relative positioning as
                          // the buffer moves.

    ParamStk *subparam;   // The params of the subattributes

    int       use_slide;  // The data is in the slide-buffer. 

    Enum     *my_enum;    // For enums.

  public:
    udParam();
   ~udParam();

    int    sze() { return attr->size_of(); }

      // Make a copy.
    udParam *dup();

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

      // Stream printing (prints state, for debugging)
    friend ostream& operator<< (ostream& out, const udParam& param);
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#define DFLT_SZE    8

class ParamStk {

  public:
    udParam **data;
    int       length;
    int       data_size;

    ParamStk(int sze = DFLT_SZE) {
                        data = new udParam* [sze];
                        length = 0;
                        data_size = sze;
                      }

   ~ParamStk() { delete [] data; }

    int  count() { return(length); }
    int  is_empty() { return(length == 0); }

      // push() returns the position
    int  push(udParam *item);

    udParam *ith(int i) {
        if (i < length)
            return(data[i]);
        return NULL;
    }

        // Pull out the last element
    udParam *pop() {
        if (length > 0) {
            length--;
            return(data[length]);
        } else {
            return NULL;
        }
    }

      // Empty out the stack
    void empty(int del=1) {
        if (del)
            for (int j=0; j < length; j++)
                delete data[j];
        length = 0;
    }

      // Stream printing
    friend ostream& operator<< (ostream& out, const ParamStk& plist);
};

#undef DFLT_SZE

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _UDPARAM_H
