#ifndef __RANDOMRBMREPLACEMENTPOLICY_H__
#define __RANDOMRBMREPLACEMENTPOLICY_H__

#include "ReplacementPolicy.h"

/* victim is the first not pinned object you can find in the hash table */
/* not truly "random" */

class ReplacementPolicyRandom: public ReplacementPolicy
{

public:

    ReplacementPolicyRandom(RangeBufferManager *rbm);
    virtual ~ReplacementPolicyRandom();

    /* should make sure that victim is not pinned */
    /* return NULL if no victim found */
    virtual ObjectDescriptor *pickVictim();

    /* BBOX level victim */
    /* should make sure that victim's object is not pinned */
    /* return NULL if no victim available */
    virtual BBoxEntry *pickVictimBBox(ObjectDescriptor *&objd);

    virtual void output(int verbose);

private:

    RangeBufferManager *_rbm;
};

#endif
