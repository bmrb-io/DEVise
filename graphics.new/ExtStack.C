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

#include <iostream.h>
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
	db = new (void*)[unitSz](NULL);
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
