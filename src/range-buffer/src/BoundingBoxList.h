#ifndef __BOUNDINGBOXLIST_H__
#define __BOUNDINGBOXLIST_H__

#include "macros.h"
#include "BoundingBox.h"

class BoundingBoxEntry
{

public:

    BoundingBoxEntry(Coord low, bool openLow, Coord high, bool openHigh);
    ~BoundingBoxEntry();

    void output(int verbose);

    friend class BoundingBoxList;
    friend class RangeBufferManagerAccessMethod;

private:

    BoundingBox *_bbox;
    BoundingBoxEntry *_prev;
    BoundingBoxEntry *_next;
};

class BoundingBoxList
{

public:

    BoundingBoxList();
    ~BoundingBoxList();

    BoundingBoxList *clone();

    /* return true iff any of the entries in this list contains the point */
    bool contain(Coord point);

    /* Insert a bounding box into the list */
    /* Merge with any existing entries that it overlas or is adjacent to */
    void insert(BoundingBox *toBeInserted);

    void deleteBoundingBoxEntry(BoundingBoxEntry *toBeDeleted);

    void output(int verbose);

    friend class RangeBufferManagerAccessMethod;

private:

    BoundingBoxEntry *_head; 	/* head of doubly linked list of bounding */
				/* boxes. */
    BoundingBoxEntry *_tail; 	/* tail of doubly linked list of bounding */
				/* boxes. */
			   	/* All bounding boxes are pair-wise disjoint */
				/* bounding boxes sorted in ascending order */

};

#endif
