// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Schema Definition
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef    _SCHEMA_H
#define    _SCHEMA_H

#include  <iostream.h>
#include  <stdio.h>
#include <math.h>
#include  <unistd.h>


#include  "Attr.h"
#include  "Enum.h"
#include  "Lexer.h"
#include  "Comment.h"
#include  "IntStack.h"
#include  "Config.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class PerlFrag;

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class Schema {

  friend class UniData;

  public:
    // Enumerations for schema-level choices.

    enum Type {
        TEXT,         // the default
        BINARY
    };
    
    
    enum CharSet {
        ASCII        // the default, :^)
    };
    
    
    enum BinOrder {
        NATIVE,       // the default
        NETWORK
    };

    enum RecLenInd {
        VAR_LEN,      // the default
        FIXED_LEN
    };

  private:
    char        *_name;     // Name of the schema
    int          _valid;    // Is this schema valid (no errors)

    Type         _type;     // Type of the schema
    CharSet      _char_set; // Character Set
    BinOrder     _bin_type; // Binary ordering (when _type == BINARY)

    RecLenInd    _rli;      // Record length indicator
    int          _reclen;   // Length, (if _rli == FIXED_LEN)

    CmmtStk     *_cmmts;    // The comment delimiters.

    PerlFrag    *_global;   // This code gets run once at the start
                            // of parsing, and at each call to reset().

    char        *_converter;// Our converter program.

    int          _nattrs;   // The number of attrs in the schema (flat).
    int          _recsze;   // The amnt of space needed to store the record.

    AttrStk     *_attr_own; // This contains all the Attr's (at all levels)
                            // in this schema, so we can share pointers
                            // like wild and still have a prayer of
                            // deleting them properly.

    EnumStk     *_enums;    // Any enums that are defined.
    AttrStk     *_typedefs; // User-defined types

    Attr        *_attr0;    // The top-level attribute of this schema.
    AttrStk     *_flat_attrs;// The attributes of this schema (flat).

    FuncOf      *_func_op;  // Attr names must be stored until the
                            // entire schema has been read.
    IntStack    *_ordering; // The order the attributes should be 
                            // processed in.

    unsigned int _subr_cnt; // Used to give names to perl subroutines.


    void init();
    void clear();

    int  get_token(UniLexType *ult);
    void dispose_token(int tok, UniLexType *ult);

      // Match the expected token, return 1 if match succeeds,
      // or 0 if it fails, and print an error.
    int match(int exp_tok, UniLexType *ult);

      // Build a literal string, ie convert all \t,\n, etc to
      // there real equivilents. (allocates memory)
    char *mkLiteral(char *in);

      // Parse a position-style range (ie: [5..13]).
    int parse_range(int& init, int& end);

      // Parse a list of integers, if the 2nd stack is non-null,
      // allow position-style also (ie: [5..13]).
    int parse_ilist(IntStack *init, IntStack *end=NULL);

      // Fill in the attribute given.
      // Recursively calls itself to parse the lower levels. 
      // If attr is NULL, the top-level is being parsed.
    int subParse(Attr *attr);

      // Pre-compute as much about the attributes as we can:
      // ordering, field offset, and anything else.
    void DeepThought(ostream *err);

      // Check that this schema is reasonable.
    int  RealityCheck(ostream *err);

      // Does this schema use perl at all?
    int  uses_perl();
      // Provide a Perl Interpreter to use.
    void set_interp(PerlInterpreter *perl);
    void setup_perl(PerlInterpreter *perl);

  public:

    Schema();
   ~Schema();

      // Parse a schema, return the offset the "data" starts at.
    off_t Parse(int fd, const char *filename, ostream *err=&cerr);

// SunOS apparently defines 'valid' as a macro somewhere...  RKW 4/9/97.
#ifdef valid
#undef valid
#endif
    int valid() { return _valid; }

    char *Name() { return _name; }

    Type      getType() { return _type; }
    CharSet   getCharSet() { return _char_set; }
    BinOrder  getBinOrder() { return _bin_type; }
    RecLenInd getRecLen(int& reclen) { reclen = _reclen; return _rli;  }

    CmmtStk *comments() { return _cmmts; }

    char *converter() { return _converter; }

    int NumAttrs() { return _attr0 ? _attr0->subattr()->nAttrs() : 0; }
    int NumFlatAttrs() { return _nattrs; }

      // Get the description of an Attribute (just to look at)
    Attr *GetAttr(int n) { return _attr0 ? _attr0->subattr()->ith(n)
                                         : (Attr*) NULL; }
    Attr *GetFlatAttr(int n) { return _flat_attrs ?
                                      _flat_attrs->ith(n) : (Attr*) NULL; }

      // Get the description of the top-level Attributes (just to look at)
    Attr  *attr0() { return _attr0; }

      // Get the description of all Attributes (just to look at)
      // (These are nested, not flat)
    AttrStk *GetAttrs() { return _attr0->subattr(); }

      // (These are flat)
    AttrStk *GetFlatAttrs() { return _flat_attrs; }

      // Maybe make an interface that requests specific things
      // instead of giving full access.
    //PerlInterpreter *perli() { return _perli; }

      // This is the sum of the sizes of all fields, it has 
      // nothing to do with the 'recsize=' line from the schema.
    int recSze() { return _recsze; }

      // duplicate this schema (deep-copy)
    Schema *dup();

      // Stream printing (for debugging)
    friend ostream& operator<< (ostream& out, const Schema& schema); 
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _SCHEMA_H
