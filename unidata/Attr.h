// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Attribute and Type Definitions
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef    _ATTR_H
#define    _ATTR_H

#include  <stdlib.h>
#include  <iostream.h>

#include  "IntStack.h"
#include  "Traits.h"
#include  "PerlFrag.h"

class ParamStk;

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stores a list of attribute names.
class FuncOf {

  public:
    char    *attr;
    int      indx;
    FuncOf  *next;

    FuncOf(char *_attr=NULL, FuncOf *_next=NULL) {
        attr = _attr;
        indx = 0;
        next = _next;
    }
    
   ~FuncOf() { delete [] attr; }

      // Make a duplicate (deep-copy)
    FuncOf *dup();
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

  // Enumeration of types
enum attr_t {
    Invalid_Attr = 0,

    Int_Attr,
    Float_Attr,
    Double_Attr,
    String_Attr,

    UnixTime_Attr,       // We need to figure these out yet.
    Date_Attr,
    DateTime_Attr,

    UserDefined_Attr     // This should be last, so we can use
                         // it to define new types at runtime.
};

class AttrStk;
class udParam;
class Schema;

  // This class is for both attributes and typedef's
class Attr {

  friend class Schema;
  friend class UniData;

  private:
    attr_t    _type;       // The type of the attribute
    size_t    _size;       // The size of the attribute
    char     *_name;       // The name of the attribute
    char     *_flat_name;  // The name of the attribute (flattened)

    int       _offset;     // Offset in the output record.

    int       _nattrs;     // Flat - sum over all sub-attributes.

    AttrStk  *_subattr;    // Defs of nested types.

        // min/max are binary versions of the values from the schema.
    attr_t    _min_type;   // The type of thing _min is.
    attr_t    _max_type;   // The type of thing _max is.
    char     *_min;        // Minimum valid value
    char     *_max;        // Maximum valid value

    int       _maxlen;     // Max length (for strings)
    char     *_date_frmt;  // for DateTime values.

    char      _have_lpos;  // _lpos is valid
    char      _have_rpos;  // _rpos is valid
    int       _lpos;       // Starting position
    int       _rpos;       // Ending position

    char     *_delimit;    // delimiter (marks end of something)
    char     *_seper;      // seperator (seperates subitems)
    char     *_white;      // whitespace (ignore these characters)

    Traits   *_traits;     // Our traits

    FuncOf   *_func_of;    // List of attrs this attr is a function_of.

    char     *_format;
    PerlFrag *_reader;     // Perl attribute reader.
    PerlFrag *_value;      // Perl computed attribute.
    PerlFrag *_filter;     // Perl filter routine.

    SV       *_perl_var;   // An equivilent perl variable, if one exists.

    IntStack *_sublen;     // For temporary storage of lengths,
    IntStack *_sublpos;    // left,
    IntStack *_subrpos;    // and right positions of sub attributes.


      // Determine size based on _type.
    size_t  determ_size();

  public:
    Attr(char *name=NULL);
   ~Attr();

    char   *name() { return _name; }
    char   *flat_name() { return _flat_name; }
    attr_t  type() { return _type; }
    int     offset() { return _offset; }

    FuncOf *func_of() { return _func_of; }
    int     nattrs() { return _nattrs; }

    void set_type(attr_t t) { _type = t; }
    void set_maxlen(int len) { _maxlen = len; }
    void set_func_of(FuncOf *f) { _func_of = f; }

      // min/max values, NULL means we don't have any.
    attr_t min_type() { return _min_type; }
    attr_t max_type() { return _max_type; }
    char *min() { return _min; };
    char *max() { return _max; };

    void set_lpos(int lpos) { _have_lpos = 1; _lpos = lpos; }
    void set_rpos(int rpos) { _have_rpos = 1; _rpos = rpos; }

    char have_lpos() { return _have_lpos; }
    char have_rpos() { return _have_rpos; }
    int  lpos() { return _lpos; }
    int  rpos() { return _rpos; }

    char *delimiter() { return _delimit; }
    char *seperator() { return _seper; }
    char *whitespace() { return _white; }

    char *date_format() { return _date_frmt; }

    void set_delimit(char *delim) { delete [] _delimit; _delimit = delim; }
    void set_seper(char *seper) { delete [] _seper; _seper = seper; }
    void set_white(char *white) { delete [] _white; _white = white; }

    char *dup_seper() { char *nw = NULL;
                        if (_seper) {
                            nw = new char[strlen(_seper)+1];
                            strcpy(nw,_seper);
                        }
                        return(nw);
                      }

    char *dup_white() { char *nw = NULL;
                        if (_white) {
                            nw = new char[strlen(_white)+1];
                            strcpy(nw,_white);
                        }
                        return(nw);
                      }

    void set_format(char *fmt) { _format = fmt; }
    void set_value(FragType type, char *val) {
        if (_value)
            delete _value;
        _value = new PerlFrag(this,type);
        _value->set_nrets(1);
        _value->set_src(val);
    }

    void set_filter(FragType type, char *flt) {
        if (_filter)
            delete _filter;
        _filter = new PerlFrag(this,type);
        _filter->set_nrets(1);
        _filter->set_src(flt);
    }

    void set_reader(FragType type, char *rdr) {
        if (_reader)
            delete _reader;
        _reader = new PerlFrag(this,type);
        _reader->set_src(rdr);
    }

    char *format() { return _format; }

    PerlFrag *value() { return _value; }
    PerlFrag *filter() { return _filter; }
    PerlFrag *reader() { return _reader; }

    SV  *perl_var() { return _perl_var; }

      // Does this attr or it's children use perl at all?
    int  uses_perl();
      // Provide a Perl Interpreter to use.
    void set_interp(PerlInterpreter *perl);

      // Set/Get the perl variable for this attribute.
    void set_var(char *field);
    void grab_var(char *field);

      // compile all perl subroutines for later use.
    void compile(unsigned int& subrcnt);
    
    size_t size_of() { return _size; }

    AttrStk *subattr() { return _subattr; }

    Traits *traits() { return _traits; }

      // Make a duplicate (deep-copy)
    Attr *dup();

      // Flatten the attribute into the AttrStk provided.
    int  flatten(AttrStk *stk, char *prename=NULL);

      // Propagate info between _sublpos,_subrpos,_sublen
      // returns true if we have completely determined _sublpos at end.
    int  propagate_pos();

      // Copy the top level of src attribute (a typedef?) to this.
    void assign(Attr *src);

      // Stream printing (for debugging)
    friend ostream& operator<< (ostream& out, const Attr& attr); 
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#define    DEFAULT_HINT    4

class AttrStk {

  //private:
  public:
    int    _num;        // Number of Attrs in the stack.  
    int    _sze;        // Size of the stack.

    Attr  **_stk;       // Doubling array of Attr*

  public:
    AttrStk(int hint=DEFAULT_HINT);
   ~AttrStk();

    int   push(Attr *attr);
    Attr *pop();

    int   nAttrs() { return _num; }

      // Return pointer to ith Attr
    Attr *ith(int i) {
        return (i < _num) ? (_stk[i]) : (Attr*) NULL;
    }

      // Empty out the stack, if del then delete the Attr* being stored.
    void empty(int del=0);

    int  is_empty() { return this ? (_num == 0) : 1; }

      // Make a duplicate (shallow-copy)
    AttrStk *dup();

      // Find an attribute with this name;
      // return it's position, or -1 if not found.
    int find(char *name);
};

#undef    DEFAULT_HINT

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _ATTR_H
