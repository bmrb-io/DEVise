#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "RangeBufferManagerAccessMethod.h"
#include "RangeBufferManager.h"
#include "ReplacementPolicy.h"

extern RangeBufferManager *globalRBM;

RBMObjectInfo *findObjectInfo(RBMObject objectId);

RangeBufferManagerAccessMethod::
RangeBufferManagerAccessMethod(RBMObject obj,
			       RangeAccessMethod *ram)
{
    _obj = obj;
    _ram = ram;
    _in = NULL;
    _notIn = NULL;
    _scanOutstanding = false;
    _lowIn = NULL;
    _lowNotIn = NULL;
}

RangeBufferManagerAccessMethod::~RangeBufferManagerAccessMethod()
{
    if (_scanOutstanding)
    {
	printf("Scan outstanding! Please close scan before destroy RBM-AM.\n");
	exit(1);
    }

}

int RangeBufferManagerAccessMethod::openScan(BoundingBox *in,
					     BoundingBoxList *notIn)
{
    ObjectDescriptor *od;
    BBoxEntry *bbe;

    int resultStartCaching;
    int resultGenerateLowLevelBoundingBox;
    int resultOpenScan;
 
    int error_number;

    bool needto_undo_startCaching = false;
    bool needto_undo_generate_llbb = false;
    bool needto_undo_open_low_level_scan = false;

    if (_scanOutstanding)
    {
	/* another scan outstanding. cannot open scan. */
	printf("Cannot open scan since another on is outstanding.\n");
	error_number = SCAN_OUTSTANDING;
	goto exit;
    }

    if (globalRBM->isCaching(_obj) == NULL)
    {
	resultStartCaching = globalRBM->startCaching(_obj, _ram);
	if (resultStartCaching < 0)
	{
	    printf("Error in opening a scan AND start caching a RBMObject\n");
	    error_number = resultStartCaching;
	    goto exit;
	}
	else
	{
	    needto_undo_startCaching = true;
	}
    }

    /* at this point, we know the object is in RBM. Find its descriptor */
    od = globalRBM->isCaching(_obj);
    assert(od);

    /* check if object is pinned */
    if (od->_pinCount != 0)
    {
	printf("RBMObject %d has been pinned. Cannot open another scan!\n",
		_obj);
	error_number = OBJECT_ALREADY_PINNED;
	goto exit;
    }

    /* In general, _lowIn and _lowNotIn become not NULL in this call. */
    /* set _lowIn to NULL if and only if we actually don't have to go */
    /* to low level to get records */
    resultGenerateLowLevelBoundingBox = generateLowLevelBoundingBox(in, notIn);
    if (resultGenerateLowLevelBoundingBox < 0)
    {
	printf("Cannot generate low level bounding box\n");
	error_number = CANNOT_GENERATE_LLBB;
	goto exit;
    }
    else
    {
	needto_undo_generate_llbb = true;
    }

    /* _lowIn is NULL means we don't have to go to low level */
    if (_lowIn)
    {
    	resultOpenScan = _ram->openScan(_lowIn, _lowNotIn);
    	if (resultOpenScan < 0)
    	{
            fprintf(stderr, "RBM call low level RAM's openScan failed.\n");
            error_number = resultOpenScan;
	    goto exit;
    	}
    	else
    	{
	    needto_undo_open_low_level_scan = true;
     	}
    }

    /* set pin count to 1 */
    od->_pinCount++;

    /* everything is fine */
    _in = in->clone();
    _notIn = notIn->clone();
    _scanOutstanding = true;
    _inMemPhase = true;

    /* We start searching for records from the first bounding box of this obj */
    _nextToConsider = od->_list;

    /* We'll start from the first page with each bounding box */
    bbe = od->_list;
    while (bbe)
    {
	bbe->_currentPage = bbe->_datapages;
	bbe->_numRecProcessed = 0;
	bbe = bbe->_next;
    }

    return 0;

exit:

    if (needto_undo_open_low_level_scan)
	_ram->closeScan();

    if (needto_undo_generate_llbb)
    {
	if (_lowIn)
	{
	    delete _lowIn;
	    _lowIn = NULL;
	}
	if (_lowNotIn)
	{
	    delete _lowNotIn;
	    _lowNotIn = NULL;
	}
    }

    if (needto_undo_startCaching)
	globalRBM->stopCaching(_obj);

    return error_number;
}

