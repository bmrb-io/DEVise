#ifndef __OBJECTDESCRIPTOR_H__
#define __OBJECTDESCRIPTOR_H__

#include "RBMObject.h"
#include "RangeAccessMethod.h"
#include "BBoxEntry.h"

/* ObjectDescriptors are used in RangeBufferManager 			      */
/* There is one ObjectDescriptor for each object cached 		      */
/* In addition to the object id, an ObjectDescriptor contains some other info */
/* These info include:	From which low level ram to get addition records      */
/*			# of Bounding boxes hanging off this ObjectDescriptor */
/*			pointer to head of the Bounding box list	      */
/*			pin count					      */
/* ObjectDescriptors are kept in (the overflow chains of) hashed buckets      */

class ObjectDescriptor
{
public:

    ObjectDescriptor(RBMObject newObj, RangeAccessMethod *ramOfNewObj);

    /* delete all bbox entries hanging off this object descriptor */
    ~ObjectDescriptor();

    /* Insert an BBoxEntry into _list */
    /* ToBeInserted should not overlap any of the existing entry */
    /* Insertion keeps the order among entries */
    void insert (BBoxEntry *toBeInserted);

    /* print out info about this instance */
    /* be concise if verbose == 0 */
    /* be verbose if verbose != 0 */
    void output(int verbose);

    friend class RangeBufferManager;
    friend class RangeBufferManagerAccessMethod;
    friend class ReplacementPolicyRandom;

private:

    RBMObject _obj;			/* object id 			      */
    RangeAccessMethod *_ram;		/* pointer to RAM 		      */
    int _numBBoxes;			/* number of bbox entries hanging off */
    BBoxEntry *_list;			/* list of BBox entries 	      */
    int _pinCount;			/* pin count 			      */

    ObjectDescriptor *_next;		/* for doubly linked list 	      */
    ObjectDescriptor *_prev;		/* for doubly linked list  	      */
};

#endif
