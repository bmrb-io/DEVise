/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of Client (and CompDate) classes.
 */

/*
  $Id$

  $Log$
*/
#ifndef _EXT_STACK_H
#define _EXT_STACK_H
class ExtStack
{
	friend ostream& operator <<(ostream& out, ExtStack* st);
	public:
		ExtStack(int unitSz, ExtStack*);
		~ExtStack();
		void* push(void*);
		void* pop();
		void* top();
		void setNext(ExtStack* next);
		int	emptySz;
		int	unitSz;
		void** db;
		ExtStack*	next;
		ExtStack*	prev;
};
ostream& operator <<(ostream& out, ExtStack* st);
#endif