int RangeBufferManagerAccessMethod::nextRec(void *&record)
{
    void *in_mem_record;
    void *low_level_record;
    int resultNextRec;
    int insert_result;

    /* first phase, deal with in Mem records first */
    if (_inMemPhase)
    {
	/* looking for record in Mem */
	in_mem_record = findNextInMemRec();
	if (in_mem_record == NULL)
	{
	    /* no more record in Mem, advance to the second phase */
	    _inMemPhase = false;
	    TELL_FACT("End of all memory residing records");
	
	    /* Create all BBoxEntrys for records that'll come from low level. */

	    /* Do nothing if _lowIn is NULL! */
	    /* This makes sure that NO cleanUpBBoxEntries is needed. */

	    createHoleBBoxes();
	}
	else
	{
	    /* Return this record */
	    record = in_mem_record;
	    return 0;
	}
    }

    /* second phase, deal with low level RAM's records */
    /* _lowIn is NULL if and only if we don't have to go to low level */
    if ((_inMemPhase == false) && (_lowIn))
    {
	resultNextRec = _ram->nextRec(low_level_record);
	if (resultNextRec == NO_MORE_RECORD)
	{
	    cleanUpBBoxEntries();
    	    TELL_FACT("End of records fetched from low level and end of all records.");
	    return NO_MORE_RECORD;
	}
	else
	{
	    /* pack the record and insert it */
	    insert_result = insertRecord(low_level_record);
	    if (insert_result < 0)
	    {
		printf("Cannot insert new record\n");
		return ERROR_AT_INSERT;
	    }

	    record = low_level_record;
	    return 0;   
	}
    }

    TELL_FACT("End of records fetched from low level and end of all records.");
    return NO_MORE_RECORD;
}

void RangeBufferManagerAccessMethod::closeScan()
{
    ObjectDescriptor *od;

    /* no more scan outstanding */
    _scanOutstanding = false;

    /* let lower level RAM close its scan */
    _ram->closeScan();

    /* unpin the object */
    od = globalRBM->isCaching(_obj);
    assert(od);
    if (od->_pinCount != 1)
    {
	printf("This and only this RBM-AM should be pinning the object.\n"
	       "PinCount should be 1. But now pinCount is %d\n", od->_pinCount);
	exit (1);
    }
    od->_pinCount--;

    if (_in)
    {
    	delete _in;
    	_in = NULL;
    }
    if (_notIn)
    {
    	delete _notIn;
    	_notIn = NULL;
    }

    if (_lowIn)
    {
   	delete _lowIn;
	_lowIn = NULL;
    }
    if (_lowNotIn)
    {
   	delete _lowNotIn;
	_lowNotIn = NULL;
    }
}

