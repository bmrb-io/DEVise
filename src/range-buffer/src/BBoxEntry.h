#ifndef __BBOXENTRY_H__
#define __BBOXENTRY_H__

#include "BoundingBox.h"
#include "PageEntry.h"

class BBoxEntry
{

public:

    BBoxEntry(Coord low, bool openLow, Coord high, bool openHigh);

    /* physically delete all pages hanging of from this bbox entry */
    ~BBoxEntry();

    /* Find the next record hanging of the bbox */
    void *nextRec();

    /* next BBoxEntry in the doubly linked list */
    BBoxEntry *getNext();

    /* previous BBoxEntry in the doubly linked list */
    BBoxEntry *getPrev();

    /* print out info about this instance */
    /* be concise if verbose == 0 */
    /* be verbose if verbose != 0 */
    void output(int verbose);

    friend class RangeBufferManager;
    friend class RangeBufferManagerAccessMethod;
    friend class ObjectDescriptor;

private:

    BoundingBox *_bbox;
    PageEntry *_datapages;
    int _numPages;
    bool _complete;
    PageEntry *_currentPage;	/* where to start the search for the next rec */
    int _numRecProcessed;	/* # of recs on _currentPage that have been */
				/* processed. Processed means have been     */
				/* checked and, eithr returned or discarded */

    bool _pageRequestDenied;  	/* Set to true when asking for more pages   */
				/* to insert new record but denied.         */

    BBoxEntry *_next;
    BBoxEntry *_prev;
};

#endif
