/* --------------------------------------------------------------- */
/* -- Copyright (c) 1994, 1995 Computer Sciences Department,    -- */
/* -- University of Wisconsin-Madison, subject to the terms     -- */
/* -- and conditions given in the file COPYRIGHT.  All Rights   -- */
/* -- Reserved.                                                 -- */
/* --------------------------------------------------------------- */

/*
 *  /p/shore/shore_cvs/src/common/mystack_t.h,v 1.6 1995/04/24 19:28:46 zwilling Exp
 */
#ifndef PTRSTACK_T2_H
#define PTRSTACK_T2_H

template <class T, int max>
class ptrstack_t {

private:
    T *_stk[max];
    int _top;

public:
    ptrstack_t()	{ _top = 0; }
    ~ptrstack_t()	{ for (int i=0; i<max; i++) delete _stk[i];}

    void set_ptr(int index, T *ptr_val) {_stk[index]=ptr_val;}

    int size() {
        return _top;
    }

    ptrstack_t& push(T& t)  {
        if (!is_full())
	  (*(_stk[_top++])) = t;
	return *this;
    }
    T& pop() {
        if (!is_empty())
	  return (*(_stk[--_top]));
    }
    T& top() {
        if (!is_empty())
	  return (*(_stk[_top - 1]));
    }
    bool is_full() 	{ return _top >= max; }
    bool is_empty()	{ return _top == 0; }
};

#endif 	// PTRSTACK_T_H