int RangeBufferManagerAccessMethod::generateLowLevelBoundingBox(
					BoundingBox *in,
					BoundingBoxList *notIn)
{
    ObjectDescriptor *od;
    BBoxEntry *bentry;
    BoundingBoxEntry *bentry_low, *bentry_high;

    /* first generate _lowNotIn. It is notIn, plus all the ranges currently */
    /* buffered who overlap in inserted into it. */ 

    _lowNotIn = notIn->clone();

    od = globalRBM->isCaching(_obj);
    assert(od);

    bentry = od->_list;
    while (bentry)
    {
	if (in->overlap(bentry->_bbox))
	    _lowNotIn->insert(bentry->_bbox);

	bentry = bentry->getNext();
    }

    /* next, generate _lowIn. Most of the time it is just in. But if */
    /* _lowNotIn covers the head and/or tail of it, the head/tail is */
    /* truncated from _lowIn, and _lowNotIn is adjusted accordingly */
    /* _lowIn is set to NULL if in is entirly covered by _lowNotIn */

    _lowIn = in->clone();
    
    bentry_low = _lowNotIn->_head;
    if (bentry_low && 
	(bentry_low->_bbox->_low < _lowIn->_low ||
	 (bentry_low->_bbox->_low == _lowIn->_low && _lowIn->_openLow) ||
	 (bentry_low->_bbox->_low == _lowIn->_low && (bentry_low->_bbox->_openLow == false))))
    {
	_lowIn->_low = bentry_low->_bbox->_high;
	if (bentry_low->_bbox->_openHigh == true)
	    _lowIn->_openLow = false;
	else
	    _lowIn->_openLow = true;
	_lowNotIn->deleteBoundingBoxEntry(_lowNotIn->_head);
    }

    bentry_high = _lowNotIn->_tail;
    if (bentry_high && 
        (bentry_high->_bbox->_high > _lowIn->_high ||
         (bentry_high->_bbox->_high == _lowIn->_high && _lowIn->_openHigh) ||
         (bentry_high->_bbox->_high == _lowIn->_high && (bentry_high->_bbox->_openHigh == false))))
    {
	_lowIn->_high = bentry_high->_bbox->_low;
	if (bentry_high->_bbox->_openLow == true)
	    _lowIn->_openHigh = false;
	else
	    _lowIn->_openHigh = true;
	_lowNotIn->deleteBoundingBoxEntry(_lowNotIn->_tail);
    }

    if (_lowIn->valid() == false)
    {
	/* we don't have to go to low level for records */
	delete _lowIn;
	_lowIn = NULL;

	delete _lowNotIn;
	_lowNotIn = NULL;
    }

    return 0;
    
}

void RangeBufferManagerAccessMethod::createHoleBBoxes()
{
    BoundingBoxEntry *bbe;

    BBoxEntry *new_rbm_bentry;

    Coord newentry_low, newentry_high;
    bool newentry_openLow, newentry_openHigh;

    ObjectDescriptor *od;

    /* Do nothing if _lowIn is NULL, which means we don't have to go to */
    /* the lower level, thus there is no need to create hole bboxes. */
    if (_lowIn == NULL)
	return;

    /* find the ObjectDescriptor in RBM for this obj */
    od = globalRBM->isCaching(_obj);
    assert(od);

    /* for each entry in _lowNotIn, create the hole-bbox in front of it */
    /* and insert into globalRBM's list for this object */

    bbe = _lowNotIn->_head;

    while (bbe)
    {
	if (bbe == _lowNotIn->_head)
	{
	    /* bbe is the first entry in list */
	    newentry_low = _lowIn->_low;
	    newentry_openLow = _lowIn->_openLow;
	}
	else
	{
	    /* bbe is not the first entry in list */
	    newentry_low = bbe->_prev->_bbox->_high;
	    if (bbe->_prev->_bbox->_openHigh == true)
		newentry_openLow = false;
	    else
		newentry_openLow = true;
	}

	newentry_high = bbe->_bbox->_low;
	if (bbe->_bbox->_openLow == true)
	    newentry_openHigh = false;
	else
	    newentry_openHigh = true;

 	/* new_rbm_bentry->_complete is set to false be constructor */
	new_rbm_bentry = new BBoxEntry(newentry_low,
				       newentry_openLow,
				       newentry_high,
				       newentry_openHigh);
	od->insert(new_rbm_bentry);
/*
	delete new_rbm_bentry;
*/

	bbe = bbe->_next;
    }

    /* create the hole bbox for the last hole (the one after the last entry */
    /* in _lowNotIn) */

    if (_lowNotIn->_tail)
    {
	    newentry_low = _lowNotIn->_tail->_bbox->_high;
	    if (_lowNotIn->_tail->_bbox->_openHigh == true)
		newentry_openLow = false;
	    else
		newentry_openLow = true;
    }
    else
    {
	newentry_low = _lowIn->_low;
	newentry_openLow = _lowIn->_openLow;
    }

    newentry_high = _lowIn->_high;
    newentry_openHigh = _lowIn->_openHigh;

    /* new_rbm_bentry->_complete is set to false be constructor */
    new_rbm_bentry = new BBoxEntry(newentry_low,
				   newentry_openLow,
				   newentry_high,
				   newentry_openHigh);
    od->insert(new_rbm_bentry);

/*
    delete new_rbm_bentry;
*/

}

