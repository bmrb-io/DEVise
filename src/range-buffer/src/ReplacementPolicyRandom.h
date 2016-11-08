#ifndef __RANDOMRBMREPLACEMENTPOLICY_H__
#define __RANDOMRBMREPLACEMENTPOLICY_H__

#include "ReplacementPolicy.h"

/* victim is the first not pinned object you can find in the hash table */
/* not truly "random" */

/************************************************************************/
/* Each RangeBufferManager has a ReplacementPolicy associated with it.  */
/* ReplacementPolicy decides which victim to kick out when more pages   */
/* are needed by other people.                                          */
/*									*/
/* ReplacementPolicyRandom is a special ReplacementPolicy.		*/
/*									*/
/* Currently ReplacementPolicyRandom makes the first thing it can find 	*/
/* a victim. It's not really *random*.  				*/
/************************************************************************/
class ReplacementPolicyRandom: public ReplacementPolicy
{

public:

    /* rbm is the RangeBufferManager that uses this ReplacementPolicyRandom */
    ReplacementPolicyRandom(RangeBufferManager *rbm);

    /* destructor */
    virtual ~ReplacementPolicyRandom();

    /* should make sure that victim is not pinned */
    /* return NULL if no victim found */
    virtual ObjectDescriptor *pickVictim();

    /* BBOX level victim */
    /* should make sure that victim's object is not pinned */
    /* return NULL if no victim available */
    virtual BBoxEntry *pickVictimBBox(ObjectDescriptor *&objd);

    /* print out info about this instance */
    /* be concise if verbose == 0 */
    /* be verbose if verbose != 0 */
    virtual void output(int verbose);

private:

    /* the RangeBufferManager that uses this ReplacementPolicyRandom */
    RangeBufferManager *_rbm;
};

#endif
