#ifndef __BOUNDINGBOX_H__
#define __BOUNDINGBOX_H__

#include "macros.h"

/* currently 1-D bounding box only */
class BoundingBox
{

public:

    BoundingBox(Coord low, bool openLow, Coord high, bool openHigh);
    ~BoundingBox();

    /* return true if this boundbox contains the point */
    bool contain(Coord point);

    /* return true iff this BoundingBox is a super set of b */
    bool superSet(BoundingBox *b);

    /* return true iff this overlaps (has at least one point in common) bbox */
    bool overlap(BoundingBox *bbox);

    /* Return true iff this overlaps bbox or this is adjacent to bbox */
    /* Two bboxes are adjacent iff they don't overlap with each other */
    /* but have no gap in between */
    bool overlapOrAdjacent(BoundingBox *bbox);

    /* Return true iff this is strictly on the left of bbox */
    /* If lessthan() is true, both overlap() and overlapOrAdjacent() */
    /* are false! */
    bool lessthan(BoundingBox *bbox);

    /* check the validity of the bounding box */
    /* Valid iff _low < _high or */
    /*_low == _high && _openLow == _openHigh == false */
    bool valid();

    BoundingBox *clone();

    void output(int verbose);

    friend class RangeBufferManagerAccessMethod;
    friend class BoundingBoxList;
    friend class ObjectDescriptor;

private:

    Coord _low;
    Coord _high;
    bool _openLow;		/* true if low end is open */
    bool _openHigh;		/* true if high end is open */

};

#endif