void RangeBufferManagerAccessMethod::cleanUpBBoxEntries()
{
    ObjectDescriptor *od;
    BBoxEntry *bbe;

    /* find the ObjectDescriptor in RBM for this obj */
    od = globalRBM->isCaching(_obj);
    assert(od);

    for (bbe = od->_list; bbe != NULL; bbe = bbe->_next)
    {
	bbe->_complete = true;
    }
}

void RangeBufferManagerAccessMethod::output(int verbose)
{
    printf("************** RangeBufferManagerAccessMethod ****************\n");

    printf("_obj = %d\n", _obj);
    printf("_ram = %d\n", (int)_ram);

    if (_scanOutstanding)
    {
	printf("Scan Outstanding! \n");
	if (_inMemPhase)
	    printf("In Mem Phase\n");
	else
	    printf("NOT In Mem Phase\n");
    }
    else
    {
	printf("NO Scan Outstanding! \n");

  	/* _inMemPhase is irrelevent */
    }

    printf("The _in range:\n");
    _in->output(verbose);

    printf("The _notIn list:\n");
    _notIn->output(verbose);

    printf("The _lowIn range:\n");
    _lowIn->output(verbose);

    printf("The _lowNotIn list:\n");
    _lowNotIn->output(verbose);

    /* may need to print out more in the future */
}

void *RangeBufferManagerAccessMethod::findNextInMemRec()
{
    void *record;

    /* start from the current bbox */
    /* And go through all of the bboxes */
    /* until we find one record or until we know there's no more at all */
    while (_nextToConsider)
    {
        record = _nextToConsider->nextRec();
	while (record)
	{
	    /* check qualification of record based on _in & _notIn */
	    float8 value;
	    RBMObjectInfo *roi;
	    roi = findObjectInfo(_obj);
	    value = DteFloat8Adt::cast(((void**)record)[roi->boundingBoxAttrNumber]);
	    if (_in->contain(value) && (_notIn->contain(value) == false))
		return record;
	    else
		record = _nextToConsider->nextRec();
	}

	_nextToConsider = _nextToConsider->_next;

	/* If we are sure this bbe can't return any record give our */
        /* _in and _notIn, skip it */
	while (_nextToConsider)
	{
	    if (boundingBoxQulify(_nextToConsider))
		break;
	    _nextToConsider = _nextToConsider->_next;
	}
    }

    return NULL;
}

