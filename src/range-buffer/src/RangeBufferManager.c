#include <stdio.h>

#include "RangeBufferManager.h"
#include "ReplacementPolicy.h"
#include "ReplacementPolicyRandom.h"

RangeBufferManager::RangeBufferManager(ReplacementPolicyType rep,
				       unsigned int numPage)
{
    _numObjects = 0;

    for (int i = 0; i < NUM_RANGE_BUF_OBJ_HASH_ENTRY; i++)
    {
	_objectDescriptors[i] = NULL;
    }

    /* create the memory manager */
    _mem = new MemoryManager(numPage);

    /* create the replacement policy */
    switch (rep)
    {
	case REPLACE_RANDOM:
		_replacementPolicy = new ReplacementPolicyRandom(this);
		break;

	default:
		printf("Unsupported replace policy type!\n");
		exit(1);
    }
}

RangeBufferManager::~RangeBufferManager()
{
    ObjectDescriptor *od, *tmpod;

    delete _replacementPolicy;

    delete _mem;

    for (int i = 0; i < NUM_RANGE_BUF_OBJ_HASH_ENTRY; i++)
    {
	od = _objectDescriptors[i];
   	while (od != NULL)
	{
	    tmpod = od;
	    od = od->_next;
	    delete tmpod;
	}
    }
}

int RangeBufferManager::startCaching(RBMObject obj, RangeAccessMethod *ram)
{
    ObjectDescriptor *od;
    ObjectDescriptor *victim;
    int hash_entry;

    /* Object should not be in the RBM now */
    if (isCaching(obj))
    {
	printf("startCaching: Object already cached!\n");
	return ALREADY_CACHED;
    }

    /* creating a new object descriptor */
    od = new ObjectDescriptor(obj, ram);

    if (_numObjects == MAX_NUM_RBMOBJECTS_CACHED)
    {
	/* must choose a victim RBMObject */
	/* victim must not be pinned! */
	victim = _replacementPolicy->pickVictim();
	
	if (victim == NULL)
	{
	    /* cannot find victim */
	    printf("No victim available. Can not start caching new object.\n");
	    return NO_VICTIM_AVAILABLE;
	}

	/* kick the victim out */
	stopCaching(victim->_obj);
	_numObjects--;
    }

    /* insert the descriptor into the head of the list for this hash bucket */
    hash_entry = RBM_HASH(obj);
    od->_next = _objectDescriptors[hash_entry];
    od->_prev = NULL;
    if (_objectDescriptors[hash_entry] != NULL)
    {
	_objectDescriptors[hash_entry]->_prev = od;
    }
    _objectDescriptors[hash_entry] = od;

    /* one more object cached */
    _numObjects++;

    return 0;
}

ObjectDescriptor* RangeBufferManager::isCaching(RBMObject obj)
{
    ObjectDescriptor *ret;
    int hash_entry;

    hash_entry = RBM_HASH(obj);
    ret = _objectDescriptors[hash_entry];

    while (ret != NULL)
    {
	/* object found */
	if (ret->_obj == obj)
	    return ret;

	ret = ret->_next;
    }

    /* object not found, ret is NULL */
    return ret;
}

void RangeBufferManager::kickOutVictim(ObjectDescriptor *victim)
{
    BBoxEntry *bbe;
    PageEntry *pe;

    /* no one should be using this object */
    assert (victim->_pinCount == 0);

    /* First recycle all pages used by this object */
    /* Has to do this explicitely since PageEntry doesn't worry about _buf */
    /* at all */
    /* whoever wants to delete PageEntry must worry about it! */
    bbe = victim->_list;
    while (bbe)
    {
	pe = bbe->_datapages;
	while (pe)
	{
	    _mem->recyclePage(pe->_buf);
	
	    pe = pe->_next;
	}

	bbe = bbe->_next;
    }

    /* Now take the victim ObjectDescriptor out of the list */
    if (victim->_next)
	victim->_next->_prev = victim->_prev;

    if (victim->_prev)
	victim->_prev->_next = victim->_next;
    else
	_objectDescriptors[RBM_HASH(victim->_obj)] = victim->_next;

    /* finally destroy victim itself */
    delete victim;

    /* one less object cached */
    _numObjects--;
}

int RangeBufferManager::stopCaching(RBMObject obj)         /* IN: Object */
{
    ObjectDescriptor *cached;

    cached = isCaching(obj);
    if (cached == NULL)
    {
	return NOT_CACHED;
    }

    if (cached->_pinCount > 0)
    {
	printf("Object pinned. Cannot stop caching it now.\n");
	return OBJECT_PINNED;
    }

    /* object can be removed from range buffer */

    /* yank the object out of the double linked list */
    if (cached->_next != NULL)
    {
	cached->_next->_prev = cached->_prev;
    }

    if (cached == _objectDescriptors[RBM_HASH(obj)])
    {
	_objectDescriptors[RBM_HASH(obj)] = cached->_next;
    }
    else
    {
	cached->_prev->_next = cached->_next;
    }

    delete cached;
    _numObjects--;

    return 0;
}

void RangeBufferManager::output(int verbose)
{
    ObjectDescriptor *od;
    int i;

    printf("***** RangeBufferManager Content *****\n\n");

    printf("There are %d objects cached.\n", _numObjects);
    
    for (i = 0; i < NUM_RANGE_BUF_OBJ_HASH_ENTRY; i++)
    {
	od = _objectDescriptors[i];
	while (od)
	{
	    od->output(verbose);
	    od = od->_next;
	}
    }
}
