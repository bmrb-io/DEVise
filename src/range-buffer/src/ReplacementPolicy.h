#ifndef __RBMREPLACEMENTPOLICY_H__
#define __RBMREPLACEMENTPOLICY_H__

#include "ObjectDescriptor.h"
#include "RangeBufferCommon.h"

class ReplacementPolicy
{

public:

    ReplacementPolicy();
    virtual ~ReplacementPolicy();

    /* should make sure that victim is not pinned */
    /* return NULL if no victim available */
    virtual ObjectDescriptor *pickVictim() = 0;

    virtual void output(int verbose) = 0;
};

#endif