int RangeBufferManagerAccessMethod::insertRecord(void *lowLevelRec)
{
    Coord bbav;	/* Bounding box attribute value of lowLevelRec. */
		/* Assuming bounding box attributes are doubles for now. */

    RBMObjectInfo *objinfo;	/* RBMObjectInfo of this cached object */

    ObjectDescriptor *od;	/* pointing to the cached object */

    BBoxEntry *bbe;

    objinfo = findObjectInfo(_obj);

    /* Get the bounding box attribute value */
    /* Cast lowLevelRec to void** to get the pointer to the value */
    /* Use Kevin's standard "cast" method */
    bbav = DteFloat8Adt::cast(((void**)lowLevelRec)[objinfo->boundingBoxAttrNumber]);

    od = globalRBM->isCaching(_obj);
    assert(od);

    for (bbe = od->_list; bbe; bbe = bbe->_next)
    {
	if (bbe->_bbox->contain(bbav))
	{
	    /* insert lowLevelRec into this BBoxEntry */

	    PageEntry *pe;
    	    bool fitOnPage;

	    /* First try to insert into existing pages */

    	    for (pe = bbe->_datapages; pe; pe = pe->_next)
    	    {
        	/* Tuple is defined as void* in Kevin's DteAdtPage.h */
        	fitOnPage = pe->_page->addTuple((Tuple*)lowLevelRec);
        	if (fitOnPage)
        	{
            	    /* Successfully added into the page */
            	    return 0;
        	}
    	    }

    	    /* Tuple doesn't fit on any existing page. */
	    /* Must find a new page for it. */

    	    /* First try to create a new page for it */

	    char *newBuf;
	    newBuf = (char*)globalRBM->_mem->generateNewPage();

	    /* If not able to create page, try replace some victim object out */
	    if (newBuf == NULL)
	    {
		ObjectDescriptor *victim =
			globalRBM->_replacementPolicy->pickVictim();

		if (!victim)
		{
		    /* victim not found. set newBuf to NULL */
		    newBuf = NULL;
		}
		else
		{
		    /* Victim found. */
		    /* Kick victim out and try _mem again. */
		    /* Should find something if the victim had some pages */
		    globalRBM->kickOutVictim(victim);
		    newBuf = (char*)globalRBM->_mem->generateNewPage();
		}
   	    }

    	    if (!newBuf)
	    {
		/* No free page left & no page can be robbed from any victim */
		fprintf(stderr, "Cannot find space in RBM for new record.\n");
		return NO_PAGE_FOR_REC;
	    }
	    else
    	    {
        	/* A page worth of space was found. */
        	/* Create the new page and do the insertion. */

        	DteAdtPage *newPage = new DteAdtPage(*(objinfo->tupleAdt));
		newPage->initPage(newBuf);

		/* insert the tuple into newPage */
		fitOnPage = newPage->addTuple((Tuple*)lowLevelRec);

		/* We deal with records no bigger than a page only. */
		assert (fitOnPage);

		/* insert the new page into list of pages */

		/* the PageEntry need to remember the pointer to DteAdtPage */
 		/* as well as the pointer to the physical mem */
		PageEntry *newPageEntry = new PageEntry(newPage, newBuf);
	
		if (bbe->_datapages)
		{
		    newPageEntry->_prev = NULL;
		    newPageEntry->_next = bbe->_datapages;
		    bbe->_datapages->_prev = newPageEntry;
		    bbe->_datapages = newPageEntry;
		}
		else
		{
		    newPageEntry->_prev = NULL;
		    newPageEntry->_next = NULL;
		    bbe->_datapages = newPageEntry;
		}

		bbe->_numPages++;

		return 0;
	    }
	}
    }

    /* Not able to find the right BBoxEntry. */
    /* This is an error since we should have created BBoxEntry for all new */
    /* records in the createHoleBBoxes() call */
    fprintf(stderr, "BBoxEntry for new record doesn't exist!\n");
    return BBOXENTRY_NOT_EXIST;
}

bool RangeBufferManagerAccessMethod::boundingBoxQulify(BBoxEntry *bbe)
{
    BoundingBoxEntry *entry;

    /* bbe can not possibly contain any useful record for this query */
    /* since it doesn't even overlap the in bbox		     */
    if (_in->overlap(bbe->_bbox) == false)
	return false;

    /* Go through all the elements in _notIn. */
    /* If any element is a superset of bbe, then bbe can not possibly contain */
    /* any useful record for this query */
    entry = _notIn->_head;
    while (entry)
    {
	if (entry->_bbox->superSet(bbe->_bbox))
	    return false;
 	entry = entry->_next;
    }

    return true;
}
