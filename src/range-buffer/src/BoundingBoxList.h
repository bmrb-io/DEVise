#ifndef __BOUNDINGBOXLIST_H__
#define __BOUNDINGBOXLIST_H__

#include "macros.h"
#include "BoundingBox.h"

/************************************************************************/
/* An element in the BoundingBoxList 					*/
/*									*/
/* Note: This is different from BBoxEntry! BBoxEntry is an internal 	*/
/* data structure of RangeBufferManager. BBoxEntry hangs off		*/
/* ObjectDescriptor. BoundingBoxEntry is elements in BoundingBoxList,	*/
/* which is used to specify the *not-in* list of queries.		*/
/************************************************************************/
class BoundingBoxEntry
{

public:

    /* initialize with specified low and high ends */
    BoundingBoxEntry(Coord low, bool openLow, Coord high, bool openHigh);

    /* destructor */
    ~BoundingBoxEntry();

    /* print out info about this instance */
    /* be concise if verbose == 0 */
    /* be verbose if verbose != 0 */
    void output(int verbose);

    friend class BoundingBoxList;
    friend class RangeBufferManagerAccessMethod;

private:

    BoundingBox *_bbox;
    BoundingBoxEntry *_prev;
    BoundingBoxEntry *_next;
};

/************************************************************************/
/* Doubly linked list of bounding boxes 				*/
/* All bounding boxes in the list pairwise have no overlap with each 	*/
/* other, and bounding boxes in the list are sorted from low to high.	*/
/*									*/
/* The main place in the range-buffer system where BoundingBoxList is	*/
/* is used is the "not-in" list when specifying queries.		*/
/************************************************************************/
class BoundingBoxList
{

public:

    /* Construct a BoundingBoxList with no element */
    BoundingBoxList();

    /* Destructor. Delete all elements. */
    ~BoundingBoxList();

    /* create a new BoundingBoxList who's exactly the same as this one */
    BoundingBoxList *clone();

    /* return true iff one entry in this list contains the point */
    bool contain(Coord point);

    /* Insert a bounding box into the list */
    /* Merge with existing entries that toBeInserted overlaps with */
    /* or is adjacent to */
    void insert(BoundingBox *toBeInserted);

    /* delete an BoundingBoxEntry */
    void deleteBoundingBoxEntry(BoundingBoxEntry *toBeDeleted);

    /* print out info about this instance */
    /* be concise if verbose == 0 */
    /* be verbose if verbose != 0 */
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
