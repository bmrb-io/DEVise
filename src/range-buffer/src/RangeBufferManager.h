#ifndef __RANGEBUFFERMANAGER_H__
#define __RANGEBUFFERMANAGER_H__

#include "macros.h"
#include "RBMObject.h"
#include "RangeAccessMethod.h"
#include "ObjectDescriptor.h"
#include "RangeBufferCommon.h"
#include "MemoryManager.h"

class RangeBufferManager
{

public:

    /**************************************************************************/
    /* Constructor							      */
    /*									      */
    /* Initialize the RangeBufferManager with the specified replacement       */
    /* policy and the number of pages to be used.			      */
    /**************************************************************************/
    RangeBufferManager(ReplacementPolicyType rep,   /* IN: replacement policy */
		       unsigned int numPage);	    /* IN: # of pages in RBM  */

    /**************************************************************************/
    /* Destructor							      */
    /*									      */
    /* Delete all ObjectDescriptors and everything hanging off from them      */
    /**************************************************************************/
    ~RangeBufferManager();

    /**************************************************************************/
    /* Starts to cache an object in memory.				      */
    /*									      */
    /* Return 0 if everything is fine.					      */
    /* Otherwise return a negtive number according to the type of error.      */
    /**************************************************************************/
    int
    startCaching(RBMObject obj,         /* IN: Object 		              */

                 RangeAccessMethod *ram);
					/* IN: pointer to RAM in lower level  */

    /**************************************************************************/
    /* Stop caching an object in memory.				      */
    /*									      */
    /* Return 0 if OK. return negtive numbers for warnings and errors 	      */
    /**************************************************************************/
    int
    stopCaching(RBMObject obj);           			/* IN: Object */

    /**************************************************************************/
    /* Check if an object is currently cached.				      */
    /*									      */
    /* Return pointer to ObjectDescriptor if found 		              */
    /* Return NULL if not found 					      */
    /**************************************************************************/
    ObjectDescriptor*
    isCaching(RBMObject obj);	       				/* IN: Object */

    /**************************************************************************/
    /* Delete the victim bounding box from RBM.				      */
    /* Release all pages used by victim. 			 	      */
    /**************************************************************************/
    void kickOutVictimBBox(ObjectDescriptor *od, BBoxEntry *victim);

    /**************************************************************************/
    /* Print out info about this instance 				      */
    /* be concise if verbose == 0 					      */
    /* be verbose if verbose != 0 					      */
    /**************************************************************************/
    void output(int verbose);

    friend class RangeBufferManagerAccessMethod;
    friend class ReplacementPolicyRandom;

private:

    /* hash table of object descriptors */
    ObjectDescriptor *_objectDescriptors[NUM_RANGE_BUF_OBJ_HASH_ENTRY];

    /* number of object cached */
    int _numObjects;

    /* replacement policy */
    ReplacementPolicy *_replacementPolicy;

    /* memory manager */
    MemoryManager *_mem;
};

#endif
