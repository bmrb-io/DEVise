/* --------------------------------------------------------------- */
/* -- Copyright (c) 1994, 1995 Computer Sciences Department,    -- */
/* -- University of Wisconsin-Madison, subject to the terms     -- */
/* -- and conditions given in the file COPYRIGHT.  All Rights   -- */
/* -- Reserved.                                                 -- */
/* --------------------------------------------------------------- */

/*
  $Id$

  $Log$
 */

#ifndef MYSTACK_T2_H
#define MYSTACK_T2_H

template <class T, int max>
class mystack_t {

private:
    T _stk[max];
    int _top;

public:
    mystack_t()	{ _top = 0; }
    ~mystack_t()	{ }

    int size() {
        return _top;
    }

    mystack_t& push(T& t)  {
        if (!is_full())
	  _stk[_top++] = t;
	return *this;
    }
    T& pop() {
        if (!is_empty())
	  return _stk[--_top];
    }
    T& top() {
        if (!is_empty())
	  return _stk[_top - 1];
    }
    bool is_full() 	{ return _top >= max; }
    bool is_empty()	{ return _top == 0; }
};

#endif 	// MYSTACK_T_H

