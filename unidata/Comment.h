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

#ifndef    _COMMENT_H
#define    _COMMENT_H

#include  <iostream.h>

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class Comment {

  public:
    char    *init;        // The start of a comment.
    char    *end;         // The end of a comment.

    Comment() {
        init = NULL;
        end  = NULL;
    }

   ~Comment() {
        delete [] init;
        delete [] end;
    }

      // Stream printing (for debugging)
    friend ostream& operator<< (ostream& out, const Comment& cmmt); 
};
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#define    DEFAULT_HINT    2

class CmmtStk {

  private:
    int    _num;        // Number of Comments in the stack.  
    int    _sze;        // Size of the stack.

    Comment  **_stk;    // Doubling array of Comment*

  public:
    CmmtStk(int hint=DEFAULT_HINT);
   ~CmmtStk();

    int      push(Comment *cmmt);
    Comment *pop();

    int   nCmmts() { return _num; }

      // Return pointer to ith Comment
    Comment *ith(int i) {
        return (i < _num) ? (_stk[i]) : (Comment*) NULL;
    }

      // Empty out the stack,
      // if del then delete the Comment* being stored.
    void empty(int del=0);
};

#undef    DEFAULT_HINT

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _COMMENT_H
