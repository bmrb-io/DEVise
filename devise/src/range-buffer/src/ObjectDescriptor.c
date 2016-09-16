#include <stdio.h>

#include "ObjectDescriptor.h"

ObjectDescriptor::ObjectDescriptor(RBMObject newObj,
				   RangeAccessMethod *ramOfNewObj)
{
    _obj = newObj;
    _ram = ramOfNewObj;
    _list = NULL;
    _pinCount = 0;
    _numBBoxes = 0;

    _next = NULL;
    _prev = NULL;
}

ObjectDescriptor::~ObjectDescriptor()
{
    BBoxEntry *p, *q;

    p = _list;
    while (p != NULL)
    {
	q = p;
	p = p->getNext();
 	delete q;
    }
}

void ObjectDescriptor::insert (BBoxEntry *toBeInserted)
{
    BBoxEntry *bbe, *bbe2 = NULL;

    for (bbe = _list; bbe != NULL; bbe = bbe->_next)
    {
	bbe2 = bbe;
	if (bbe->_bbox->_low >= toBeInserted->_bbox->_high)
	    break;
    }

    if (bbe)
    {
	/* insert toBeInserted to the left of bbe */
	toBeInserted->_next = bbe;
	toBeInserted->_prev = bbe->_prev;
	if (bbe->_prev)
	    bbe->_prev->_next = toBeInserted;
	else
	    _list = toBeInserted;
	bbe->_prev = toBeInserted;
    }
    else if (bbe2)
    {
	/* insert toBeInserted as the last entry */
	toBeInserted->_prev = bbe2;
	toBeInserted->_next = NULL;
	bbe2->_next = toBeInserted;
    }
    else /* bbe == bbe2 == NULL */
    {
	/* insert toBeInserted as the only entry */
	toBeInserted->_prev = NULL;
	toBeInserted->_next = NULL;
	_list = toBeInserted;
    }

    _numBBoxes++;
}

void ObjectDescriptor::output(int verbose)
{
    BBoxEntry *p;
    int i;

    printf("**** ObjectDescriptor ****\n"

    "The object is: %d\n", _obj);

    printf("The range access method is: \n");
    _ram->output(verbose);

    printf("The pin count is: %d\n", _pinCount);

    printf("There are %d BBox entries hanging off here.\n", _numBBoxes);
    p = _list;
    i = 0;
    while (p != NULL)
    {
	printf("BBox entry %d:\n", i);
	p->output(verbose);
	p = p->_next;
    }
}
