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

#include  "Enum.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
Enum::Enum(char *nme)
{
    _etype = 0;
    _name  = nme;

    _num = 1;        // Always define an "Invalid" type.
    _sze = ENUM_SZE_DFLT;

    _syms = new char* [_sze];
    _vals = new char* [_sze];
    _vtypes = new attr_t [_sze];

    _syms[0] = new char [strlen(ENUM_INVALID)+1];
    strcpy(_syms[0], ENUM_INVALID);
    _vals[0] = NULL;
    _vtypes[0] = String_Attr;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
Enum::~Enum()
{
    ushort j;

    for (j=0; j < _num; j++) {
        if (_vals[j] != _syms[j])
            delete [] _vals[j];
        delete [] _syms[j];
    }

    delete [] _syms;
    delete [] _vals;
    delete [] _vtypes;

    delete [] _name;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Add this symbol to this enumeration.
ushort Enum::add(char *sym, char *val, attr_t vtype)
{
    ushort j;

    if (!val) {        // A new default/invalid value.
        delete [] _syms[0];
        _syms[0] = sym;
        return 0;
    }

    if (_num == _sze) {    // Need to double array sizes
        char   **old_syms = _syms;
        char   **old_vals = _vals;
        attr_t  *old_vtypes = _vtypes;

        _sze += _sze;
        _syms = new char* [_sze];
        _vals = new char* [_sze];
        _vtypes = new attr_t [_sze];

        for (j=0; j < _num; j++) {
            _syms[j]   = old_syms[j];
            _vals[j]   = old_vals[j];
            _vtypes[j] = old_vtypes[j];
        }

        delete [] old_syms;
        delete [] old_vals;
        delete [] old_vtypes;
    }

    _syms[_num]   = sym;
    _vals[_num]   = val;
    _vtypes[_num] = vtype;

    return _num++;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Locate the code for this symbol.
ushort Enum::find( char *sym )
{
    ushort j;

    for (j=0; j < _num; j++)
        if (STREQ(sym,_syms[j]))
            return j;

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Locate the code for this value.
ushort Enum::vfind( char *val, attr_t vtype )
{
    ushort j;

    // NYI - does not yet handle regular expressions.

    for (j=1; j < _num; j++)
        if ((vtype == _vtypes[j]) && (*val == *(_vals[j]))) {

            switch (vtype) {
                default:
                    return 0;

                case Int_Attr:
                    if ( *((int*) val) == *((int*) _vals[j]))
                        return j;
                    break;

                case Double_Attr:
                    if ( *((double*) val) == *((double*) _vals[j]))
                        return j;
                    break;

                case String_Attr:
                    if (strncmp(val,_vals[j],strlen(_vals[j])) == 0)
                        return j;
                    break;
            }
        }

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (for debugging)
ostream& operator<< (ostream& out, const Enum& em)
{
    ushort j;

    out << "Enum " << em._name << ":\n";
    out << "type is: " << em._etype << endl;

    out << "Num/Size is: " << em._num << "/" << em._sze << endl;

    for (j=0; j < em._num; j++) {

        out << "[" << j << "]: " << em._syms[j] << " is ";

        switch (em._vtypes[j]) {
            default:
                break;

            case Int_Attr:
                out << *((int*) em._vals[j]);
                break;

            case Double_Attr:
                out << *((double*) em._vals[j]);
                break;

            case String_Attr:
                if (em._vals[j])
                    out << "'" << em._vals[j] << "'";
                else
                    out << "NULL";
                break;
        }
        out << endl;
    }
    out << endl;

    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
EnumStk::EnumStk(int hint)
{
    _num = 0;
    _sze = hint;

    if (_sze <= 0)
        _sze = 1;

    _stk = new Enum* [_sze]; 
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
EnumStk::~EnumStk()
{
    delete [] _stk;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Empty out the stack, if del then delete the Enum* being stored.
void EnumStk::empty(int del)
{
    if (del)
      for (int j=0; j < _num; j++)
        delete _stk[j];

    _num = 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int  EnumStk::push(Enum *attr)
{
    if (_num >= _sze) {

        int len = _sze;
        _sze += _sze;
        Enum **tmp_stk = _stk;
        _stk = new Enum* [_sze];
        memcpy(_stk, tmp_stk, len * sizeof(Enum*));
        delete [] tmp_stk;
    }

    _stk[_num++] = attr;

    return(_num-1);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
Enum *EnumStk::pop()
{
    if (_num > 0) {
        return _stk[--_num];
    } else {
        return NULL;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Find an enum with this name;
// return it's position, or -1 if not found.
int EnumStk::find(char *name)
{
    for (int j=0; j < _num; j++) {
        if (_stk[j] && _stk[j]->name() 
                    && (strcmp(name,_stk[j]->name()) == 0))
            return j;
    }

    return -1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Return a pointer to an Enum of this type.
Enum *EnumStk::findtype(int type)
{
    for (int j=0; j < _num; j++) {
        if (_stk[j] && (_stk[j]->type() == type))
            return _stk[j];
    }

    return NULL;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
