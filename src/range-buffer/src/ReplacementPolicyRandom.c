#include <stdio.h>
#include <assert.h>

#include "ReplacementPolicyRandom.h"
#include "RangeBufferManager.h"

ReplacementPolicyRandom::ReplacementPolicyRandom(RangeBufferManager *rbm)
{
    _rbm = rbm;
}

ReplacementPolicyRandom::~ReplacementPolicyRandom()
{
}

ObjectDescriptor *ReplacementPolicyRandom::pickVictim()
{
    ObjectDescriptor *ret = NULL;
    int i;

    for (i = 0; i < NUM_RANGE_BUF_OBJ_HASH_ENTRY; i++)
    {
	ret = _rbm->_objectDescriptors[i];
 
	while (ret != NULL)
	{
	    if (ret->_pinCount == 0)
	    {
		/* victim found */
		printf("------------------------------\n");
		printf("Object %d replaced as a victim.\n", ret->_obj);
		printf("------------------------------\n");
		return ret;
	    }
	    
	    ret = ret->_next;
	}
    }

    assert (ret == NULL);
    return ret;
}

BBoxEntry *ReplacementPolicyRandom::pickVictimBBox(ObjectDescriptor *&objd)
{
    ObjectDescriptor *od = NULL;
    int i;

    for (i = 0; i < NUM_RANGE_BUF_OBJ_HASH_ENTRY; i++)
    {
	od = _rbm->_objectDescriptors[i];
 
	while (od != NULL)
	{
	    if (od->_pinCount == 0)
	    {
		if (od->_list && od->_list->_numPages > 0)
		{
		    /* victim found */
	
		    char l[2];
		    char r[2];

		    if (od->_list->_bbox->_openLow)
			strcpy(l, "(");
		    else
			strcpy(l, "[");

		    if (od->_list->_bbox->_openHigh)
			strcpy(r, ")");
		    else
			strcpy(r, "]");

		    Coord lv = od->_list->_bbox->_low;
		    Coord rv = od->_list->_bbox->_high;
		
		    printf("-----------------------------------------------\n");
		    printf("Bounding Box %s%f, %f%s of Object %d"
			   " replaced as a victim.\n",
			   l, lv, rv, r, od->_obj);
		    printf("-----------------------------------------------\n");
		    objd = od;
		    return od->_list;
		}
	    }
	    
	    od = od->_next;
	}
    }

    assert (od == NULL);
    return NULL;
}

void ReplacementPolicyRandom::output(int verbose)
{
    printf("\n****** ReplacementPolicyRandom ********\n\n");
    printf("Pick up victim arbitrarily \n\n");
}
