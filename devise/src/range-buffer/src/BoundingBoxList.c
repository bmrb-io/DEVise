#include <stdio.h>
#include <assert.h>

#include "BoundingBoxList.h"

BoundingBoxEntry::BoundingBoxEntry(Coord low, bool openLow,
				   Coord high, bool openHigh)
{
    _bbox = new BoundingBox(low, openLow, high, openHigh);
    _prev = NULL;
    _next = NULL;
}

void BoundingBoxEntry::output(int verbose)
{
    _bbox->output(verbose);
}

BoundingBoxEntry::~BoundingBoxEntry()
{
    delete _bbox;
}

BoundingBoxList::BoundingBoxList()
{
    _head = NULL;
    _tail = NULL;
}

BoundingBoxList::~BoundingBoxList()
{
    BoundingBoxEntry *bbe1, *bbe2;

    bbe1 = _head;
    while (bbe1)
    {
	bbe2 = bbe1;
	bbe1 = bbe1->_next;
	delete bbe2;
    }
}

BoundingBoxList *BoundingBoxList::clone()
{
    BoundingBoxList *bbl;
    BoundingBoxEntry *bbe_old;
    BoundingBox *bb_new;

    bbl = new BoundingBoxList;

    bbe_old = _head;
    while (bbe_old)
    {
	bb_new = new BoundingBox(bbe_old->_bbox->_low,
				 bbe_old->_bbox->_openLow,
				 bbe_old->_bbox->_high,
				 bbe_old->_bbox->_openHigh);
  	bbl->insert(bb_new);
	delete bb_new;

	bbe_old = bbe_old->_next;
    }

    return bbl;
}

bool BoundingBoxList::contain(Coord point)
{
    BoundingBoxEntry *bbe = _head;

    while (bbe)
    {
	if (bbe->_bbox->contain(point))
	    return true;
	bbe = bbe->_next;
    }

    return false;
}

void BoundingBoxList::insert(BoundingBox *toBeInserted)
{
    BoundingBoxEntry *leftnei, *leftov, *rightov, *rightnei,
		     *entry, *entry2, *newentry;

    /* first create the BoundingBoxEntry for toBeInserted */

    newentry = new BoundingBoxEntry(toBeInserted->_low,
                                    toBeInserted->_openLow,
                                    toBeInserted->_high,
                                    toBeInserted->_openHigh);

    /* then insert the new BoundingBoxEntry into the list */

    leftnei = NULL;
    leftov = NULL;
    rightov = NULL;
    rightnei = NULL;

    entry = _head;
    while (entry)
    {
	if (entry->_bbox->overlapOrAdjacent(toBeInserted))
	{
	    leftov = entry;
	    leftnei = entry->_prev;
	    break;
	}
	
 	if (toBeInserted->lessthan(entry->_bbox))
	{
	    leftnei = entry->_prev;
	    leftov = NULL;
	    break;
	}

	if (entry == _tail)
	{
	    assert(entry->_bbox->lessthan(toBeInserted));
	    leftov = NULL;
	    leftnei = entry;
	    break;
	}
    	entry = entry->_next;
    }

    entry = _tail;
    while (entry)
    {
	if (entry->_bbox->overlapOrAdjacent(toBeInserted))
        {
            rightov = entry;
            rightnei = entry->_next;
	    break;
	}

     	if (entry->_bbox->lessthan(toBeInserted))
 	{
	    rightnei = entry->_next;
	    rightov = NULL;
	    break;
	}

	if (entry == _head)
	{
	    assert(toBeInserted->lessthan(entry->_bbox));
	    rightov = NULL;
	    rightnei = entry;
	    break;
	}
    	entry = entry->_prev;
    }

    if (leftov)
    {
	/* toBeInserted overlaps/is adjacent to some of the existing */
	/* boundingboxes */
	assert(rightov);

	/* make newentry the merge of toBeInserted and all existing */
	/* boundingboxes that it overlaps/is adjacent to */
	if (leftov->_bbox->_low < newentry->_bbox->_low)
	{
	    newentry->_bbox->_low = leftov->_bbox->_low;
	    newentry->_bbox->_openLow = leftov->_bbox->_openLow;
	}
	else if (leftov->_bbox->_low == newentry->_bbox->_low &&
		 (leftov->_bbox->_openLow == false))
	{
	    newentry->_bbox->_openLow = false;
	}

   	if (rightov->_bbox->_high > newentry->_bbox->_high)
	{
	    newentry->_bbox->_high = rightov->_bbox->_high;
	    newentry->_bbox->_openHigh = rightov->_bbox->_openHigh;
	}
	else if (rightov->_bbox->_high == newentry->_bbox->_high &&
		 (rightov->_bbox->_openHigh == false))
	{
	    newentry->_bbox->_openHigh = false;
	}

 	/* replace the segment from leftov to rightov with newentry */
	newentry->_prev = leftov->_prev;
	newentry->_next = rightov->_next;
	if (newentry->_prev)
	    newentry->_prev->_next = newentry;
	else
	    _head = newentry;
	if (newentry->_next)
	    newentry->_next->_prev = newentry;
	else
	    _tail = newentry;

	leftov->_prev = NULL;
	rightov->_next = NULL;

 	/* delete the segment from leftov to rightov */
	entry = leftov;
	while (entry)
	{
	    entry2 = entry;
	    entry = entry->_next;
	    delete entry2;
	}
    }

    /* now we know there is no overlap or Adjacency */
    if (leftnei)
    {
	newentry->_prev = leftnei;
	leftnei->_next = newentry;
    }
    else
    {
	_head = newentry;
	newentry->_prev = NULL;
    }

    if (rightnei)
    {
	newentry->_next = rightnei;
	rightnei->_prev = newentry;
    }
    else
    {
	_tail = newentry;
	newentry->_next = NULL;
    }
}

void BoundingBoxList::deleteBoundingBoxEntry(BoundingBoxEntry *toBeDeleted)
{
    if (toBeDeleted->_prev)
    {
	toBeDeleted->_prev->_next = toBeDeleted->_next;
    }
    else
    {
	_head = toBeDeleted->_next;
    }

    if (toBeDeleted->_next)
    {
	toBeDeleted->_next->_prev = toBeDeleted->_prev;
    }
    else
    {
	_tail = toBeDeleted->_prev;
    }

    delete toBeDeleted;
}

void BoundingBoxList::output(int verbose)
{
    BoundingBoxEntry *bbe;
    int i;

    printf("\n************** BoundBoxList: *****************\n");

    if (_head == NULL)
    {
	printf("BoundBoxList is empty.\n");
	return;
    }

    for (bbe = _head, i = 1; bbe != NULL; bbe = bbe->_next, i++)
    {
	printf("BoundingBoxEntry %d:\n", i);
	bbe->output(verbose);
    }

    printf("************** END OF BoundBoxList *****************\n\n");
}
