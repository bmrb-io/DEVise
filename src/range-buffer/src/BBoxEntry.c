#include <stdio.h>

#include "BBoxEntry.h"

BBoxEntry::BBoxEntry(Coord low, bool openLow, Coord high, bool openHigh)
{
    _bbox = new BoundingBox(low, openLow, high, openHigh);
    _datapages = NULL;
    _numPages = 0;
    _next = NULL;
    _prev = NULL;
    _complete = false;
}

BBoxEntry::~BBoxEntry()
{
    PageEntry *p, *q;

    p = _datapages;
    while (p != NULL)
    {
	q = p;
	p = p->_next;
	delete q;
    }

    delete _bbox;
}

void *BBoxEntry::nextRec()
{
    if (_currentPage == NULL)
	return NULL;

    if (_numRecProcessed < _currentPage->_page->getNumSlots())
    {
	/* Still records on _currentPage that have NOT been processed */
	Tuple *tup = _currentPage->_page->getTuple(_numRecProcessed);
	_numRecProcessed++;

	/* Don't check if the tuple qualifies */
	/* Leave this to RangeBufferManagerAccessMethod */
	return tup;
    }

    _currentPage = _currentPage->_next;
    _numRecProcessed = 0;

    while (_currentPage)
    {
	if (_numRecProcessed < _currentPage->_page->getNumSlots())
	    break;

	_currentPage = _currentPage->_next;
	_numRecProcessed = 0;
    }

    if (_currentPage == NULL)
    {
	/* Sorry, no more record associated with this bbox */
	return NULL;
    }

    /* Found a page who has records that have NOT been processed */
    Tuple *tup = _currentPage->_page->getTuple(_numRecProcessed);
    _numRecProcessed++;

    /* Don't check if the tuple qualifies */
    /* Leave this to RangeBufferManagerAccessMethod */
    return tup;
}

BBoxEntry *BBoxEntry::getNext()
{
    return _next;
}

BBoxEntry *BBoxEntry::getPrev()
{
    return _prev;
}

void BBoxEntry::output(int verbose)
{
    printf("**** BBoxEntry object ****\n"

    "The BoundingBox is:\n");
    _bbox->output(verbose);

    printf("%d pages are associated with this BBoxEntry\n", _numPages);

    if (verbose)
    {
	/* print out all record pages. to be implemented ...*/

	if (_prev)
 	{
	    printf("This is not the head of the BBoxEntry list\n");
	}
	else
 	{
	    printf("This is the head of the BBoxEntry list\n");
	}

    	if (_next)
        {
            printf("This is not the end of the BBoxEntry list\n");
        }
        else
        {
            printf("This is the end of the BBoxEntry list\n");
        }
    }
}
