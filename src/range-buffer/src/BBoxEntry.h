#ifndef __BBOXENTRY_H__
#define __BBOXENTRY_H__

#include "BoundingBox.h"
#include "PageEntry.h"

/************************************************************************/
/* Part of the internal data structure of RangeBufferManager.		*/
/*									*/
/* Each ObjectDescriptor has a list of BBoxEntry hang off it.		*/
/* Each BBoxEntry keeps a track of records within a bounding box.	*/
/* Each BBoxEntry has 0 or more pages associated with it.		*/
/************************************************************************/

class BBoxEntry
{

public:

    /* Constructor. Initialize with the specified low and high end. */
    BBoxEntry(Coord low, bool openLow, Coord high, bool openHigh);

    /* Destructor. */
    /* Physically delete all pages hanging of from this bbox entry */
    ~BBoxEntry();

    /* Find the next record hanging of the bbox */
    /* Used to iterate through all records in this bounding box and */
    /* decide if should be returned as query result.		    */
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
    friend class ReplacementPolicyRandom;

private:

    BoundingBox *_bbox;		/* The bounding box 			      */
    PageEntry *_datapages;	/* The list of data pages 		      */
    int _numPages;		/* Number of pages 			      */
    bool _complete;		/* If this bounding box entry is complete     */
				/* Used internally only. 		      */
				/* Used to keep track of whether we have      */
				/* finished the job of getting records from   */
				/* low level.				      */
    PageEntry *_currentPage;	/* where to start the search for the next rec */
    int _numRecProcessed;	/* # of recs on _currentPage that have been   */
				/* processed. Processed means have been       */
				/* checked and, eithr returned or discarded   */

    bool _pageRequestDenied;  	/* Set to true when asking for more pages     */
				/* to insert new record but denied.           */

    BBoxEntry *_next;		/* For linked list                            */
    BBoxEntry *_prev;		/* For linked list			      */
};

#endif
