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

    RangeBufferManager(ReplacementPolicyType rep,   /* IN: replacement policy */
		       unsigned int numPage);	    /* IN: # of pages in RBM  */

    /* delete all ObjectDescriptors and everything hanging off from them */
    ~RangeBufferManager();

    /* return 0 if everything is fine */
    /* otherwise return a negtive number according to the type of error */
    int
    startCaching(RBMObject obj,           /* IN: Object */
                 RangeAccessMethod *ram);
					/* IN: pointer to RAM in lower level */

    /* return 0 if OK. return negtive numbers for warnings and errors */
    int
    stopCaching(RBMObject obj);           /* IN: Object */

    /* return pointer to ObjectDescriptor if found */
    /* return NULL if not found */
    ObjectDescriptor*
    isCaching(RBMObject obj);	       /* IN: Object */

    /* delete the victim object from RBM */
    /* return all pages used by victim to _mem's free page list */
    void kickOutVictimBBox(ObjectDescriptor *od, BBoxEntry *victim);

    /* print out info about this instance */
    /* be concise if verbose == 0 */
    /* be verbose if verbose != 0 */
    void output(int verbose);

    friend class RangeBufferManagerAccessMethod;
    friend class ReplacementPolicyRandom;

private:

    /* hash table of object descriptors */
    ObjectDescriptor *_objectDescriptors[NUM_RANGE_BUF_OBJ_HASH_ENTRY];

    /* number of object cached */
    int _numObjects;

    ReplacementPolicy *_replacementPolicy;

    MemoryManager *_mem;
};

#endif
