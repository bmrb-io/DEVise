// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Comments
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#include  "Comment.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (for debugging)
ostream& operator<< (ostream& out, const Comment& cmmt)
{
    out << "Comment: '" << cmmt.init << "' .. '" << cmmt.end << "'\n";
    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
CmmtStk::CmmtStk(int hint)
{
    _num = 0;
    _sze = hint;

    if (_sze <= 0)
        _sze = 1;

    _stk = new Comment* [_sze]; 
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
CmmtStk::~CmmtStk()
{
    for (int j=0; j < _num; j++)
        delete _stk[j];

    delete [] _stk;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int CmmtStk::push(Comment *cmmt)
{
    if (_num >= _sze) {

        int len = _sze;
        _sze += _sze;
        Comment **tmp_stk = _stk;
        _stk = new Comment* [_sze];
        memcpy(_stk, tmp_stk, len * sizeof(Comment*));
        delete [] tmp_stk;
    }

    _stk[_num++] = cmmt;

    return(_num-1);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
Comment *CmmtStk::pop()
{
    if (_num > 0) {
        return _stk[--_num];
    } else {
        return NULL;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Empty out the stack,
// if del then delete the Comment* being stored.
void CmmtStk::empty(int del)
{
    if (del)
      for (int j=0; j < _num; j++)
        delete _stk[j];

    _num = 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
