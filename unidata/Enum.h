// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Enum Definitions
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef    _ENUM_H
#define    _ENUM_H

#include  <string.h>
#include  <iostream.h>

#include  "Attr.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#define    ENUM_INVALID    "Invalid"

typedef    unsigned short ushort;

#define    ENUM_SZE_DFLT    8

  // A slighter faster, more convienent string comparator.
#define    STREQ(X,Y)    (((X)==(Y)) || \
            ((*(X)==*(Y)) && (strcmp((X),(Y))==0)))

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class Enum {

  private:
    int      _etype;       // The type of the enumeration.
    char    *_name;        // The name of the enumeration.

    ushort   _num;         // The number of symbols in this Enum
    ushort   _sze;         // The size of our arrays.

    char   **_syms;        // The symbols of the enumeration.
    char   **_vals;        // and their values, which
                           // might share ptrs with _syms.

    attr_t  *_vtypes;      // The types of the values.


  public:
    Enum(char *nme=NULL);
   ~Enum();

    char  *name() { return _name; }
    int    type() { return _etype; }
    ushort nsyms() { return _num; }

    char  *symbol( ushort ecode ) {
               return (ecode < _num) ? _syms[ecode] : _syms[0];
           }

    char  *value( ushort ecode ) {
               return (ecode < _num) ? _vals[ecode] : _vals[0];
           }

    attr_t vtype( ushort ecode ) {
               return (ecode < _num) ? _vtypes[ecode] : _vtypes[0];
           }

      // Add this symbol to this enumeration.
    ushort add(char *sym, char *val, attr_t vtype);

      // Locate the code for this value.
    ushort vfind( char *val, attr_t vtype );

      // Locate the code for this symbol.
    ushort find( char *sym );

      // Stream printing (for debugging)
    friend ostream& operator<< (ostream& out, const Enum& em); 
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#define    DEFAULT_HINT    4

class EnumStk {

  private:
    int    _num;        // Number of Enums in the stack.  
    int    _sze;        // Size of the stack.

    Enum **_stk;       // Doubling array of Enum*

  public:
    EnumStk(int hint=DEFAULT_HINT);
   ~EnumStk();

    int   push(Enum *em);
    Enum *pop();

    int   nEnums() { return _num; }

      // Return pointer to ith Enum
    Enum *ith(int i) {
        return (i < _num) ? (_stk[i]) : (Enum*) NULL;
    }

      // Empty out the stack, if del then delete the Enum* being stored.
    void empty(int del=0);

      // Find an enum with this name;
      // return it's position, or -1 if not found.
    int find(char *name);
};

#undef    DEFAULT_HINT

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _ENUM_H
