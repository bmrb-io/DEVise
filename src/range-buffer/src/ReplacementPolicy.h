#ifndef __RBMREPLACEMENTPOLICY_H__
#define __RBMREPLACEMENTPOLICY_H__

#include "ObjectDescriptor.h"
#include "RangeBufferCommon.h"

/************************************************************************/
/* Each RangeBufferManager has a ReplacementPolicy associated with it.	*/
/* ReplacementPolicy decides which victim to kick out when more pages	*/
/* are needed by other people.						*/
/************************************************************************/
class ReplacementPolicy
{

public:

    ReplacementPolicy();
    virtual ~ReplacementPolicy();

    /* Find a victim. */
    /* should make sure that victim is not pinned */
    /* return NULL if no victim available */
    virtual ObjectDescriptor *pickVictim() = 0;

    /* Find a BBOX level victim */
    /* should make sure that victim's object is not pinned */
    /* return NULL if no victim available */
    virtual BBoxEntry *pickVictimBBox(ObjectDescriptor *&objd) = 0;

    /* print out info about this instance */
    /* be concise if verbose == 0 */
    /* be verbose if verbose != 0 */
    virtual void output(int verbose) = 0;
};

#endif
