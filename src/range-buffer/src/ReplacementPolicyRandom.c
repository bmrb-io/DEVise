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
		printf("Object %d replaced as a victim.\n", ret->_obj);
		return ret;
	    }
	    
	    ret = ret->_next;
	}
    }

    assert (ret == NULL);
    return ret;
}

void ReplacementPolicyRandom::output(int verbose)
{
    printf("\n****** ReplacementPolicyRandom ********\n\n");
    printf("Pick up victim arbitrarily \n\n");
}
