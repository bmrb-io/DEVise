/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.4  2005/12/06 20:04:00  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.3.18.1  2005/09/06 21:20:17  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.3  1999/01/29 15:18:15  wenger
  Fixed egcs 1.1.1 fixes -- taking out array constructor arguments caused
  problems in some places.

  Revision 1.2  1999/01/18 18:24:04  beyer
  fixed compile error for egcs v 1.1.1

  Revision 1.1  1998/03/03 05:21:04  taodb
  Added extendable stack to command object

*/

#include <iostream>
#include <assert.h>
#include "ExtStack.h"

ostream& operator <<(ostream& out, ExtStack* st)
{
	int		i= 0;
	out <<"-----------------------\n";
	while (st!=NULL)
	{
		int	j;
		out <<"Stack segment["<<i<<"] has "<< st->emptySz<<" elements:"<<endl;
		for (j = 0; j<st->emptySz; ++j)
			out <<st->db[j]<<" ";
		out <<endl;
		i++;
		st = st->next;
	}
	out <<"-----------------------\n";
	return out;
}
void
ExtStack::setNext(ExtStack* next)
{
	this->next = next;
}

ExtStack::ExtStack(int unitSz, ExtStack* father)
{
	db = new void*[unitSz];
	for (int index = 0; index < unitSz; index++) {
		db[index] = NULL;
	}

	this->unitSz = unitSz;
	emptySz = 0;
	next = NULL;
	prev = father;

	// link himself to father's tail
	if (father != NULL)
		father->setNext(this);
}

void*
ExtStack::push(void* pt)
{
	if (emptySz < unitSz)
	{
		db[emptySz] = pt;
		emptySz ++;
	}
	else
	{
		if (next ==NULL)
			next = new ExtStack(unitSz, this);
		next->push(pt);
	}
	return pt;
}

void*
ExtStack::top()
{
	ExtStack	*pt;
	pt = this;
	if (next != NULL)
	{
		assert(emptySz == unitSz);
		return next->top();
	}
	else
	if (emptySz ==0)
		return NULL;
	else
		return db[emptySz -1];
}

void*
ExtStack::pop()
{
	ExtStack	*pt;
	void		*value;

	pt = this;
	if (next != NULL)
	{
		assert(emptySz == unitSz);
		return next->pop();
	}
	else
	if (emptySz ==0)
	{
		// nothing to pop out
		return NULL;
	}
	else
	{
		value =db[emptySz -1];
		emptySz --;
		if (emptySz ==0)
		{
			//delete self if not the first node
			if (prev != NULL)
			{
				prev->setNext(NULL);
				delete this;
			}
		}
	}
	return value;
}

ExtStack::~ExtStack()
{
	delete []db;
	if (next != NULL)
		delete next;
}
